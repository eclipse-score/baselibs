/*******************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

//! Rust-level unit tests for the `memres` crate.
//!
//! These tests exercise the Rust API (`MemoryResource`, `ResourceKind`,
//! `PoolOptions`) and verify the end-to-end path through the C++ FFI layer.
//!
//! The hermetic abort scenario (second fill in hermetic mode → `std::abort()`)
//! cannot be tested directly from a Rust `#[test]` because `std::abort()` is
//! an OS-level process termination that cannot be caught by the Rust test
//! harness.  That scenario is covered by the C++ `EXPECT_DEATH` tests in
//! `memres_test.cpp`.

use memres::{MemoryResource, PoolOptions, ResourceKind};
use std::ops::Deref;

// ---------------------------------------------------------------------------
// C++ fill helpers (re-declared here so the test binary can link them)
// ---------------------------------------------------------------------------

mod helpers {
    use core::marker::PhantomData;
    use memres::ScoreMemoryResource;

    /// Opaque handle corresponding to `MemresExampleVec` on the C++ side.
    #[repr(C)]
    pub struct ExampleVec {
        _data: [u8; 0],
        _marker: PhantomData<(*mut u8, core::marker::PhantomPinned)>,
    }

    #[link(name = "example_helpers")]
    unsafe extern "C" {
        pub fn memres_example_bytes_needed(count: usize) -> usize;
        pub fn memres_example_fill(mr_ptr: *mut ScoreMemoryResource, count: usize) -> *mut ExampleVec;
        pub fn memres_example_vec_destroy(vec_handle: *mut ExampleVec);
        pub fn memres_example_vec_data(vec_handle: *const ExampleVec) -> *const i32;
        pub fn memres_example_vec_len(vec_handle: *const ExampleVec) -> usize;
    }
}

/// RAII wrapper around the opaque C++ PMR-backed `vector<int32_t>` handle.
struct FilledVec(*mut helpers::ExampleVec);

impl FilledVec {
    fn new(mr: &MemoryResource, count: usize) -> Self {
        let handle = unsafe { helpers::memres_example_fill(mr.as_memory_resource_ptr().as_ptr(), count) };
        assert!(!handle.is_null());
        Self(handle)
    }
}

impl Deref for FilledVec {
    type Target = [i32];
    fn deref(&self) -> &Self::Target {
        unsafe {
            let data = helpers::memres_example_vec_data(self.0);
            let len = helpers::memres_example_vec_len(self.0);
            std::slice::from_raw_parts(data, len)
        }
    }
}

impl Drop for FilledVec {
    fn drop(&mut self) {
        unsafe { helpers::memres_example_vec_destroy(self.0) }
    }
}

// ---------------------------------------------------------------------------
// Handle creation and pointer validity
// ---------------------------------------------------------------------------

#[test]
fn monotonic_resource_ptr_is_non_null() {
    let mr = MemoryResource::new(ResourceKind::Monotonic { size: 1024 }, false);
    assert!(!mr.as_memory_resource_ptr().as_ptr().is_null());
}

#[test]
fn unsynchronized_pool_resource_ptr_is_non_null() {
    let mr = MemoryResource::new(ResourceKind::UnsynchronizedPool(PoolOptions::default()), false);
    assert!(!mr.as_memory_resource_ptr().as_ptr().is_null());
}

/// Two independently created resources must have distinct backing pointers.
#[test]
fn two_monotonic_resources_have_distinct_ptrs() {
    let mr1 = MemoryResource::new(ResourceKind::Monotonic { size: 256 }, false);
    let mr2 = MemoryResource::new(ResourceKind::Monotonic { size: 256 }, false);
    assert_ne!(
        mr1.as_memory_resource_ptr().as_ptr(),
        mr2.as_memory_resource_ptr().as_ptr()
    );
}

// ---------------------------------------------------------------------------
// PoolOptions default contract
// ---------------------------------------------------------------------------

#[test]
fn pool_options_default_fields_are_zero() {
    let opts = PoolOptions::default();
    assert_eq!(opts.max_blocks_per_chunk, 0);
    assert_eq!(opts.largest_required_pool_block, 0);
}

// ---------------------------------------------------------------------------
// End-to-end fill: values are correct
// ---------------------------------------------------------------------------

#[test]
fn monotonic_first_fill_produces_correct_squares() {
    const COUNT: usize = 10;
    let bytes = unsafe { helpers::memres_example_bytes_needed(COUNT) };
    let mr = MemoryResource::new(ResourceKind::Monotonic { size: bytes }, false);

    let filled = FilledVec::new(&mr, COUNT);
    assert_eq!(filled.len(), COUNT);
    for (i, &val) in filled.iter().enumerate() {
        assert_eq!(val, (i * i) as i32, "element {i}");
    }
}

/// Non-hermetic mode: when the monotonic buffer is exhausted the
/// `GuardedUpstream` warns and falls back to the system heap.  No abort occurs.
#[test]
fn monotonic_non_hermetic_overflow_does_not_abort() {
    const COUNT: usize = 10;
    let bytes = unsafe { helpers::memres_example_bytes_needed(COUNT) };
    let mr = MemoryResource::new(ResourceKind::Monotonic { size: bytes }, false);

    let first = FilledVec::new(&mr, COUNT);
    assert_eq!(first.len(), COUNT);
    drop(first); // exhausts the PMR buffer (monotonic: dealloc is a no-op)

    // Second fill: GuardedUpstream delegates to new_delete_resource() — no abort.
    let second = FilledVec::new(&mr, COUNT);
    assert_eq!(second.len(), COUNT);
    for (i, &val) in second.iter().enumerate() {
        assert_eq!(val, (i * i) as i32, "element {i}");
    }
}
