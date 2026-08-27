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

use core::fmt;
use core::marker::PhantomData;
use core::mem::needs_drop;
use core::ops;
use core::ptr;
use score_log::fmt::{FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

use crate::storage::Storage;
use crate::InsufficientCapacity;

#[repr(C)]
pub struct GenericVec<T, S: Storage<T>> {
    len: u32,
    storage: S,
    _marker: PhantomData<T>,
}

impl<T, S: Storage<T>> GenericVec<T, S> {
    /// Creates an empty vector backed by the given storage.
    pub fn new(storage: S) -> Self {
        Self {
            len: 0,
            storage,
            _marker: PhantomData,
        }
    }

    /// Extracts a slice containing the entire vector.
    ///
    /// Equivalent to `&v[..]`.
    pub fn as_slice(&self) -> &[T] {
        unsafe { &*self.storage.subslice(0, self.len) }
    }

    /// Extracts a mutable slice of the entire vector.
    ///
    /// Equivalent to `&mut v[..]`.
    pub fn as_mut_slice(&mut self) -> &mut [T] {
        unsafe { &mut *self.storage.subslice_mut(0, self.len) }
    }

    /// Returns the maximum number of elements the vector can hold.
    pub fn capacity(&self) -> usize {
        self.storage.capacity() as usize
    }

    /// Returns the current number of elements in the vector.
    pub fn len(&self) -> usize {
        self.len as usize
    }

    /// Returns `true` if and only if the vector doesn't contain any elements.
    pub fn is_empty(&self) -> bool {
        self.len == 0
    }

    /// Returns `true` if and only if the vector has reached its capacity.
    pub fn is_full(&self) -> bool {
        self.len() == self.capacity()
    }

    /// Tries to push an element to the back of the vector.
    ///
    /// If the vector has spare capacity, the push succeeds and a reference to that element
    /// is returned; otherwise, `Err(InsufficientCapacity)` is returned.
    pub fn push(&mut self, value: T) -> Result<&mut T, InsufficientCapacity> {
        if self.len < self.storage.capacity() {
            let element = unsafe { self.storage.element_mut(self.len) }.write(value);
            self.len += 1;
            Ok(element)
        } else {
            Err(InsufficientCapacity)
        }
    }

    /// Tries to pop an element from the back of the vector.
    ///
    /// If the vector has at least one element, the pop succeeds; otherwise, `None` is returned.
    pub fn pop(&mut self) -> Option<T> {
        if self.len > 0 {
            let element = unsafe { self.storage.element(self.len - 1).assume_init_read() };
            self.len -= 1;
            Some(element)
        } else {
            None
        }
    }

    /// Clears the vector, removing all values.
    pub fn clear(&mut self) {
        let len = self.len;
        // Mark vector as empty before dropping elements, to prevent double-drop in case there's a panic in drop_in_place
        self.len = 0;
        if needs_drop::<T>() {
            unsafe {
                ptr::drop_in_place(self.storage.subslice_mut(0, len));
            }
        }
    }

    /// Manually sets the length of the vector.
    ///
    /// # Safety
    ///
    /// - `new_len <= self.capacity()` must hold
    /// - if `new_len` is greater than the current length, the elements in the new range must have been initialized
    pub(super) unsafe fn set_len(&mut self, new_len: usize) {
        debug_assert!(new_len <= self.capacity());
        self.len = new_len as u32;
    }
}

impl<T: Copy, S: Storage<T>> GenericVec<T, S> {
    /// Tries to append a copy of the given slice to the end of the vector.
    ///
    /// If the vector has sufficient spare capacity, the operation succeeds and a reference to those elements is returned;
    /// otherwise, `Err(InsufficientCapacity)` is returned.
    pub fn extend_from_slice(&mut self, other: &[T]) -> Result<&mut [T], InsufficientCapacity> {
        let new_len = (self.len as usize)
            .checked_add(other.len())
            .ok_or(InsufficientCapacity)?;
        if new_len <= self.capacity() {
            // No overflow, because new_len <= capacity <= u32::MAX
            let new_len = new_len as u32;
            // SAFETY:
            // - `self.len <= new_len``, because the addition didn't overflow
            // - `new_len <= self.capacity()` as per check above
            let target = unsafe { self.storage.subslice_mut(self.len, new_len) };
            // SAFETY:
            // - `other.as_ptr()` is valid for reads of `other.len()` elements, because it's a valid slice reference
            // - `target` is valid for writes of `other.len()` elements, because we got it from `subslice_mut()`,
            //   and `new_len - self.len == other.len()`
            // - the memory regions don't overlap, because `&mut self` precludes `other: &[T]` from overlapping
            unsafe {
                (target as *mut T).copy_from_nonoverlapping(other.as_ptr(), other.len());
            }
            self.len = new_len;
            // SAFETY: the memory in the `target` slice has now been initialized
            Ok(unsafe { &mut *target })
        } else {
            Err(InsufficientCapacity)
        }
    }
}

impl<T, S: Storage<T>> ops::Deref for GenericVec<T, S> {
    type Target = [T];

    fn deref(&self) -> &Self::Target {
        self.as_slice()
    }
}

impl<T, S: Storage<T>> ops::DerefMut for GenericVec<T, S> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        self.as_mut_slice()
    }
}

impl<T: fmt::Debug, S: Storage<T>> fmt::Debug for GenericVec<T, S> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(self.as_slice(), f)
    }
}

impl<T: ScoreDebug, S: Storage<T>> ScoreDebug for GenericVec<T, S> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(self.as_slice(), f, spec)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::storage::test_utils::TestVec;

    #[test]
    fn push_and_pop() {
        fn run_test(n: usize) {
            let storage = TestVec::new(n);
            let mut vector = GenericVec::new(storage);
            let mut control = vec![];

            let result = vector.pop();
            assert_eq!(result, None);

            for i in 0..n {
                let value = i as i64 * 123 + 456;
                let result = vector.push(value);
                assert_eq!(*result.unwrap(), value);
                control.push(value);
                assert_eq!(vector.as_slice(), control.as_slice());
            }

            let result = vector.push(123456);
            assert!(result.is_err());

            for _ in 0..n {
                let expected = control.pop().unwrap();
                let actual = vector.pop();
                assert_eq!(actual, Some(expected));
            }

            let result = vector.pop();
            assert_eq!(result, None);
        }

        for i in 0..6 {
            run_test(i);
        }
    }

    #[test]
    fn is_full_and_is_empty() {
        fn run_test(n: usize) {
            let storage = TestVec::new(n);
            let mut vector = GenericVec::new(storage);
            assert!(vector.is_empty());

            for i in 0..n {
                assert!(!vector.is_full());
                vector.push(i as i64 * 123 + 456).unwrap();
                assert!(!vector.is_empty());
            }

            assert!(vector.is_full());

            for _ in 0..n {
                assert!(!vector.is_empty());
                vector.pop();
                assert!(!vector.is_full());
            }

            assert!(vector.is_empty());
        }

        for i in 0..6 {
            run_test(i);
        }
    }
}
