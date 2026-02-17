/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Pmr.MemoryResource component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_UNSYNCHRONIZED_POOL_RESOURCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_UNSYNCHRONIZED_POOL_RESOURCE_HPP

#include <score/private/memory_resource/chunk_list.hpp>
#include <score/private/memory_resource/memory_resource.hpp>
#include <score/private/memory_resource/polymorphic_allocator.hpp>
#include <score/private/memory_resource/pool.hpp>
#include <score/private/memory_resource/pool_options.hpp>

#include <score/assert.hpp>
#include <score/bit.hpp>
#include <score/size.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace score::cpp
{
namespace pmr
{

namespace detail
{

constexpr bool is_aligned(const std::size_t x, const std::size_t alignment) { return (x & (alignment - 1U)) == 0U; }

struct unsynchronized_pool_resource_utils
{
    inline static constexpr std::array<std::size_t, 32U> block_sizes{
        8U,          16U,         24U,         32U,         48U,         64U,         80U,         96U,
        112U,        128U,        192U,        256U,        320U,        384U,        448U,        512U,
        768U,        1024U,       1536U,       2048U,       3072U,       4096U,       1_UZ << 13U, 1_UZ << 14U,
        1_UZ << 15U, 1_UZ << 16U, 1_UZ << 17U, 1_UZ << 18U, 1_UZ << 19U, 1_UZ << 20U, 1_UZ << 21U, 1_UZ << 22U};

    /// \returns i \in [0-31] iff block_sizes[i] is the first pool size that satisfies the size and alignment
    /// requirement, and 32 otherwise.
    static std::ptrdiff_t get_pool_index(const std::size_t bytes, const std::size_t alignment)
    {
        const std::size_t size{score::cpp::align_up(bytes, alignment)};
        if (size < 4096U)
        {
            constexpr std::ptrdiff_t pos4096{21};
            static_assert(block_sizes[pos4096] == 4096U, "pos4096 must the index of 4096 in block_sizes.");
            return std::distance(std::begin(block_sizes),
                                 std::lower_bound(std::begin(block_sizes), std::begin(block_sizes) + pos4096, size));
        }
        else if (size <= block_sizes.back())
        {
            constexpr std::size_t offset4096{9U};
            static_assert(block_sizes[offset4096 + score::cpp::bit_width(4096U - 1U)] == 4096U,
                          "offset4096 must be the offset added to lg(4096) such that the resulting number is the index "
                          "of 4096 in block_sizes.");
            return static_cast<std::ptrdiff_t>(offset4096 + score::cpp::bit_width(size - 1U));
        }
        else
        {
            return score::cpp::ssize(block_sizes);
        }
    }
};

} // namespace detail

/// \brief A thread-unsafe score::cpp::pmr::memory_resource for managing allocations in pools of different block sizes
///
/// The class score::cpp::pmr::unsynchronized_pool_resource is a general-purpose memory resource class with the following
/// properties:
///
/// It owns the allocated memory and frees it on destruction, even if deallocate has not been called for some of the
/// allocated blocks. It consists of a collection of pools that serves requests for different block sizes. Each pool
/// manages a collection of chunks that are then divided into blocks of uniform size. Calls to do_allocate are
/// dispatched to the pool serving the smallest blocks accommodating the requested size. Exhausting memory in the pool
/// causes the next allocation request for that pool to allocate an additional chunk of memory from the upstream
/// allocator to replenish the pool. The chunk size obtained increases geometrically. Allocation requests that exceed
/// the largest block size are served from the upstream allocator directly. The largest block size and maximum chunk
/// size may be tuned by passing a score::cpp::pmr::pool_options struct to its constructor.
///
/// unsynchronized_pool_resource is not thread-safe, and cannot be accessed from multiple threads simultaneously; use
/// synchronized_pool_resource if access from multiple threads is required.
///
/// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions) Follows literaly the C++ standard
class unsynchronized_pool_resource : public memory_resource
{
public:
    /// \brief Constructs an unsynchronized_pool_resource using the specified upstream memory resource and tuned
    /// according to the specified options.
    ///
    /// The resulting object holds a copy of upstream but does not own the resource to which upstream points.
    ///
    /// \pre upstream != nullptr
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/unsynchronized_pool_resource
    ///
    /// \param opts A score::cpp::pmr::pool_options struct containing the constructor options
    /// \param upstream The upstream memory resource to use
    unsynchronized_pool_resource(const pool_options& opts, memory_resource* const upstream);

    /// \brief Constructs an unsynchronized_pool_resource using a default constructed instance of pool_options as the
    /// options and the return value of score::cpp::pmr::get_default_resource() as the upstream memory resource.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/unsynchronized_pool_resource
    unsynchronized_pool_resource() : unsynchronized_pool_resource(pool_options{}, get_default_resource()) {}

    /// \brief Constructs an unsynchronized_pool_resource using the specified upstream memory resource and a default
    /// constructed instance of pool_options as the options.
    ///
    /// The resulting object holds a copy of upstream but does not own the resource to which upstream points.
    ///
    /// \pre upstream != nullptr
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/unsynchronized_pool_resource
    ///
    /// \param upstream The upstream memory resource to use
    explicit unsynchronized_pool_resource(memory_resource* const upstream)
        : unsynchronized_pool_resource{pool_options{}, upstream}
    {
    }

    /// \brief Constructs an unsynchronized_pool_resource tuned according to the specified options and using the
    /// return value of score::cpp::pmr::get_default_resource() as the upstream memory resource.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/unsynchronized_pool_resource
    ///
    /// \param opts A score::cpp::pmr::pool_options struct containing the constructor options
    explicit unsynchronized_pool_resource(const pool_options& opts)
        : unsynchronized_pool_resource{opts, get_default_resource()}
    {
    }

    /// \brief Copy constructor is deleted.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/unsynchronized_pool_resource
    unsynchronized_pool_resource(const unsynchronized_pool_resource&) = delete;

    /// \brief Destroys an unsynchronized_pool_resource, releasing all allocated memory.
    ///
    /// Deallocates all memory owned by this resource by calling this->release().
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/%7Eunsynchronized_pool_resource
    ~unsynchronized_pool_resource() override { release(); }

    /// \brief Copy assignment operator is deleted. unsynchronized_pool_resource is not copy assignable.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource
    unsynchronized_pool_resource& operator=(const unsynchronized_pool_resource&) = delete;

    /// \brief Releases all memory owned by this resource by calling the deallocate function of the upstream memory
    /// resource as needed.
    ///
    /// Memory is released back to the upstream resource even if deallocate has not been called for some of the
    /// allocated blocks.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/release
    void release();

    /// \brief Returns a pointer to the upstream memory resource.
    ///
    /// This is the same value as the upstream argument passed to the constructor of this object.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/upstream_resource
    memory_resource* upstream_resource() const { return upstream_resource_; }

    /// \brief Returns the options that control the pooling behavior of this resource.
    ///
    /// The values in the returned struct may differ from those supplied to the constructor in the following ways:
    /// * Values of zero will be replaced with implementation-specified defaults;
    /// * Sizes may be rounded to an unspecified granularity.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/options
    pool_options options() const { return opts_; }

protected:
    /// \brief Allocates storage.
    ///
    /// If the pool selected for a block of size bytes is unable to satisfy the request from its internal data
    /// structures, calls allocate() on the upstream memory resource to obtain memory.
    ///
    /// If the size requested is larger than what the largest pool can handle, memory is allocated by calling allocate()
    /// on the upstream memory resource.
    ///
    /// \pre alignment is a power of 2.
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/do_allocate
    ///
    /// \return A pointer to allocated storage of at least bytes bytes in size, aligned to the specified alignment if
    /// such alignment is supported, and to alignof(std::max_align_t) otherwise.
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    /// \brief Returns the memory at p to the pool.
    ///
    /// It is unspecified if or under what circumstances this operation will result in a call to deallocate() on the
    /// upstream memory resource.
    ///
    /// \pre p has been allocated from this object with the specified bytes and alignment.
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/do_deallocate
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;

    /// \brief Compare for equality with another score::cpp::pmr::memory_resource
    ///
    /// Compare *this with \p other for identity - memory allocated using a monotonic_buffer_resource can only be
    /// deallocated using that same resource.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/unsynchronized_pool_resource/do_is_equal
    ///
    /// \return this == &other
    bool do_is_equal(const memory_resource& other) const noexcept override { return this == &other; }

private:
    void allocate_pools();

    pool_options opts_;
    memory_resource* upstream_resource_;
    std::ptrdiff_t pool_count_;
    detail::pool* pools_;
    detail::chunk_list big_blocks_;
};

inline unsynchronized_pool_resource::unsynchronized_pool_resource(const pool_options& opts,
                                                                  memory_resource* const upstream)
    : memory_resource{}, opts_{opts}, upstream_resource_{upstream}, pool_count_{}, pools_{}, big_blocks_{}
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(upstream != nullptr);
    if (opts_.largest_required_pool_block == 0U)
    {
        opts_.largest_required_pool_block = 8192U;
    }
    if (opts_.max_blocks_per_chunk == 0U)
    {
        opts_.max_blocks_per_chunk = 8192U;
    }
    opts_.largest_required_pool_block =
        std::min(opts_.largest_required_pool_block, detail::unsynchronized_pool_resource_utils::block_sizes.back());
    pool_count_ =
        detail::unsynchronized_pool_resource_utils::get_pool_index(opts_.largest_required_pool_block, 1U) + 1U;
    opts_.largest_required_pool_block =
        score::cpp::at(detail::unsynchronized_pool_resource_utils::block_sizes, pool_count_ - 1);
}

inline void unsynchronized_pool_resource::allocate_pools()
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(pools_ == nullptr);
    polymorphic_allocator<detail::pool> pool_allocator{upstream_resource()};
    pools_ = pool_allocator.allocate(static_cast<std::size_t>(pool_count_));
    SCORE_LANGUAGE_FUTURECPP_ASSERT(pools_ != nullptr);
    for (std::ptrdiff_t i{0}; i < pool_count_; ++i)
    {
        const std::size_t block_size{score::cpp::at(detail::unsynchronized_pool_resource_utils::block_sizes, i)};
        constexpr std::size_t min_chunk_size{4096U};
        constexpr std::size_t min_block_count{1U};
        pool_allocator.construct(
            pools_ + i,
            block_size,
            std::min(std::max(min_block_count, min_chunk_size / block_size), opts_.max_blocks_per_chunk));
    }
}

inline void unsynchronized_pool_resource::release()
{
    if (pools_ != nullptr)
    {
        for (auto* pool = pools_; pool != (pools_ + pool_count_); ++pool)
        {
            pool->release(upstream_resource());
        }
        polymorphic_allocator<detail::pool> pool_allocator{upstream_resource()};
        static_assert(std::is_trivially_destructible<detail::pool>::value, "pool must be trivially destructible.");
        pool_allocator.deallocate(pools_, static_cast<std::size_t>(pool_count_));
        pools_ = nullptr;
    }

    big_blocks_.release(upstream_resource());
}

inline void* unsynchronized_pool_resource::do_allocate(const std::size_t bytes, const std::size_t alignment)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(score::cpp::has_single_bit(alignment));

    const std::ptrdiff_t pool_index{detail::unsynchronized_pool_resource_utils::get_pool_index(bytes, alignment)};
    if (pool_index >= pool_count_)
    {
        std::uint8_t* const ptr{big_blocks_.allocate(upstream_resource(), bytes, alignment)};
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(ptr != nullptr);
        return ptr;
    }
    else
    {
        if (pools_ == nullptr)
        {
            allocate_pools();
        }
        void* const ptr{pools_[pool_index].allocate(upstream_resource(), opts_)};
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(ptr != nullptr);
        return ptr;
    }
}

inline void
unsynchronized_pool_resource::do_deallocate(void* const p, const std::size_t bytes, const std::size_t alignment)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(score::cpp::has_single_bit(alignment));

    const std::ptrdiff_t pool_index{detail::unsynchronized_pool_resource_utils::get_pool_index(bytes, alignment)};
    if (pool_index >= pool_count_)
    {
        big_blocks_.deallocate(upstream_resource(), p, bytes, alignment);
    }
    else
    {
        pools_[pool_index].deallocate(p);
    }
}

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_UNSYNCHRONIZED_POOL_RESOURCE_HPP
