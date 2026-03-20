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

#include "score/shm/monotonic_buffer_resource.h"

#include <cstddef>

namespace score::shm
{

MonotonicBufferResource::MonotonicBufferResource(void* buffer,
                                                 const std::size_t size,
                                                 MemoryResource* upstream) noexcept
    : buffer_{static_cast<std::byte*>(buffer)}, size_{size}, offset_{0U}, upstream_{upstream}
{
}

void MonotonicBufferResource::release() noexcept
{
    offset_ = 0U;
}

std::size_t MonotonicBufferResource::remaining() const noexcept
{
    return size_ - offset_;
}

void* MonotonicBufferResource::allocate(const std::size_t bytes, const std::size_t alignment) noexcept
{
    if (bytes == 0U)
    {
        return nullptr;
    }

    std::byte* const base = buffer_;
    const auto current_addr = reinterpret_cast<std::uintptr_t>(base + offset_);
    const std::size_t padding = (alignment - (current_addr % alignment)) % alignment;
    const std::size_t total = padding + bytes;

    if (total <= (size_ - offset_))
    {
        void* const result = base + offset_ + padding;
        offset_ += total;
        return result;
    }

    return upstream_->allocate(bytes, alignment);
}

void MonotonicBufferResource::deallocate(void*, std::size_t, std::size_t) noexcept {}

bool MonotonicBufferResource::is_equal(const MemoryResource& other) const noexcept
{
    return this == &other;
}

}  // namespace score::shm
