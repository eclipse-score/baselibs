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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POOL_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POOL_HPP

#include <score/private/memory_resource/chunk_list.hpp>
#include <score/private/memory_resource/free_list.hpp>
#include <score/private/memory_resource/memory_resource.hpp>
#include <score/private/memory_resource/pool_options.hpp>

#include <score/assert.hpp>
#include <score/bit.hpp>
#include <score/size.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

namespace score::cpp
{
namespace pmr
{
namespace detail
{

class pool
{
public:
    explicit pool(const std::size_t block_size, const std::size_t initial_block_count)
        : block_size_{block_size}
        , next_block_count_{initial_block_count}
        , current_{}
        , stop_{}
        , free_block_list_{}
        , chunks_{}
    {
    }

    void* allocate(memory_resource* const resource, const pool_options& options)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(resource != nullptr);
        if (!free_block_list_.empty())
        {
            return free_block_list_.pop_front();
        }
        else
        {
            if (current_ == stop_)
            {
                // pool allocates chunks with an alignment of 1 << score::cpp::countr_zero(block_size_) because this is the
                // strictest alignment that can be guaranteed given block_size_. An example: Given a block_size_ of 48
                // and an initial alignment of at least 16, every second block has an alignment of 16, so this is the
                // best we can guarantee. 48 == 0b110000, so countr_zero(48) = 4 and 1 << 4 == 16, as desired.
                const std::size_t size{block_size_ * next_block_count_};
                current_ = chunks_.allocate(resource, size, 1_UZ << score::cpp::countr_zero(block_size_));
                SCORE_LANGUAGE_FUTURECPP_ASSERT(current_ != nullptr);
                stop_ = current_ + size;
                constexpr std::size_t max_chunk_size{std::numeric_limits<std::ptrdiff_t>::max()};
                next_block_count_ =
                    std::min({max_chunk_size / block_size_, options.max_blocks_per_chunk, 2U * next_block_count_});
            }
            return std::exchange(current_, current_ + block_size_);
        }
    }

    void deallocate(void* const p)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(p != nullptr);
        free_block_list_.push_front(p);
    }

    void release(memory_resource* const resource)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(resource != nullptr);
        chunks_.release(resource);
        free_block_list_.clear();
        current_ = nullptr;
        stop_ = nullptr;
    }

private:
    std::size_t block_size_;
    std::size_t next_block_count_;
    std::uint8_t* current_;
    std::uint8_t* stop_;
    free_list free_block_list_;
    detail::chunk_list chunks_;
};

} // namespace detail
} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POOL_HPP
