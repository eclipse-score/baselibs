// *******************************************************************************
// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// <https://www.apache.org/licenses/LICENSE-2.0>
//
// SPDX-License-Identifier: Apache-2.0
// *******************************************************************************

//! Affinity handling differs between Linux and QNX.
//! Module ensures similar behavior between both OSes.

extern crate alloc;

use crate::{c_int, errno};
use score_log::ScoreDebug;

#[cfg(target_os = "linux")]
#[repr(C)]
pub struct cpu_set_t {
    #[cfg(all(target_pointer_width = "32", not(target_arch = "x86_64")))]
    bits: [u32; 32],
    #[cfg(not(all(target_pointer_width = "32", not(target_arch = "x86_64"))))]
    bits: [u64; 16],
}

#[cfg(target_os = "linux")]
extern "C" {
    pub fn sched_getaffinity(pid: crate::pid_t, cpusetsize: crate::size_t, cpuset: *mut cpu_set_t) -> c_int;
    pub fn sched_setaffinity(pid: crate::pid_t, cpusetsize: crate::size_t, cpuset: *const cpu_set_t) -> c_int;
}

#[cfg(target_os = "nto")]
const _NTO_TCTL_RUNMASK_GET_AND_SET_INHERIT: u32 = 10;
#[cfg(target_os = "nto")]
extern "C" {
    pub fn ThreadCtl(__cmd: c_int, __data: *mut crate::c_void) -> c_int;
}

#[cfg(target_os = "nto")]
pub const _SC_NPROCESSORS_ONLN: c_int = 91;

const MAX_CPU_NUM: usize = 1024;
const CPU_MASK_SIZE: usize = MAX_CPU_NUM / (u8::BITS as usize);

/// Common CPU set representation.
/// Limited to 1024 CPUs.
#[derive(Clone, Copy, Debug, ScoreDebug, PartialEq, Eq)]
pub struct CpuSet {
    mask: [u8; CPU_MASK_SIZE],
}

impl CpuSet {
    /// Create a new CPU set.
    pub fn new(affinity: &[usize]) -> Self {
        let mask = Self::create_mask(affinity);
        Self { mask }
    }

    /// Create mask based on provided list.
    fn create_mask(affinity: &[usize]) -> [u8; CPU_MASK_SIZE] {
        let mut mask = [0u8; _];
        for cpu_id in affinity.iter().copied() {
            const MAX_CPU_ID: usize = MAX_CPU_NUM - 1;
            assert!(
                cpu_id <= MAX_CPU_ID,
                "CPU ID provided to affinity exceeds max supported size, provided: {cpu_id}, max: {MAX_CPU_ID}"
            );

            let index = cpu_id / 8;
            let offset = cpu_id % 8;
            mask[index] |= 1 << offset;
        }

        mask
    }

    pub fn set(&mut self, affinity: &[usize]) {
        self.mask = Self::create_mask(affinity);
    }

    pub fn get(&self) -> Box<[usize]> {
        let mut cpu_id_array = [0usize; MAX_CPU_NUM];
        let mut counter = 0;
        for cpu_id in 0..MAX_CPU_NUM {
            let index = cpu_id / 8;
            let offset = cpu_id % 8;

            if (self.mask[index] & (1 << offset)) != 0 {
                cpu_id_array[counter] = cpu_id;
                counter += 1;
            }
        }

        Box::from(&cpu_id_array[..counter])
    }
}

#[cfg(target_os = "linux")]
impl From<cpu_set_t> for CpuSet {
    fn from(value: cpu_set_t) -> Self {
        assert!(
            core::mem::size_of::<cpu_set_t>() == CPU_MASK_SIZE,
            "unsupported native CPU mask size"
        );

        // SAFETY: CPU mask layout and size is ensured.
        let mask: [u8; CPU_MASK_SIZE] = unsafe { core::mem::transmute(value) };
        Self { mask }
    }
}

#[cfg(target_os = "linux")]
impl From<CpuSet> for cpu_set_t {
    fn from(value: CpuSet) -> Self {
        assert!(
            core::mem::size_of::<cpu_set_t>() == CPU_MASK_SIZE,
            "unsupported native CPU mask size"
        );

        // SAFETY: CPU mask layout and size is ensured.
        unsafe { core::mem::transmute(value.mask) }
    }
}

/// QNX representation of a CPU set.
///
/// Number of CPUs is restricted to 1024 - same as for Linux.
/// QNX docs recommend the following:
/// - read the number of CPUs from `_syspage_ptr->num_cpu`
/// - allocate mask fields dynamically
///
/// Current approach avoids dynamic alloc and aligns the behavior with Linux.
///
/// Refer to QNX docs for more information:
/// https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/t/threadctl.html
#[cfg(target_os = "nto")]
#[repr(C)]
#[derive(Clone, Copy)]
struct NtoCpuSet {
    // `ThreadCtl` data internal representation.
    // `size` must be determined based on number of CPU, with max allowed value equal to 32.
    // [ size: i32 | run_mask: size * u32 | inherit_mask: size * u32 ]
    data: [u32; 1 + 32 + 32],
}

#[cfg(target_os = "nto")]
impl NtoCpuSet {
    fn new() -> Self {
        // Get number of CPUs.
        let num_cpus = unsafe { crate::sysconf(_SC_NPROCESSORS_ONLN) } as u32;
        assert!(
            num_cpus >= 1 && num_cpus <= MAX_CPU_NUM as u32,
            "number of CPUs in the system out of expected range: {num_cpus}"
        );

        // Get number of elements and set it at 0.
        let size = (num_cpus - 1) / u32::BITS + 1;
        let mut data = [0; _];
        data[0] = size;

        Self { data }
    }

    fn run_mask(&self) -> &[u32] {
        let size = self.data[0] as usize;
        &self.data[1..1 + size]
    }

    fn run_mask_mut(&mut self) -> &mut [u32] {
        let size = self.data[0] as usize;
        &mut self.data[1..1 + size]
    }
}

#[cfg(target_os = "nto")]
impl From<NtoCpuSet> for CpuSet {
    fn from(value: NtoCpuSet) -> Self {
        // Reorient data from array of u32 to array of u8.
        let mut mask = [0u8; CPU_MASK_SIZE];
        for (i, &word) in value.run_mask().iter().enumerate() {
            const CHUNK_SIZE: usize = i32::BITS as usize / 8;
            mask[i * CHUNK_SIZE..i * CHUNK_SIZE + CHUNK_SIZE].copy_from_slice(&word.to_le_bytes());
        }
        Self { mask }
    }
}

#[cfg(target_os = "nto")]
impl From<CpuSet> for NtoCpuSet {
    fn from(value: CpuSet) -> Self {
        // Reorient data from array of u8 to array of u32.
        let mut nto_cpu_set = Self::new();
        let size = nto_cpu_set.data[0] as usize;
        let used_bytes = size * (u32::BITS as usize / 8);
        assert!(
            value.mask[used_bytes..].iter().all(|&b| b == 0),
            "provided CpuSet contains CPU IDs out of range for NtoCpuSet in this configuration"
        );

        for (i, chunk) in value.mask[..used_bytes].chunks_exact(4).enumerate() {
            nto_cpu_set.run_mask_mut()[i] = u32::from_le_bytes(chunk.try_into().expect("chunk is 4 bytes"));
        }
        nto_cpu_set
    }
}

/// Set affinity of a current thread.
pub fn set_affinity(cpu_set: CpuSet) {
    #[cfg(target_os = "linux")]
    {
        let native_cpu_set = cpu_set_t::from(cpu_set);
        let cpu_set_size = core::mem::size_of::<cpu_set_t>();
        // SAFETY:
        // Native CPU set is properly initialized.
        // Pointer is ensured to be valid.
        let rc = unsafe { sched_setaffinity(0, cpu_set_size, &native_cpu_set) };
        if rc != 0 {
            let errno = errno();
            panic!("sched_setaffinity failed, rc: {rc}, errno: {errno}");
        }
    }

    #[cfg(target_os = "nto")]
    {
        let mut native_cpu_set = NtoCpuSet::from(cpu_set);
        // SAFETY:
        // Native CPU set is properly initialized.
        // `NtoCpuSet` layout must be as expected by `ThreadCtl`.
        // Pointer is ensured to be valid.
        let rc = unsafe {
            ThreadCtl(
                _NTO_TCTL_RUNMASK_GET_AND_SET_INHERIT as crate::c_int,
                (&mut native_cpu_set as *mut NtoCpuSet).cast(),
            )
        };
        if rc != 0 {
            let errno = errno();
            panic!("ThreadCtl failed, rc: {rc}, errno: {errno}");
        }
    }
}

/// Get affinity of a current thread.
pub fn get_affinity() -> Box<[usize]> {
    #[cfg(target_os = "linux")]
    {
        let mut native_cpu_set = core::mem::MaybeUninit::zeroed();
        let cpu_set_size = core::mem::size_of::<cpu_set_t>();
        // SAFETY:
        // Provided native CPU set is zeroed, then initialized by a `sched_getaffinity` call.
        // Pointer is ensured to be valid.
        let rc = unsafe { sched_getaffinity(0, cpu_set_size, native_cpu_set.as_mut_ptr()) };
        if rc != 0 {
            let errno = errno();
            panic!("sched_getaffinity failed, rc: {rc}, errno: {errno}");
        }

        // SAFETY: refer to a comment above.
        let cpu_set = CpuSet::from(unsafe { native_cpu_set.assume_init() });
        cpu_set.get()
    }

    #[cfg(target_os = "nto")]
    {
        let mut native_cpu_set = NtoCpuSet::new();
        // SAFETY:
        // Native CPU set is properly initialized.
        // `NtoCpuSet` layout must be as expected by `ThreadCtl`.
        // Pointer is ensured to be valid.
        let rc = unsafe {
            ThreadCtl(
                _NTO_TCTL_RUNMASK_GET_AND_SET_INHERIT as crate::c_int,
                (&mut native_cpu_set as *mut NtoCpuSet).cast(),
            )
        };
        if rc != 0 {
            let errno = errno();
            panic!("ThreadCtl failed, rc: {rc}, errno: {errno}");
        }

        let cpu_set = CpuSet::from(native_cpu_set);
        cpu_set.get()
    }
}

#[cfg(test)]
mod tests {
    use crate::affinity::{get_affinity, set_affinity, CpuSet, MAX_CPU_NUM};
    use std::sync::mpsc::channel;

    #[test]
    fn cpu_set_new_empty_succeeds() {
        let cpu_set = CpuSet::new(&[]);
        assert!(cpu_set.mask.iter().all(|x| *x == 0));
    }

    #[test]
    fn cpu_set_new_some_succeeds() {
        let cpu_set = CpuSet::new(&[0, 123, 1023]);
        let mut data_vec = cpu_set.mask.to_vec();

        // Test removes from `Vec`, start from the end.
        assert_eq!(data_vec.remove(127), 0x80);
        assert_eq!(data_vec.remove(15), 0x08);
        assert_eq!(data_vec.remove(0), 0x01);
    }

    #[test]
    fn cpu_set_new_full_succeeds() {
        let all_ids: Vec<_> = (0..MAX_CPU_NUM).collect();
        let cpu_set = CpuSet::new(&all_ids);
        assert!(cpu_set.mask.iter().all(|x| *x == 0xFF));
    }

    #[test]
    #[should_panic(expected = "CPU ID provided to affinity exceeds max supported size, provided: 1024, max: 1023")]
    fn cpu_set_new_out_of_range() {
        let _ = CpuSet::new(&[0, 123, 1023, 1024]);
    }

    #[test]
    fn cpu_set_set_succeeds() {
        let mut cpu_set = CpuSet::new(&[]);
        cpu_set.set(&[0, 123, 1023]);
        let mut data_vec = cpu_set.mask.to_vec();

        // Test removes from `Vec`, start from the end.
        assert_eq!(data_vec.remove(127), 0x80);
        assert_eq!(data_vec.remove(15), 0x08);
        assert_eq!(data_vec.remove(0), 0x01);
    }

    #[test]
    #[should_panic(expected = "CPU ID provided to affinity exceeds max supported size, provided: 1024, max: 1023")]
    fn cpu_set_set_out_of_range() {
        let mut cpu_set = CpuSet::new(&[]);
        cpu_set.set(&[0, 123, 1023, 1024]);
    }

    #[test]
    fn cpu_set_get_succeeds() {
        let exp = vec![0, 123, 1023];
        let cpu_set = CpuSet::new(&exp);
        let got = cpu_set.get();
        assert_eq!(exp, got.iter().copied().collect::<Vec<_>>());
    }

    #[test]
    fn set_affinity_succeeds() {
        let exp_affinity = vec![0];
        let cpu_set = CpuSet::new(&exp_affinity);
        let (tx, rx) = channel();
        let join_handle = std::thread::spawn(move || {
            set_affinity(cpu_set);
            tx.send(get_affinity()).unwrap();
        });
        join_handle.join().unwrap();

        assert_eq!(rx.recv().unwrap().iter().copied().collect::<Vec<_>>(), exp_affinity);
    }

    #[test]
    fn set_affinity_out_of_range() {
        // Assuming test is not running on a 1000-core machine, still within valid CPU mask.
        let cpu_set = CpuSet::new(&[1000]);
        let join_handle = std::thread::spawn(move || {
            set_affinity(cpu_set);
        });
        let result = join_handle.join();
        assert!(result.is_err());
    }
}
