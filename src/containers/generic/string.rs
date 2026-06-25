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
use core::str;
use score_log::fmt::{FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

use super::vec::GenericVec;
use crate::storage::Storage;
use crate::InsufficientCapacity;

/// A UTF-8 encoded string which is generic over its storage method.
#[repr(transparent)]
pub struct GenericString<S: Storage<u8>> {
    /// The UTF-8 encoded characters of the string.
    vec: GenericVec<u8, S>,
}

impl<S: Storage<u8>> GenericString<S> {
    /// Creates an empty string backed by the given storage.
    ///
    /// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
    pub fn new(storage: S) -> Self {
        Self {
            vec: GenericVec::new(storage),
        }
    }

    pub fn as_bytes(&self) -> &[u8] {
        self.vec.as_slice()
    }

    /// Extracts a string slice containing the entire string.
    ///
    /// Equivalent to `&v[..]`.
    pub fn as_str(&self) -> &str {
        unsafe { str::from_utf8_unchecked(self.vec.as_slice()) }
    }

    /// Extracts a mutable string slice of the entire string.
    ///
    /// Equivalent to `&mut v[..]`.
    pub fn as_mut_str(&mut self) -> &mut str {
        unsafe { str::from_utf8_unchecked_mut(self.vec.as_mut_slice()) }
    }

    /// Returns the maximum length of the string in bytes.
    ///
    /// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
    pub fn capacity(&self) -> usize {
        self.vec.capacity()
    }

    /// Returns the length of the string in bytes.
    ///
    /// Note that the string is encoded as UTF-8, so each character (Unicode codepoint) requires between 1 and 4 bytes of storage.
    pub fn len(&self) -> usize {
        self.vec.len()
    }

    /// Returns `true` if and only if the string doesn't contain any characters.
    pub fn is_empty(&self) -> bool {
        self.vec.is_empty()
    }

    /// Returns `true` if and only if the string has reached its capacity.
    pub fn is_full(&self) -> bool {
        self.vec.is_full()
    }

    /// Tries to append the given character to the end of the string.
    ///
    /// If the string has sufficient spare capacity, the operation succeeds; otherwise, `Err(InsufficientCapacity)` is returned.
    pub fn push(&mut self, ch: char) -> Result<(), InsufficientCapacity> {
        let mut buffer = [0_u8; 4];
        self.push_str(ch.encode_utf8(&mut buffer))
    }

    /// Tries to append the given string slice to the end of the string.
    ///
    /// If the string has sufficient spare capacity, the operation succeeds; otherwise, `Err(InsufficientCapacity)` is returned.
    pub fn push_str(&mut self, other: &str) -> Result<(), InsufficientCapacity> {
        match self.vec.extend_from_slice(other.as_bytes()) {
            Ok(_) => Ok(()),
            Err(_) => Err(InsufficientCapacity),
        }
    }

    /// Removes the last character from the string and returns it.
    ///
    /// Returns `None` if the string is empty.
    pub fn pop(&mut self) -> Option<char> {
        let ch = self.chars().next_back()?;
        let new_len = self.len() - ch.len_utf8();
        // SAFETY:
        // - This decreases the length of the internal vector, so it doesn't expose any uninitialized bytes.
        // - The string was valid UTF-8 before; we've removed exactly one codepoint, so the rest is still valid UTF-8.
        unsafe {
            self.vec.set_len(new_len);
        }
        Some(ch)
    }

    /// Clears the string, removing all characters.
    pub fn clear(&mut self) {
        self.vec.clear();
    }
}

impl<S: Storage<u8>> ops::Deref for GenericString<S> {
    type Target = str;

    fn deref(&self) -> &Self::Target {
        self.as_str()
    }
}

impl<S: Storage<u8>> ops::DerefMut for GenericString<S> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        self.as_mut_str()
    }
}

impl<S: Storage<u8>> fmt::Debug for GenericString<S> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(self.as_str(), f)
    }
}

impl<S: Storage<u8>> ScoreDebug for GenericString<S> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(self.as_str(), f, spec)
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
            let mut string = GenericString::new(storage);
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
            let storage = TestVec::new(n);
            let mut string = GenericString::new(storage);
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
            let storage = TestVec::new(n);
            let mut string = GenericString::new(storage);
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
