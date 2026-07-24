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

#include <score/memory_resource.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "gtest/gtest.h"

namespace
{

constexpr std::size_t kElementCount{10U};

/// Number of bytes consumed by one PMR-backed `vector<int32_t>` with
/// `kElementCount` elements when `reserve()` is called before any push_back.
/// This is exactly the allocation requested from the memory resource.
constexpr std::size_t kBufferBytes{sizeof(std::int32_t) * kElementCount};

using ScorePmrVec = std::vector<std::int32_t, score::cpp::pmr::polymorphic_allocator<std::int32_t>>;

/// Allocates a score::cpp PMR-backed `vector<int32_t>` from the memory resource
/// owned by `handle`, reserves `kElementCount` slots, and fills it with the
/// squares of 0..kElementCount-1.
void FillVector(score::language::rust::memres::MemResHandle* handle)
{
    auto* mr = memres_as_memory_resource(handle);
    ScorePmrVec v{score::cpp::pmr::polymorphic_allocator<std::int32_t>{mr}};
    v.reserve(kElementCount);
    for (std::size_t i = 0U; i < kElementCount; ++i)
    {
        v.push_back(static_cast<std::int32_t>(i * i));
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Handle creation
// ---------------------------------------------------------------------------

TEST(MemResMonotonicTest, HandleCreationReturnsNonNull)
{
    score::language::rust::memres::MemResHandle* handle = memres_new_monotonic(kBufferBytes, /*hermetic=*/true);
    ASSERT_NE(handle, nullptr);
    memres_destroy(handle);
}

TEST(MemResUnsynchronizedPoolTest, HandleCreationReturnsNonNull)
{
    score::language::rust::memres::MemResHandle* handle = memres_new_unsynchronized_pool(
        /*max_blocks_per_chunk=*/0, /*largest_required_pool_block=*/0, /*hermetic=*/false);
    ASSERT_NE(handle, nullptr);
    memres_destroy(handle);
}

// ---------------------------------------------------------------------------
// Hermetic monotonic: allocation within capacity succeeds
// ---------------------------------------------------------------------------

TEST(MemResMonotonicHermeticTest, FirstFillSucceeds)
{
    score::language::rust::memres::MemResHandle* handle = memres_new_monotonic(kBufferBytes, /*hermetic=*/true);
    ASSERT_NE(handle, nullptr);
    FillVector(handle);
    memres_destroy(handle);
}

TEST(MemResMonotonicHermeticTest, FirstFillProducesCorrectValues)
{
    score::language::rust::memres::MemResHandle* handle = memres_new_monotonic(kBufferBytes, /*hermetic=*/true);
    ASSERT_NE(handle, nullptr);

    {
        auto* mr = memres_as_memory_resource(handle);
        ScorePmrVec v{score::cpp::pmr::polymorphic_allocator<std::int32_t>{mr}};
        v.reserve(kElementCount);
        for (std::size_t i = 0U; i < kElementCount; ++i)
        {
            v.push_back(static_cast<std::int32_t>(i * i));
        }

        ASSERT_EQ(v.size(), kElementCount);
        for (std::size_t i = 0U; i < kElementCount; ++i)
        {
            EXPECT_EQ(v[i], static_cast<std::int32_t>(i * i));
        }
    }

    memres_destroy(handle);
}

// ---------------------------------------------------------------------------
// Hermetic monotonic: overflow aborts (buffer exhausted, no fallback)
// ---------------------------------------------------------------------------

TEST(MemResMonotonicHermeticTest, SecondFillAbortsDueToExhaustion)
{
    score::language::rust::memres::MemResHandle* handle = memres_new_monotonic(kBufferBytes, /*hermetic=*/true);
    ASSERT_NE(handle, nullptr);

    // The monotonic buffer is now full after the first fill. Monotonic buffer
    // resources do not reclaim deallocated memory (deallocate is a no-op), so a
    // second allocation attempt triggers GuardedUpstream::do_allocate() which
    // logs a fatal message and calls std::abort().
    FillVector(handle);
    EXPECT_DEATH(FillVector(handle), "");

    memres_destroy(handle);
}

// ---------------------------------------------------------------------------
// Non-hermetic monotonic: overflow falls back to the heap (no abort)
// ---------------------------------------------------------------------------

TEST(MemResMonotonicNonHermeticTest, OverflowFallsBackToHeap)
{
    score::language::rust::memres::MemResHandle* handle = memres_new_monotonic(kBufferBytes, /*hermetic=*/false);
    ASSERT_NE(handle, nullptr);

    FillVector(handle);  // served from the PMR buffer
    FillVector(handle);  // buffer exhausted → GuardedUpstream warns and uses new_delete_resource

    memres_destroy(handle);
}

// ---------------------------------------------------------------------------
// Pool resources: basic fill
// ---------------------------------------------------------------------------

TEST(MemResUnsynchronizedPoolTest, FillSucceeds)
{
    score::language::rust::memres::MemResHandle* handle = memres_new_unsynchronized_pool(0, 0, /*hermetic=*/false);
    ASSERT_NE(handle, nullptr);

    {
        auto* mr = memres_as_memory_resource(handle);
        ScorePmrVec v{score::cpp::pmr::polymorphic_allocator<std::int32_t>{mr}};
        v.reserve(kElementCount);
        for (std::size_t i = 0U; i < kElementCount; ++i)
        {
            v.push_back(static_cast<std::int32_t>(i * i));
        }
        ASSERT_EQ(v.size(), kElementCount);
    }  // v is destroyed here while mr is still alive; its destructor calls mr->deallocate safely

    memres_destroy(handle);
}
