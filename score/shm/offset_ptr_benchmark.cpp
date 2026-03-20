/********************************************************************************
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
 ********************************************************************************/

/// @file Benchmark measuring OffsetPtr<T> operational overhead on nested container random access.
///
/// Compares four implementations of Vector<Vector<int>> with 1000x1000 elements and 100,000
/// random (i, j) accesses. The random access pattern prevents CPU register caching of resolved
/// OffsetPtr base addresses, exercising the full offset-arithmetic path on every dereference.
///
/// Variants:
///   1. std::vector baseline (raw T*)
///   2. score::memory::shared::Vector — OffsetPtr without bounds checking
///   3. score::memory::shared::Vector — OffsetPtr with bounds checking
///   4. score::shm::Vector — header-inline OffsetPtr (intptr_t-based, no cross-TU barriers)

#include "score/memory/shared/fake/my_bounded_memory_resource.h"
#include "score/memory/shared/new_delete_delegate_resource.h"
#include "score/memory/shared/offset_ptr.h"
#include "score/memory/shared/vector.h"
#include "score/shm/vector.h"

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

namespace
{

constexpr std::size_t kOuterSize = 1000U;
constexpr std::size_t kInnerSize = 1000U;
constexpr std::size_t kAccessCount = 100000U;

// 64 MB — generous for bump allocator that doesn't reclaim on dealloc.
// Total data is ~4 MB, but vector growth without reuse needs headroom.
constexpr std::size_t kBoundedResourceSize = 64U * 1024U * 1024U;

/// Pre-generated random access pattern with fixed seed for reproducible benchmarks.
/// Uniform random (i, j) pairs ensure resolved OffsetPtr base addresses cannot be
/// cached in processor registers across consecutive accesses.
class RandomAccessPattern
{
  public:
    RandomAccessPattern(const std::size_t count, const std::size_t outer_size, const std::size_t inner_size)
    {
        std::mt19937 rng{42U};
        std::uniform_int_distribution<std::size_t> outer_dist{0U, outer_size - 1U};
        std::uniform_int_distribution<std::size_t> inner_dist{0U, inner_size - 1U};
        indices_.reserve(count);
        for (std::size_t i = 0U; i < count; ++i)
        {
            indices_.emplace_back(outer_dist(rng), inner_dist(rng));
        }
    }

    const std::vector<std::pair<std::size_t, std::size_t>>& indices() const noexcept
    {
        return indices_;
    }

  private:
    std::vector<std::pair<std::size_t, std::size_t>> indices_;
};

// Shared access pattern instance — same random sequence for all benchmarks.
const RandomAccessPattern& GetAccessPattern()
{
    static const RandomAccessPattern pattern{kAccessCount, kOuterSize, kInnerSize};
    return pattern;
}

// --- Setup helpers ---

std::vector<std::vector<int>> MakeStdNestedVector()
{
    std::vector<std::vector<int>> data;
    data.reserve(kOuterSize);
    for (std::size_t i = 0U; i < kOuterSize; ++i)
    {
        data.emplace_back(kInnerSize);
        std::iota(data.back().begin(), data.back().end(), 0);
    }
    return data;
}

score::memory::shared::Vector<score::memory::shared::Vector<int>> MakeOffsetPtrNestedVector(
    score::memory::shared::ManagedMemoryResource& resource)
{
    score::memory::shared::Vector<score::memory::shared::Vector<int>> data(resource);
    data.reserve(kOuterSize);
    for (std::size_t i = 0U; i < kOuterSize; ++i)
    {
        data.emplace_back();
        data.back().reserve(kInnerSize);
        data.back().resize(kInnerSize);
        std::iota(data.back().begin(), data.back().end(), 0);
    }
    return data;
}

score::shm::Vector<score::shm::Vector<int>> MakeShmNestedVector()
{
    auto outer = score::shm::Vector<score::shm::Vector<int>>::CreateWithCapacityOrAbort(kOuterSize);
    for (std::size_t i = 0U; i < kOuterSize; ++i)
    {
        auto inner = score::shm::Vector<int>::CreateOrAbort(kInnerSize);
        std::iota(inner.begin(), inner.end(), 0);
        outer.PushBackOrAbort(std::move(inner));
    }
    return outer;
}

// --- Benchmarks ---

/// Baseline: std::vector<std::vector<int>> with raw T* pointers.
void BM_StdVector_RandomAccess(benchmark::State& state)
{
    const auto data = MakeStdNestedVector();
    const auto& pattern = GetAccessPattern();

    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [i, j] : pattern.indices())
        {
            sum += data[i][j];
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_StdVector_RandomAccess);

/// OffsetPtr without bounds checking: NewDeleteDelegateMemoryResource bypasses
/// OffsetPtr bounds check (IsOffsetPtrBoundsCheckBypassingEnabled = true).
/// Measures pure OffsetPtr offset-arithmetic overhead vs raw T*.
void BM_OffsetPtrVector_NoBoundsCheck_RandomAccess(benchmark::State& state)
{
    score::memory::shared::NewDeleteDelegateMemoryResource resource{1U};
    auto data = MakeOffsetPtrNestedVector(resource);
    const auto& pattern = GetAccessPattern();

    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [i, j] : pattern.indices())
        {
            sum += data[i][j];
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_OffsetPtrVector_NoBoundsCheck_RandomAccess);

/// OffsetPtr with bounds checking: MyBoundedMemoryResource enables the full
/// OffsetPtr bounds-check path (IsOffsetPtrBoundsCheckBypassingEnabled = false).
/// Measures combined overhead of offset arithmetic + bounds validation.
void BM_OffsetPtrVector_BoundsChecked_RandomAccess(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto data = MakeOffsetPtrNestedVector(resource);
    const auto& pattern = GetAccessPattern();

    const bool previous = score::memory::shared::EnableOffsetPtrBoundsChecking(true);
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [i, j] : pattern.indices())
        {
            sum += data[i][j];
        }
        benchmark::DoNotOptimize(sum);
    }
    score::memory::shared::EnableOffsetPtrBoundsChecking(previous);
}
BENCHMARK(BM_OffsetPtrVector_BoundsChecked_RandomAccess);

/// score::shm::Vector with inline OffsetPtr — measures the overhead of a
/// header-inline offset pointer without cross-TU barriers or bounds checking.
void BM_ShmVector_RandomAccess(benchmark::State& state)
{
    auto data = MakeShmNestedVector();
    const auto& pattern = GetAccessPattern();

    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [i, j] : pattern.indices())
        {
            sum += data[i][j];
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_ShmVector_RandomAccess);

}  // namespace
