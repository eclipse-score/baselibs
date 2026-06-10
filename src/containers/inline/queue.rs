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

use crate::generic::queue::GenericQueue;
use crate::storage::Inline;

/// A fixed-capacity, ABI-compatible queue.
///
/// The queue can hold between 0 and `CAPACITY` elements, and behaves similarly to Rust's `VecDeque`,
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
///     front_index: u32,
///     elements: [T; N],
/// }
/// ```
#[repr(transparent)]
pub struct InlineQueue<T: Copy, const CAPACITY: usize> {
    inner: GenericQueue<T, Inline<T, CAPACITY>>,
}

impl<T: Copy, const CAPACITY: usize> InlineQueue<T, CAPACITY> {
    const CHECK_CAPACITY: () = assert!(0 < CAPACITY && CAPACITY <= u32::MAX as usize);

    /// Creates an empty queue.
    #[must_use]
    pub fn new() -> Self {
        let () = Self::CHECK_CAPACITY;

        let storage = Inline::<T, CAPACITY>::new();
        let inner = GenericQueue::new(storage);
        Self { inner }
    }
}

impl<T: Copy, const CAPACITY: usize> Default for InlineQueue<T, CAPACITY> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T: Copy, const CAPACITY: usize> ops::Deref for InlineQueue<T, CAPACITY> {
    type Target = GenericQueue<T, Inline<T, CAPACITY>>;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<T: Copy, const CAPACITY: usize> ops::DerefMut for InlineQueue<T, CAPACITY> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl<T: Copy + fmt::Debug, const CAPACITY: usize> fmt::Debug for InlineQueue<T, CAPACITY> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(&self.inner, f)
    }
}

impl<T: Copy + ScoreDebug, const CAPACITY: usize> ScoreDebug for InlineQueue<T, CAPACITY> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(&self.inner, f, spec)
    }
}

#[cfg(test)]
mod tests {
    use std::collections::VecDeque;

    use super::*;

    fn to_vec<T: Copy>((first, second): (&[T], &[T])) -> Vec<T> {
        let mut elements = first.to_vec();
        elements.extend_from_slice(second);
        elements
    }

    #[test]
    fn front_and_back() {
        fn check_front_and_back<const N: usize>(queue: &mut InlineQueue<i64, N>, control: &mut VecDeque<i64>) {
            assert_eq!(queue.front(), control.front());
            assert_eq!(queue.front_mut(), control.front_mut());
            assert_eq!(queue.back(), control.back());
            assert_eq!(queue.back_mut(), control.back_mut());
        }

        fn run_test<const N: usize>() {
            let mut queue = InlineQueue::<i64, N>::new();
            let mut control = VecDeque::new();

            // Completely fill and empty the queue n times, but move the internal start point
            // ahead by one each time
            for _ in 0..N {
                check_front_and_back(&mut queue, &mut control);

                for i in 0..N {
                    let value = i as i64 * 123 + 456;
                    queue.push_back(value).unwrap();
                    control.push_back(value);
                    check_front_and_back(&mut queue, &mut control);
                }

                for _ in 0..N {
                    control.pop_front().unwrap();
                    queue.pop_front().unwrap();
                    check_front_and_back(&mut queue, &mut control);
                }

                // One push and one pop to move the internal start point ahead
                queue.push_back(987).unwrap();
                queue.pop_front().unwrap();
                check_front_and_back(&mut queue, &mut control);
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn push_back_and_pop_front() {
        fn run_test<const N: usize>() {
            let mut queue = InlineQueue::<i64, N>::new();
            let mut control = VecDeque::new();

            // Completely fill and empty the queue N times, but move the internal start point
            // ahead by one each time
            for _ in 0..N {
                let result = queue.pop_front();
                assert_eq!(result, None);

                for i in 0..N {
                    let value = i as i64 * 123 + 456;
                    let result = queue.push_back(value);
                    assert_eq!(*result.unwrap(), value);
                    control.push_back(value);
                    assert_eq!(to_vec(queue.as_slices()), to_vec(control.as_slices()));
                }

                let result = queue.push_back(123456);
                assert!(result.is_err());

                for _ in 0..N {
                    let expected = control.pop_front().unwrap();
                    let actual = queue.pop_front();
                    assert_eq!(actual, Some(expected));
                }

                let result = queue.pop_front();
                assert_eq!(result, None);

                // One push and one pop to move the internal start point ahead
                queue.push_back(987).unwrap();
                assert_eq!(queue.pop_front(), Some(987));
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn push_front_and_pop_back() {
        fn run_test<const N: usize>() {
            let mut queue = InlineQueue::<i64, N>::new();
            let mut control = VecDeque::new();

            // Completely fill and empty the queue N times, but move the internal start point
            // ahead by one each time
            for _ in 0..N {
                let result = queue.pop_back();
                assert_eq!(result, None);

                for i in 0..N {
                    let value = i as i64 * 123 + 456;
                    let result = queue.push_front(value);
                    assert_eq!(*result.unwrap(), value);
                    control.push_front(value);
                    assert_eq!(to_vec(queue.as_slices()), to_vec(control.as_slices()));
                }

                let result = queue.push_front(123456);
                assert!(result.is_err());

                for _ in 0..N {
                    let expected = control.pop_back().unwrap();
                    let actual = queue.pop_back();
                    assert_eq!(actual, Some(expected));
                }

                let result = queue.pop_back();
                assert_eq!(result, None);

                // One push and one pop to move the internal start point ahead
                queue.push_front(987).unwrap();
                assert_eq!(queue.pop_back(), Some(987));
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }

    #[test]
    fn is_empty_and_is_full() {
        fn run_test<const N: usize>() {
            let mut queue = InlineQueue::<i64, N>::new();

            // Completely fill and empty the queue N times, but move the internal start point
            // ahead by one each time
            for _ in 0..N {
                assert!(queue.is_empty());

                for i in 0..N {
                    assert!(!queue.is_full());
                    queue.push_back(i as i64 * 123 + 456).unwrap();
                    assert!(!queue.is_empty());
                }

                assert!(queue.is_full());

                for _ in 0..N {
                    assert!(!queue.is_empty());
                    queue.pop_front();
                    assert!(!queue.is_full());
                }

                assert!(queue.is_empty());

                // One push and one pop to move the internal start point ahead
                queue.push_back(987).unwrap();
                assert_eq!(queue.pop_front(), Some(987));
            }
        }

        run_test::<1>();
        run_test::<2>();
        run_test::<3>();
        run_test::<4>();
        run_test::<5>();
    }
}
