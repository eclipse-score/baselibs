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

#ifndef SCORE_LANGUAGE_RUST_MEMRES_CPP_FFI_H
#define SCORE_LANGUAGE_RUST_MEMRES_CPP_FFI_H

#include <cstddef>

// Forward-declare the C++ types exposed through the extern "C" interface so
// that callers do not need to include the full implementation headers.
namespace score::language::rust::memres
{
class MemResHandle;
}
namespace score::cpp::pmr
{
class memory_resource;
}

extern "C" {

/// Creates a `monotonic_buffer_resource` backed by a C++-allocated buffer of
/// `size` bytes. Returns a typed handle that must be freed with
/// `memres_destroy()`. Returns `nullptr` if the initial allocation fails.
score::language::rust::memres::MemResHandle* memres_new_monotonic(std::size_t size, bool hermetic) noexcept;

/// Creates an `unsynchronized_pool_resource` (single-threaded pool allocator).
/// Pass `0` for `max_blocks_per_chunk` or `largest_required_pool_block` to use
/// the implementation defaults. Returns a typed handle that must be freed with
/// `memres_destroy()`. Returns `nullptr` if allocation fails.
score::language::rust::memres::MemResHandle* memres_new_unsynchronized_pool(std::size_t max_blocks_per_chunk,
                                                                            std::size_t largest_required_pool_block,
                                                                            bool hermetic) noexcept;

/// Destroys a handle previously returned by one of the `memres_new_*` functions
/// and frees all associated storage. Passing `nullptr` is safe (no-op).
void memres_destroy(score::language::rust::memres::MemResHandle* handle) noexcept;

/// Returns a pointer to the `score::cpp::pmr::memory_resource` owned by `handle`.
/// The returned pointer is valid for the lifetime of `handle`.
score::cpp::pmr::memory_resource* memres_as_memory_resource(
    score::language::rust::memres::MemResHandle* handle) noexcept;

/// Creates a thin handle wrapping the process-global default memory resource
/// (`score::cpp::pmr::get_default_resource()`).  The handle must be freed with
/// `memres_destroy()`, which releases only the wrapper object; the underlying
/// resource has process lifetime and is not deleted.  Returns `nullptr` if the
/// wrapper allocation fails.
score::language::rust::memres::MemResHandle* memres_new_default() noexcept;

}  // extern "C"

#endif  // SCORE_LANGUAGE_RUST_MEMRES_CPP_FFI_H
