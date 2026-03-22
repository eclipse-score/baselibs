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
///   4. score::shm::Vector with direct-pointer policy
///   5. score::shm::Vector with relocatable offset-pointer policy
///
/// In addition, compares std::map, score::memory::shared::Map and score::shm::Map
/// (direct-pointer policy + relocatable policy) for:
///   - random-order build
///   - random key lookup
///   - full in-order iteration

#include "score/memory/shared/fake/my_bounded_memory_resource.h"
#include "score/memory/shared/map.h"
#include "score/memory/shared/new_delete_delegate_resource.h"
#include "score/memory/shared/offset_ptr.h"
#include "score/memory/shared/vector.h"
#include "score/shm/map.h"
#include "score/shm/vector.h"

#include <benchmark/benchmark.h>

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

// 64 MB — generous for bump allocator that doesn't reclaim on dealloc.
// Total data is ~4 MB, but vector growth without reuse needs headroom.
constexpr std::size_t kBoundedResourceSize = 64U * 1024U * 1024U;

template <typename T>
class BenchmarkRelativeOffsetPtr
{
  public:
    BenchmarkRelativeOffsetPtr(T const* const pointer) noexcept
        : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)}
    {
    }

    BenchmarkRelativeOffsetPtr(const BenchmarkRelativeOffsetPtr& other) noexcept
        : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)}
    {
    }

    BenchmarkRelativeOffsetPtr& operator=(const BenchmarkRelativeOffsetPtr& other) noexcept
    {
        offset_from_this_ = PointerToInteger(other.get()) - PointerToInteger(this);
        return *this;
    }

    T* get() noexcept { return IntegerToPointer(PointerToInteger(this) + offset_from_this_); }

    const T* get() const noexcept { return IntegerToConstPointer(PointerToInteger(this) + offset_from_this_); }

  private:
    static std::intptr_t PointerToInteger(const void* const pointer) noexcept
    {
        return reinterpret_cast<std::intptr_t>(pointer);
    }

    static T* IntegerToPointer(const std::intptr_t value) noexcept
    {
        return static_cast<T*>(reinterpret_cast<void*>(value));
    }

    static const T* IntegerToConstPointer(const std::intptr_t value) noexcept
    {
        return static_cast<const T*>(reinterpret_cast<const void*>(value));
    }

    std::intptr_t offset_from_this_{};
};

template <typename T>
class BenchmarkRelativeNullableOffsetPtr
{
  public:
    BenchmarkRelativeNullableOffsetPtr(T const* const pointer) noexcept
        : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)}, is_nullptr_{pointer == nullptr}
    {
    }

    BenchmarkRelativeNullableOffsetPtr(const BenchmarkRelativeNullableOffsetPtr& other) noexcept
        : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)},
          is_nullptr_{other.get() == nullptr}
    {
    }

    BenchmarkRelativeNullableOffsetPtr& operator=(const BenchmarkRelativeNullableOffsetPtr& other) noexcept
    {
        offset_from_this_ = PointerToInteger(other.get()) - PointerToInteger(this);
        is_nullptr_ = other.get() == nullptr;
        return *this;
    }

    T* get() noexcept
    {
        return is_nullptr_ ? nullptr : IntegerToPointer(PointerToInteger(this) + offset_from_this_);
    }

    const T* get() const noexcept
    {
        return is_nullptr_ ? nullptr : IntegerToConstPointer(PointerToInteger(this) + offset_from_this_);
    }

  private:
    static std::intptr_t PointerToInteger(const void* const pointer) noexcept
    {
        return reinterpret_cast<std::intptr_t>(pointer);
    }

    static T* IntegerToPointer(const std::intptr_t value) noexcept
    {
        return static_cast<T*>(reinterpret_cast<void*>(value));
    }

    static const T* IntegerToConstPointer(const std::intptr_t value) noexcept
    {
        return static_cast<const T*>(reinterpret_cast<const void*>(value));
    }

    std::intptr_t offset_from_this_{};
    bool is_nullptr_{true};
};

using ShmDirectPointerPolicy = score::shm::ShmDirectPointerPolicy;

struct ShmRelocPointerPolicy
{
    template <typename T>
    using Ptr = BenchmarkRelativeOffsetPtr<T>;

    template <typename T>
    using NullablePtr = BenchmarkRelativeNullableOffsetPtr<T>;
};

using ShmDirectInnerVector =
    score::shm::Vector<int, score::shm::PolymorphicAllocator<int>, ShmDirectPointerPolicy>;
using ShmDirectNestedVector = score::shm::Vector<ShmDirectInnerVector,
                                                 score::shm::PolymorphicAllocator<ShmDirectInnerVector>,
                                                 ShmDirectPointerPolicy>;
using ShmRelocInnerVector =
    score::shm::Vector<int, score::shm::PolymorphicAllocator<int>, ShmRelocPointerPolicy>;
using ShmRelocNestedVector = score::shm::Vector<ShmRelocInnerVector,
                                                score::shm::PolymorphicAllocator<ShmRelocInnerVector>,
                                                ShmRelocPointerPolicy>;

using ShmDirectMap = score::shm::Map<int,
                                     int,
                                     std::less<int>,
                                     score::shm::PolymorphicAllocator<std::pair<const int, int>>,
                                     ShmDirectPointerPolicy>;
using ShmRelocMap = score::shm::Map<int,
                                    int,
                                    std::less<int>,
                                    score::shm::PolymorphicAllocator<std::pair<const int, int>>,
                                    ShmRelocPointerPolicy>;

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

ShmDirectNestedVector MakeShmDirectNestedVector()
{
    auto outer = ShmDirectNestedVector::CreateWithCapacityOrAbort(kOuterSize);
    for (std::size_t i = 0U; i < kOuterSize; ++i)
    {
        auto inner = ShmDirectInnerVector::CreateOrAbort(kInnerSize);
        std::iota(inner.begin(), inner.end(), 0);
        outer.PushBackOrAbort(std::move(inner));
    }
    return outer;
}

ShmRelocNestedVector MakeShmRelocNestedVector()
{
    auto outer = ShmRelocNestedVector::CreateWithCapacityOrAbort(kOuterSize);
    for (std::size_t i = 0U; i < kOuterSize; ++i)
    {
        auto inner = ShmRelocInnerVector::CreateOrAbort(kInnerSize);
        std::iota(inner.begin(), inner.end(), 0);
        outer.PushBackOrAbort(std::move(inner));
    }
    return outer;
}

std::map<int, int> MakeStdMap()
{
    std::map<int, int> data;
    for (const auto& [key, value] : GetMapPattern().entries())
    {
        data.emplace(key, value);
    }
    return data;
}

score::memory::shared::Map<int, int> MakeMemorySharedMap(score::memory::shared::ManagedMemoryResource& resource)
{
    score::memory::shared::Map<int, int> data{resource};
    for (const auto& [key, value] : GetMapPattern().entries())
    {
        data.emplace(key, value);
    }
    return data;
}

ShmDirectMap MakeShmDirectMap()
{
    auto data = ShmDirectMap::CreateOrAbort();
    for (const auto& [key, value] : GetMapPattern().entries())
    {
        data.EmplaceOrAbort(key, value);
    }
    return data;
}

ShmRelocMap MakeShmRelocMap()
{
    auto data = ShmRelocMap::CreateOrAbort();
    for (const auto& [key, value] : GetMapPattern().entries())
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
BENCHMARK(BM_StdVector_RandomAccess);

/// OffsetPtr without bounds checking: NewDeleteDelegateMemoryResource bypasses
/// OffsetPtr bounds check (IsOffsetPtrBoundsCheckBypassingEnabled = true).
/// Measures pure OffsetPtr offset-arithmetic overhead vs raw T*.
void BM_MemorySharedVector_NoBoundsCheck_RandomAccess(benchmark::State& state)
{
    score::memory::shared::NewDeleteDelegateMemoryResource resource{1U};
    auto data = MakeMemorySharedNestedVector(resource);
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
BENCHMARK(BM_MemorySharedVector_NoBoundsCheck_RandomAccess);

/// OffsetPtr with bounds checking: MyBoundedMemoryResource enables the full
/// OffsetPtr bounds-check path (IsOffsetPtrBoundsCheckBypassingEnabled = false).
/// Measures combined overhead of offset arithmetic + bounds validation.
void BM_MemorySharedVector_BoundsChecked_RandomAccess(benchmark::State& state)
{
    score::memory::shared::test::MyBoundedMemoryResource resource{kBoundedResourceSize};
    auto data = MakeMemorySharedNestedVector(resource);
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
BENCHMARK(BM_MemorySharedVector_BoundsChecked_RandomAccess);

/// score::shm::Vector with direct-pointer policy.
void BM_ShmDirectVector_RandomAccess(benchmark::State& state)
{
    auto data = MakeShmDirectNestedVector();
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
BENCHMARK(BM_ShmDirectVector_RandomAccess);

/// score::shm::Vector with relocatable offset-pointer policy.
void BM_ShmRelocVector_RandomAccess(benchmark::State& state)
{
    auto data = MakeShmRelocNestedVector();
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
BENCHMARK(BM_ShmRelocVector_RandomAccess);

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
BENCHMARK(BM_StdMap_RandomBuild);

/// score::memory::shared::Map random-order build.
void BM_MemorySharedMap_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    score::memory::shared::NewDeleteDelegateMemoryResource resource{2U};
    for (auto _ : state)
    {
        score::memory::shared::Map<int, int> data{resource};
        for (const auto& [key, value] : entries)
        {
            data.emplace(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}
BENCHMARK(BM_MemorySharedMap_RandomBuild);

/// score::shm::Map random-order build with direct-pointer policy.
void BM_ShmDirectMap_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    for (auto _ : state)
    {
        auto data = ShmDirectMap::CreateOrAbort();
        for (const auto& [key, value] : entries)
        {
            data.EmplaceOrAbort(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}
BENCHMARK(BM_ShmDirectMap_RandomBuild);

/// score::shm::Map random-order build with relocatable offset-pointer policy.
void BM_ShmRelocMap_RandomBuild(benchmark::State& state)
{
    const auto& entries = GetMapPattern().entries();
    for (auto _ : state)
    {
        auto data = ShmRelocMap::CreateOrAbort();
        for (const auto& [key, value] : entries)
        {
            data.EmplaceOrAbort(key, value);
        }
        benchmark::DoNotOptimize(data.size());
    }
}
BENCHMARK(BM_ShmRelocMap_RandomBuild);

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
BENCHMARK(BM_StdMap_RandomAccess);

/// score::memory::shared::Map randomized key lookup.
void BM_MemorySharedMap_RandomAccess(benchmark::State& state)
{
    score::memory::shared::NewDeleteDelegateMemoryResource resource{3U};
    const auto data = MakeMemorySharedMap(resource);
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
BENCHMARK(BM_MemorySharedMap_RandomAccess);

/// score::shm::Map randomized key lookup with direct-pointer policy.
void BM_ShmDirectMap_RandomAccess(benchmark::State& state)
{
    const auto data = MakeShmDirectMap();
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
BENCHMARK(BM_ShmDirectMap_RandomAccess);

/// score::shm::Map randomized key lookup with relocatable offset-pointer policy.
void BM_ShmRelocMap_RandomAccess(benchmark::State& state)
{
    const auto data = MakeShmRelocMap();
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
BENCHMARK(BM_ShmRelocMap_RandomAccess);

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
BENCHMARK(BM_StdMap_Iterate);

/// score::memory::shared::Map begin->end iteration.
void BM_MemorySharedMap_Iterate(benchmark::State& state)
{
    score::memory::shared::NewDeleteDelegateMemoryResource resource{4U};
    const auto data = MakeMemorySharedMap(resource);
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
BENCHMARK(BM_MemorySharedMap_Iterate);

/// score::shm::Map begin->end iteration with direct-pointer policy.
void BM_ShmDirectMap_Iterate(benchmark::State& state)
{
    const auto data = MakeShmDirectMap();
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
BENCHMARK(BM_ShmDirectMap_Iterate);

/// score::shm::Map begin->end iteration with relocatable offset-pointer policy.
void BM_ShmRelocMap_Iterate(benchmark::State& state)
{
    const auto data = MakeShmRelocMap();
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
BENCHMARK(BM_ShmRelocMap_Iterate);

}  // namespace
