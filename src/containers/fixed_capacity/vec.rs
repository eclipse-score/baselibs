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

use crate::generic::vec::GenericVec;
use crate::storage::Heap;
use core::fmt;
use core::ops;
use elementary::{BasicAllocator, HeapAllocator, GLOBAL_ALLOCATOR};
use score_log::fmt::{FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

/// A fixed-capacity vector, using provided allocator.
///
/// The vector can hold between 0 and `capacity` elements, and behaves similarly to Rust's `Vec`,
/// except that it allocates memory immediately on construction, and can't shrink or grow.
pub struct FixedCapacityVecIn<'alloc, T, A: BasicAllocator> {
    inner: GenericVec<T, Heap<'alloc, T, A>>,
}

impl<'alloc, T, A: BasicAllocator> FixedCapacityVecIn<'alloc, T, A> {
    /// Creates an empty vector and allocates memory for up to `capacity` elements, where `capacity <= u32::MAX`.
    ///
    /// # Panics
    ///
    /// - Panics if `capacity > u32::MAX`.
    /// - Panics if the memory allocation fails.
    #[must_use]
    pub fn new(capacity: usize, alloc: &'alloc A) -> Self {
        assert!(
            capacity <= u32::MAX as usize,
            "FixedCapacityVec can hold at most u32::MAX elements"
        );

        let storage = Heap::new(capacity as u32, alloc);
        let inner = GenericVec::new(storage);
        Self { inner }
    }

    /// Tries to create an empty vector for up to `capacity` elements, where `capacity <= u32::MAX`.
    ///
    /// Returns `None` if `capacity > u32::MAX`, or if the memory allocation fails.
    #[must_use]
    pub fn try_new(capacity: usize, alloc: &'alloc A) -> Option<Self> {
        if capacity <= u32::MAX as usize {
            let storage = Heap::try_new(capacity as u32, alloc)?;
            let inner = GenericVec::new(storage);
            Some(Self { inner })
        } else {
            None
        }
    }
}

impl<T, A: BasicAllocator> Drop for FixedCapacityVecIn<'_, T, A> {
    fn drop(&mut self) {
        self.inner.clear();
    }
}

impl<'alloc, T, A: BasicAllocator> ops::Deref for FixedCapacityVecIn<'alloc, T, A> {
    type Target = GenericVec<T, Heap<'alloc, T, A>>;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<T, A: BasicAllocator> ops::DerefMut for FixedCapacityVecIn<'_, T, A> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl<T: fmt::Debug, A: BasicAllocator> fmt::Debug for FixedCapacityVecIn<'_, T, A> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(self.as_slice(), f)
    }
}

impl<T: ScoreDebug, A: BasicAllocator> ScoreDebug for FixedCapacityVecIn<'_, T, A> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(self.as_slice(), f, spec)
    }
}

/// A fixed-capacity vector, using global allocator.
/// Refer to [`FixedCapacityVecIn`] for more information.
pub struct FixedCapacityVec<T>(FixedCapacityVecIn<'static, T, HeapAllocator>);

impl<T> FixedCapacityVec<T> {
    /// Creates an empty vector and allocates memory for up to `capacity` elements, where `capacity <= u32::MAX`.
    ///
    /// # Panics
    ///
    /// - Panics if `capacity > u32::MAX`.
    /// - Panics if the memory allocation fails.
    #[must_use]
    pub fn new(capacity: usize) -> Self {
        Self(FixedCapacityVecIn::new(capacity, &GLOBAL_ALLOCATOR))
    }

    /// Tries to create an empty vector for up to `capacity` elements, where `capacity <= u32::MAX`.
    ///
    /// Returns `None` if `capacity > u32::MAX`, or if the memory allocation fails.
    #[must_use]
    pub fn try_new(capacity: usize) -> Option<Self> {
        let inner = FixedCapacityVecIn::try_new(capacity, &GLOBAL_ALLOCATOR)?;
        Some(Self(inner))
    }
}

impl<T> ops::Deref for FixedCapacityVec<T> {
    type Target = GenericVec<T, Heap<'static, T, HeapAllocator>>;

    fn deref(&self) -> &Self::Target {
        &self.0.inner
    }
}

impl<T> ops::DerefMut for FixedCapacityVec<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.0.inner
    }
}

impl<T: fmt::Debug> fmt::Debug for FixedCapacityVec<T> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(self.0.as_slice(), f)
    }
}

impl<T: ScoreDebug> ScoreDebug for FixedCapacityVec<T> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(&self.0, f, spec)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn push_and_pop() {
        fn run_test(n: usize) {
            let mut vector = FixedCapacityVec::<i64>::new(n);
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
            let mut vector = FixedCapacityVec::<i64>::new(n);
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
