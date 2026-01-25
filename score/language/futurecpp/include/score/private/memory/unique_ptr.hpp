/********************************************************************************
 * Copyright (c) 2018 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2018 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Memory component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNIQUE_PTR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNIQUE_PTR_HPP

#include <score/private/memory/unique_ptr_deleter.hpp> // IWYU pragma: export
#include <score/assert.hpp>
#include <score/memory_resource.hpp>

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace score::cpp
{

namespace pmr
{

/// A unique pointer with a deleter that uses a polymorphic memory resource.
///
/// Refer to https://en.cppreference.com/w/cpp/memory/unique_ptr for an API description.
///
/// The following APIs are not available, because they don't allow to "reset" the deleter:
/// \code
/// explicit unique_ptr( pointer p ) noexcept;
/// void reset( pointer ptr = pointer() ) noexcept;
/// \endcode
///
/// Also support for std::auto_ptr is dropped.
template <typename T>
class unique_ptr
{
public:
    using pointer = typename std::unique_ptr<T, score::cpp::pmr::detail::unique_ptr_deleter>::pointer;
    using element_type = typename std::unique_ptr<T, score::cpp::pmr::detail::unique_ptr_deleter>::element_type;
    using deleter_type = typename std::unique_ptr<T, score::cpp::pmr::detail::unique_ptr_deleter>::deleter_type;

    constexpr unique_ptr() noexcept = default;

    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr unique_ptr(std::nullptr_t) noexcept : ptr_{} {}
    unique_ptr(pointer p, score::cpp::pmr::detail::unique_ptr_deleter&& r) noexcept
        : ptr_{p, std::forward<score::cpp::pmr::detail::unique_ptr_deleter>(r)}
    {
    }
    unique_ptr(unique_ptr&& other) noexcept
        : ptr_{other.release(), std::forward<score::cpp::pmr::detail::unique_ptr_deleter>(other.get_deleter())}
    {
    }
    template <typename U>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    unique_ptr(unique_ptr<U>&& other) noexcept
        : ptr_{other.release(), std::forward<score::cpp::pmr::detail::unique_ptr_deleter>(other.get_deleter())}
    {
    }

    unique_ptr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }
    unique_ptr& operator=(unique_ptr&& other) noexcept
    {
        ptr_.reset(other.release());
        ptr_.get_deleter() = std::forward<score::cpp::pmr::detail::unique_ptr_deleter>(other.get_deleter());
        return *this;
    }
    template <typename U>
    unique_ptr& operator=(unique_ptr<U>&& other) noexcept
    {
        ptr_.reset(other.release());
        ptr_.get_deleter() = std::forward<score::cpp::pmr::detail::unique_ptr_deleter>(other.get_deleter());
        return *this;
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    ~unique_ptr() = default;

    pointer release() noexcept { return ptr_.release(); }
    void reset() noexcept { ptr_.reset(); }
    void swap(unique_ptr& other) noexcept { ptr_.swap(other); }
    pointer get() const noexcept { return ptr_.get(); }
    score::cpp::pmr::detail::unique_ptr_deleter& get_deleter() noexcept { return ptr_.get_deleter(); }
    const score::cpp::pmr::detail::unique_ptr_deleter& get_deleter() const noexcept { return ptr_.get_deleter(); }
    explicit operator bool() const noexcept { return ptr_.operator bool(); }
    typename std::add_lvalue_reference<T>::type operator*() const { return ptr_.operator*(); }
    pointer operator->() const noexcept { return ptr_.operator->(); }

    template <typename U>
    bool operator==(const unique_ptr<U>& other) const
    {
        return ptr_ == other.ptr_;
    }
    template <typename U>
    bool operator<(const unique_ptr<U>& other) const
    {
        return ptr_ < other.ptr_;
    }

    bool operator==(std::nullptr_t) const noexcept { return ptr_ == nullptr; }
    bool operator<(std::nullptr_t) const { return ptr_ < nullptr; }

private:
    std::unique_ptr<T, score::cpp::pmr::detail::unique_ptr_deleter> ptr_;
};

template <typename T, typename U>
bool operator!=(const score::cpp::pmr::unique_ptr<T>& lhs, const score::cpp::pmr::unique_ptr<U>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename U>
bool operator<=(const score::cpp::pmr::unique_ptr<T>& lhs, const score::cpp::pmr::unique_ptr<U>& rhs)
{
    return !(rhs < lhs);
}

template <typename T, typename U>
bool operator>(const score::cpp::pmr::unique_ptr<T>& lhs, const score::cpp::pmr::unique_ptr<U>& rhs)
{
    return rhs < lhs;
}

template <typename T, typename U>
bool operator>=(const score::cpp::pmr::unique_ptr<T>& lhs, const score::cpp::pmr::unique_ptr<U>& rhs)
{
    return !(lhs < rhs);
}

template <typename T>
bool operator==(std::nullptr_t, const score::cpp::pmr::unique_ptr<T>& rhs) noexcept
{
    return rhs == nullptr;
}

template <typename T>
bool operator!=(const score::cpp::pmr::unique_ptr<T>& lhs, std::nullptr_t) noexcept
{
    return !(lhs == nullptr);
}
template <typename T>
bool operator!=(std::nullptr_t, const score::cpp::pmr::unique_ptr<T>& rhs) noexcept
{
    return !(rhs == nullptr);
}

template <typename T>
bool operator<(std::nullptr_t, const score::cpp::pmr::unique_ptr<T>& rhs)
{
    return nullptr < rhs;
}

template <typename T>
bool operator<=(const score::cpp::pmr::unique_ptr<T>& lhs, std::nullptr_t)
{
    return !(nullptr < lhs);
}
template <typename T>
bool operator<=(std::nullptr_t, const score::cpp::pmr::unique_ptr<T>& rhs)
{
    return !(rhs < nullptr);
}

template <typename T>
bool operator>(const score::cpp::pmr::unique_ptr<T>& lhs, std::nullptr_t)
{
    return nullptr < lhs;
}
template <typename T>
bool operator>(std::nullptr_t, const score::cpp::pmr::unique_ptr<T>& rhs)
{
    return rhs < nullptr;
}

template <typename T>
bool operator>=(const score::cpp::pmr::unique_ptr<T>& lhs, std::nullptr_t)
{
    return !(lhs < nullptr);
}
template <typename T>
bool operator>=(std::nullptr_t, const score::cpp::pmr::unique_ptr<T>& rhs)
{
    return !(nullptr < rhs);
}

/// Creates a unique pointer that manages a new object allocated using a memory resource.
template <typename T, typename... Args>
score::cpp::pmr::unique_ptr<T> make_unique(const score::cpp::pmr::polymorphic_allocator<T>& allocator, Args&&... args)
{
    static_assert(!std::is_array<T>::value, "array types are not supported");
    score::cpp::pmr::polymorphic_allocator<T> non_const_allocator{allocator};

    T* const p{non_const_allocator.allocate(1U)};
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(p != nullptr);
    non_const_allocator.construct(p, std::forward<Args>(args)...);

    return score::cpp::pmr::unique_ptr<T>(
        p, score::cpp::pmr::detail::unique_ptr_deleter(non_const_allocator.resource(), sizeof(T), alignof(T)));
}

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_UNIQUE_PTR_HPP
