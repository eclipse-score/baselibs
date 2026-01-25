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
/// @copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/memory_resource.hpp>

#include <atomic>
#include <new>
#include <type_traits>

namespace score::cpp
{
namespace pmr
{
namespace
{

class new_delete_resource_t final : public memory_resource
{
public:
    new_delete_resource_t() = default;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        // in C++17, extended alignment can be supported by inspecting __STDCPP_DEFAULT_NEW_ALIGNMENT__
        // and selecting an overload of global operator new for extended alignment if necessary.
        // since we are stuck with C++14, we have to do this on our own
        if (alignment > alignof(std::max_align_t))
        {
            std::size_t const extended_size = bytes + sizeof(void*) + alignment - 1;
            void* p = ::operator new(extended_size);
            return score::cpp::pmr::detail::establish_extended_alignment(p, bytes, extended_size, alignment);
        }
        else
        {
            return ::operator new(bytes);
        }
    }

    void do_deallocate(void* p, std::size_t /*unused*/, std::size_t alignment) override
    {
        if (alignment > alignof(std::max_align_t))
        {
            void* orig_ptr = score::cpp::pmr::detail::retrieve_unaligned_pointer(p);
            ::operator delete(orig_ptr);
        }
        else
        {
            ::operator delete(p);
        }
    }

    bool do_is_equal(const memory_resource& other) const noexcept override { return &other == this; }
};

new_delete_resource_t* initialize_new_delete_singleton()
{
    static std::aligned_storage_t<sizeof(new_delete_resource_t), alignof(new_delete_resource_t)> storage;
    return ::new (&storage) new_delete_resource_t();
}

class null_memory_resource_t final : public memory_resource
{
public:
    null_memory_resource_t() = default;

protected:
    void* do_allocate(std::size_t /*unused*/, std::size_t /*unused*/) override { throw std::bad_alloc{}; }

    void do_deallocate(void* /*unused*/, std::size_t /*unused*/, std::size_t /*unused*/) override
    {
        // do nothing
    }

    bool do_is_equal(const memory_resource& other) const noexcept override { return &other == this; }
};

null_memory_resource_t* initialize_null_memory_singleton()
{
    static std::aligned_storage_t<sizeof(null_memory_resource_t), alignof(null_memory_resource_t)> storage;
    return ::new (&storage) null_memory_resource_t();
}

std::atomic<memory_resource*>& get_default_resource_singleton()
{
    static std::atomic<memory_resource*> resource{new_delete_resource()};
    return resource;
}

} // namespace

memory_resource* new_delete_resource() noexcept
{
    static new_delete_resource_t* const resource{score::cpp::pmr::initialize_new_delete_singleton()};
    return resource;
}

memory_resource* null_memory_resource() noexcept
{
    static null_memory_resource_t* const resource{score::cpp::pmr::initialize_null_memory_singleton()};
    return resource;
}

memory_resource* get_default_resource() noexcept { return score::cpp::pmr::get_default_resource_singleton().load(); }

memory_resource* set_default_resource(memory_resource* new_resource) noexcept
{
    if (new_resource == nullptr)
    {
        new_resource = new_delete_resource();
    }

    return score::cpp::pmr::get_default_resource_singleton().exchange(new_resource);
}

} // namespace pmr
} // namespace score::cpp
