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
#ifndef SCORE_SHM_MONOTONIC_BUFFER_RESOURCE_H
#define SCORE_SHM_MONOTONIC_BUFFER_RESOURCE_H

#include "score/shm/memory_resource.h"

#include <cstddef>

namespace score::shm
{

/// @brief A memory resource that allocates from a fixed buffer by advancing a pointer.
///
/// Follows the std::pmr::monotonic_buffer_resource model with the same nothrow
/// contract as score::shm::MemoryResource: allocation failure returns nullptr
/// instead of throwing, allowing score::shm containers to propagate errors via
/// score::Result.
///
/// Does not allocate additional buffers internally; on buffer exhaustion,
/// delegates to the upstream resource (defaults to GetDefaultResource()).
/// Individual deallocate() calls are no-ops. Use release() to reclaim the
/// entire buffer for reuse.
class MonotonicBufferResource final : public MemoryResource
{
  public:
    /// @brief Constructs from an externally owned buffer with an upstream overflow resource.
    /// @param buffer Start of the memory region. Must remain valid for the resource lifetime.
    /// @param size Size of the buffer in bytes.
    /// @param upstream Resource used when the buffer is exhausted. Defaults to GetDefaultResource().
    explicit MonotonicBufferResource(void* buffer,
                                     std::size_t size,
                                     MemoryResource* upstream = GetDefaultResource()) noexcept;

    MonotonicBufferResource(const MonotonicBufferResource&) = delete;
    MonotonicBufferResource& operator=(const MonotonicBufferResource&) = delete;
    MonotonicBufferResource(MonotonicBufferResource&&) = delete;
    MonotonicBufferResource& operator=(MonotonicBufferResource&&) = delete;

    ~MonotonicBufferResource() override = default;

    /// @brief Resets the bump pointer to the beginning of the buffer.
    ///
    /// Does not release upstream allocations. After release(), the full buffer
    /// is available for new allocations again.
    void release() noexcept;

    /// @brief Returns the number of bytes remaining in the buffer.
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

}  // namespace score::shm

#endif  // SCORE_SHM_MONOTONIC_BUFFER_RESOURCE_H
