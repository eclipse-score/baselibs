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

use crate::storage::Storage;
use alloc::alloc::Layout;
use core::marker::PhantomData;
use core::mem::MaybeUninit;
use core::ptr;
use core::ptr::NonNull;
use elementary::BasicAllocator;

/// Fixed-capacity, heap-allocated storage.
pub struct Heap<'alloc, T, A: BasicAllocator> {
    /// Allocated capacity, in number of elements.
    capacity: u32,
    /// Pointer to the allocated memory.
    ///
    /// If `self.capacity > 0`, this points to an allocated memory area of size `self.capacity * size_of<T>` and alignment `align_of<T>`.
    elements: NonNull<T>,
    /// Allocator used by the storage.
    alloc: &'alloc A,
    _marker: PhantomData<T>,
}

// SAFETY: `Heap<T>` can be sent to another thread if `T` can be sent to another thread and used allocator is send-safe.
unsafe impl<T: Send, A: BasicAllocator + Send> Send for Heap<'_, T, A> {}

impl<T, A: BasicAllocator> Heap<'_, T, A> {
    fn layout(capacity: u32) -> Option<Layout> {
        (capacity as usize)
            .checked_mul(size_of::<T>())
            .and_then(|size| Layout::from_size_align(size, align_of::<T>()).ok())
    }
}

impl<'alloc, T, A: BasicAllocator> Heap<'alloc, T, A> {
    /// Creates a new instance with capacity for exactly the given number of elements.
    ///
    /// # Panics
    ///
    /// Panics if the memory allocation failed.
    pub fn new(capacity: u32, alloc: &'alloc A) -> Self {
        Self::try_new(capacity, alloc).unwrap_or_else(|| {
            panic!(
                "failed to allocate {capacity} elements of {typ}",
                typ = core::any::type_name::<T>()
            )
        })
    }

    /// Tries to create a new instance with capacity for exactly the given number of elements.
    ///
    /// Returns `None` if the memory allocation failed.
    pub fn try_new(capacity: u32, alloc: &'alloc A) -> Option<Self> {
        let storage = if capacity > 0 {
            let layout = Self::layout(capacity)?;
            // SAFETY: `layout` has a non-zero size (because `capacity` is > 0)
            NonNull::new(alloc.allocate(layout).ok()?.as_ptr())?
        } else {
            NonNull::dangling()
        };
        Some(Self {
            capacity,
            elements: storage.cast::<T>(),
            alloc,
            _marker: PhantomData,
        })
    }
}

impl<T, A: BasicAllocator> Storage<T> for Heap<'_, T, A> {
    fn capacity(&self) -> u32 {
        self.capacity
    }

    unsafe fn element(&self, index: u32) -> &MaybeUninit<T> {
        debug_assert!(index < self.capacity);
        let index = index as usize;
        // SAFETY:
        // - `index` is in-bounds of the memory allocation, as per the pre-condition on the trait method
        // - `MaybeUninit<T>` has the same memory layout as `T`, so the cast is valid
        unsafe { self.elements.add(index).cast::<MaybeUninit<T>>().as_ref() }
    }

    unsafe fn element_mut(&mut self, index: u32) -> &mut MaybeUninit<T> {
        debug_assert!(index < self.capacity);
        let index = index as usize;
        // SAFETY:
        // - `index` is in-bounds of the memory allocation, as per the pre-condition on the trait method
        // - `MaybeUninit<T>` has the same memory layout as `T`, so the cast is valid
        unsafe { self.elements.add(index).cast::<MaybeUninit<T>>().as_mut() }
    }

    unsafe fn subslice(&self, start: u32, end: u32) -> *const [T] {
        let start = start as usize;
        let end = end as usize;
        debug_assert!(start <= end);
        debug_assert!(end <= self.capacity as usize);
        // SAFETY: `start` is in-bounds of the memory allocation, as per the pre-condition on the trait method.
        let ptr = unsafe { self.elements.as_ptr().add(start) };
        ptr::slice_from_raw_parts(ptr, end - start)
    }

    unsafe fn subslice_mut(&mut self, start: u32, end: u32) -> *mut [T] {
        let start = start as usize;
        let end = end as usize;
        debug_assert!(start <= end);
        debug_assert!(end <= self.capacity as usize);
        // SAFETY: `start` is in-bounds of the memory allocation, as per the pre-condition on the trait method.
        let ptr = unsafe { self.elements.as_ptr().add(start) };
        ptr::slice_from_raw_parts_mut(ptr, end - start)
    }
}

impl<T, A: BasicAllocator> Drop for Heap<'_, T, A> {
    fn drop(&mut self) {
        if self.capacity > 0 {
            let layout = Self::layout(self.capacity).unwrap();
            // SAFETY:
            // - `self.elements` has previously been allocated with `alloc`
            // - `layout` is the same as the one used for the allocation
            unsafe {
                let ptr = self.elements.cast();
                self.alloc.deallocate(ptr, layout);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use elementary::{HeapAllocator, GLOBAL_ALLOCATOR};

    #[test]
    fn subslice() {
        type T = u64;

        fn run_test(capacity: u32) {
            let instance = Heap::<T, _>::new(capacity, &GLOBAL_ALLOCATOR);

            let empty_slice = unsafe { instance.subslice(0, 0) };
            assert_eq!(empty_slice.len(), 0);
            assert_eq!(empty_slice as *const T, instance.elements.as_ptr());

            let full_slice = unsafe { instance.subslice(0, capacity) };
            assert_eq!(full_slice.len(), capacity as usize);
            assert_eq!(full_slice as *const T, instance.elements.as_ptr());

            if capacity > 2 {
                let partial_slice = unsafe { instance.subslice(1, 2) };
                assert_eq!(partial_slice.len(), 1);
                assert_eq!(partial_slice as *const T, instance.elements.as_ptr().wrapping_add(1));

                let end_slice = unsafe { instance.subslice(capacity - 1, capacity) };
                assert_eq!(end_slice.len(), 1);
                assert_eq!(
                    end_slice as *const T,
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1)
                );
            }
        }

        for cap in [0, 1, 2, 3, 4, 5, i32::MAX as u32 / size_of::<T>() as u32] {
            run_test(cap);
        }
    }

    #[test]
    fn subslice_mut() {
        type T = u64;

        fn run_test(capacity: u32) {
            let mut instance = Heap::<T, HeapAllocator>::new(capacity, &GLOBAL_ALLOCATOR);

            let empty_slice = unsafe { instance.subslice_mut(0, 0) };
            assert_eq!(empty_slice.len(), 0);
            assert_eq!(empty_slice as *mut T, instance.elements.as_ptr());

            let full_slice = unsafe { instance.subslice_mut(0, capacity) };
            assert_eq!(full_slice.len(), capacity as usize);
            assert_eq!(full_slice as *mut T, instance.elements.as_ptr());

            if capacity >= 2 {
                let partial_slice = unsafe { instance.subslice_mut(1, 2) };
                assert_eq!(partial_slice.len(), 1);
                assert_eq!(partial_slice as *mut T, instance.elements.as_ptr().wrapping_add(1));

                let end_slice = unsafe { instance.subslice_mut(capacity - 1, capacity) };
                assert_eq!(end_slice.len(), 1);
                assert_eq!(
                    end_slice as *mut T,
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1)
                );
            }
        }

        for cap in [0, 1, 2, 3, 4, 5, i32::MAX as u32 / size_of::<T>() as u32] {
            run_test(cap);
        }
    }

    #[test]
    fn element() {
        type T = u64;

        fn run_test(capacity: u32) {
            let instance = Heap::<T, HeapAllocator>::new(capacity, &GLOBAL_ALLOCATOR);

            if capacity >= 1 {
                let first_element = unsafe { instance.element(0) };
                assert_eq!(first_element.as_ptr(), instance.elements.as_ptr());

                let last_element = unsafe { instance.element(capacity - 1) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1)
                );
            }

            if capacity >= 2 {
                let second_element = unsafe { instance.element(1) };
                assert_eq!(second_element.as_ptr(), instance.elements.as_ptr().wrapping_add(1));

                let last_element = unsafe { instance.element(capacity - 2) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 2)
                );
            }
        }

        for cap in [0, 1, 2, 3, 4, 5, i32::MAX as u32 / size_of::<T>() as u32] {
            run_test(cap);
        }
    }

    #[test]
    fn element_mut() {
        type T = u64;

        fn run_test(capacity: u32) {
            let mut instance = Heap::<T, HeapAllocator>::new(capacity, &GLOBAL_ALLOCATOR);

            if capacity >= 1 {
                let first_element = unsafe { instance.element_mut(0) };
                assert_eq!(first_element.as_ptr(), instance.elements.as_ptr());

                let last_element = unsafe { instance.element_mut(capacity - 1) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1)
                );
            }

            if capacity >= 2 {
                let second_element = unsafe { instance.element_mut(1) };
                assert_eq!(second_element.as_ptr(), instance.elements.as_ptr().wrapping_add(1));

                let last_element = unsafe { instance.element_mut(capacity - 2) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 2)
                );
            }
        }

        for cap in [0, 1, 2, 3, 4, 5, i32::MAX as u32 / size_of::<T>() as u32] {
            run_test(cap);
        }
    }
}
