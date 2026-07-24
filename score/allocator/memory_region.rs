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

//! Memory region.

use crate::AllocationError;
use core::alloc::Layout;
use core::ptr::{null_mut, NonNull};
use pal::{errno, mmap, munmap, sysconf, MAP_ANONYMOUS, MAP_PRIVATE, PROT_READ, PROT_WRITE, _SC_PAGESIZE};

/// Get current page size.
pub fn page_size() -> usize {
    // SAFETY: both `sysconf` and `_SC_PAGESIZE` are ensured to be valid.
    let result = unsafe { sysconf(_SC_PAGESIZE) };
    assert!(result >= 1, "page size must not be less than 1");
    result as usize
}

/// Get aligned value - rounded up to a multiplication of the second value.
pub(crate) fn align_to(value: usize, align: usize) -> usize {
    let mut v_div = value / align;
    if !value.is_multiple_of(align) {
        v_div += 1;
    }
    v_div * align
}

/// Memory region interface.
///
/// # Safety
///
/// - Object must be safe to bitwise-copy.
/// - `ptr()` and `layout()` must return same values over the lifetime of the object.
/// - Underlying storage must be released on drop.
pub unsafe trait MemoryRegion {
    /// Pointer to region start.
    fn ptr(&self) -> NonNull<u8>;
    /// Layout of the region.
    fn layout(&self) -> Layout;
}

/// mmap-backed memory region.
pub struct MmapMemoryRegion {
    ptr: NonNull<u8>,
    layout: Layout,
}

impl MmapMemoryRegion {
    /// Create new memory region using mmap.
    /// Capacity of the region is rounded up to the nearest multiplication of a page size.
    pub fn new(capacity: usize) -> Result<Self, AllocationError> {
        // Disallow zero size allocation.
        if capacity == 0 {
            return Err(AllocationError::ZeroSizeAllocation);
        }

        // Determine final capacity with regard to page size.
        let page_size = page_size();
        let capacity = align_to(capacity, page_size);

        // Create a memory mapping.
        let prot = PROT_READ | PROT_WRITE;
        let flags = MAP_PRIVATE | MAP_ANONYMOUS;
        let fd = -1;
        // SAFETY: parameters are ensured to be valid.
        let ptr: *mut u8 = unsafe { mmap(null_mut(), capacity, prot, flags, fd, 0) }.cast();
        if ptr as isize == -1 || ptr.is_null() {
            // TODO: information about errno is missing.
            return Err(AllocationError::Internal);
        }

        // SAFETY: checked against null above.
        let ptr = unsafe { NonNull::new_unchecked(ptr) };
        // Create layout - aligned to page size, capacity is rounded up to fill the page.
        let layout = Layout::from_size_align(capacity, page_size).map_err(|_| AllocationError::Internal)?;

        Ok(Self { ptr, layout })
    }
}

impl Drop for MmapMemoryRegion {
    fn drop(&mut self) {
        // SAFETY: `ptr` and `layout` come from a successful `mmap` in `new`.
        let rc = unsafe { munmap(self.ptr.as_ptr().cast(), self.layout.size()) };
        if rc != 0 {
            let errno = errno();
            panic!("munmap failed, rc: {rc}, errno: {errno}");
        }
    }
}

unsafe impl MemoryRegion for MmapMemoryRegion {
    fn ptr(&self) -> NonNull<u8> {
        self.ptr
    }

    fn layout(&self) -> Layout {
        self.layout
    }
}

// SAFETY: region contains process global memory, it's safe to send and sync as long as object is alive.
unsafe impl Send for MmapMemoryRegion {}
unsafe impl Sync for MmapMemoryRegion {}

#[cfg(all(test, not(loom)))]
mod tests {
    use crate::memory_region::{page_size, MemoryRegion, MmapMemoryRegion};
    use crate::AllocationError;

    #[test]
    fn test_page_size_ok() {
        // Check no panic, exact size is platform-specific.
        let _ = page_size();
    }

    #[test]
    fn test_new_ok() {
        let region = MmapMemoryRegion::new(page_size()).unwrap();
        assert_eq!(region.layout().size(), page_size());
        assert_eq!(region.layout().align(), page_size());
    }

    #[test]
    fn test_new_zero() {
        let result = MmapMemoryRegion::new(0);
        assert!(result.is_err_and(|e| e == AllocationError::ZeroSizeAllocation));
    }

    #[test]
    fn test_new_capacity_aligned_to_page_size() {
        let capacity = 4 * page_size();
        let region = MmapMemoryRegion::new(capacity).unwrap();
        assert_eq!(region.layout().size(), capacity);
    }

    #[test]
    fn test_new_capacity_rounds_up() {
        // Slightly over one page size - rounds up to two page sizes.
        let region = MmapMemoryRegion::new(page_size() + 1).unwrap();
        assert_eq!(region.layout().size(), 2 * page_size());

        // A single byte - rounds up to a full page size.
        let region = MmapMemoryRegion::new(1).unwrap();
        assert_eq!(region.layout().size(), page_size());
    }

    #[test]
    fn test_send_sync() {
        fn assert_send_sync<T: Send + Sync>() {}
        assert_send_sync::<MmapMemoryRegion>();
    }
}
