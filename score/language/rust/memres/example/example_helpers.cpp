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

#include "score/language/rust/memres/example/example_helpers.h"

#include <score/memory_resource.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace
{
using ScorePmrVec = std::vector<std::int32_t, score::cpp::pmr::polymorphic_allocator<std::int32_t>>;
}  // namespace

/// Concrete definition of the opaque handle type declared in the header.
struct MemresExampleVec
{
    ScorePmrVec vec;
    explicit MemresExampleVec(score::cpp::pmr::memory_resource* mr)
        : vec{score::cpp::pmr::polymorphic_allocator<std::int32_t>{mr}}
    {
    }
};

extern "C" {

std::size_t memres_example_bytes_needed(std::size_t count) noexcept
{
    // One contiguous allocation of `count` int32_t values after reserve().
    return sizeof(std::int32_t) * count;
}

MemresExampleVec* memres_example_fill(score::cpp::pmr::memory_resource* mr_ptr, std::size_t count) noexcept
{
    // The vector *object* lives on the heap (managed by make_unique).
    // The element *storage* is requested from `mr_ptr` (the PMR).
    auto handle = std::make_unique<MemresExampleVec>(mr_ptr);
    handle->vec.reserve(count);
    for (std::size_t i = 0U; i < count; ++i)
    {
        handle->vec.push_back(static_cast<std::int32_t>(i * i));
    }
    return handle.release();  // ownership transferred to caller
}

void memres_example_vec_destroy(MemresExampleVec* vec_handle) noexcept
{
    delete vec_handle;
}

const std::int32_t* memres_example_vec_data(const MemresExampleVec* vec_handle) noexcept
{
    return vec_handle->vec.data();
}

std::size_t memres_example_vec_len(const MemresExampleVec* vec_handle) noexcept
{
    return vec_handle->vec.size();
}

}  // extern "C"
