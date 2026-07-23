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

#ifndef SCORE_LANGUAGE_RUST_MEMRES_EXAMPLE_EXAMPLE_HELPERS_H
#define SCORE_LANGUAGE_RUST_MEMRES_EXAMPLE_EXAMPLE_HELPERS_H

#include <cstddef>
#include <cstdint>

// Forward declarations for types used in the extern "C" interface.
namespace score::cpp::pmr
{
class memory_resource;
}

/// Opaque handle for the PMR-backed `vector<int32_t>` allocated by
/// `memres_example_fill()`.  The concrete layout is private to the
/// implementation.
struct MemresExampleVec;

extern "C" {

/// Returns the number of bytes that `memres_example_fill()` will request from
/// the PMR when called with `count` elements.  The caller uses this to size the
/// monotonic buffer that is passed as `mr_ptr`.
///
/// This is necessary because the element layout is determined by C++ (including
/// any padding/alignment that `std::pmr::vector` may add).
std::size_t memres_example_bytes_needed(std::size_t count) noexcept;

/// Heap-allocates a PMR-backed `vector<int32_t>` (via `std::make_unique`), uses
/// `mr_ptr` as the element allocator, reserves `count` slots, and fills the
/// vector with the squares of 0..count-1.
///
/// The vector *object* lives on the heap (allocated by `make_unique`).
/// The element *storage* is allocated from `mr_ptr` (the PMR).
///
/// Returns a typed handle.  The caller owns the vector and must destroy it
/// exactly once with `memres_example_vec_destroy()`.
MemresExampleVec* memres_example_fill(score::cpp::pmr::memory_resource* mr_ptr, std::size_t count) noexcept;

/// Destroys the vector handle returned by `memres_example_fill()`.
/// Passing `nullptr` is safe (no-op).
void memres_example_vec_destroy(MemresExampleVec* vec_handle) noexcept;

/// Returns a pointer to the first element of the vector.
/// Valid only while the handle is alive.
const std::int32_t* memres_example_vec_data(const MemresExampleVec* vec_handle) noexcept;

/// Returns the number of elements in the vector.
std::size_t memres_example_vec_len(const MemresExampleVec* vec_handle) noexcept;

}  // extern "C"

#endif  // SCORE_LANGUAGE_RUST_MEMRES_EXAMPLE_EXAMPLE_HELPERS_H
