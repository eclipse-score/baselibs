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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_CHUNK_SIZE_AND_ALIGNMENT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_CHUNK_SIZE_AND_ALIGNMENT_HPP

#include <score/assert.hpp>
#include <score/bit.hpp>
#include <score/size.hpp>

#include <cstddef>

namespace score::cpp
{
namespace pmr
{
namespace detail
{

constexpr bool is_multiple_of(const std::size_t x, const std::size_t factor) { return (x % factor) == 0U; }

class chunk_size_and_alignment
{
public:
    chunk_size_and_alignment(const std::size_t size, const std::size_t alignment)
        : val_{size | (score::cpp::bit_width(alignment) - 1U)}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(has_single_bit(alignment));
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(is_multiple_of(size, 64U));
        // `is_multiple_of(size, 64U)` implies that the last 6 bits of size are zeros and can be used to store
        // log2(alignment), i.e. 0-63.
        SCORE_LANGUAGE_FUTURECPP_ASSERT(size == get_size());
        SCORE_LANGUAGE_FUTURECPP_ASSERT(alignment == get_alignment());
    }

    std::size_t get_size() const { return val_ & ~0b111111_UZ; }

    std::size_t get_alignment() const { return 1_UZ << (val_ & 0b111111_UZ); }

private:
    std::size_t val_;
};

} // namespace detail
} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_CHUNK_SIZE_AND_ALIGNMENT_HPP
