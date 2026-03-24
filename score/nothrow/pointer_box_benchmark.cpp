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

/// @file Benchmark measuring OffsetPtr<T> operational overhead for vector and map scenarios.
///
/// Compares five implementations of Vector<Vector<int>> with 1000x1000 elements and 100,000
/// random (i, j) accesses. The random access pattern prevents CPU register caching of resolved
/// OffsetPtr base addresses, exercising the full offset-arithmetic path on every dereference.
///
/// Variants:
///   1. std::vector baseline (raw T*)
///   2. score::memory::shared::Vector — OffsetPtr without bounds checking
///   3. score::memory::shared::Vector — OffsetPtr with bounds checking
///   4. score::nothrow::Vector with direct-pointer policy
///   5. score::nothrow::Vector with relocatable offset-pointer policy
///   6. (linux only) boost::interprocess::vector/map with boost::interprocess::offset_ptr
///
/// In addition, compares std::map, score::memory::shared::Map and score::nothrow::Map
/// (direct-pointer policy + relocatable policy) for:
///   - random-order build
///   - random key lookup
///   - full in-order iteration

#include "score/memory/shared/fake/my_bounded_memory_resource.h"
#include "score/memory/shared/map.h"
#include "score/memory/shared/offset_ptr.h"
#include "score/memory/shared/vector.h"
#include "score/nothrow/map.h"
#include "score/nothrow/vector.h"

#include <benchmark/benchmark.h>

#if defined(__linux__)
#include <boost/container/scoped_allocator.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
#endif  // __linux__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numeric>
#include <random>
#include <utility>
#include <map>
#include <vector>

namespace
{

constexpr std::size_t kOuterSize = 1000U;
constexpr std::size_t kInnerSize = 1000U;
constexpr std::size_t kAccessCount = 100000U;
constexpr std::size_t kMapElementCount = 10000U;
constexpr std::size_t kMapAccessCount = 100000U;
constexpr std::size_t kMapInternalElementCount = 4096U;
constexpr std::size_t kMapInternalLookupCount = 100000U;
constexpr std::size_t kBoostVectorHeapSize = 64U * 1024U * 1024U;
constexpr std::size_t kBoostMapHeapSize = 64U * 1024U * 1024U;

// 64 MB — generous for bump allocator that doesn't reclaim on dealloc.
// Total data is ~4 MB, but vector growth without reuse needs headroom.
constexpr std::size_t kBoundedResourceSize = 64U * 1024U * 1024U;

class ScopedOffsetPtrBoundsChecking
{
  public:
    explicit ScopedOffsetPtrBoundsChecking(const bool enabled) noexcept
        : previous_{score::memory::shared::EnableOffsetPtrBoundsChecking(enabled)}
    {
    }

    ~ScopedOffsetPtrBoundsChecking()
    {
        score::memory::shared::EnableOffsetPtrBoundsChecking(previous_);
    }

    ScopedOffsetPtrBoundsChecking(const ScopedOffsetPtrBoundsChecking&) = delete;
    ScopedOffsetPtrBoundsChecking& operator=(const ScopedOffsetPtrBoundsChecking&) = delete;

  private:
    bool previous_{};
};

using RawBoxPolicy = score::nothrow::RawBoxPolicy;
using OffsetBoxPolicy = score::nothrow::OffsetBoxPolicy;

using RawBoxInnerVector =
    score::nothrow::Vector<int, score::nothrow::PolymorphicAllocator<int>, RawBoxPolicy>;
using RawBoxNestedVector = score::nothrow::Vector<RawBoxInnerVector,
                                                  score::nothrow::PolymorphicAllocator<RawBoxInnerVector>,
                                                  RawBoxPolicy>;
using OffsetBoxInnerVector =
    score::nothrow::Vector<int, score::nothrow::PolymorphicAllocator<int>, OffsetBoxPolicy>;
using OffsetBoxNestedVector = score::nothrow::Vector<OffsetBoxInnerVector,
                                                     score::nothrow::PolymorphicAllocator<OffsetBoxInnerVector>,
                                                     OffsetBoxPolicy>;

using RawBoxMap = score::nothrow::Map<int,
                                     int,
                                     std::less<int>,
                                     score::nothrow::PolymorphicAllocator<std::pair<const int, int>>,
                                     RawBoxPolicy>;
using OffsetBoxMap = score::nothrow::Map<int,
                                         int,
                                         std::less<int>,
                                         score::nothrow::PolymorphicAllocator<std::pair<const int, int>>,
                                         OffsetBoxPolicy>;

#if defined(__linux__)
template <typename T>
using BoostInterprocessAllocator =
    boost::interprocess::allocator<T, boost::interprocess::managed_heap_memory::segment_manager>;

using BoostInterprocessInnerVector = boost::interprocess::vector<int, BoostInterprocessAllocator<int>>;
using BoostInterprocessNestedVector = boost::interprocess::vector<
    BoostInterprocessInnerVector,
    boost::container::scoped_allocator_adaptor<BoostInterprocessAllocator<BoostInterprocessInnerVector>>>;
using BoostInterprocessMap =
    boost::interprocess::map<int,
                             int,
                             std::less<int>,
                             boost::container::scoped_allocator_adaptor<
                                 BoostInterprocessAllocator<std::pair<const int, int>>>>;
#endif  // __linux__

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

/// Pre-generated random map insertion and lookup sequences with fixed seed for reproducible
/// map benchmark runs.
class RandomMapPattern
{
  public:
    RandomMapPattern(const std::size_t element_count, const std::size_t access_count)
    {
        std::mt19937 rng{1337U};

        std::vector<int> keys(element_count);
        std::vector<int> values(element_count);
        std::iota(keys.begin(), keys.end(), 0);
        std::iota(values.begin(), values.end(), 0);
        std::shuffle(keys.begin(), keys.end(), rng);
        std::shuffle(values.begin(), values.end(), rng);

        entries_.reserve(element_count);
        for (std::size_t index = 0U; index < element_count; ++index)
        {
            entries_.emplace_back(keys[index], values[index]);
        }

        std::uniform_int_distribution<std::size_t> key_dist{0U, element_count - 1U};
        lookup_keys_.reserve(access_count);
        for (std::size_t access = 0U; access < access_count; ++access)
        {
            lookup_keys_.push_back(entries_[key_dist(rng)].first);
        }
    }

    const std::vector<std::pair<int, int>>& entries() const noexcept
    {
        return entries_;
    }

    const std::vector<int>& lookup_keys() const noexcept
    {
        return lookup_keys_;
    }

  private:
    std::vector<std::pair<int, int>> entries_;
    std::vector<int> lookup_keys_;
};

const RandomMapPattern& GetMapPattern()
{
    static const RandomMapPattern pattern{kMapElementCount, kMapAccessCount};
    return pattern;
}

/// Focused pattern for map-internals microbenchmarks (insert/rebalance/find/erase).
class FocusedMapPattern
{
  public:
    FocusedMapPattern(const std::size_t element_count, const std::size_t lookup_count)
    {
        sorted_entries_.reserve(element_count);
        random_entries_.reserve(element_count);
        erase_keys_.reserve(element_count);
        for (std::size_t index = 0U; index < element_count; ++index)
        {
            const int key = static_cast<int>(index);
            const int value = static_cast<int>((index * 17U) + 1U);
            sorted_entries_.emplace_back(key, value);
            random_entries_.emplace_back(key, value);
            erase_keys_.push_back(key);
        }

        std::mt19937 rng{2026U};
        std::shuffle(random_entries_.begin(), random_entries_.end(), rng);
        std::shuffle(erase_keys_.begin(), erase_keys_.end(), rng);

        std::uniform_int_distribution<std::size_t> key_dist{0U, element_count - 1U};
        lookup_keys_.reserve(lookup_count);
        for (std::size_t access = 0U; access < lookup_count; ++access)
        {
            lookup_keys_.push_back(static_cast<int>(key_dist(rng)));
        }
    }

    const std::vector<std::pair<int, int>>& sorted_entries() const noexcept
    {
        return sorted_entries_;
    }

    const std::vector<std::pair<int, int>>& random_entries() const noexcept
    {
        return random_entries_;
    }

    const std::vector<int>& erase_keys() const noexcept
    {
        return erase_keys_;
    }

    const std::vector<int>& lookup_keys() const noexcept
    {
        return lookup_keys_;
    }

  private:
    std::vector<std::pair<int, int>> sorted_entries_;
    std::vector<std::pair<int, int>> random_entries_;
    std::vector<int> erase_keys_;
    std::vector<int> lookup_keys_;
};

const FocusedMapPattern& GetFocusedMapPattern()
{
    static const FocusedMapPattern pattern{kMapInternalElementCount, kMapInternalLookupCount};
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

score::memory::shared::Vector<score::memory::shared::Vector<int>> MakeMemorySharedNestedVector(
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

RawBoxNestedVector MakeRawBoxNestedVector()
{
    auto outer = RawBoxNestedVector::CreateWithCapacityOrAbort(kOuterSize);
    for (std::size_t i = 0U; i < kOuterSize; ++i)
    {
        auto inner = RawBoxInnerVector::CreateOrAbort(kInnerSize);
        std::iota(inner.begin(), inner.end(), 0);
        outer.PushBackOrAbort(std::move(inner));
    }
    return outer;
}

OffsetBoxNestedVector MakeOffsetBoxNestedVector()
{
    auto outer = OffsetBoxNestedVector::CreateWithCapacityOrAbort(kOuterSize);
    for (std::size_t i = 0U; i < kOuterSize; ++i)
    {
        auto inner = OffsetBoxInnerVector::CreateOrAbort(kInnerSize);
        std::iota(inner.begin(), inner.end(), 0);
        outer.PushBackOrAbort(std::move(inner));
    }
    return outer;
}

#if defined(__linux__)
BoostInterprocessNestedVector MakeBoostInterprocessNestedVector(boost::interprocess::managed_heap_memory& heap_memory)
{
    BoostInterprocessNestedVector data{heap_memory.get_segment_manager()};
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
#endif  // __linux__

std::map<int, int> MakeStdMap()
{
    std::map<int, int> data;
    for (const auto& [key, value] : GetMapPattern().entries())
    {
        data.emplace(key, value);
    }
    return data;
}

void PopulateMemorySharedMapFromEntries(score::memory::shared::Map<int, int>& data,
                                        const std::vector<std::pair<int, int>>& entries)
{
    for (const auto& [key, value] : entries)
    {
        data.emplace(key, value);
    }
}

score::memory::shared::Map<int, int>* MakeMemorySharedMapInResource(score::memory::shared::ManagedMemoryResource& resource)
{
    auto* const data = resource.construct<score::memory::shared::Map<int, int>>(resource);
    PopulateMemorySharedMapFromEntries(*data, GetMapPattern().entries());
    return data;
}

score::memory::shared::Map<int, int>* MakeMemorySharedMapInResourceFromEntries(
    score::memory::shared::ManagedMemoryResource& resource, const std::vector<std::pair<int, int>>& entries)
{
    auto* const data = resource.construct<score::memory::shared::Map<int, int>>(resource);
    PopulateMemorySharedMapFromEntries(*data, entries);
    return data;
}

RawBoxMap MakeRawBoxMap()
{
    auto data = RawBoxMap::CreateOrAbort();
    for (const auto& [key, value] : GetMapPattern().entries())
    {
        data.EmplaceOrAbort(key, value);
    }
    return data;
}

OffsetBoxMap MakeOffsetBoxMap()
{
    auto data = OffsetBoxMap::CreateOrAbort();
    for (const auto& [key, value] : GetMapPattern().entries())
    {
        data.EmplaceOrAbort(key, value);
    }
    return data;
}

#if defined(__linux__)
void PopulateBoostInterprocessMap(BoostInterprocessMap& data)
{
    for (const auto& [key, value] : GetMapPattern().entries())
    {
        data.emplace(key, value);
    }
}

BoostInterprocessMap MakeBoostInterprocessMap(boost::interprocess::managed_heap_memory& heap_memory)
{
    BoostInterprocessMap data{heap_memory.get_segment_manager()};
    PopulateBoostInterprocessMap(data);
    return data;
}

BoostInterprocessMap MakeBoostInterprocessMapFromEntries(boost::interprocess::managed_heap_memory& heap_memory,
                                                         const std::vector<std::pair<int, int>>& entries)
{
    BoostInterprocessMap data{heap_memory.get_segment_manager()};
    for (const auto& [key, value] : entries)
    {
        data.emplace(key, value);
    }
    return data;
}
#endif  // __linux__

std::map<int, int> MakeStdMapFromEntries(const std::vector<std::pair<int, int>>& entries)
{
    std::map<int, int> data;
    for (const auto& [key, value] : entries)
    {
        data.emplace(key, value);
    }
    return data;
}

RawBoxMap MakeRawBoxMapFromEntries(const std::vector<std::pair<int, int>>& entries)
{
    auto data = RawBoxMap::CreateOrAbort();
    for (const auto& [key, value] : entries)
    {
        data.EmplaceOrAbort(key, value);
    }
    return data;
}

OffsetBoxMap MakeOffsetBoxMapFromEntries(const std::vector<std::pair<int, int>>& entries)
{
    auto data = OffsetBoxMap::CreateOrAbort();
    for (const auto& [key, value] : entries)
    {
        data.EmplaceOrAbort(key, value);
    }
    return data;
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

/// OffsetPtr without bounds checking.
/// Uses the same managed resource as the bounds-checked variant but disables checks globally.
void BM_MemorySharedVector_NoBoundsCheck_RandomAccess(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto data = MakeMemorySharedNestedVector(resource);
    const auto& pattern = GetAccessPattern();
    const ScopedOffsetPtrBoundsChecking bounds_checking{false};

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

/// OffsetPtr with bounds checking.
/// Uses the same managed resource as the no-bounds variant with checks enabled globally.
/// Measures combined overhead of offset arithmetic + bounds validation.
void BM_MemorySharedVector_BoundsChecked_RandomAccess(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto data = MakeMemorySharedNestedVector(resource);
    const auto& pattern = GetAccessPattern();
    const ScopedOffsetPtrBoundsChecking bounds_checking{true};
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

/// score::nothrow::Vector with direct-pointer policy.
void BM_RawBoxVector_RandomAccess(benchmark::State& state)
{
    auto data = MakeRawBoxNestedVector();
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

/// score::nothrow::Vector with relocatable offset-pointer policy.
void BM_OffsetBoxVector_RandomAccess(benchmark::State& state)
{
    auto data = MakeOffsetBoxNestedVector();
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

#if defined(__linux__)
/// boost::interprocess::vector with boost::interprocess::offset_ptr-backed allocator.
void BM_BoostInterprocessVector_RandomAccess(benchmark::State& state)
{
    boost::interprocess::managed_heap_memory heap_memory{kBoostVectorHeapSize};
    auto data = MakeBoostInterprocessNestedVector(heap_memory);
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
#endif  // __linux__

// --- Map benchmarks ---

/// Baseline: std::map random-order build.
void BM_StdMap_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    for (auto _ : state)
    {
        std::map<int, int> data;
        for (const auto& [key, value] : entries)
        {
            data.emplace(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}

/// score::memory::shared::Map random-order build without bounds checking.
/// Map object is allocated inside the managed region to keep map-internal sentinels in-region.
void BM_MemorySharedMap_NoBoundsCheck_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    const ScopedOffsetPtrBoundsChecking bounds_checking{false};
    for (auto _ : state)
    {
        state.PauseTiming();
        score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
        auto* const data = resource.construct<score::memory::shared::Map<int, int>>(resource);
        state.ResumeTiming();
        for (const auto& [key, value] : entries)
        {
            data->emplace(key, value);
        }
        benchmark::DoNotOptimize(data->size());
        state.PauseTiming();
        resource.destruct(*data);
        state.ResumeTiming();
    }
}

/// score::memory::shared::Map random-order build with bounds checking.
/// Map object is allocated inside the managed region to keep map-internal sentinels in-region.
void BM_MemorySharedMap_BoundsChecked_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    const ScopedOffsetPtrBoundsChecking bounds_checking{true};
    for (auto _ : state)
    {
        state.PauseTiming();
        score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
        auto* const data = resource.construct<score::memory::shared::Map<int, int>>(resource);
        state.ResumeTiming();
        for (const auto& [key, value] : entries)
        {
            data->emplace(key, value);
        }
        benchmark::DoNotOptimize(data->size());
        state.PauseTiming();
        resource.destruct(*data);
        state.ResumeTiming();
    }
}

#if defined(__linux__)
/// boost::interprocess::map random-order build.
/// Heap setup/teardown is excluded from timing to match other map build benchmarks.
void BM_BoostInterprocessMap_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    for (auto _ : state)
    {
        state.PauseTiming();
        boost::interprocess::managed_heap_memory heap_memory{kBoostMapHeapSize};
        BoostInterprocessMap data{heap_memory.get_segment_manager()};
        state.ResumeTiming();
        for (const auto& [key, value] : entries)
        {
            data.emplace(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}
#endif  // __linux__

/// score::nothrow::Map random-order build with direct-pointer policy.
void BM_RawBoxMap_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    for (auto _ : state)
    {
        auto data = RawBoxMap::CreateOrAbort();
        for (const auto& [key, value] : entries)
        {
            data.EmplaceOrAbort(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}

/// score::nothrow::Map random-order build with relocatable offset-pointer policy.
void BM_OffsetBoxMap_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    for (auto _ : state)
    {
        auto data = OffsetBoxMap::CreateOrAbort();
        for (const auto& [key, value] : entries)
        {
            data.EmplaceOrAbort(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}

/// Baseline: std::map randomized key lookup.
void BM_StdMap_RandomAccess(benchmark::State& state)
{
    const auto data = MakeStdMap();
    const auto& lookup_keys = GetMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// score::memory::shared::Map randomized key lookup without bounds checking.
/// Map object is allocated inside the managed region to keep map-internal sentinels in-region.
void BM_MemorySharedMap_NoBoundsCheck_RandomAccess(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto* const data = MakeMemorySharedMapInResource(resource);
    const auto& lookup_keys = GetMapPattern().lookup_keys();
    const ScopedOffsetPtrBoundsChecking bounds_checking{false};
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data->find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
    resource.destruct(*data);
}

/// score::memory::shared::Map randomized key lookup with bounds checking.
/// Map object is allocated inside the managed region to keep map-internal sentinels in-region.
void BM_MemorySharedMap_BoundsChecked_RandomAccess(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto* const data = MakeMemorySharedMapInResource(resource);
    const auto& lookup_keys = GetMapPattern().lookup_keys();
    const ScopedOffsetPtrBoundsChecking bounds_checking{true};
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data->find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
    resource.destruct(*data);
}

#if defined(__linux__)
/// boost::interprocess::map randomized key lookup.
void BM_BoostInterprocessMap_RandomAccess(benchmark::State& state)
{
    boost::interprocess::managed_heap_memory heap_memory{kBoostMapHeapSize};
    const auto data = MakeBoostInterprocessMap(heap_memory);
    const auto& lookup_keys = GetMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}
#endif  // __linux__

/// score::nothrow::Map randomized key lookup with direct-pointer policy.
void BM_RawBoxMap_RandomAccess(benchmark::State& state)
{
    const auto data = MakeRawBoxMap();
    const auto& lookup_keys = GetMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// score::nothrow::Map randomized key lookup with relocatable offset-pointer policy.
void BM_OffsetBoxMap_RandomAccess(benchmark::State& state)
{
    const auto data = MakeOffsetBoxMap();
    const auto& lookup_keys = GetMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// Baseline: std::map begin->end iteration.
void BM_StdMap_Iterate(benchmark::State& state)
{
    const auto data = MakeStdMap();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [key, value] : data)
        {
            benchmark::DoNotOptimize(&key);
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// score::memory::shared::Map begin->end iteration without bounds checking.
/// Map object is allocated inside the managed region to keep map-internal sentinels in-region.
void BM_MemorySharedMap_NoBoundsCheck_Iterate(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto* const data = MakeMemorySharedMapInResource(resource);
    const ScopedOffsetPtrBoundsChecking bounds_checking{false};
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [key, value] : *data)
        {
            benchmark::DoNotOptimize(&key);
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }
    resource.destruct(*data);
}

/// score::memory::shared::Map begin->end iteration with bounds checking.
/// Map object is allocated inside the managed region to keep map-internal sentinels in-region.
void BM_MemorySharedMap_BoundsChecked_Iterate(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto* const data = MakeMemorySharedMapInResource(resource);
    const ScopedOffsetPtrBoundsChecking bounds_checking{true};
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [key, value] : *data)
        {
            benchmark::DoNotOptimize(&key);
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }
    resource.destruct(*data);
}

#if defined(__linux__)
/// boost::interprocess::map begin->end iteration.
void BM_BoostInterprocessMap_Iterate(benchmark::State& state)
{
    boost::interprocess::managed_heap_memory heap_memory{kBoostMapHeapSize};
    const auto data = MakeBoostInterprocessMap(heap_memory);
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [key, value] : data)
        {
            benchmark::DoNotOptimize(&key);
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }
}
#endif  // __linux__

/// score::nothrow::Map begin->end iteration with direct-pointer policy.
void BM_RawBoxMap_Iterate(benchmark::State& state)
{
    const auto data = MakeRawBoxMap();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [key, value] : data)
        {
            benchmark::DoNotOptimize(&key);
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// score::nothrow::Map begin->end iteration with relocatable offset-pointer policy.
void BM_OffsetBoxMap_Iterate(benchmark::State& state)
{
    const auto data = MakeOffsetBoxMap();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const auto& [key, value] : data)
        {
            benchmark::DoNotOptimize(&key);
            sum += value;
        }
        benchmark::DoNotOptimize(sum);
    }
}

// --- Focused map-internals microbenchmarks ---

/// Focused insert/rebalance path: ascending inserts force balancing work.
void BM_StdMap_Internal_InsertRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().sorted_entries();
    for (auto _ : state)
    {
        std::map<int, int> data;
        for (const auto& [key, value] : entries)
        {
            data.emplace(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}

/// Focused insert/rebalance path for score::memory::shared::Map without bounds checking.
/// Resource setup/teardown excluded from timing to match RandomBuild pattern.
void BM_MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().sorted_entries();
    const ScopedOffsetPtrBoundsChecking bounds_checking{false};
    for (auto _ : state)
    {
        state.PauseTiming();
        score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
        auto* const data = resource.construct<score::memory::shared::Map<int, int>>(resource);
        state.ResumeTiming();
        for (const auto& [key, value] : entries)
        {
            data->emplace(key, value);
        }
        benchmark::DoNotOptimize(data->size());
        state.PauseTiming();
        resource.destruct(*data);
        state.ResumeTiming();
    }
}

/// Focused insert/rebalance path for score::memory::shared::Map with bounds checking.
/// Resource setup/teardown excluded from timing to match RandomBuild pattern.
void BM_MemorySharedMap_BoundsChecked_Internal_InsertRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().sorted_entries();
    const ScopedOffsetPtrBoundsChecking bounds_checking{true};
    for (auto _ : state)
    {
        state.PauseTiming();
        score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
        auto* const data = resource.construct<score::memory::shared::Map<int, int>>(resource);
        state.ResumeTiming();
        for (const auto& [key, value] : entries)
        {
            data->emplace(key, value);
        }
        benchmark::DoNotOptimize(data->size());
        state.PauseTiming();
        resource.destruct(*data);
        state.ResumeTiming();
    }
}

#if defined(__linux__)
/// Focused insert/rebalance path for boost::interprocess::map with ascending inserts.
/// Heap setup excluded from timing to match RandomBuild pattern.
void BM_BoostInterprocessMap_Internal_InsertRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().sorted_entries();
    for (auto _ : state)
    {
        state.PauseTiming();
        boost::interprocess::managed_heap_memory heap_memory{kBoostMapHeapSize};
        BoostInterprocessMap data{heap_memory.get_segment_manager()};
        state.ResumeTiming();
        for (const auto& [key, value] : entries)
        {
            data.emplace(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}
#endif  // __linux__

/// Focused insert/rebalance path for score::nothrow::Map with direct-pointer policy.
void BM_RawBoxMap_Internal_InsertRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().sorted_entries();
    for (auto _ : state)
    {
        auto data = RawBoxMap::CreateOrAbort();
        for (const auto& [key, value] : entries)
        {
            data.EmplaceOrAbort(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}

/// Focused insert/rebalance path for score::nothrow::Map with relocatable offset-pointer policy.
void BM_OffsetBoxMap_Internal_InsertRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().sorted_entries();
    for (auto _ : state)
    {
        auto data = OffsetBoxMap::CreateOrAbort();
        for (const auto& [key, value] : entries)
        {
            data.EmplaceOrAbort(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}

/// Focused find path using pre-built tree and randomized successful lookups.
void BM_StdMap_Internal_Find(benchmark::State& state)
{
    const auto data = MakeStdMapFromEntries(GetFocusedMapPattern().random_entries());
    const auto& lookup_keys = GetFocusedMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// Focused find path for score::memory::shared::Map without bounds checking.
void BM_MemorySharedMap_NoBoundsCheck_Internal_Find(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto* const data = MakeMemorySharedMapInResourceFromEntries(resource, GetFocusedMapPattern().random_entries());
    const auto& lookup_keys = GetFocusedMapPattern().lookup_keys();
    const ScopedOffsetPtrBoundsChecking bounds_checking{false};
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data->find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
    resource.destruct(*data);
}

/// Focused find path for score::memory::shared::Map with bounds checking.
void BM_MemorySharedMap_BoundsChecked_Internal_Find(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto* const data = MakeMemorySharedMapInResourceFromEntries(resource, GetFocusedMapPattern().random_entries());
    const auto& lookup_keys = GetFocusedMapPattern().lookup_keys();
    const ScopedOffsetPtrBoundsChecking bounds_checking{true};
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data->find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
    resource.destruct(*data);
}

#if defined(__linux__)
/// Focused find path for boost::interprocess::map with randomized successful lookups.
void BM_BoostInterprocessMap_Internal_Find(benchmark::State& state)
{
    boost::interprocess::managed_heap_memory heap_memory{kBoostMapHeapSize};
    const auto data = MakeBoostInterprocessMapFromEntries(heap_memory, GetFocusedMapPattern().random_entries());
    const auto& lookup_keys = GetFocusedMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}
#endif  // __linux__

/// Focused find path for score::nothrow::Map with direct-pointer policy.
void BM_RawBoxMap_Internal_Find(benchmark::State& state)
{
    const auto data = MakeRawBoxMapFromEntries(GetFocusedMapPattern().random_entries());
    const auto& lookup_keys = GetFocusedMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// Focused find path for score::nothrow::Map with relocatable offset-pointer policy.
void BM_OffsetBoxMap_Internal_Find(benchmark::State& state)
{
    const auto data = MakeOffsetBoxMapFromEntries(GetFocusedMapPattern().random_entries());
    const auto& lookup_keys = GetFocusedMapPattern().lookup_keys();
    for (auto _ : state)
    {
        std::int64_t sum = 0;
        for (const int key : lookup_keys)
        {
            const auto it = data.find(key);
            sum += it->second;
        }
        benchmark::DoNotOptimize(sum);
    }
}

/// Focused erase/rebalance path using randomized erase order.
void BM_StdMap_Internal_EraseRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().random_entries();
    const auto& erase_keys = GetFocusedMapPattern().erase_keys();
    for (auto _ : state)
    {
        auto data = MakeStdMapFromEntries(entries);
        std::size_t erased = 0U;
        for (const int key : erase_keys)
        {
            erased += data.erase(key);
        }
        benchmark::DoNotOptimize(erased);
        benchmark::DoNotOptimize(data.size());
    }
}

/// Focused erase/rebalance path for score::memory::shared::Map without bounds checking.
/// Resource setup/teardown excluded from timing; map build remains timed to match std/nothrow.
void BM_MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().random_entries();
    const auto& erase_keys = GetFocusedMapPattern().erase_keys();
    const ScopedOffsetPtrBoundsChecking bounds_checking{false};
    for (auto _ : state)
    {
        state.PauseTiming();
        score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
        state.ResumeTiming();
        auto* const data = MakeMemorySharedMapInResourceFromEntries(resource, entries);
        std::size_t erased = 0U;
        for (const int key : erase_keys)
        {
            erased += data->erase(key);
        }
        benchmark::DoNotOptimize(erased);
        benchmark::DoNotOptimize(data->size());
        state.PauseTiming();
        resource.destruct(*data);
        state.ResumeTiming();
    }
}

/// Focused erase/rebalance path for score::memory::shared::Map with bounds checking.
/// Resource setup/teardown excluded from timing; map build remains timed to match std/nothrow.
void BM_MemorySharedMap_BoundsChecked_Internal_EraseRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().random_entries();
    const auto& erase_keys = GetFocusedMapPattern().erase_keys();
    const ScopedOffsetPtrBoundsChecking bounds_checking{true};
    for (auto _ : state)
    {
        state.PauseTiming();
        score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
        state.ResumeTiming();
        auto* const data = MakeMemorySharedMapInResourceFromEntries(resource, entries);
        std::size_t erased = 0U;
        for (const int key : erase_keys)
        {
            erased += data->erase(key);
        }
        benchmark::DoNotOptimize(erased);
        benchmark::DoNotOptimize(data->size());
        state.PauseTiming();
        resource.destruct(*data);
        state.ResumeTiming();
    }
}

#if defined(__linux__)
/// Focused erase/rebalance path for boost::interprocess::map with randomized erase order.
/// Heap setup excluded from timing; map build remains timed to match std/nothrow.
void BM_BoostInterprocessMap_Internal_EraseRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().random_entries();
    const auto& erase_keys = GetFocusedMapPattern().erase_keys();
    for (auto _ : state)
    {
        state.PauseTiming();
        boost::interprocess::managed_heap_memory heap_memory{kBoostMapHeapSize};
        state.ResumeTiming();
        auto data = MakeBoostInterprocessMapFromEntries(heap_memory, entries);
        std::size_t erased = 0U;
        for (const int key : erase_keys)
        {
            erased += data.erase(key);
        }
        benchmark::DoNotOptimize(erased);
        benchmark::DoNotOptimize(data.size());
    }
}
#endif  // __linux__

/// Focused erase/rebalance path for score::nothrow::Map with direct-pointer policy.
void BM_RawBoxMap_Internal_EraseRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().random_entries();
    const auto& erase_keys = GetFocusedMapPattern().erase_keys();
    for (auto _ : state)
    {
        auto data = MakeRawBoxMapFromEntries(entries);
        std::size_t erased = 0U;
        for (const int key : erase_keys)
        {
            erased += data.erase(key);
        }
        benchmark::DoNotOptimize(erased);
        benchmark::DoNotOptimize(data.size());
    }
}

/// Focused erase/rebalance path for score::nothrow::Map with relocatable offset-pointer policy.
void BM_OffsetBoxMap_Internal_EraseRebalance(benchmark::State& state)
{
    const auto& entries = GetFocusedMapPattern().random_entries();
    const auto& erase_keys = GetFocusedMapPattern().erase_keys();
    for (auto _ : state)
    {
        auto data = MakeOffsetBoxMapFromEntries(entries);
        std::size_t erased = 0U;
        for (const int key : erase_keys)
        {
            erased += data.erase(key);
        }
        benchmark::DoNotOptimize(erased);
        benchmark::DoNotOptimize(data.size());
    }
}

// --- Benchmark registration (ordered output) ---
BENCHMARK(BM_StdVector_RandomAccess);
BENCHMARK(BM_MemorySharedVector_NoBoundsCheck_RandomAccess);
BENCHMARK(BM_MemorySharedVector_BoundsChecked_RandomAccess);
BENCHMARK(BM_RawBoxVector_RandomAccess);
BENCHMARK(BM_OffsetBoxVector_RandomAccess);
#if defined(__linux__)
BENCHMARK(BM_BoostInterprocessVector_RandomAccess);
#endif  // __linux__

BENCHMARK(BM_StdMap_RandomBuild);
BENCHMARK(BM_MemorySharedMap_NoBoundsCheck_RandomBuild);
BENCHMARK(BM_MemorySharedMap_BoundsChecked_RandomBuild);
BENCHMARK(BM_RawBoxMap_RandomBuild);
BENCHMARK(BM_OffsetBoxMap_RandomBuild);
#if defined(__linux__)
BENCHMARK(BM_BoostInterprocessMap_RandomBuild);
#endif  // __linux__

BENCHMARK(BM_StdMap_RandomAccess);
BENCHMARK(BM_MemorySharedMap_NoBoundsCheck_RandomAccess);
BENCHMARK(BM_MemorySharedMap_BoundsChecked_RandomAccess);
BENCHMARK(BM_RawBoxMap_RandomAccess);
BENCHMARK(BM_OffsetBoxMap_RandomAccess);
#if defined(__linux__)
BENCHMARK(BM_BoostInterprocessMap_RandomAccess);
#endif  // __linux__

BENCHMARK(BM_StdMap_Iterate);
BENCHMARK(BM_MemorySharedMap_NoBoundsCheck_Iterate);
BENCHMARK(BM_MemorySharedMap_BoundsChecked_Iterate);
BENCHMARK(BM_RawBoxMap_Iterate);
BENCHMARK(BM_OffsetBoxMap_Iterate);
#if defined(__linux__)
BENCHMARK(BM_BoostInterprocessMap_Iterate);
#endif  // __linux__

BENCHMARK(BM_StdMap_Internal_InsertRebalance);
BENCHMARK(BM_MemorySharedMap_NoBoundsCheck_Internal_InsertRebalance);
BENCHMARK(BM_MemorySharedMap_BoundsChecked_Internal_InsertRebalance);
BENCHMARK(BM_RawBoxMap_Internal_InsertRebalance);
BENCHMARK(BM_OffsetBoxMap_Internal_InsertRebalance);
#if defined(__linux__)
BENCHMARK(BM_BoostInterprocessMap_Internal_InsertRebalance);
#endif  // __linux__

BENCHMARK(BM_StdMap_Internal_Find);
BENCHMARK(BM_MemorySharedMap_NoBoundsCheck_Internal_Find);
BENCHMARK(BM_MemorySharedMap_BoundsChecked_Internal_Find);
BENCHMARK(BM_RawBoxMap_Internal_Find);
BENCHMARK(BM_OffsetBoxMap_Internal_Find);
#if defined(__linux__)
BENCHMARK(BM_BoostInterprocessMap_Internal_Find);
#endif  // __linux__

BENCHMARK(BM_StdMap_Internal_EraseRebalance);
BENCHMARK(BM_MemorySharedMap_NoBoundsCheck_Internal_EraseRebalance);
BENCHMARK(BM_MemorySharedMap_BoundsChecked_Internal_EraseRebalance);
BENCHMARK(BM_RawBoxMap_Internal_EraseRebalance);
BENCHMARK(BM_OffsetBoxMap_Internal_EraseRebalance);
#if defined(__linux__)
BENCHMARK(BM_BoostInterprocessMap_Internal_EraseRebalance);
#endif  // __linux__

}  // namespace
