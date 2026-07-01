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
use core::ops;
use score_log::fmt::{FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

use crate::generic::vec::GenericVec;
use crate::storage::Inline;

/// A fixed-capacity vector with inline storage.
///
/// The vector can hold between 0 and `CAPACITY` elements, and behaves similarly to Rust's `Vec`,
/// except that it stores the elements inline and doesn't allocate.
/// `CAPACITY` must be `>= 1` and `<= u32::MAX`.
///
/// This data structure has a stable, well-defined memory layout and satisfies the requirements for
/// [ABI-compatible types](https://eclipse-score.github.io/score/main/features/communication/abi_compatible_data_types/index.html).
/// Its layout is structurally equivalent to:
///
/// ```ignore
/// #[repr(C)]
/// struct Vec<T, const N: usize> {
///     len: u32,
///     elements: [T; N],
/// }
/// ```
#[repr(transparent)]
pub struct InlineVec<T: Copy, const CAPACITY: usize> {
    inner: GenericVec<T, Inline<T, CAPACITY>>,
}

impl<T: Copy, const CAPACITY: usize> InlineVec<T, CAPACITY> {
    const CHECK_CAPACITY: () = assert!(0 < CAPACITY && CAPACITY <= u32::MAX as usize);

    /// Creates an empty vector.
    #[must_use]
    pub fn new() -> Self {
        let () = Self::CHECK_CAPACITY;

        let storage = Inline::<T, CAPACITY>::new();
        let inner = GenericVec::new(storage);
        Self { inner }
    }
}

impl<T: Copy, const CAPACITY: usize> Default for InlineVec<T, CAPACITY> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T: Copy, const CAPACITY: usize> ops::Deref for InlineVec<T, CAPACITY> {
    type Target = GenericVec<T, Inline<T, CAPACITY>>;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<T: Copy, const CAPACITY: usize> ops::DerefMut for InlineVec<T, CAPACITY> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl<T: Copy + fmt::Debug, const CAPACITY: usize> fmt::Debug for InlineVec<T, CAPACITY> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(self.as_slice(), f)
    }
}

impl<T: Copy + ScoreDebug, const CAPACITY: usize> ScoreDebug for InlineVec<T, CAPACITY> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(self.as_slice(), f, spec)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn push_and_pop() {
        fn run_test<const N: usize>() {
            let mut vector = InlineVec::<i64, N>::new();
            let mut control = vec![];

            let result = vector.pop();
            assert_eq!(result, None);

            for i in 0..N {
                let value = i as i64 * 123 + 456;
                let result = vector.push(value);
                assert_eq!(*result.unwrap(), value);
                control.push(value);
                assert_eq!(vector.as_slice(), control.as_slice());
            }

            let result = vector.push(123456);
            assert!(result.is_err());

            for _ in 0..N {
                let expected = control.pop().unwrap();
                let actual = vector.pop();
                assert_eq!(actual, Some(expected));
            }

            let result = vector.pop();
            assert_eq!(result, None);
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn is_full_and_is_empty() {
        fn run_test<const N: usize>() {
            let mut vector = InlineVec::<i64, N>::new();
            assert!(vector.is_empty());

            for i in 0..N {
                assert!(!vector.is_full());
                vector.push(i as i64 * 123 + 456).unwrap();
                assert!(!vector.is_empty());
            }

            assert!(vector.is_full());

            for _ in 0..N {
                assert!(!vector.is_empty());
                vector.pop();
                assert!(!vector.is_full());
            }

            assert!(vector.is_empty());
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }
}
