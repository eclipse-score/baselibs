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
#ifndef SCORE_NOTHROW_POINTER_BOX_H
#define SCORE_NOTHROW_POINTER_BOX_H

#include <cstdint>

namespace score::nothrow
{

template <typename T>
class OffsetBox
{
  public:
    OffsetBox(T const* const pointer) noexcept
        : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)}
    {
    }

    OffsetBox(const OffsetBox& other) noexcept : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)} {}

    OffsetBox& operator=(const OffsetBox& other) noexcept
    {
        offset_from_this_ = PointerToInteger(other.get()) - PointerToInteger(this);
        return *this;
    }

    T* get() noexcept { return IntegerToPointer(PointerToInteger(this) + offset_from_this_); }

    const T* get() const noexcept { return IntegerToConstPointer(PointerToInteger(this) + offset_from_this_); }

  private:
    static std::intptr_t PointerToInteger(const void* const pointer) noexcept
    {
        return reinterpret_cast<std::intptr_t>(pointer);
    }

    static T* IntegerToPointer(const std::intptr_t value) noexcept
    {
        return static_cast<T*>(reinterpret_cast<void*>(value));
    }

    static const T* IntegerToConstPointer(const std::intptr_t value) noexcept
    {
        return static_cast<const T*>(reinterpret_cast<const void*>(value));
    }

    std::intptr_t offset_from_this_{};
};

template <typename T>
class NullableOffsetBox
{
  public:
    NullableOffsetBox(T const* const pointer) noexcept
        : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)}, is_nullptr_{pointer == nullptr}
    {
    }

    NullableOffsetBox(const NullableOffsetBox& other) noexcept
        : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)},
          is_nullptr_{other.get() == nullptr}
    {
    }

    NullableOffsetBox& operator=(const NullableOffsetBox& other) noexcept
    {
        offset_from_this_ = PointerToInteger(other.get()) - PointerToInteger(this);
        is_nullptr_ = other.get() == nullptr;
        return *this;
    }

    T* get() noexcept
    {
        return is_nullptr_ ? nullptr : IntegerToPointer(PointerToInteger(this) + offset_from_this_);
    }

    const T* get() const noexcept
    {
        return is_nullptr_ ? nullptr : IntegerToConstPointer(PointerToInteger(this) + offset_from_this_);
    }

  private:
    static std::intptr_t PointerToInteger(const void* const pointer) noexcept
    {
        return reinterpret_cast<std::intptr_t>(pointer);
    }

    static T* IntegerToPointer(const std::intptr_t value) noexcept
    {
        return static_cast<T*>(reinterpret_cast<void*>(value));
    }

    static const T* IntegerToConstPointer(const std::intptr_t value) noexcept
    {
        return static_cast<const T*>(reinterpret_cast<const void*>(value));
    }

    std::intptr_t offset_from_this_{};
    bool is_nullptr_{true};
};

template <typename T>
class RawBox
{
  public:
    RawBox(T const* const pointer) noexcept : pointer_{pointer} {}

    RawBox(const RawBox& other) noexcept = default;

    RawBox& operator=(const RawBox& other) noexcept = default;

    T* get() noexcept { return const_cast<T*>(pointer_); }

    const T* get() const noexcept { return pointer_; }

  private:
    const T* pointer_{nullptr};
};

struct OffsetBoxPolicy
{
    template <typename T>
    using Ptr = OffsetBox<T>;

    template <typename T>
    using NullablePtr = NullableOffsetBox<T>;
};

struct RawBoxPolicy
{
    template <typename T>
    using Ptr = RawBox<T>;

    template <typename T>
    using NullablePtr = RawBox<T>;
};

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_POINTER_BOX_H
