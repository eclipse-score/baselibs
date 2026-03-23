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
#ifndef SCORE_NOTHROW_MONOTONIC_BUFFER_RESOURCE_H
#define SCORE_NOTHROW_MONOTONIC_BUFFER_RESOURCE_H

#include "score/nothrow/memory_resource.h"

#include <cstddef>

namespace score::nothrow
{

/// @brief Nothrow counterpart to `std::pmr::monotonic_buffer_resource`.
///
/// Deviations:
/// - Allocation failure is reported as `nullptr` instead of exception.
/// - This implementation only uses the caller-provided buffer; when exhausted it
///   delegates to `upstream` and does not allocate internal growth buffers.
class MonotonicBufferResource final : public MemoryResource
{
  public:
    /// @brief Constructs from external buffer and upstream fallback resource.
    /// @param buffer Start of region; must outlive this resource.
    /// @param size Buffer size in bytes.
    /// @param upstream Fallback when local buffer is exhausted.
    explicit MonotonicBufferResource(void* buffer,
                                     std::size_t size,
                                     MemoryResource* upstream = GetDefaultResource()) noexcept;

    MonotonicBufferResource(const MonotonicBufferResource&) = delete;
    MonotonicBufferResource& operator=(const MonotonicBufferResource&) = delete;
    MonotonicBufferResource(MonotonicBufferResource&&) = delete;
    MonotonicBufferResource& operator=(MonotonicBufferResource&&) = delete;

    ~MonotonicBufferResource() override = default;

    /// @brief Resets local bump allocation state.
    ///
    /// Same intent as `std::pmr::monotonic_buffer_resource::release()` for the
    /// local buffer. Upstream allocations are unaffected.
    void release() noexcept;

    /// @brief Returns bytes still available in the local buffer.
    [[nodiscard]] std::size_t remaining() const noexcept;

    void* allocate(std::size_t bytes, std::size_t alignment) noexcept override;
    void deallocate(void* pointer, std::size_t bytes, std::size_t alignment) noexcept override;
    bool is_equal(const MemoryResource& other) const noexcept override;

  private:
    std::byte* buffer_;
    std::size_t size_;
    std::size_t offset_;
    MemoryResource* upstream_;
};

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_MONOTONIC_BUFFER_RESOURCE_H
