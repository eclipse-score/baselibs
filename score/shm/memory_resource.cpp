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

#include "score/shm/memory_resource.h"

#include <atomic>
#include <cstddef>
#include <new>

namespace score::shm
{
namespace
{

class NewDeleteMemoryResource final : public MemoryResource
{
  public:
    void* allocate(std::size_t bytes, std::size_t alignment) noexcept override
    {
        if (bytes == 0U)
        {
            return nullptr;
        }

        if (alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            return ::operator new(bytes, std::align_val_t{alignment}, std::nothrow);
        }

        return ::operator new(bytes, std::nothrow);
    }

    void deallocate(void* pointer, std::size_t bytes, std::size_t alignment) noexcept override
    {
        if (alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            ::operator delete(pointer, bytes, std::align_val_t{alignment});
        }
        else
        {
            ::operator delete(pointer, bytes);
        }
    }

    bool is_equal(const MemoryResource& other) const noexcept override
    {
        return this == &other;
    }
};

class NullMemoryResource final : public MemoryResource
{
  public:
    void* allocate(std::size_t, std::size_t) noexcept override
    {
        return nullptr;
    }

    void deallocate(void*, std::size_t, std::size_t) noexcept override {}

    bool is_equal(const MemoryResource& other) const noexcept override
    {
        return this == &other;
    }
};

NewDeleteMemoryResource& GetNewDeleteResourceInstance() noexcept
{
    static NewDeleteMemoryResource instance{};
    return instance;
}

NullMemoryResource& GetNullMemoryResourceInstance() noexcept
{
    static NullMemoryResource instance{};
    return instance;
}

std::atomic<MemoryResource*>& GetDefaultResourceAtomic() noexcept
{
    static std::atomic<MemoryResource*> resource{&GetNewDeleteResourceInstance()};
    return resource;
}

}  // namespace

MemoryResource* GetNewDeleteResource() noexcept
{
    return &GetNewDeleteResourceInstance();
}

MemoryResource* GetNullMemoryResource() noexcept
{
    return &GetNullMemoryResourceInstance();
}

MemoryResource* GetDefaultResource() noexcept
{
    return GetDefaultResourceAtomic().load(std::memory_order_acquire);
}

MemoryResource* SetDefaultResource(MemoryResource* resource) noexcept
{
    if (resource == nullptr)
    {
        resource = GetNewDeleteResource();
    }
    return GetDefaultResourceAtomic().exchange(resource, std::memory_order_acq_rel);
}

}  // namespace score::shm
