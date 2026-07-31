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

//! Tests that the inline building blocks correctly support non-`Copy` (movable, droppable) payloads
//! and arbitrary nesting, without leaking or double-dropping their contents.

use core::mem::{align_of, size_of};
use std::cell::Cell;
use std::rc::Rc;

use crate::inline::{InlineOption, InlineQueue, InlineResult, InlineString, InlineVec};

/// A non-`Copy` payload that tracks how many live instances exist and how many times a value has been
/// dropped, so tests can detect both leaks (live count doesn't return to zero) and double-drops
/// (drop count exceeds the number of created values).
struct Tracked {
    alive: Rc<Cell<i64>>,
    drops: Rc<Cell<usize>>,
    value: u32,
}

impl Tracked {
    fn new(counters: &Counters, value: u32) -> Self {
        counters.alive.set(counters.alive.get() + 1);
        Self {
            alive: Rc::clone(&counters.alive),
            drops: Rc::clone(&counters.drops),
            value,
        }
    }
}

impl Clone for Tracked {
    fn clone(&self) -> Self {
        self.alive.set(self.alive.get() + 1);
        Self {
            alive: Rc::clone(&self.alive),
            drops: Rc::clone(&self.drops),
            value: self.value,
        }
    }
}

impl Drop for Tracked {
    fn drop(&mut self) {
        self.alive.set(self.alive.get() - 1);
        self.drops.set(self.drops.get() + 1);
    }
}

#[derive(Clone, Default)]
struct Counters {
    alive: Rc<Cell<i64>>,
    drops: Rc<Cell<usize>>,
}

impl Counters {
    fn new() -> Self {
        Self::default()
    }

    fn make(&self, value: u32) -> Tracked {
        Tracked::new(self, value)
    }
}

#[test]
fn inline_vec_drops_all_elements() {
    let counters = Counters::new();
    {
        let mut vec = InlineVec::<Tracked, 4>::new();
        for i in 0..4 {
            vec.push(counters.make(i)).unwrap();
        }
        assert_eq!(counters.alive.get(), 4);
        assert_eq!(counters.drops.get(), 0);
    }
    // Dropping the vector must drop every contained element exactly once.
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 4);
}

#[test]
fn inline_vec_clear_and_pop_are_drop_safe() {
    let counters = Counters::new();
    let mut vec = InlineVec::<Tracked, 4>::new();
    for i in 0..3 {
        vec.push(counters.make(i)).unwrap();
    }

    // `pop` moves the value out; it is dropped only when the returned value goes out of scope.
    let popped = vec.pop().unwrap();
    assert_eq!(popped.value, 2);
    assert_eq!(counters.drops.get(), 0);
    drop(popped);
    assert_eq!(counters.drops.get(), 1);

    // `clear` drops the remaining elements exactly once.
    vec.clear();
    assert_eq!(counters.drops.get(), 3);
    assert_eq!(counters.alive.get(), 0);

    // Dropping the now-empty vector must not drop anything again.
    drop(vec);
    assert_eq!(counters.drops.get(), 3);
}

#[test]
fn inline_queue_drops_all_elements() {
    let counters = Counters::new();
    {
        let mut queue = InlineQueue::<Tracked, 4>::new();
        queue.push_back(counters.make(0)).unwrap();
        queue.push_back(counters.make(1)).unwrap();
        queue.push_front(counters.make(2)).unwrap();
        assert_eq!(counters.alive.get(), 3);
    }
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 3);
}

#[test]
fn inline_option_is_drop_safe() {
    let counters = Counters::new();

    // Some drops its payload.
    {
        let _some = InlineOption::some(counters.make(1));
        assert_eq!(counters.alive.get(), 1);
    }
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 1);

    // None has nothing to drop.
    let none = InlineOption::<Tracked>::none();
    drop(none);
    assert_eq!(counters.drops.get(), 1);
}

#[test]
fn inline_option_into_option_does_not_double_drop() {
    let counters = Counters::new();
    let some = InlineOption::some(counters.make(7));
    let recovered = some.into_option();
    assert_eq!(recovered.as_ref().unwrap().value, 7);
    // The value has been moved out exactly once, so it is still alive and not double-dropped.
    assert_eq!(counters.alive.get(), 1);
    assert_eq!(counters.drops.get(), 0);
    drop(recovered);
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 1);
}

#[test]
fn inline_option_clone_tracks_ownership() {
    let counters = Counters::new();
    let original = InlineOption::some(counters.make(3));
    let cloned = original.clone();
    assert_eq!(counters.alive.get(), 2);
    drop(original);
    drop(cloned);
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 2);
}

#[test]
fn inline_result_is_drop_safe() {
    let counters = Counters::new();

    {
        let _ok = InlineResult::<Tracked, Tracked>::ok(counters.make(1));
        let _err = InlineResult::<Tracked, Tracked>::err(counters.make(2));
        assert_eq!(counters.alive.get(), 2);
    }
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 2);
}

#[test]
fn inline_result_into_result_does_not_double_drop() {
    let counters = Counters::new();

    let ok = InlineResult::<Tracked, Tracked>::ok(counters.make(5));
    let recovered = ok.into_result();
    assert_eq!(recovered.as_ref().ok().unwrap().value, 5);
    assert_eq!(counters.alive.get(), 1);
    assert_eq!(counters.drops.get(), 0);
    drop(recovered);
    assert_eq!(counters.drops.get(), 1);

    let err = InlineResult::<Tracked, Tracked>::err(counters.make(6));
    let recovered = err.into_result();
    assert_eq!(recovered.as_ref().err().unwrap().value, 6);
    drop(recovered);
    assert_eq!(counters.drops.get(), 2);
    assert_eq!(counters.alive.get(), 0);
}

#[test]
fn inline_result_clone_tracks_ownership() {
    let counters = Counters::new();
    let original = InlineResult::<Tracked, Tracked>::ok(counters.make(9));
    let cloned = original.clone();
    assert_eq!(counters.alive.get(), 2);
    drop(original);
    drop(cloned);
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 2);
}

#[test]
fn nested_vec_of_strings() {
    let mut vec = InlineVec::<InlineString<8>, 4>::new();
    for word in ["ab", "cde", "fghi"] {
        let mut string = InlineString::<8>::new();
        string.push_str(word).unwrap();
        vec.push(string).unwrap();
    }

    assert_eq!(vec.len(), 3);
    assert_eq!(vec[0].as_str(), "ab");
    assert_eq!(vec[1].as_str(), "cde");
    assert_eq!(vec[2].as_str(), "fghi");
}

#[test]
fn nested_vec_of_droppable_options_is_drop_safe() {
    let counters = Counters::new();
    {
        let mut vec = InlineVec::<InlineOption<Tracked>, 4>::new();
        vec.push(InlineOption::some(counters.make(0))).unwrap();
        vec.push(InlineOption::none()).unwrap();
        vec.push(InlineOption::some(counters.make(1))).unwrap();
        assert_eq!(counters.alive.get(), 2);
    }
    // Dropping the outer vector must recursively drop the inner options' payloads.
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 2);
}

#[test]
fn nested_option_of_vec_is_drop_safe() {
    let counters = Counters::new();
    {
        let mut inner = InlineVec::<Tracked, 4>::new();
        inner.push(counters.make(0)).unwrap();
        inner.push(counters.make(1)).unwrap();
        let _outer = InlineOption::some(inner);
        assert_eq!(counters.alive.get(), 2);
    }
    assert_eq!(counters.alive.get(), 0);
    assert_eq!(counters.drops.get(), 2);
}

#[test]
fn nested_types_have_stable_repr_c_layout() {
    // `InlineString<8>` is `{ len: u32, [u8; 8] }` => 12 bytes, align 4.
    assert_eq!(size_of::<InlineString<8>>(), 12);
    assert_eq!(align_of::<InlineString<8>>(), 4);

    // `InlineVec<InlineString<8>, 4>` is `{ len: u32, [InlineString<8>; 4] }` => 4 + 4*12 = 52 bytes.
    assert_eq!(size_of::<InlineVec<InlineString<8>, 4>>(), 52);
    assert_eq!(align_of::<InlineVec<InlineString<8>, 4>>(), 4);

    // `InlineOption<u64>` is `{ MaybeUninit<u64>, bool }` => 8 + 1 padded to 16 bytes, align 8.
    assert_eq!(size_of::<InlineOption<u64>>(), 16);
    assert_eq!(align_of::<InlineOption<u64>>(), 8);
}
