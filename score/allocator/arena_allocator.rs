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

//! Arena allocator backed by a [`MemoryRegion`].

// TODO: safety clauses.

use crate::atomic::{fence, AtomicUsize, Ordering};
use crate::memory_region::{align_to, MemoryRegion};
use crate::{AllocationError, BasicAllocator};
use core::alloc::Layout;
use core::fmt as core_fmt;
use core::mem::ManuallyDrop;
use core::ptr::NonNull;
use score_log::fmt as score_fmt;

/// Arena header, keeps track of living object references and current pointer position.
struct ArenaHeader {
    ref_count: AtomicUsize,
    offset: AtomicUsize,
}

/// Arena allocator backed by a [`MemoryRegion`].
///
/// Allocated chunks are from a continuous chunk of memory provided by the memory region.
/// Deallocation is a no-op.
///
/// Internal storage is reference counted.
/// Cloning provides a new handle to same data.
pub struct ArenaAllocator<R: MemoryRegion> {
    /// Memory region, contains [`ArenaHeader`] at its start.
    /// It is wrapped in [`ManuallyDrop`] to ensure it is dropped once.
    memory_region: ManuallyDrop<R>,
}

// SAFETY: only arena header contains mutable state, and it's always accessed using atomics.
unsafe impl<R: MemoryRegion + Send> Send for ArenaAllocator<R> {}
unsafe impl<R: MemoryRegion + Sync> Sync for ArenaAllocator<R> {}

impl<R: MemoryRegion> ArenaAllocator<R> {
    /// Create a new allocator using given memory region.
    /// Allocator own data is written at the region start.
    pub fn new(memory_region: R) -> Result<Self, AllocationError> {
        // Disallow memory region smaller than size of `ArenaHeader`.
        if memory_region.layout().size() < size_of::<ArenaHeader>() {
            return Err(AllocationError::InvalidLayout);
        }
        // Disallow memory region alignments causing an offset.
        if memory_region.ptr().as_ptr().align_offset(align_of::<ArenaHeader>()) != 0 {
            return Err(AllocationError::InvalidLayout);
        }

        // Initialize the header at the start of the memory region.
        // Offset is set with regard to header size.
        // SAFETY: bounds and alignment checked above.
        unsafe {
            memory_region.ptr().cast::<ArenaHeader>().as_ptr().write(ArenaHeader {
                ref_count: AtomicUsize::new(1),
                offset: AtomicUsize::new(size_of::<ArenaHeader>()),
            });
        }

        Ok(Self {
            memory_region: ManuallyDrop::new(memory_region),
        })
    }

    /// Access the header.
    fn header(&self) -> &ArenaHeader {
        // SAFETY: header remains valid for the whole lifetime of the object.
        unsafe { self.memory_region.ptr().cast::<ArenaHeader>().as_ref() }
    }

    /// Capacity of the allocator.
    pub fn capacity(&self) -> usize {
        self.memory_region.layout().size()
    }
}

impl<R: MemoryRegion> Clone for ArenaAllocator<R> {
    fn clone(&self) -> Self {
        // Increase reference count.
        self.header().ref_count.fetch_add(1, Ordering::Relaxed);
        // SAFETY: `MemoryRegion` implementations are obliged to be bitwise copyable.
        let memory_region = ManuallyDrop::new(unsafe { core::ptr::read(&*self.memory_region) });
        Self { memory_region }
    }
}

impl<R: MemoryRegion> Drop for ArenaAllocator<R> {
    fn drop(&mut self) {
        // Release the region on last reference dropping.
        if self.header().ref_count.fetch_sub(1, Ordering::Release) == 1 {
            fence(Ordering::Acquire);
            // Drop the header before its memory is removed.
            // SAFETY: header remains valid for the whole lifetime of the object.
            unsafe { core::ptr::drop_in_place(self.memory_region.ptr().cast::<ArenaHeader>().as_ptr()) };
            // SAFETY: memory region is dropped exactly once.
            unsafe { ManuallyDrop::drop(&mut self.memory_region) };
        }
    }
}

impl<R: MemoryRegion> BasicAllocator for ArenaAllocator<R> {
    fn allocate(&self, layout: Layout) -> Result<NonNull<u8>, AllocationError> {
        // Disallow zero size allocation.
        if layout.size() == 0 {
            return Err(AllocationError::ZeroSizeAllocation);
        }

        // Get aligned size.
        let aligned_size = align_to(layout.size(), layout.align());

        // Bump the offset with compare-and-swap loop.
        // Concurrent allocations must receive non-overlapping regions.
        let offset = &self.header().offset;
        let mut current_offset = offset.load(Ordering::Relaxed);
        let ptr_as_non_null = loop {
            // Align current position and check the allocation will not exceed capacity.
            let aligned_offset = align_to(current_offset, layout.align());
            let new_offset = aligned_offset + aligned_size;
            if new_offset >= self.capacity() {
                return Err(AllocationError::OutOfMemory);
            }

            match offset.compare_exchange(current_offset, new_offset, Ordering::Relaxed, Ordering::Relaxed) {
                Ok(_) => {
                    break unsafe {
                        let ptr_with_offset = self.memory_region.ptr().as_ptr().byte_offset(aligned_offset as isize);
                        NonNull::new_unchecked(ptr_with_offset)
                    };
                },
                Err(actual) => current_offset = actual,
            }
        };

        Ok(ptr_as_non_null)
    }

    unsafe fn deallocate(&self, _ptr: NonNull<u8>, _layout: Layout) {}
}

impl<R: MemoryRegion> core_fmt::Debug for ArenaAllocator<R> {
    fn fmt(&self, f: &mut core_fmt::Formatter<'_>) -> core_fmt::Result {
        f.debug_struct("ArenaAllocator")
            .field("capacity", &self.capacity())
            .finish()
    }
}

impl<R: MemoryRegion> score_fmt::ScoreDebug for ArenaAllocator<R> {
    fn fmt(&self, f: score_fmt::Writer, spec: &score_fmt::FormatSpec) -> score_fmt::Result {
        score_fmt::DebugStruct::new(f, spec, "ArenaAllocator")
            .field("capacity", &self.capacity())
            .finish()
    }
}

#[cfg(all(test, not(loom)))]
mod tests {
    use crate::arena_allocator::ArenaHeader;
    use crate::atomic::Ordering;
    use crate::memory_region::{MemoryRegion, MmapMemoryRegion};
    use crate::{page_size, AllocationError, ArenaAllocator, BasicAllocator};
    use core::alloc::Layout;
    use core::unimplemented;
    use std::collections::BTreeSet;
    use std::sync::Arc;
    use std::thread;

    const HEADER_SIZE: usize = size_of::<ArenaHeader>();

    #[test]
    fn test_new_ok() {
        let capacity = page_size();
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let result = ArenaAllocator::new(memory_region);
        assert!(result.is_ok());
    }

    #[test]
    fn test_new_zero() {
        struct MockMemoryRegion;

        unsafe impl MemoryRegion for MockMemoryRegion {
            fn ptr(&self) -> core::ptr::NonNull<u8> {
                unimplemented!()
            }

            fn layout(&self) -> Layout {
                Layout::from_size_align(0, 4).unwrap()
            }
        }

        let memory_region = MockMemoryRegion;
        let result = ArenaAllocator::new(memory_region);
        assert!(result.is_err_and(|e| e == AllocationError::InvalidLayout));
    }

    #[test]
    fn test_new_unaligned() {
        struct MockMemoryRegion;

        unsafe impl MemoryRegion for MockMemoryRegion {
            fn ptr(&self) -> core::ptr::NonNull<u8> {
                // Address is deliberately misaligned for `ArenaHeader`.
                // It is never dereferenced.
                #[allow(clippy::manual_dangling_ptr, reason = "pointer is deliberately misaligned")]
                unsafe {
                    core::ptr::NonNull::new_unchecked(1 as *mut u8)
                }
            }

            fn layout(&self) -> Layout {
                // Large enough to pass the size check and reach the alignment check.
                Layout::from_size_align(HEADER_SIZE, 1).unwrap()
            }
        }

        let memory_region = MockMemoryRegion;
        let result = ArenaAllocator::new(memory_region);
        assert!(result.is_err_and(|e| e == AllocationError::InvalidLayout));
    }

    #[test]
    fn test_capacity_ok() {
        let capacity = page_size();
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();
        assert_eq!(allocator.capacity(), capacity);
    }

    #[test]
    fn test_allocate_single() {
        let capacity = 0x1000;
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(253, 4).unwrap();
        let result = allocator.allocate(layout);
        assert!(result.is_ok());

        // Check offset.
        assert_eq!(allocator.header().offset.load(Ordering::Relaxed), HEADER_SIZE + 256);
    }

    #[test]
    fn test_allocate_multiple() {
        let capacity = 0x1000;
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();

        // Cases to check, contains:
        // - layout
        // - allocation pointer offset from memory region start
        // - internal allocator offset
        let cases = vec![
            (Layout::from_size_align(253, 1).unwrap(), HEADER_SIZE, HEADER_SIZE + 253),
            (
                Layout::from_size_align(557, 2).unwrap(),
                HEADER_SIZE + 254,
                HEADER_SIZE + 812,
            ),
            (
                Layout::from_size_align(39, 8).unwrap(),
                HEADER_SIZE + 816,
                HEADER_SIZE + 856,
            ),
        ];

        let memory_region_ptr = allocator.memory_region.ptr().as_ptr();
        for (layout, exp_ptr_offset, exp_allocator_offset) in cases {
            // Allocate.
            let alloc_ptr = allocator.allocate(layout).unwrap().as_ptr();

            // Check position relative to memory region and allocator offset.
            let allocation_ptr_offset = unsafe { alloc_ptr.offset_from(memory_region_ptr) };
            assert_eq!(allocation_ptr_offset, exp_ptr_offset as isize);
            assert_eq!(allocator.header().offset.load(Ordering::Relaxed), exp_allocator_offset);
        }
    }

    #[test]
    fn test_allocate_oom() {
        let capacity = page_size();
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(2 * page_size(), 4).unwrap();
        let result = allocator.allocate(layout);
        assert!(result.is_err_and(|e| e == AllocationError::OutOfMemory));

        // Check offset.
        assert_eq!(allocator.header().offset.load(Ordering::Relaxed), HEADER_SIZE);
    }

    #[test]
    fn test_allocate_zero() {
        let capacity = page_size();
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(0, 4).unwrap();
        let result = allocator.allocate(layout);
        assert!(result.is_err_and(|e| e == AllocationError::ZeroSizeAllocation));

        // Check offset.
        assert_eq!(allocator.header().offset.load(Ordering::Relaxed), HEADER_SIZE);
    }

    #[test]
    fn test_deallocate_panic() {
        let capacity = page_size();
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();

        // Allocate.
        let layout = Layout::from_size_align(253, 4).unwrap();
        let ptr = allocator.allocate(layout).unwrap();

        // Deallocate - no-op.
        unsafe { allocator.deallocate(ptr, layout) };
    }

    #[test]
    fn test_clone_shared_offset() {
        // Create allocator and its clone.
        let capacity = page_size();
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();
        let clone = allocator.clone();

        // Allocate using cloned object.
        let layout = Layout::from_size_align(253, 4).unwrap();
        clone.allocate(layout).unwrap();

        // Check offset is bumped for both objects.
        let expected = HEADER_SIZE + 256;
        assert_eq!(clone.header().offset.load(Ordering::Relaxed), expected);
        assert_eq!(allocator.header().offset.load(Ordering::Relaxed), expected);
    }

    #[test]
    fn test_clone_ref_count() {
        let memory_region = MmapMemoryRegion::new(page_size()).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();
        assert_eq!(allocator.header().ref_count.load(Ordering::Relaxed), 1);

        let clone = allocator.clone();
        assert_eq!(allocator.header().ref_count.load(Ordering::Relaxed), 2);

        drop(clone);
        assert_eq!(allocator.header().ref_count.load(Ordering::Relaxed), 1);
    }

    #[test]
    fn test_clone_single_unmap() {
        // Check no panic happens when both instances are dropped.
        let memory_region = MmapMemoryRegion::new(page_size()).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();
        let clone = allocator.clone();
        drop(allocator);
        drop(clone);
    }

    #[test]
    fn test_concurrent_allocate_distinct_regions() {
        // Perform allocations from multiple threads to check every region is distinct and in-bounds.
        const THREADS: usize = 8;
        const PER_THREAD: usize = 32;
        let capacity = 4 * page_size();
        let memory_region = MmapMemoryRegion::new(capacity).unwrap();
        let allocator = Arc::new(ArenaAllocator::new(memory_region).unwrap());
        let layout = Layout::from_size_align(16, 8).unwrap();

        let handles: Vec<_> = (0..THREADS)
            .map(|_| {
                let allocator = allocator.clone();
                thread::spawn(move || {
                    let mut ptrs = Vec::with_capacity(PER_THREAD);
                    for _ in 0..PER_THREAD {
                        ptrs.push(allocator.allocate(layout).unwrap().as_ptr() as usize);
                    }
                    ptrs
                })
            })
            .collect();

        let base = allocator.memory_region.ptr().as_ptr() as usize;
        let mut all = BTreeSet::new();
        for handle in handles {
            for addr in handle.join().unwrap() {
                // In-bounds and (with size 16) non-overlapping given distinct start addresses.
                assert!(addr >= base && addr < base + capacity);
                assert!(all.insert(addr), "region handed out twice: {addr:#x}");
            }
        }
        assert_eq!(all.len(), THREADS * PER_THREAD);
    }

    #[test]
    fn test_concurrent_clone_drop_single_unmap() {
        // Perform clones and drops in multiple threads.
        // The original is dropped last.
        let memory_region = MmapMemoryRegion::new(page_size()).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();
        let shared = Arc::new(allocator.clone());

        let handles: Vec<_> = (0..8)
            .map(|_| {
                let shared = shared.clone();
                thread::spawn(move || {
                    for _ in 0..64 {
                        let c = (*shared).clone();
                        drop(c);
                    }
                })
            })
            .collect();
        for handle in handles {
            handle.join().unwrap();
        }

        drop(Arc::try_unwrap(shared).ok().unwrap());
        assert_eq!(allocator.header().ref_count.load(Ordering::Relaxed), 1);
    }
}

#[cfg(all(test, loom))]
mod loom_tests {
    use crate::memory_region::MmapMemoryRegion;
    use crate::{page_size, ArenaAllocator, BasicAllocator};
    use core::alloc::Layout;
    use loom::thread;

    #[test]
    fn loom_concurrent_allocate_distinct() {
        // Perform two concurrent allocations on a shared arena.
        loom::model(|| {
            let memory_region = MmapMemoryRegion::new(page_size()).unwrap();
            let allocator = ArenaAllocator::new(memory_region).unwrap();
            let layout = Layout::from_size_align(16, 8).unwrap();

            let a = allocator.clone();
            let b = allocator.clone();
            let h1 = thread::spawn(move || a.allocate(layout).unwrap().as_ptr() as usize);
            let h2 = thread::spawn(move || b.allocate(layout).unwrap().as_ptr() as usize);

            let p1 = h1.join().unwrap();
            let p2 = h2.join().unwrap();
            assert_ne!(p1, p2);
            // Size is 16, so distinct start addresses cannot overlap.
            assert!(p1.abs_diff(p2) >= 16);
        });
    }

    #[test]
    fn loom_concurrent_clone_drop() {
        // Perform two concurrent drops on a shared handle.
        loom::model(|| {
            let memory_region = MmapMemoryRegion::new(page_size()).unwrap();
            let allocator = ArenaAllocator::new(memory_region).unwrap();

            let a = allocator.clone();
            let b = allocator.clone();
            let h1 = thread::spawn(move || drop(a));
            let h2 = thread::spawn(move || drop(b));
            h1.join().unwrap();
            h2.join().unwrap();
            // `allocator` drops here as the last reference.
        });
    }
}
