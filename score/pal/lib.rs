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

//! Minimal POSIX adaptation layer.

#![allow(non_camel_case_types)]
#![allow(missing_docs)]

mod affinity;
mod errno;

pub use affinity::{get_affinity, set_affinity, CpuSet};
pub use errno::errno;

pub type c_int = core::ffi::c_int;
pub type c_long = core::ffi::c_long;
pub type c_ulong = core::ffi::c_ulong;
pub type size_t = usize;
pub type c_void = core::ffi::c_void;
pub type pid_t = i32;
pub type pthread_t = c_ulong;

#[cfg(target_os = "linux")]
#[repr(C)]
pub struct pthread_attr_t {
    #[cfg(target_pointer_width = "32")]
    __size: [u32; 8],
    #[cfg(target_pointer_width = "64")]
    __size: [u64; 7],
}
#[cfg(target_os = "nto")]
#[repr(C)]
pub struct pthread_attr_t {
    __data1: c_long,
    __data2: [u8; 96],
}

#[cfg(target_os = "linux")]
#[repr(C)]
pub struct sched_param {
    pub sched_priority: c_int,
}

#[cfg(target_os = "nto")]
#[repr(C)]
#[repr(align(8))]
pub struct sched_param {
    pub sched_priority: c_int,
    pub sched_curpriority: c_int,
    pub reserved: [c_int; 10],
}

extern "C" {
    pub fn sysconf(__name: c_int) -> c_long;
    pub fn pthread_self() -> pthread_t;
    pub fn pthread_join(native: pthread_t, value: *mut *mut c_void) -> c_int;
    pub fn pthread_attr_init(attr: *mut pthread_attr_t) -> c_int;
    pub fn pthread_attr_destroy(attr: *mut pthread_attr_t) -> c_int;
    pub fn pthread_attr_getstacksize(attr: *const pthread_attr_t, stacksize: *mut size_t) -> c_int;
    pub fn pthread_attr_setstacksize(attr: *mut pthread_attr_t, stack_size: size_t) -> c_int;
    pub fn pthread_create(
        native: *mut pthread_t,
        attr: *const pthread_attr_t,
        f: extern "C" fn(*mut c_void) -> *mut c_void,
        value: *mut c_void,
    ) -> c_int;
    pub fn pthread_getschedparam(native: pthread_t, policy: *mut c_int, param: *mut sched_param) -> c_int;
    pub fn pthread_setschedparam(native: pthread_t, policy: c_int, param: *const sched_param) -> c_int;
    pub fn sched_get_priority_max(policy: c_int) -> c_int;
    pub fn sched_get_priority_min(policy: c_int) -> c_int;
    pub fn pthread_attr_getinheritsched(attr: *const pthread_attr_t, inheritsched: *mut c_int) -> c_int;
    pub fn pthread_attr_setinheritsched(attr: *mut pthread_attr_t, inheritsched: c_int) -> c_int;
    pub fn pthread_attr_getschedparam(attr: *const pthread_attr_t, param: *mut sched_param) -> c_int;
    pub fn pthread_attr_setschedparam(attr: *mut pthread_attr_t, param: *const sched_param) -> c_int;
    pub fn pthread_attr_getschedpolicy(attr: *const pthread_attr_t, policy: *mut c_int) -> c_int;
    pub fn pthread_attr_setschedpolicy(attr: *mut pthread_attr_t, policy: c_int) -> c_int;
}

#[cfg(target_os = "linux")]
pub const SCHED_OTHER: c_int = 0;
#[cfg(target_os = "nto")]
pub const SCHED_OTHER: c_int = 3;
pub const SCHED_FIFO: c_int = 1;
pub const SCHED_RR: c_int = 2;

pub const PTHREAD_INHERIT_SCHED: c_int = 0;
#[cfg(target_os = "linux")]
pub const PTHREAD_EXPLICIT_SCHED: c_int = 1;
#[cfg(target_os = "nto")]
pub const PTHREAD_EXPLICIT_SCHED: c_int = 2;
