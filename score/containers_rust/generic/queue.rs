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
use core::iter::FusedIterator;
use core::marker::PhantomData;
use core::mem::needs_drop;
use core::ops::Range;
use core::ptr;
use core::slice;
use score_log::fmt::{DebugList, FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

use crate::storage::Storage;
use crate::InsufficientCapacity;

#[repr(C)]
pub struct GenericQueue<T, S: Storage<T>> {
    /// The current number of elements in the queue.
    len: u32,
    /// The index of the next element to be returned by [`pop_front()`](Self::pop_front);
    /// this points to a valid element if and only if `self.len > 0`.
    front_index: u32,
    storage: S,
    _marker: PhantomData<T>,
}

impl<T, S: Storage<T>> GenericQueue<T, S> {
    /// Creates an empty queue backed by the given storage.
    pub fn new(storage: S) -> Self {
        Self {
            len: 0,
            front_index: 0,
            storage,
            _marker: PhantomData,
        }
    }

    /// Extracts the slices containing the entire queue contents, in order.
    ///
    /// The caller should not make any assumptions about the distribution of the elements between
    /// the two slices, except for ordering.
    /// In particular, the first slice might be empty even though the second isn't.
    ///
    /// # Example
    ///
    /// ```ignore
    /// let (first, second) = queue.as_slices();
    /// let elements: Vec<_> = std::iter::chain(first, second).collect();
    /// println!("Elements in queue: {elements:?}");
    /// ```
    pub fn as_slices(&self) -> (&[T], &[T]) {
        let (first, second) = self.slice_ranges();
        let first = unsafe { &*self.storage.subslice(first.start, first.end) };
        let second = unsafe { &*self.storage.subslice(second.start, second.end) };
        (first, second)
    }

    /// Extracts the slices containing the entire queue contents, in order.
    ///
    /// The caller should not make any assumptions about the distribution of the elements between
    /// the two slices, except for ordering.
    /// In particular, the first slice might be empty even though the second isn't.
    ///
    /// # Example
    ///
    /// ```ignore
    /// let (first, second) = queue.as_mut_slices();
    /// for elements in std::iter::chain(first, second) {
    ///     *element *= 2;
    /// }
    /// ```
    pub fn as_mut_slices(&mut self) -> (&mut [T], &mut [T]) {
        let (first, second) = self.slice_ranges();
        let first = unsafe { &mut *self.storage.subslice_mut(first.start, first.end) };
        let second = unsafe { &mut *self.storage.subslice_mut(second.start, second.end) };
        (first, second)
    }

    /// Returns a reference to the front of the queue (the element which would be returned by [`pop_front()`](Self::pop_front)),
    /// or None if the queue is empty.
    pub fn front(&self) -> Option<&T> {
        if self.len > 0 {
            // SAFETY: self.len > 0, therefore self.front_index points to a valid (initialized) slot in the storage
            Some(unsafe { self.storage.element(self.front_index).assume_init_ref() })
        } else {
            None
        }
    }

    /// Returns a mutable reference to the front of the queue (the element which would be returned by [`pop_front()`](Self::pop_front)),
    /// or None if the queue is empty.
    pub fn front_mut(&mut self) -> Option<&mut T> {
        if self.len > 0 {
            // SAFETY: self.len > 0, therefore self.front_index points to a valid (initialized) slot in the storage
            Some(unsafe { self.storage.element_mut(self.front_index).assume_init_mut() })
        } else {
            None
        }
    }

    /// Returns a reference to the back of the queue (the element which would be returned by [`pop_back()`](Self::pop_back)),
    /// or None if the queue is empty.
    pub fn back(&self) -> Option<&T> {
        self.back_index().map(|back_index| {
            // SAFETY: self.back_index() returned Some(), therefore back_index points to a valid (initialized) slot in the storage
            unsafe { self.storage.element(back_index).assume_init_ref() }
        })
    }

    /// Returns a mutable reference to the back of the queue (the element which would be returned by [`pop_back()`](Self::pop_back)),
    /// or None if the queue is empty.
    pub fn back_mut(&mut self) -> Option<&mut T> {
        self.back_index().map(|back_index| {
            // SAFETY: self.back_index() returned Some(), therefore back_index points to a valid (initialized) slot in the storage
            unsafe { self.storage.element_mut(back_index).assume_init_mut() }
        })
    }

    /// Returns a front-to-back iterator over the elements.
    pub fn iter(&self) -> Iter<'_, T> {
        let (first, second) = self.as_slices();
        Iter {
            first: first.iter(),
            second: second.iter(),
        }
    }

    /// Returns a front-to-back iterator over the mutable elements.
    pub fn iter_mut(&mut self) -> IterMut<'_, T> {
        let (first, second) = self.as_mut_slices();
        IterMut {
            first: first.iter_mut(),
            second: second.iter_mut(),
        }
    }

    /// Returns the maximum number of elements the queue can hold.
    pub fn capacity(&self) -> usize {
        self.storage.capacity() as usize
    }

    /// Returns the current number of elements in the queue.
    pub fn len(&self) -> usize {
        self.len as usize
    }

    /// Returns `true` if and only if the queue doesn't contain any elements.
    pub fn is_empty(&self) -> bool {
        self.len == 0
    }

    /// Returns `true` if and only if the queue has reached its capacity.
    pub fn is_full(&self) -> bool {
        self.len() == self.capacity()
    }

    /// Tries to push an element to the back of the queue.
    ///
    /// If the queue has spare capacity, the push succeeds and a reference to that element
    /// is returned; otherwise, `Err(InsufficientCapacity)` is returned.
    pub fn push_back(&mut self, value: T) -> Result<&mut T, InsufficientCapacity> {
        let capacity = self.storage.capacity();
        if self.len < capacity {
            let write_pos = self.front_index as u64 + self.len as u64;
            let write_pos = if write_pos < capacity as u64 {
                write_pos as u32
            } else {
                (write_pos - capacity as u64) as u32
            };
            self.len += 1;
            Ok(unsafe { self.storage.element_mut(write_pos).write(value) })
        } else {
            Err(InsufficientCapacity)
        }
    }

    /// Tries to push an element to the front of the queue.
    ///
    /// If the queue has spare capacity, the push succeeds and a reference to that element
    /// is returned; otherwise, `Err(InsufficientCapacity)` is returned.
    pub fn push_front(&mut self, value: T) -> Result<&mut T, InsufficientCapacity> {
        let capacity = self.storage.capacity();
        if self.len < capacity {
            let write_pos = if self.front_index > 0 {
                self.front_index - 1
            } else {
                capacity - 1
            };
            let element = unsafe { self.storage.element_mut(write_pos).write(value) };
            self.len += 1;
            self.front_index = write_pos;
            Ok(element)
        } else {
            Err(InsufficientCapacity)
        }
    }

    /// Tries to pop an element from the front of the queue.
    ///
    /// If the queue has at least one element, the pop succeeds; otherwise, `None` is returned.
    pub fn pop_front(&mut self) -> Option<T> {
        if self.len > 0 {
            let element = unsafe { self.storage.element(self.front_index).assume_init_read() };
            self.len -= 1;
            if self.front_index < self.storage.capacity() - 1 {
                self.front_index += 1;
            } else {
                self.front_index = 0;
            }
            Some(element)
        } else {
            None
        }
    }

    /// Tries to pop an element from the back of the queue.
    ///
    /// If the queue has at least one element, the pop succeeds; otherwise, `None` is returned.
    pub fn pop_back(&mut self) -> Option<T> {
        if let Some(read_pos) = self.back_index() {
            self.len -= 1;
            Some(unsafe { self.storage.element(read_pos).assume_init_read() })
        } else {
            None
        }
    }

    /// Clears the queue, removing all values.
    pub fn clear(&mut self) {
        let (first, second) = self.slice_ranges();
        // Mark queue as empty before dropping elements, to prevent double-drop in case there's a panic in drop_in_place
        self.len = 0;
        self.front_index = 0;
        if needs_drop::<T>() {
            unsafe {
                ptr::drop_in_place(self.storage.subslice_mut(first.start, first.end));
                ptr::drop_in_place(self.storage.subslice_mut(second.start, second.end));
            }
        }
    }

    /// Computes the bounds of the two slices containing the queue's contents, in order.
    fn slice_ranges(&self) -> (Range<u32>, Range<u32>) {
        // Cast to u64 to avoid overflow
        let end = self.front_index as u64 + self.len as u64;
        let capacity = self.storage.capacity();
        if end > capacity as u64 {
            let end = (end - capacity as u64) as u32;
            (self.front_index..capacity, 0..end)
        } else {
            let end = end as u32;
            (self.front_index..end, end..end)
        }
    }

    /// Returns the index of the last element (the one which would be returned by [`pop_back()`](Self::pop_back)),
    /// or `None` if the queue is empty.
    fn back_index(&self) -> Option<u32> {
        if self.len > 0 {
            let capacity = self.storage.capacity() as u64;
            let read_pos = self.front_index as u64 + (self.len as u64 - 1);
            if read_pos < capacity {
                Some(read_pos as u32)
            } else {
                Some((read_pos - capacity) as u32)
            }
        } else {
            None
        }
    }
}

impl<T: fmt::Debug, S: Storage<T>> fmt::Debug for GenericQueue<T, S> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_list().entries(self.iter()).finish()
    }
}

impl<T: ScoreDebug, S: Storage<T>> ScoreDebug for GenericQueue<T, S> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        DebugList::new(f, spec).entries(self.iter()).finish()
    }
}

pub struct Iter<'a, T> {
    first: slice::Iter<'a, T>,
    second: slice::Iter<'a, T>,
}

// Manually implement Clone, because auto-derive would limit it to T: Clone
impl<T> Clone for Iter<'_, T> {
    fn clone(&self) -> Self {
        Self {
            first: self.first.clone(),
            second: self.second.clone(),
        }
    }
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        self.first.next().or_else(|| {
            // When one slice iterator is done, swap them and continue with the other iterator.
            // This works repeatedly, because slice::Iter is fused.
            core::mem::swap(&mut self.first, &mut self.second);
            self.first.next()
        })
    }

    fn last(mut self) -> Option<Self::Item> {
        self.next_back()
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        let len = self.len();
        (len, Some(len))
    }
}

impl<'a, T> DoubleEndedIterator for Iter<'a, T> {
    fn next_back(&mut self) -> Option<Self::Item> {
        self.second.next_back().or_else(|| {
            // When one slice iterator is done, swap them and continue with the other iterator.
            // This works repeatedly, because slice::Iter is fused.
            core::mem::swap(&mut self.first, &mut self.second);
            self.second.next_back()
        })
    }
}

impl<'a, T> ExactSizeIterator for Iter<'a, T> {
    fn len(&self) -> usize {
        self.first.len() + self.second.len()
    }
}

impl<T> FusedIterator for Iter<'_, T> {}

pub struct IterMut<'a, T> {
    first: slice::IterMut<'a, T>,
    second: slice::IterMut<'a, T>,
}

impl<'a, T> Iterator for IterMut<'a, T> {
    type Item = &'a mut T;

    fn next(&mut self) -> Option<Self::Item> {
        self.first.next().or_else(|| {
            // When one slice iterator is done, swap them and continue with the other iterator.
            // This works repeatedly, because slice::IterMut is fused.
            core::mem::swap(&mut self.first, &mut self.second);
            self.first.next()
        })
    }

    fn last(mut self) -> Option<Self::Item> {
        self.next_back()
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        let len = self.len();
        (len, Some(len))
    }
}

impl<'a, T> DoubleEndedIterator for IterMut<'a, T> {
    fn next_back(&mut self) -> Option<Self::Item> {
        self.second.next_back().or_else(|| {
            // When one slice iterator is done, swap them and continue with the other iterator.
            // This works repeatedly, because slice::IterMut is fused.
            core::mem::swap(&mut self.first, &mut self.second);
            self.second.next_back()
        })
    }
}

impl<'a, T> ExactSizeIterator for IterMut<'a, T> {
    fn len(&self) -> usize {
        self.first.len() + self.second.len()
    }
}

impl<T> FusedIterator for IterMut<'_, T> {}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::storage::test_utils::TestVec;
    use std::collections::VecDeque;

    fn to_vec<T: Copy>((first, second): (&[T], &[T])) -> Vec<T> {
        let mut elements = first.to_vec();
        elements.extend_from_slice(second);
        elements
    }

    #[test]
    fn front_and_back() {
        fn check_front_and_back(queue: &mut GenericQueue<i64, TestVec<i64>>, control: &mut VecDeque<i64>) {
            assert_eq!(queue.front(), control.front());
            assert_eq!(queue.front_mut(), control.front_mut());
            assert_eq!(queue.back(), control.back());
            assert_eq!(queue.back_mut(), control.back_mut());
        }

        fn run_test(n: usize) {
            let storage = TestVec::new(n);
            let mut queue = GenericQueue::new(storage);
            let mut control = VecDeque::new();

            // Completely fill and empty the queue n times, but move the internal start point
            // ahead by one each time
            for _ in 0..n {
                check_front_and_back(&mut queue, &mut control);

                for i in 0..n {
                    let value = i as i64 * 123 + 456;
                    queue.push_back(value).unwrap();
                    control.push_back(value);
                    check_front_and_back(&mut queue, &mut control);
                }

                for _ in 0..n {
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

        for i in 0..6 {
            run_test(i);
        }
    }

    #[test]
    fn iter() {
        fn check_iter(queue: &mut GenericQueue<i64, TestVec<i64>>, control: &mut VecDeque<i64>) {
            // Test the Iterator::next() implementation:
            assert_eq!(queue.iter().collect::<Vec<_>>(), control.iter().collect::<Vec<_>>());
            assert_eq!(
                queue.iter_mut().collect::<Vec<_>>(),
                control.iter_mut().collect::<Vec<_>>(),
            );
            // Test the DoubleEndedIterator::next_back() implementation:
            assert_eq!(
                queue.iter().rev().collect::<Vec<_>>(),
                control.iter().rev().collect::<Vec<_>>(),
            );
            assert_eq!(
                queue.iter_mut().rev().collect::<Vec<_>>(),
                control.iter_mut().rev().collect::<Vec<_>>(),
            );
        }

        fn run_test(n: usize) {
            let storage = TestVec::new(n);
            let mut queue = GenericQueue::new(storage);
            let mut control = VecDeque::new();

            // Completely fill and empty the queue n times, but move the internal start point
            // ahead by one each time
            for _ in 0..n {
                check_iter(&mut queue, &mut control);

                for i in 0..n {
                    let value = i as i64 * 123 + 456;
                    queue.push_back(value).unwrap();
                    control.push_back(value);
                    check_iter(&mut queue, &mut control);
                }

                for _ in 0..n {
                    control.pop_front().unwrap();
                    queue.pop_front().unwrap();
                    check_iter(&mut queue, &mut control);
                }

                // One push and one pop to move the internal start point ahead
                queue.push_back(987).unwrap();
                queue.pop_front().unwrap();
                check_iter(&mut queue, &mut control);
            }
        }

        for i in 0..6 {
            run_test(i);
        }
    }

    #[test]
    fn push_back_and_pop_front() {
        fn run_test(n: usize) {
            let storage = TestVec::new(n);
            let mut queue = GenericQueue::new(storage);
            let mut control = VecDeque::new();

            // Completely fill and empty the queue n times, but move the internal start point
            // ahead by one each time
            for _ in 0..n {
                let result = queue.pop_front();
                assert_eq!(result, None);

                for i in 0..n {
                    let value = i as i64 * 123 + 456;
                    let result = queue.push_back(value);
                    assert_eq!(*result.unwrap(), value);
                    control.push_back(value);
                    assert_eq!(to_vec(queue.as_slices()), to_vec(control.as_slices()));
                }

                let result = queue.push_back(123456);
                assert!(result.is_err());

                for _ in 0..n {
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

        for i in 0..6 {
            run_test(i);
        }
    }

    #[test]
    fn push_front_and_pop_back() {
        fn run_test(n: usize) {
            let storage = TestVec::new(n);
            let mut queue = GenericQueue::new(storage);
            let mut control = VecDeque::new();

            // Completely fill and empty the queue n times, but move the internal start point
            // ahead by one each time
            for _ in 0..n {
                let result = queue.pop_back();
                assert_eq!(result, None);

                for i in 0..n {
                    let value = i as i64 * 123 + 456;
                    let result = queue.push_front(value);
                    assert_eq!(*result.unwrap(), value);
                    control.push_front(value);
                    assert_eq!(to_vec(queue.as_slices()), to_vec(control.as_slices()));
                }

                let result = queue.push_front(123456);
                assert!(result.is_err());

                for _ in 0..n {
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

        for i in 0..6 {
            run_test(i);
        }
    }

    #[test]
    fn is_empty_and_is_full() {
        fn run_test(n: usize) {
            let storage = TestVec::new(n);
            let mut queue = GenericQueue::new(storage);

            // Completely fill and empty the queue n times, but move the internal start point
            // ahead by one each time
            for _ in 0..n {
                assert!(queue.is_empty());

                for i in 0..n {
                    assert!(!queue.is_full());
                    queue.push_back(i as i64 * 123 + 456).unwrap();
                    assert!(!queue.is_empty());
                }

                assert!(queue.is_full());

                for _ in 0..n {
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

        for i in 0..6 {
            run_test(i);
        }
    }
}
