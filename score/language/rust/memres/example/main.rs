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

//! Demonstrates `MemoryResource` in hermetic monotonic mode.
//!
//! Execution sequence:
//!  1. Ask C++ how many bytes one `vector<i32>` with `ELEMENT_COUNT` elements needs.
//!  2. Create a monotonic `MemoryResource` of exactly that size in hermetic mode.
//!  3. Call C++, which heap-allocates a PMR-backed `vector<int32_t>` whose element
//!     buffer lives inside our PMR.  Rust wraps the returned handle in `FilledVec`,
//!     which implements `Deref<Target=[i32]>` and destroys the C++ vector on drop.
//!  4. Print the contents, then drop `FilledVec`.
//!  5. Attempt to fill a second vector using the same (exhausted) resource.
//!     The monotonic buffer does not reclaim deallocated memory, so `GuardedUpstream`
//!     intercepts the overflow allocation, logs a fatal message, and calls
//!     `std::abort()`.  The process terminates here.

use memres::{MemoryResource, ResourceKind};
use std::ops::Deref;

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

/// Owns a heap-allocated C++ PMR-backed `vector<int32_t>`.
/// The element storage lives inside the `MemoryResource` that was passed to
/// `memres_example_fill()`; it is the caller's responsibility to ensure that
/// `MemoryResource` outlives every `FilledVec` derived from it.
struct FilledVec {
    handle: *mut helpers::ExampleVec,
}

impl FilledVec {
    fn new(mr: &MemoryResource, count: usize) -> Self {
        // SAFETY: The raw pointer is valid for the duration of this call;
        // `MemoryResourcePtr<'_>` borrows `mr` and enforces this at compile time.
        let handle = unsafe { helpers::memres_example_fill(mr.as_memory_resource_ptr().as_ptr(), count) };
        assert!(!handle.is_null(), "memres_example_fill: allocation failed");
        Self { handle }
    }
}

impl Deref for FilledVec {
    type Target = [i32];
    fn deref(&self) -> &Self::Target {
        // SAFETY: handle is non-null and we have unique ownership; the
        // underlying memory is valid for our lifetime.
        unsafe {
            let data = helpers::memres_example_vec_data(self.handle);
            let len = helpers::memres_example_vec_len(self.handle);
            std::slice::from_raw_parts(data, len)
        }
    }
}

impl Drop for FilledVec {
    fn drop(&mut self) {
        // SAFETY: handle is non-null and we are the sole owner.
        unsafe { helpers::memres_example_vec_destroy(self.handle) }
    }
}

const ELEMENT_COUNT: usize = 10;

fn main() {
    // Step 1 — query the required buffer size from C++.
    let bytes_needed = unsafe { helpers::memres_example_bytes_needed(ELEMENT_COUNT) };
    println!(
        "Bytes needed for one vector<i32> with {} elements: {}",
        ELEMENT_COUNT, bytes_needed
    );

    // Step 2 — allocate a monotonic buffer of exactly that size in hermetic mode.
    let mr = MemoryResource::new(ResourceKind::Monotonic { size: bytes_needed }, true);

    // Step 3 — C++ heap-allocates the vector; its element buffer lives in `mr`.
    // `FilledVec` is dropped (and the C++ vector destroyed) before `mr` thanks
    // to Rust's LIFO drop order for local variables.
    let filled = FilledVec::new(&mr, ELEMENT_COUNT);
    println!("First fill succeeded. Contents: {:?}", &*filled);
    drop(filled); // explicit drop to make the order clear

    // Step 4 — second fill: the monotonic buffer is exhausted.  GuardedUpstream
    // intercepts the overflow allocation, logs a fatal message, and calls
    // std::abort().
    println!(
        "Attempting a second fill on the exhausted resource (hermetic mode) — \
         the process will now abort:"
    );
    let _aborts = FilledVec::new(&mr, ELEMENT_COUNT);

    // Unreachable: std::abort() was called inside FilledVec::new above.
    println!("UNREACHABLE");
}
