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

//! Memory pool.

use crate::atomic::{fence, AtomicU8, AtomicUsize, Ordering};
use crate::{AllocationError, BasicAllocator};
use core::alloc::Layout;
use core::fmt as core_fmt;
use core::ops::Drop;
use core::ptr::NonNull;
use score_log::fmt as score_fmt;
use score_log::ScoreDebug;

#[derive(Debug, ScoreDebug, Eq, PartialEq)]
#[repr(u8)]
enum NodeState {
    Free = 0,
    Occupied,
}

/// Fixed-capacity pool of equally-sized elements backed by an allocator implementing [`BasicAllocator`].
///
/// Internal storage is reference counted.
/// Cloning provides a new handle to same data.
pub struct MemoryPool<A: BasicAllocator> {
    /// Allocator.
    /// - Allocates on creation.
    /// - Deallocates on drop.
    allocator: A,

    /// Free list storage - one atomic slot per element, holding a [`NodeState`] discriminant.
    free_list_storage: NonNull<AtomicU8>,
    /// Free list layout.
    free_list_layout: Layout,

    /// Shared reference count storage.
    ref_count_storage: NonNull<AtomicUsize>,
    /// Reference count layout.
    ref_count_layout: Layout,

    /// Internal pool storage.
    pool_storage: NonNull<u8>,
    /// Internal pool layout.
    pool_layout: Layout,

    /// Layout of a single element.
    /// Used for pre-condition checks.
    element_layout: Layout,
    /// Layout of a single element - padded.
    /// Used for actual work.
    element_layout_padded: Layout,
    /// Number of pre-allocated elements.
    capacity: usize,
}

impl<A: BasicAllocator> MemoryPool<A> {
    /// Create new memory pool.
    ///
    /// Internally three allocations are performed:
    /// - free list block
    /// - reference count
    /// - pool storage
    ///
    /// # Parameters
    ///   * `element_layout`: Layout of a single element.
    ///   * `capacity`: Number of elements to be allocated.
    ///   * `allocator`: Memory allocator.
    pub fn new(element_layout: Layout, capacity: usize, allocator: A) -> Result<Self, AllocationError> {
        // Disallow zero size allocation.
        if capacity == 0 || element_layout.size() == 0 {
            return Err(AllocationError::ZeroSizeAllocation);
        }

        // Free list and pool storages have a layout of an array.
        // An array of `[T; N]` has a size of `size_of::<T>() * N` and the same alignment of `T`.
        // https://doc.rust-lang.org/reference/type-layout.html#r-layout.array

        // Get internal free list storage layout - one atomic slot per element.
        let free_list_size = size_of::<AtomicU8>()
            .checked_mul(capacity)
            .ok_or(AllocationError::Internal)?;
        let free_list_align = align_of::<AtomicU8>();
        let free_list_layout =
            Layout::from_size_align(free_list_size, free_list_align).map_err(|_| AllocationError::Internal)?;
        // Allocate internal free list storage.
        let free_list_storage: NonNull<AtomicU8> = allocator.allocate(free_list_layout)?.cast();
        // Initialize every slot to `Free`.
        // Performed element-wise to ensure correct representation of each element.
        for index in 0..capacity {
            // SAFETY: `free_list_storage` is allocated with `capacity` size.
            unsafe {
                free_list_storage
                    .as_ptr()
                    .add(index)
                    .write(AtomicU8::new(NodeState::Free as u8))
            };
        }

        // Allocate and initialize the reference count.
        let ref_count_layout = Layout::new::<AtomicUsize>();
        let ref_count_storage: NonNull<AtomicUsize> = allocator.allocate(ref_count_layout)?.cast();
        // SAFETY: `ref_count_storage` is allocated with same layout as `AtomicUsize`.
        unsafe { ref_count_storage.as_ptr().write(AtomicUsize::new(1)) };

        // Get internal pool storage layout.
        let element_layout_padded = element_layout.pad_to_align();
        let pool_size = element_layout_padded
            .size()
            .checked_mul(capacity)
            .ok_or(AllocationError::Internal)?;
        let pool_align = element_layout_padded.align();
        let pool_layout = Layout::from_size_align(pool_size, pool_align).map_err(|_| AllocationError::Internal)?;
        // Allocate internal pool storage.
        let pool_storage = allocator.allocate(pool_layout)?;

        Ok(Self {
            allocator,
            ref_count_storage,
            ref_count_layout,
            free_list_storage,
            free_list_layout,
            pool_storage,
            pool_layout,
            element_layout,
            element_layout_padded,
            capacity,
        })
    }

    /// Access the shared reference count.
    fn ref_count(&self) -> &AtomicUsize {
        // SAFETY: `ref_count_storage` is initialized in `new`.
        unsafe { self.ref_count_storage.as_ref() }
    }

    /// Access the free-list slot for the given element index.
    fn free_list_slot(&self, index: usize) -> &AtomicU8 {
        assert!(index < self.capacity);
        // SAFETY:
        // `free_list_storage` is initialized in `new` with `capacity` number of slots.
        // `index < capacity` check is performed.
        unsafe { &*self.free_list_storage.as_ptr().add(index) }
    }

    /// Return element capacity of the memory pool.
    pub fn capacity(&self) -> usize {
        self.capacity
    }

    /// Return layout of the element.
    pub fn element_layout(&self) -> Layout {
        self.element_layout
    }
}

impl<A: BasicAllocator> BasicAllocator for MemoryPool<A> {
    /// Allocates a block of memory as described by `layout`.
    /// Provided layout must match the one provided at construction.
    fn allocate(&self, layout: Layout) -> Result<NonNull<u8>, AllocationError> {
        // Check layout is matching.
        // NOTE: zero size allocation is not rechecked here.
        if layout != self.element_layout {
            return Err(AllocationError::InvalidLayout);
        }

        // Scan for a free slot and atomically claim it.
        for index in 0..self.capacity {
            if self
                .free_list_slot(index)
                .compare_exchange(
                    NodeState::Free as u8,
                    NodeState::Occupied as u8,
                    Ordering::Acquire,
                    Ordering::Relaxed,
                )
                .is_ok()
            {
                // Get pointer in pool memory with correct offset.
                let offset = self.element_layout_padded.size() * index;
                // SAFETY: `pool_storage` is initialized in `new` with `capacity` number of slots.
                return Ok(unsafe { self.pool_storage.byte_add(offset) });
            }
        }

        Err(AllocationError::OutOfMemory)
    }

    unsafe fn deallocate(&self, ptr: NonNull<u8>, layout: Layout) {
        // Check layout is matching.
        if layout != self.element_layout {
            panic!("invalid layout provided to deallocation: {:?}", layout);
        }

        // Get index based on offset.
        let offset = ptr.byte_offset_from(self.pool_storage) as usize;
        let index = offset / self.element_layout_padded.size();

        // Set node state to free.
        self.free_list_slot(index)
            .store(NodeState::Free as u8, Ordering::Release);
    }
}

// SAFETY: all shared mutable state is accessed using atomics.
unsafe impl<A: BasicAllocator + Send> Send for MemoryPool<A> {}
unsafe impl<A: BasicAllocator + Sync> Sync for MemoryPool<A> {}

impl<A: BasicAllocator + Clone> Clone for MemoryPool<A> {
    fn clone(&self) -> Self {
        // Increase reference count.
        self.ref_count().fetch_add(1, Ordering::Relaxed);
        Self {
            allocator: self.allocator.clone(),
            ref_count_storage: self.ref_count_storage,
            ref_count_layout: self.ref_count_layout,
            free_list_storage: self.free_list_storage,
            free_list_layout: self.free_list_layout,
            pool_storage: self.pool_storage,
            pool_layout: self.pool_layout,
            element_layout: self.element_layout,
            element_layout_padded: self.element_layout_padded,
            capacity: self.capacity,
        }
    }
}

impl<A: BasicAllocator> Drop for MemoryPool<A> {
    fn drop(&mut self) {
        // Release allocations on last reference dropping.
        if self.ref_count().fetch_sub(1, Ordering::Release) == 1 {
            fence(Ordering::Acquire);
            // SAFETY: deallocation is ensured to be done once.
            unsafe {
                self.allocator
                    .deallocate(self.free_list_storage.cast(), self.free_list_layout);
                self.allocator.deallocate(self.pool_storage, self.pool_layout);
                self.allocator
                    .deallocate(self.ref_count_storage.cast(), self.ref_count_layout);
            }
        }
    }
}

impl<A: BasicAllocator> core_fmt::Debug for MemoryPool<A> {
    fn fmt(&self, f: &mut core_fmt::Formatter<'_>) -> core_fmt::Result {
        f.debug_struct("MemoryPool")
            .field("element_layout", &self.element_layout())
            .field("capacity", &self.capacity())
            .finish()
    }
}

impl<A: BasicAllocator> score_fmt::ScoreDebug for MemoryPool<A> {
    fn fmt(&self, f: score_fmt::Writer, spec: &score_fmt::FormatSpec) -> score_fmt::Result {
        score_fmt::DebugStruct::new(f, spec, "MemoryPool")
            .field("element_layout", &self.element_layout())
            .field("capacity", &self.capacity())
            .finish()
    }
}

#[cfg(all(test, not(loom)))]
mod tests {
    use crate::atomic::Ordering;
    use crate::memory_region::MmapMemoryRegion;
    use crate::{page_size, AllocationError, ArenaAllocator, BasicAllocator, HeapAllocator, MemoryPool};
    use core::alloc::Layout;
    use std::collections::BTreeSet;
    use std::sync::Arc;
    use std::thread;

    #[test]
    fn test_new_ok() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let result = MemoryPool::new(element_layout, capacity, allocator);
        assert!(result.is_ok());
    }

    #[test]
    fn test_new_zero_cap() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0;
        let result = MemoryPool::new(element_layout, capacity, allocator);
        assert!(result.is_err_and(|e| e == AllocationError::ZeroSizeAllocation));
    }

    #[test]
    fn test_new_zero_element_size() {
        let allocator = HeapAllocator;
        let element_layout = Layout::from_size_align(0, 4).unwrap();
        let capacity = 0x20;
        let result = MemoryPool::new(element_layout, capacity, allocator);
        assert!(result.is_err_and(|e| e == AllocationError::ZeroSizeAllocation));
    }

    #[test]
    fn test_new_internal_allocator_oom() {
        let internal_allocator_capacity = page_size();
        let memory_region = MmapMemoryRegion::new(internal_allocator_capacity).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();

        let element_layout = Layout::new::<u32>();
        let capacity = 0x2000;
        let result = MemoryPool::new(element_layout, capacity, allocator);
        assert!(result.is_err_and(|e| e == AllocationError::OutOfMemory));
    }

    #[test]
    fn test_capacity_ok() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();
        assert_eq!(memory_pool.capacity(), capacity);
    }

    #[test]
    fn test_element_layout_ok() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();
        assert_eq!(memory_pool.element_layout(), element_layout);
    }

    #[test]
    fn test_allocate_single() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        let result = memory_pool.allocate(element_layout);
        assert!(result.is_ok());
    }

    #[test]
    fn test_allocate_multiple() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        let first_ptr = memory_pool.allocate(element_layout).unwrap();
        for index in 1..capacity {
            let ptr = memory_pool.allocate(element_layout).unwrap();
            let offset = unsafe { ptr.byte_offset_from(first_ptr) } as usize;
            let calculated_index = offset / element_layout.size();
            assert_eq!(index, calculated_index);
        }
    }

    #[test]
    fn test_allocate_full_capacity() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        // Fill the capacity.
        for _ in 0..capacity {
            let _ = memory_pool.allocate(element_layout);
        }

        // Do one allocation too many.
        let result = memory_pool.allocate(element_layout);
        assert!(result.is_err_and(|e| e == AllocationError::OutOfMemory));
    }

    #[test]
    fn test_allocate_invalid_layout() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        let invalid_layout = Layout::new::<u64>();
        let result = memory_pool.allocate(invalid_layout);
        assert!(result.is_err_and(|e| e == AllocationError::InvalidLayout));
    }

    #[test]
    fn test_deallocate_single() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        let ptr = memory_pool.allocate(element_layout).unwrap();
        unsafe { memory_pool.deallocate(ptr, element_layout) };
    }

    #[test]
    fn test_deallocate_multiple() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        // Allocate.
        let mut allocations = vec![];
        for _ in 0..capacity {
            let ptr = memory_pool.allocate(element_layout).unwrap();
            allocations.push(ptr)
        }

        // Deallocate.
        for ptr in allocations {
            unsafe { memory_pool.deallocate(ptr, element_layout) };
        }
    }

    #[test]
    #[should_panic(expected = "invalid layout provided to deallocation")]
    fn test_deallocate_invalid_layout() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        let ptr = memory_pool.allocate(element_layout).unwrap();
        let invalid_layout = Layout::new::<u64>();
        unsafe { memory_pool.deallocate(ptr, invalid_layout) };
    }

    #[test]
    fn test_deallocate_already_deallocated() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        let ptr = memory_pool.allocate(element_layout).unwrap();
        // Double deallocation does nothing wrong - state is cleared twice.
        unsafe { memory_pool.deallocate(ptr, element_layout) };
        unsafe { memory_pool.deallocate(ptr, element_layout) };
    }

    #[test]
    fn test_allocate_deallocate_middle_element() {
        // Allocate until full, deallocate some element in the middle, try to allocate again.
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();

        // Allocate.
        let mut allocations = vec![];
        for _ in 0..capacity {
            let ptr = memory_pool.allocate(element_layout).unwrap();
            allocations.push(ptr)
        }

        // Take some element in the middle.
        let ptr_middle = allocations[5];
        unsafe { memory_pool.deallocate(ptr_middle, element_layout) };

        // Allocate again and compare pointer.
        let ptr_new = memory_pool.allocate(element_layout).unwrap();

        assert_eq!(ptr_middle, ptr_new);
    }

    #[test]
    fn test_clone_ref_count() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();
        assert_eq!(memory_pool.ref_count().load(Ordering::Relaxed), 1);

        let clone = memory_pool.clone();
        assert_eq!(memory_pool.ref_count().load(Ordering::Relaxed), 2);

        // Dropping the clone decrements the count without releasing the storage.
        drop(clone);
        assert_eq!(memory_pool.ref_count().load(Ordering::Relaxed), 1);
    }

    #[test]
    fn test_clone_shares_pool() {
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();
        let clone = memory_pool.clone();

        // Exhaust the whole capacity through the clone.
        for _ in 0..capacity {
            clone.allocate(element_layout).unwrap();
        }

        // The original observes the exhausted pool - the free list is shared, not copied.
        let result = memory_pool.allocate(element_layout);
        assert!(result.is_err_and(|e| e == AllocationError::OutOfMemory));
    }

    #[test]
    fn test_clone_frees_once() {
        // Create, clone, then drop both handles.
        // The storage must be released exactly once.
        let allocator = HeapAllocator;
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();
        let clone = memory_pool.clone();
        drop(memory_pool);
        drop(clone);
    }

    #[test]
    fn test_clone_with_mmap_allocator() {
        // Nested reference counting.
        // The pool's count and the arena's count must both release cleanly, unmapping the arena exactly once.
        let memory_region = MmapMemoryRegion::new(page_size()).unwrap();
        let allocator = ArenaAllocator::new(memory_region).unwrap();
        let element_layout = Layout::new::<u32>();
        let capacity = 0x20;
        let memory_pool = MemoryPool::new(element_layout, capacity, allocator).unwrap();
        let clone = memory_pool.clone();
        drop(memory_pool);
        drop(clone);
    }

    #[test]
    fn test_concurrent_allocate_distinct_slots() {
        // Perform allocations from multiple threads to check every claimed slot is distinct.
        const THREADS: usize = 8;
        const PER_THREAD: usize = 16;
        let element_layout = Layout::new::<u64>();
        let pool = Arc::new(MemoryPool::new(element_layout, THREADS * PER_THREAD, HeapAllocator).unwrap());

        let handles: Vec<_> = (0..THREADS)
            .map(|_| {
                let pool = pool.clone();
                thread::spawn(move || {
                    (0..PER_THREAD)
                        .map(|_| pool.allocate(element_layout).unwrap().as_ptr() as usize)
                        .collect::<Vec<_>>()
                })
            })
            .collect();

        let mut all = BTreeSet::new();
        for handle in handles {
            for addr in handle.join().unwrap() {
                assert!(all.insert(addr), "slot handed out twice: {addr:#x}");
            }
        }
        assert_eq!(all.len(), THREADS * PER_THREAD);
    }

    #[test]
    fn test_concurrent_allocate_contended_capacity() {
        // Perform more allocations than available slots.
        // Success until capacity is reached, the rest observe `OutOfMemory`.
        const THREADS: usize = 16;
        const CAPACITY: usize = 4;
        let element_layout = Layout::new::<u64>();
        let pool = Arc::new(MemoryPool::new(element_layout, CAPACITY, HeapAllocator).unwrap());

        let handles: Vec<_> = (0..THREADS)
            .map(|_| {
                let pool = pool.clone();
                thread::spawn(move || pool.allocate(element_layout).map(|p| p.as_ptr() as usize).ok())
            })
            .collect();

        let mut claimed = BTreeSet::new();
        for handle in handles {
            if let Some(addr) = handle.join().unwrap() {
                assert!(claimed.insert(addr), "slot handed out twice: {addr:#x}");
            }
        }
        assert_eq!(claimed.len(), CAPACITY);
    }

    #[test]
    fn test_concurrent_clone_drop_single_release() {
        // Perform clones and drops in multiple threads.
        // The original is dropped last.
        let element_layout = Layout::new::<u32>();
        let pool = MemoryPool::new(element_layout, 0x20, HeapAllocator).unwrap();
        let shared = Arc::new(pool.clone());

        let handles: Vec<_> = (0..8)
            .map(|_| {
                let shared = shared.clone();
                thread::spawn(move || {
                    for _ in 0..64 {
                        drop((*shared).clone());
                    }
                })
            })
            .collect();
        for handle in handles {
            handle.join().unwrap();
        }

        drop(Arc::try_unwrap(shared).ok().unwrap());
        assert_eq!(pool.ref_count().load(Ordering::Relaxed), 1);
    }
}

#[cfg(all(test, loom))]
mod loom_tests {
    use crate::{BasicAllocator, HeapAllocator, MemoryPool};
    use core::alloc::Layout;
    use loom::thread;

    #[test]
    fn loom_concurrent_allocate_distinct() {
        // Perform two concurrent allocations on a shared pool.
        loom::model(|| {
            let element_layout = Layout::new::<u64>();
            let pool = MemoryPool::new(element_layout, 2, HeapAllocator).unwrap();

            let p1 = pool.clone();
            let p2 = pool.clone();
            let h1 = thread::spawn(move || p1.allocate(element_layout).unwrap().as_ptr() as usize);
            let h2 = thread::spawn(move || p2.allocate(element_layout).unwrap().as_ptr() as usize);

            let a = h1.join().unwrap();
            let b = h2.join().unwrap();
            assert_ne!(a, b);
        });
    }

    #[test]
    fn loom_concurrent_clone_drop() {
        // Perform two concurrent drops on a shared handle.
        loom::model(|| {
            let element_layout = Layout::new::<u32>();
            let pool = MemoryPool::new(element_layout, 2, HeapAllocator).unwrap();

            let p1 = pool.clone();
            let p2 = pool.clone();
            let h1 = thread::spawn(move || drop(p1));
            let h2 = thread::spawn(move || drop(p2));
            h1.join().unwrap();
            h2.join().unwrap();
        });
    }
}
