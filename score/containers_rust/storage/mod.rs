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

mod heap;
mod inline;

pub use self::heap::Heap;
pub use self::inline::Inline;

use core::mem::MaybeUninit;

/// Interface to abstract over element storage kinds.
///
/// # Panics
///
/// The methods in this trait are *not* allowed to panic when `cfg(debug_assertions)` is not enabled.
/// Implementors should use `debug_assert!` to check that preconditions are fulfilled.
pub trait Storage<T> {
    /// Returns the allocated capacity.
    fn capacity(&self) -> u32;

    /// Returns a `const` pointer to a specific element, which isn't necessarily initialized.
    ///
    /// # Safety
    ///
    /// `index < self.capacity()` must hold.
    unsafe fn element(&self, index: u32) -> &MaybeUninit<T>;

    /// Returns a `mut` pointer to a specific element, which isn't necessarily initialized.
    ///
    /// # Safety
    ///
    /// `index < self.capacity()` must hold.
    unsafe fn element_mut(&mut self, index: u32) -> &mut MaybeUninit<T>;

    /// Returns a pointer to a subslice of elements, which aren't necessarily initialized.
    ///
    /// # Safety
    ///
    /// `start <= end <= self.capacity()` must hold.
    unsafe fn subslice(&self, start: u32, end: u32) -> *const [T];

    /// Returns a pointer to a mutable subslice of elements, which aren't necessarily initialized.
    ///
    /// # Safety
    ///
    /// `start <= end <= self.capacity()` must hold.
    unsafe fn subslice_mut(&mut self, start: u32, end: u32) -> *mut [T];
}

#[cfg(test)]
pub(crate) mod test_utils {
    //! A simple impl of [`Storage`] for [`Vec`], to be used for tests of generic containers.

    use super::*;
    use core::ptr;

    pub struct TestVec<T>(Vec<MaybeUninit<T>>);

    impl<T> TestVec<T> {
        pub fn new(capacity: usize) -> Self {
            Self::try_new(capacity).unwrap_or_else(|| panic!("failed to allocate for {capacity} elements"))
        }

        pub fn try_new(capacity: usize) -> Option<Self>
        where
            Self: Sized,
        {
            let mut instance = vec![];
            instance.try_reserve_exact(capacity).ok()?;
            instance.extend((0..capacity).map(|_| MaybeUninit::zeroed()));
            Some(Self(instance))
        }
    }

    impl<T> Storage<T> for TestVec<T> {
        fn capacity(&self) -> u32 {
            self.0.capacity() as u32
        }

        unsafe fn element(&self, index: u32) -> &MaybeUninit<T> {
            &self.0[index as usize]
        }

        unsafe fn element_mut(&mut self, index: u32) -> &mut MaybeUninit<T> {
            &mut self.0[index as usize]
        }

        unsafe fn subslice(&self, start: u32, end: u32) -> *const [T] {
            debug_assert!(start <= end);
            debug_assert!(end <= Storage::capacity(self));
            // SAFETY: `start` is in-bounds of the array, as per the pre-condition on the trait method.
            let ptr = unsafe { self.0.as_ptr().add(start as usize).cast() };
            let len = end - start;
            ptr::slice_from_raw_parts(ptr, len as usize)
        }

        unsafe fn subslice_mut(&mut self, start: u32, end: u32) -> *mut [T] {
            debug_assert!(start <= end);
            debug_assert!(end <= Storage::capacity(self));
            // SAFETY: `start` is in-bounds of the array, as per the pre-condition on the trait method.
            let ptr = unsafe { self.0.as_mut_ptr().add(start as usize).cast() };
            let len = end - start;
            ptr::slice_from_raw_parts_mut(ptr, len as usize)
        }
    }
}
