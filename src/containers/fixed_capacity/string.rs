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

use crate::generic::string::GenericString;
use crate::storage::Heap;
use core::fmt;
use core::ops;
use elementary::{BasicAllocator, HeapAllocator, GLOBAL_ALLOCATOR};
use score_log::fmt::{FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

/// A fixed-capacity Unicode string, using provided allocator..
///
/// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
///
/// The string can hold between 0 and `CAPACITY` **bytes**, and behaves similarly to Rust's `String`,
/// except that it allocates memory immediately on construction, and can't shrink or grow.
pub struct FixedCapacityStringIn<'alloc, A: BasicAllocator> {
    inner: GenericString<Heap<'alloc, u8, A>>,
}

impl<'alloc, A: BasicAllocator> FixedCapacityStringIn<'alloc, A> {
    /// Creates an empty string and allocates memory for up to `capacity` bytes, where `capacity <= u32::MAX`.
    ///
    /// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
    ///
    /// # Panics
    ///
    /// - Panics if `capacity > u32::MAX`.
    /// - Panics if the memory allocation fails.
    #[must_use]
    pub fn new(capacity: usize, alloc: &'alloc A) -> Self {
        assert!(
            capacity <= u32::MAX as usize,
            "FixedCapacityString can hold at most u32::MAX bytes"
        );

        let storage = Heap::new(capacity as u32, alloc);
        let inner = GenericString::new(storage);
        Self { inner }
    }

    /// Tries to create an empty string for up to `capacity` bytes, where `capacity <= u32::MAX`.
    ///
    /// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
    ///
    /// Returns `None` if `capacity > u32::MAX`, or if the memory allocation fails.
    #[must_use]
    pub fn try_new(capacity: usize, alloc: &'alloc A) -> Option<Self> {
        if capacity <= u32::MAX as usize {
            let storage = Heap::try_new(capacity as u32, alloc)?;
            let inner = GenericString::new(storage);
            Some(Self { inner })
        } else {
            None
        }
    }
}

impl<A: BasicAllocator> Drop for FixedCapacityStringIn<'_, A> {
    fn drop(&mut self) {
        self.inner.clear();
    }
}

impl<'alloc, A: BasicAllocator> ops::Deref for FixedCapacityStringIn<'alloc, A> {
    type Target = GenericString<Heap<'alloc, u8, A>>;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<A: BasicAllocator> ops::DerefMut for FixedCapacityStringIn<'_, A> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl<A: BasicAllocator> fmt::Display for FixedCapacityStringIn<'_, A> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(self.as_str(), f)
    }
}

impl<A: BasicAllocator> fmt::Debug for FixedCapacityStringIn<'_, A> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(self.as_str(), f)
    }
}

impl<A: BasicAllocator> ScoreDebug for FixedCapacityStringIn<'_, A> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(self.as_str(), f, spec)
    }
}

/// A fixed-capacity Unicode string, using global allocator.
/// Refer to [`FixedCapacityStringIn`] for more information.
pub struct FixedCapacityString(FixedCapacityStringIn<'static, HeapAllocator>);

impl FixedCapacityString {
    /// Creates an empty string and allocates memory for up to `capacity` bytes, where `capacity <= u32::MAX`.
    ///
    /// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
    ///
    /// # Panics
    ///
    /// - Panics if `capacity > u32::MAX`.
    /// - Panics if the memory allocation fails.
    #[must_use]
    pub fn new(capacity: usize) -> Self {
        Self(FixedCapacityStringIn::new(capacity, &GLOBAL_ALLOCATOR))
    }

    /// Tries to create an empty string for up to `capacity` bytes, where `capacity <= u32::MAX`.
    ///
    /// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
    ///
    /// Returns `None` if `capacity > u32::MAX`, or if the memory allocation fails.
    #[must_use]
    pub fn try_new(capacity: usize) -> Option<Self> {
        let inner = FixedCapacityStringIn::try_new(capacity, &GLOBAL_ALLOCATOR)?;
        Some(Self(inner))
    }
}

impl ops::Deref for FixedCapacityString {
    type Target = GenericString<Heap<'static, u8, HeapAllocator>>;

    fn deref(&self) -> &Self::Target {
        &self.0.inner
    }
}

impl ops::DerefMut for FixedCapacityString {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.0.inner
    }
}

impl fmt::Display for FixedCapacityString {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(self.0.as_str(), f)
    }
}

impl fmt::Debug for FixedCapacityString {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(&self.0, f)
    }
}

impl ScoreDebug for FixedCapacityString {
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
            let mut string = FixedCapacityString::new(n);
            let mut control = String::new();

            let result = string.pop();
            assert_eq!(result, None);

            let sample = "abcdefghi";
            for ch in sample.chars().take(n) {
                let result = string.push(ch);
                assert!(result.is_ok());
                control.push(ch);
                assert_eq!(string.as_str(), control.as_str());
            }

            let result = string.push('x');
            assert!(result.is_err());

            for _ in 0..n {
                let expected = control.pop().unwrap();
                let actual = string.pop();
                assert_eq!(actual, Some(expected));
            }

            let result = string.pop();
            assert_eq!(result, None);
        }

        for i in 0..6 {
            run_test(i);
        }
    }

    #[test]
    fn push_str() {
        fn run_test(n: usize) {
            let mut string = FixedCapacityString::new(n);
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

        for i in [0, 1, 5, 20, 30] {
            run_test(i);
        }
    }

    #[test]
    fn is_full_and_is_empty() {
        fn run_test(n: usize) {
            let mut string = FixedCapacityString::new(n);
            assert!(string.is_empty());

            let sample = "abcdefghi";
            for ch in sample.chars().take(n) {
                assert!(!string.is_full());
                string.push(ch).unwrap();
                assert!(!string.is_empty());
            }

            assert!(string.is_full());

            for _ in 0..n {
                assert!(!string.is_empty());
                string.pop();
                assert!(!string.is_full());
            }

            assert!(string.is_empty());
        }

        for i in 0..6 {
            run_test(i);
        }
    }
}
