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

//! mmap-backed arena allocator.

use crate::{AllocationError, BasicAllocator};
use core::alloc::Layout;
use core::cell::Cell;
use core::fmt as core_fmt;
use core::ptr::{null_mut, NonNull};
use pal::{errno, mmap, munmap, sysconf, MAP_ANONYMOUS, MAP_PRIVATE, PROT_READ, PROT_WRITE, _SC_PAGESIZE};
use score_log::fmt as score_fmt;

/// Get current page size.
pub fn page_size() -> usize {
    let result = unsafe { sysconf(_SC_PAGESIZE) };
    assert!(result >= 1, "page size must not be less than 1");
    result as usize
}

/// Get aligned value - rounded up to a multiplication of the second value.
fn align_to(value: usize, align: usize) -> usize {
    let mut v_div = value / align;
    if !value.is_multiple_of(align) {
        v_div += 1;
    }
    v_div * align
}

/// mmap-backed memory region.
struct MemoryRegion {
    ptr: *mut u8,
    capacity: usize,
}

impl MemoryRegion {
    /// Create new memory region using mmap.
    /// Capacity of the region is rounded up to the nearest multiplication of a page size.
    pub fn new(capacity: usize) -> Result<Self, AllocationError> {
        // Disallow zero size allocation.
        if capacity == 0 {
            return Err(AllocationError::ZeroSizeAllocation);
        }

        // Determine final capacity with regard to page size.
        let capacity = align_to(capacity, page_size());

        // Create a memory mapping.
        let prot = PROT_READ | PROT_WRITE;
        let flags = MAP_PRIVATE | MAP_ANONYMOUS;
        let fd = -1;
        let ptr = unsafe { mmap(null_mut(), capacity, prot, flags, fd, 0) };
        if ptr as isize == -1 || ptr.is_null() {
            // TODO: information about errno is missing.
            return Err(AllocationError::Internal);
        }

        Ok(Self {
            ptr: ptr.cast(),
            capacity,
        })
    }
}

impl Drop for MemoryRegion {
    fn drop(&mut self) {
        let rc = unsafe { munmap(self.ptr.cast(), self.capacity) };
        if rc != 0 {
            let errno = errno();
            panic!("munmap failed, rc: {rc}, errno: {errno}");
        }
    }
}

/// mmap-backed arena allocator.
///
/// Allocated chunks are from a continuous chunk of memory obtained using mmap.
/// Deallocation is a no-op.
pub struct MmapArenaAllocator {
    memory_region: MemoryRegion,
    offset: Cell<usize>,
}

impl MmapArenaAllocator {
    /// Create new allocator.
    /// Capacity is rounded up to the nearest multiplication of a page size.
    pub fn new(capacity: usize) -> Result<Self, AllocationError> {
        let memory_region = MemoryRegion::new(capacity)?;
        let offset = Cell::new(0);
        Ok(Self { memory_region, offset })
    }

    /// Capacity of the allocator.
    pub fn capacity(&self) -> usize {
        self.memory_region.capacity
    }
}

impl BasicAllocator for MmapArenaAllocator {
    fn allocate(&self, layout: Layout) -> Result<NonNull<u8>, AllocationError> {
        // Disallow zero size allocation.
        if layout.size() == 0 {
            return Err(AllocationError::ZeroSizeAllocation);
        }

        // Get aligned size.
        let aligned_size = align_to(layout.size(), layout.align());

        // Get current pointer position and align.
        let current_offset = align_to(self.offset.get(), layout.align());

        // Check if allocation will not exceed capacity.
        let new_offset = current_offset + aligned_size;
        if new_offset >= self.capacity() {
            return Err(AllocationError::OutOfMemory);
        }

        // Get pointer at given offset from memory region.
        let ptr_as_non_null = unsafe {
            let ptr_with_offset = self.memory_region.ptr.byte_offset(current_offset as isize);
            NonNull::new_unchecked(ptr_with_offset)
        };

        // Set new offset.
        self.offset.set(new_offset);

        Ok(ptr_as_non_null)
    }

    unsafe fn deallocate(&self, _ptr: NonNull<u8>, _layout: Layout) {}
}

impl core_fmt::Debug for MmapArenaAllocator {
    fn fmt(&self, f: &mut core_fmt::Formatter<'_>) -> core_fmt::Result {
        f.debug_struct("MmapArenaAllocator")
            .field("capacity", &self.capacity())
            .finish()
    }
}

impl score_fmt::ScoreDebug for MmapArenaAllocator {
    fn fmt(&self, f: score_fmt::Writer, spec: &score_fmt::FormatSpec) -> score_fmt::Result {
        score_fmt::DebugStruct::new(f, spec, "MmapArenaAllocator")
            .field("capacity", &self.capacity())
            .finish()
    }
}

#[cfg(test)]
mod tests {
    use crate::{page_size, AllocationError, BasicAllocator, MmapArenaAllocator};
    use core::alloc::Layout;

    #[test]
    fn test_page_size_ok() {
        // Check no panic, exact size is platform-specific.
        let _ = page_size();
    }

    #[test]
    fn test_new_ok() {
        let capacity = page_size();
        let result = MmapArenaAllocator::new(capacity);
        assert!(result.is_ok());
    }

    #[test]
    fn test_new_zero() {
        let capacity = 0;
        let result = MmapArenaAllocator::new(capacity);
        assert!(result.is_err_and(|e| e == AllocationError::ZeroSizeAllocation));
    }

    #[test]
    fn test_capacity_aligned_to_page_size() {
        let capacity = 4 * page_size();
        let allocator = MmapArenaAllocator::new(capacity).unwrap();
        assert_eq!(allocator.capacity(), capacity);
    }

    #[test]
    fn test_capacity_not_aligned_to_page_size() {
        let expected_capacity = 4 * page_size();
        let capacity = expected_capacity - 123;
        let allocator = MmapArenaAllocator::new(capacity).unwrap();
        assert_eq!(allocator.capacity(), expected_capacity);
    }

    #[test]
    fn test_allocate_single() {
        let capacity = 0x1000;
        let allocator = MmapArenaAllocator::new(capacity).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(253, 4).unwrap();
        let result = allocator.allocate(layout.clone());
        assert!(result.is_ok());

        // Check offset.
        assert_eq!(allocator.offset.get(), 256);
    }

    #[test]
    fn test_allocate_multiple() {
        let capacity = 0x1000;
        let allocator = MmapArenaAllocator::new(capacity).unwrap();

        // Cases to check, contains:
        // - layout
        // - allocation pointer offset from memory region start
        // - internal allocator offset
        let cases = vec![
            (Layout::from_size_align(253, 1).unwrap(), 0, 253),
            (Layout::from_size_align(557, 2).unwrap(), 254, 812),
            (Layout::from_size_align(39, 8).unwrap(), 816, 856),
        ];

        let memory_region_ptr = allocator.memory_region.ptr;
        for (layout, exp_ptr_offset, exp_allocator_offset) in cases {
            // Allocate.
            let alloc_ptr = allocator.allocate(layout).unwrap().as_ptr();

            // Check position relative to memory region and allocator offset.
            let allocation_ptr_offset = unsafe { alloc_ptr.offset_from(memory_region_ptr) };
            assert_eq!(allocation_ptr_offset, exp_ptr_offset);
            assert_eq!(allocator.offset.get(), exp_allocator_offset);
        }
    }

    #[test]
    fn test_allocate_oom() {
        let capacity = page_size();
        let allocator = MmapArenaAllocator::new(capacity).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(2 * page_size(), 4).unwrap();
        let result = allocator.allocate(layout.clone());
        assert!(result.is_err_and(|e| e == AllocationError::OutOfMemory));

        // Check offset.
        assert_eq!(allocator.offset.get(), 0);
    }

    #[test]
    fn test_allocate_zero() {
        let capacity = page_size();
        let allocator = MmapArenaAllocator::new(capacity).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(0, 4).unwrap();
        let result = allocator.allocate(layout.clone());
        assert!(result.is_err_and(|e| e == AllocationError::ZeroSizeAllocation));

        // Check offset.
        assert_eq!(allocator.offset.get(), 0);
    }

    #[test]
    fn test_deallocate_panic() {
        let capacity = page_size();
        let allocator = MmapArenaAllocator::new(capacity).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(253, 4).unwrap();
        let ptr = allocator.allocate(layout.clone()).unwrap();

        // Deallocate - no-op.
        unsafe { allocator.deallocate(ptr, layout) };
    }
}
