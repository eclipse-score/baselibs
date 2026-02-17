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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_CHUNK_LIST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_CHUNK_LIST_HPP

#include <score/private/memory_resource/chunk_size_and_alignment.hpp>
#include <score/private/memory_resource/memory_resource.hpp>

#include <score/assert.hpp>
#include <score/bit.hpp>
#include <score/size.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace score::cpp
{
namespace pmr
{
namespace detail
{

class chunk_list
{
    struct element
    {
        chunk_size_and_alignment properties;
        element* next;
        element* prev;
    };

public:
    std::uint8_t* allocate(memory_resource* const resource, const std::size_t bytes, const std::size_t alignment)
    {
        // To support `release()`, `chunk_list` has to keep track of all allocated chunks together with their size and
        // alignment. To that end an amount of memory of size `s` greater than the amount requested by the caller
        // (`bytes`) plus `sizeof(element)` is allocated upstream so that an `element` can be stored in the chunk
        // together with the user data.
        // To avoid wasting memory, the `element` is stored at the *end* of the chunk. Storing it at the beginning would
        // mean that the size of the padding area between `element` and user data would depend on the requested
        // `alignment`, which can become almost arbitrarily large.

        // Care is taken to both ...
        // * ... satisfy the requested `alignment`: The pointer returned from `do_allocate` is the one returned from
        //   upstream_rsrc_->allocate, where we request an alignment that is at least as strict as the requested
        //   `alignment`.
        // * ... and ensure that the address of the `element` is properly aligned: The address of `element` is the sum
        //   of three terms divisible by `alignof(element)`, which is therefore divisible by `alignof(element)` as
        //   well, i.e. aligned:
        // ** `result` has an alignment that is as least as strict as alignof(element).
        // ** `s` is a multiple of 64, which is divisible by `alignof(element)`, i.e. 8.
        // ** `sizeof(detail::element)`, 24, is divisible by `alignof(element)` as well.

        // `chunk_list` always allocates chunks that have a size divisible by 64 so that the last six bits of the chunk
        // size are zeros. To reduce the size of the `element`, it uses these zeros to store the base-2 logarithm of the
        // chunk alignment.
        // <------------------ s --------------->
        // |------------------------------------|
        // |-------------------------------|----|
        // ^                               ^
        // result                          data_
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(resource != nullptr);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(score::cpp::has_single_bit(alignment));
        const std::size_t s{internal_size(bytes)};
        const std::size_t a{internal_alignment(alignment)};
        auto* const result = static_cast<std::uint8_t*>(resource->allocate(s, a));
        SCORE_LANGUAGE_FUTURECPP_ASSERT(result != nullptr);
        link(::new (get_element(result, s)) element{{s, a}, nullptr, nullptr});
        return result;
    }

    void
    deallocate(memory_resource* const resource, void* const p, const std::size_t bytes, const std::size_t alignment)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(resource != nullptr);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(p != nullptr);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(score::cpp::has_single_bit(alignment));
        const std::size_t s{internal_size(bytes)};
        const std::size_t a{internal_alignment(alignment)};
        unlink(reinterpret_cast<const element*>(get_element(p, s)));
        resource->deallocate(p, s, a);
    }

    void release(memory_resource* const resource)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(resource != nullptr);
        while (head_ != nullptr)
        {
            element* const descriptor{std::exchange(head_, head_->next)};
            resource->deallocate(
                get_chunk(descriptor), descriptor->properties.get_size(), descriptor->properties.get_alignment());
        }
    }

    static std::size_t internal_alignment(const std::size_t alignment) { return std::max(alignment, alignof(element)); }

    static std::size_t internal_size(const std::size_t bytes) { return score::cpp::align_up(bytes + sizeof(element), 64_UZ); }

private:
    static void* get_element(void* const chunk, const std::size_t chunk_size)
    {
        return static_cast<std::uint8_t*>(chunk) + chunk_size - sizeof(element);
    }

    static void* get_chunk(element* const descriptor)
    {
        return reinterpret_cast<std::uint8_t*>(descriptor + 1U) - descriptor->properties.get_size();
    }

    void link(element* const descriptor)
    {
        if (head_ != nullptr)
        {
            head_->prev = descriptor;
        }
        descriptor->next = head_;
        head_ = descriptor;
    }

    void unlink(const element* const descriptor)
    {
        element* const next{descriptor->next};
        element* const prev{descriptor->prev};
        if (next != nullptr)
        {
            SCORE_LANGUAGE_FUTURECPP_ASSERT(next->prev == descriptor);
            next->prev = prev;
        }
        if (prev != nullptr)
        {
            SCORE_LANGUAGE_FUTURECPP_ASSERT(prev->next == descriptor);
            prev->next = next;
        }
        if (head_ == descriptor)
        {
            head_ = next;
        }
    }

    element* head_{};
};

} // namespace detail
} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_CHUNK_LIST_HPP
