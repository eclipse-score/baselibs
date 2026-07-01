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

use crate::generic::string::GenericString;
use crate::storage::Inline;

/// A fixed-capacity Unicode string with inline storage.
///
/// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
///
/// The string can hold between 0 and `CAPACITY` **bytes**, and behaves similarly to Rust's `String`,
/// except that it stores the characters inline and doesn't allocate.
/// `CAPACITY` must be `>= 1` and `<= u32::MAX`.
///
/// This data structure has a stable, well-defined memory layout and satisfies the requirements for
/// [ABI-compatible types](https://eclipse-score.github.io/score/main/features/communication/abi_compatible_data_types/index.html).
/// Its layout is structurally equivalent to:
///
/// ```ignore
/// #[repr(C)]
/// struct String<const N: usize> {
///     len: u32,
///     bytes: [u8; N],
/// }
/// ```
#[repr(transparent)]
pub struct InlineString<const CAPACITY: usize> {
    inner: GenericString<Inline<u8, CAPACITY>>,
}

impl<const CAPACITY: usize> InlineString<CAPACITY> {
    const CHECK_CAPACITY: () = assert!(0 < CAPACITY && CAPACITY <= u32::MAX as usize);

    /// Creates an empty string.
    #[must_use]
    pub fn new() -> Self {
        let () = Self::CHECK_CAPACITY;

        let storage = Inline::<_, CAPACITY>::new();
        let inner = GenericString::new(storage);
        Self { inner }
    }
}

impl<const CAPACITY: usize> Default for InlineString<CAPACITY> {
    fn default() -> Self {
        Self::new()
    }
}

impl<const CAPACITY: usize> ops::Deref for InlineString<CAPACITY> {
    type Target = GenericString<Inline<u8, CAPACITY>>;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<const CAPACITY: usize> ops::DerefMut for InlineString<CAPACITY> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl<const CAPACITY: usize> fmt::Debug for InlineString<CAPACITY> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(self.as_str(), f)
    }
}

impl<const CAPACITY: usize> ScoreDebug for InlineString<CAPACITY> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(self.as_str(), f, spec)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn push_and_pop() {
        fn run_test<const N: usize>() {
            let mut string = InlineString::<N>::new();
            let mut control = String::new();

            let result = string.pop();
            assert_eq!(result, None);

            let sample = "abcdefghi";
            for ch in sample.chars().take(N) {
                let result = string.push(ch);
                assert!(result.is_ok());
                control.push(ch);
                assert_eq!(string.as_str(), control.as_str());
            }

            let result = string.push('x');
            assert!(result.is_err());

            for _ in 0..N {
                let expected = control.pop().unwrap();
                let actual = string.pop();
                assert_eq!(actual, Some(expected));
            }

            let result = string.pop();
            assert_eq!(result, None);
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn push_str() {
        fn run_test<const N: usize>() {
            let mut string = InlineString::<N>::new();
            let mut control = String::new();

            let samples = ["abc", "\0", "😉", "👍🏼🚀", "αβγ"];
            for sample in samples {
                let should_fit = string.capacity() - string.len() >= sample.len();
                let result = string.push_str(sample);
                if should_fit {
                    assert!(result.is_ok());
                    control.push_str(sample);
                } else {
                    assert!(result.is_err());
                }
                assert_eq!(string.as_str(), control.as_str());
                assert_eq!(string.len(), control.len());
                assert_eq!(string.is_empty(), string.is_empty());
                assert_eq!(string.is_full(), string.capacity() - string.len() == 0);
            }
        }

        run_test::<1>();
        run_test::<5>();
        run_test::<20>();
        run_test::<30>();
    }

    #[test]
    fn is_full_and_is_empty() {
        fn run_test<const N: usize>() {
            let mut string = InlineString::<N>::new();
            assert!(string.is_empty());

            let sample = "abcdefghi";
            for ch in sample.chars().take(N) {
                assert!(!string.is_full());
                string.push(ch).unwrap();
                assert!(!string.is_empty());
            }

            assert!(string.is_full());

            for _ in 0..N {
                assert!(!string.is_empty());
                string.pop();
                assert!(!string.is_full());
            }

            assert!(string.is_empty());
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }
}
