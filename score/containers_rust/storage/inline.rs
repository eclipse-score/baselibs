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

use core::mem::MaybeUninit;
use core::ptr;

use super::Storage;

/// Fixed-capacity, inline storage, suitable for ABI compatible containers.
///
/// `CAPACITY` is in number of elements, not bytes.
/// It must not be zero (for compatibility with C++), and it must be `<= u32::MAX`.
pub struct Inline<T, const CAPACITY: usize> {
    elements: [MaybeUninit<T>; CAPACITY],
}

impl<T, const CAPACITY: usize> Inline<T, CAPACITY> {
    // Compile-time check. This condition _must_ be referenced in every function that depends on it,
    // otherwise it will be removed during monomorphization.
    const CHECK_CAPACITY: () = assert!(0 < CAPACITY && CAPACITY <= (u32::MAX as usize));

    /// Creates a new instance.
    pub fn new() -> Self {
        let () = Self::CHECK_CAPACITY;

        Self {
            elements: [const { MaybeUninit::uninit() }; CAPACITY],
        }
    }
}

impl<T, const CAPACITY: usize> Default for Inline<T, CAPACITY> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T, const CAPACITY: usize> Storage<T> for Inline<T, CAPACITY> {
    fn capacity(&self) -> u32 {
        let () = Self::CHECK_CAPACITY;

        CAPACITY as u32
    }

    unsafe fn element(&self, index: u32) -> &MaybeUninit<T> {
        let () = Self::CHECK_CAPACITY;

        let index = index as usize;
        debug_assert!(index < CAPACITY);
        // SAFETY: `index` is in-bounds of the array, as per the pre-condition on the trait method.
        unsafe { self.elements.get_unchecked(index) }
    }

    unsafe fn element_mut(&mut self, index: u32) -> &mut MaybeUninit<T> {
        let () = Self::CHECK_CAPACITY;

        let index = index as usize;
        debug_assert!(index < CAPACITY);
        // SAFETY: `index` is in-bounds of the array, as per the pre-condition on the trait method.
        unsafe { self.elements.get_unchecked_mut(index) }
    }

    unsafe fn subslice(&self, start: u32, end: u32) -> *const [T] {
        let () = Self::CHECK_CAPACITY;

        let start = start as usize;
        let end = end as usize;
        debug_assert!(start <= end);
        debug_assert!(end <= CAPACITY);
        // SAFETY: `start` is in-bounds of the array, as per the pre-condition on the trait method.
        let ptr = unsafe { self.elements.as_ptr().add(start) };
        ptr::slice_from_raw_parts(ptr.cast::<T>(), end - start)
    }

    unsafe fn subslice_mut(&mut self, start: u32, end: u32) -> *mut [T] {
        let () = Self::CHECK_CAPACITY;

        let start = start as usize;
        let end = end as usize;
        debug_assert!(start <= end);
        debug_assert!(end <= CAPACITY);
        // SAFETY: `start` is in-bounds of the array, as per the pre-condition on the trait method.
        let ptr = unsafe { self.elements.as_mut_ptr().add(start) };
        ptr::slice_from_raw_parts_mut(ptr.cast::<T>(), end - start)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn subslice() {
        type T = u64;

        fn run_test<const N: usize>() {
            let capacity = N as u32;
            let instance = Inline::<T, N>::new();

            let empty_slice = unsafe { instance.subslice(0, 0) };
            assert_eq!(empty_slice.len(), 0);
            assert_eq!(empty_slice as *const T, instance.elements.as_ptr() as *const T);

            let full_slice = unsafe { instance.subslice(0, capacity) };
            assert_eq!(full_slice.len(), capacity as usize);
            assert_eq!(full_slice as *const T, instance.elements.as_ptr() as *const T);

            if capacity > 2 {
                let partial_slice = unsafe { instance.subslice(1, 2) };
                assert_eq!(partial_slice.len(), 1);
                assert_eq!(
                    partial_slice as *const T,
                    instance.elements.as_ptr().wrapping_add(1) as *const T
                );

                let end_slice = unsafe { instance.subslice(capacity - 1, capacity) };
                assert_eq!(end_slice.len(), 1);
                assert_eq!(
                    end_slice as *const T,
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1) as *const T
                );
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn subslice_mut() {
        type T = u64;

        fn run_test<const N: usize>() {
            let capacity = N as u32;
            let mut instance = Inline::<T, N>::new();

            let empty_slice = unsafe { instance.subslice_mut(0, 0) };
            assert_eq!(empty_slice.len(), 0);
            assert_eq!(empty_slice as *mut T, instance.elements.as_ptr() as *mut T);

            let full_slice = unsafe { instance.subslice_mut(0, capacity) };
            assert_eq!(full_slice.len(), capacity as usize);
            assert_eq!(full_slice as *mut T, instance.elements.as_ptr() as *mut T);

            if capacity >= 2 {
                let partial_slice = unsafe { instance.subslice_mut(1, 2) };
                assert_eq!(partial_slice.len(), 1);
                assert_eq!(
                    partial_slice as *mut T,
                    instance.elements.as_ptr().wrapping_add(1) as *mut T
                );

                let end_slice = unsafe { instance.subslice_mut(capacity - 1, capacity) };
                assert_eq!(end_slice.len(), 1);
                assert_eq!(
                    end_slice as *mut T,
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1) as *mut T
                );
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn element() {
        type T = u64;

        fn run_test<const N: usize>() {
            let capacity = N as u32;
            let instance = Inline::<T, N>::new();

            if capacity >= 1 {
                let first_element = unsafe { instance.element(0) };
                assert_eq!(first_element.as_ptr(), instance.elements.as_ptr() as *const T);

                let last_element = unsafe { instance.element(capacity - 1) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1) as *const T,
                );
            }

            if capacity >= 2 {
                let second_element = unsafe { instance.element(1) };
                assert_eq!(
                    second_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(1) as *const T
                );

                let last_element = unsafe { instance.element(capacity - 2) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 2) as *const T,
                );
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn element_mut() {
        type T = u64;

        fn run_test<const N: usize>() {
            let capacity = N as u32;
            let mut instance = Inline::<T, N>::new();

            if capacity >= 1 {
                let first_element = unsafe { instance.element_mut(0) };
                assert_eq!(first_element.as_ptr(), instance.elements.as_ptr() as *mut T);

                let last_element = unsafe { instance.element_mut(capacity - 1) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 1) as *mut T,
                );
            }

            if capacity >= 2 {
                let second_element = unsafe { instance.element_mut(1) };
                assert_eq!(
                    second_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(1) as *mut T
                );

                let last_element = unsafe { instance.element_mut(capacity - 2) };
                assert_eq!(
                    last_element.as_ptr(),
                    instance.elements.as_ptr().wrapping_add(capacity as usize - 2) as *mut T,
                );
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }
}
