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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_MONOTONIC_BUFFER_RESOURCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_MONOTONIC_BUFFER_RESOURCE_HPP

#include <score/private/memory_resource/chunk_list.hpp>
#include <score/private/memory_resource/memory_resource.hpp>
#include <score/assert.hpp>
#include <score/bit.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace score::cpp
{
namespace pmr
{

/// \brief a special-purpose score::cpp::pmr::memory_resource that releases the allocated memory only when the resource is
/// destroyed
///
/// It is intended for very fast memory allocations in situations where memory is used to build up a few objects and
/// then is released all at once.
///
/// monotonic_buffer_resource can be constructed with an initial buffer. If there is no initial buffer, or if the buffer
/// is exhausted, additional buffers are obtained from an upstream memory resource supplied at construction. The size of
/// buffers obtained follows a geometric progression.
///
/// monotonic_buffer_resource is not thread-safe.
///
/// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions) Follows literaly the C++ standard
class monotonic_buffer_resource : public memory_resource
{
public:
    /// \brief Constructs a monotonic_buffer_resource.
    ///
    /// Uses the return value of score::cpp::pmr::get_default_resource as the upstream memory resource. Sets the current buffer
    /// to null and the next buffer size to an implementation-defined size.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/monotonic_buffer_resource
    monotonic_buffer_resource() : monotonic_buffer_resource{get_default_resource()} {}

    /// \brief Constructs a monotonic_buffer_resource.
    ///
    /// Sets the current buffer to null and the next buffer size to an implementation-defined size.
    ///
    /// \pre \p upstream != nullptr
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/monotonic_buffer_resource
    /// \param upstream The upstream memory resource to use; must point to a valid memory resource
    explicit monotonic_buffer_resource(memory_resource* const upstream)
        : monotonic_buffer_resource{default_initial_size, upstream}
    {
    }

    /// \brief Constructs a monotonic_buffer_resource.
    ///
    /// Uses the return value of score::cpp::pmr::get_default_resource as the upstream memory resource. Sets the current buffer
    /// to null and the next buffer size to a size no smaller than \p initial_size.
    ///
    /// \pre \p initial_size > 0
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/monotonic_buffer_resource
    ///
    /// \param initial_size The minimum size of the first buffer to allocate; must be greater than zero.
    explicit monotonic_buffer_resource(const std::size_t initial_size)
        : monotonic_buffer_resource{initial_size, get_default_resource()}
    {
    }

    /// \brief Constructs a monotonic_buffer_resource.
    ///
    /// Sets the current buffer to null and the next buffer size to a size no smaller than \p initial_size.
    ///
    /// \pre \p initial_size > 0
    /// \pre \p upstream != nullptr
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/monotonic_buffer_resource
    ///
    /// \param initial_size The minimum size of the first buffer to allocate; must be greater than zero.
    /// \param upstream The upstream memory resource to use; must point to a valid memory resource.
    monotonic_buffer_resource(const std::size_t initial_size, memory_resource* const upstream)
        : memory_resource{}
        , upstream_rsrc_{upstream}
        , original_buffer_{nullptr}
        , original_buffer_size_{std::max(initial_size, default_initial_size)}
        , current_buffer_{nullptr}
        , next_buffer_size_{original_buffer_size_}
        , available_{0U}
        , chunks_{}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(initial_size > 0U);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(upstream != nullptr);
    }

    /// \brief Constructs a monotonic_buffer_resource.
    ///
    /// Sets the current buffer to \p buffer and the next buffer size to \p buffer_size (but not less than 1). Then
    /// increase the next buffer size by an implementation-defined growth factor (which does not have to be integral).
    ///
    /// \pre \p buffer != nullptr || \p buffer_size == 0
    /// \pre \p upstream != nullptr
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/monotonic_buffer_resource
    ///
    /// \param buffer the initial buffer to use
    /// \param buffer_size the size of the initial buffer; cannot be greater than the number of bytes in \p buffer.
    /// \param upstream The upstream memory resource to use; must point to a valid memory resource.
    monotonic_buffer_resource(void* const buffer, const std::size_t buffer_size, memory_resource* const upstream)
        : memory_resource{}
        , upstream_rsrc_{upstream}
        , original_buffer_{static_cast<std::uint8_t*>(buffer)}
        , original_buffer_size_{(buffer != nullptr) ? buffer_size : compute_next_buffer_size(buffer_size)}
        , current_buffer_{original_buffer_}
        , next_buffer_size_{compute_next_buffer_size(buffer_size)}
        , available_{buffer_size}
        , chunks_{}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION((buffer != nullptr) || (buffer_size == 0U));
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(upstream != nullptr);
    }

    /// \brief Constructs a monotonic_buffer_resource.
    ///
    /// Uses the return value of score::cpp::pmr::get_default_resource as the upstream memory resource. Sets the current buffer
    /// to \p buffer and the next buffer size to \p buffer_size (but not less than 1). Then increase the next buffer
    /// size by an implementation-defined growth factor (which does not have to be integral).
    ///
    /// \pre \p buffer != nullptr || \p buffer_size == 0
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/monotonic_buffer_resource
    ///
    /// \param buffer the initial buffer to use
    /// \param buffer_size the size of the initial buffer; cannot be greater than the number of bytes in \p buffer.
    monotonic_buffer_resource(void* const buffer, const std::size_t buffer_size)
        : monotonic_buffer_resource{buffer, buffer_size, get_default_resource()}
    {
    }

    /// \brief Copy constructor is deleted.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/monotonic_buffer_resource
    monotonic_buffer_resource(const monotonic_buffer_resource&) = delete;

    /// \brief Destroys a monotonic_buffer_resource, releasing all allocated memory.
    ///
    /// Deallocates all memory owned by this resource by calling this->release().
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/%7Emonotonic_buffer_resource
    ~monotonic_buffer_resource() override { release(); }

    /// \brief Copy assignment operator is deleted. monotonic_buffer_resource is not copy assignable
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource
    monotonic_buffer_resource& operator=(const monotonic_buffer_resource&) = delete;

    /// \brief Release all allocated memory.
    ///
    /// Releases all allocated memory by calling the deallocate function on the upstream memory resource as
    /// necessary. Resets current buffer and next buffer size to their initial values at construction.
    /// Memory is released back to the upstream resource even if deallocate has not been called for some of the
    /// allocated blocks.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/release
    void release();

    /// \brief Returns a pointer to the upstream memory resource.
    ///
    /// This is the same value as the upstream argument passed to the constructor of this object.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/upstream_resource
    memory_resource* upstream_resource() const { return upstream_rsrc_; }

protected:
    /// \brief Allocates storage.
    ///
    /// If the current buffer has sufficient unused space to fit a block with the specified size and alignment,
    /// allocates the return block from the current buffer.
    ///
    /// Otherwise, this function allocates a new buffer by calling upstream_resource()->allocate(n, m), where n is not
    /// less than the greater of \p bytes and the next buffer size and m is not less than \p alignment. It sets the new
    /// buffer as the current buffer, increases the next buffer size by an implementation-defined growth factor (which
    /// is not necessarily integral), and then allocates the return block from the newly allocated buffer.
    ///
    /// \pre \p alignment is a power of 2.
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/do_allocate
    ///
    /// \return A pointer to allocated storage of at least bytes bytes in size, aligned to the specified alignment if
    /// such alignment is supported, and to alignof(std::max_align_t) otherwise.
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    /// \brief No-op
    ///
    /// This function has no effect. Memory used by a monotonic_buffer_resource, as its name indicates, increases
    /// monotonically until the resource is destroyed.
    void do_deallocate(void* /*p*/, std::size_t /*bytes*/, std::size_t /*alignment*/) override {}

    /// \brief Compare for equality with another score::cpp::pmr::memory_resource
    ///
    /// Compare *this with \p other for identity - memory allocated using a monotonic_buffer_resource can only be
    /// deallocated using that same resource.
    ///
    /// \see https://en.cppreference.com/w/cpp/memory/monotonic_buffer_resource/do_is_equal
    ///
    /// \return this == &other
    bool do_is_equal(const memory_resource& other) const noexcept override { return this == &other; }

private:
    inline static constexpr std::size_t default_initial_size{4096U};

    static constexpr std::size_t compute_next_buffer_size(const std::size_t buffer_size)
    {
        return std::max(default_initial_size, 2U * buffer_size);
    }

    memory_resource* upstream_rsrc_;
    std::uint8_t* original_buffer_;
    std::size_t original_buffer_size_;
    std::uint8_t* current_buffer_;
    std::size_t next_buffer_size_;
    std::size_t available_;
    detail::chunk_list chunks_;
};

inline void monotonic_buffer_resource::release()
{
    chunks_.release(upstream_rsrc_);
    current_buffer_ = original_buffer_;
    if (current_buffer_ == nullptr)
    {
        available_ = 0U;
        next_buffer_size_ = original_buffer_size_;
    }
    else
    {
        available_ = original_buffer_size_;
        next_buffer_size_ = compute_next_buffer_size(original_buffer_size_);
    }
}

inline void* monotonic_buffer_resource::do_allocate(const std::size_t bytes, const std::size_t alignment)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(score::cpp::has_single_bit(alignment));

    void* result{current_buffer_};
    result = std::align(alignment, bytes, result, available_);
    if (result == nullptr)
    {
        const std::size_t usable_size{std::max(bytes, next_buffer_size_)};
        current_buffer_ = chunks_.allocate(upstream_rsrc_, usable_size, alignment);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(current_buffer_ != nullptr);
        next_buffer_size_ = compute_next_buffer_size(usable_size);
        result = current_buffer_;
        available_ = usable_size;
    }

    current_buffer_ = static_cast<std::uint8_t*>(result) + bytes;
    available_ -= bytes;
    return result;
}

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_MONOTONIC_BUFFER_RESOURCE_HPP
