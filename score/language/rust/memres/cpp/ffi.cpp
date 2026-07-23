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

#include "score/language/rust/memres/cpp/ffi.h"

#include "score/language/rust/memres/cpp/memory_resource_handle.h"

#include <score/memory_resource.hpp>
#include <cstddef>
#include <new>

extern "C" {

score::language::rust::memres::MemResHandle* memres_new_monotonic(std::size_t size, bool hermetic) noexcept
{
    return new (std::nothrow) score::language::rust::memres::MonotonicHandle{size, hermetic};
}

score::language::rust::memres::MemResHandle* memres_new_unsynchronized_pool(std::size_t max_blocks_per_chunk,
                                                                            std::size_t largest_required_pool_block,
                                                                            bool hermetic) noexcept
{
    const score::cpp::pmr::pool_options opts{max_blocks_per_chunk, largest_required_pool_block};
    return new (std::nothrow) score::language::rust::memres::UnsynchronizedPoolHandle{opts, hermetic};
}

void memres_destroy(score::language::rust::memres::MemResHandle* handle) noexcept
{
    delete handle;
}

score::language::rust::memres::MemResHandle* memres_new_default() noexcept
{
    return new (std::nothrow) score::language::rust::memres::DefaultHandle{};
}

score::cpp::pmr::memory_resource* memres_as_memory_resource(
    score::language::rust::memres::MemResHandle* handle) noexcept
{
    return handle->resource();
}

}  // extern "C"
