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
#ifndef SCORE_SHM_OFFSET_PTR_H
#define SCORE_SHM_OFFSET_PTR_H

#include <cstdint>

namespace score::shm
{

template <typename T>
class OffsetPtr
{
  public:
    OffsetPtr(T const* const pointer) noexcept : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)} {}

    OffsetPtr(const OffsetPtr& other) noexcept
        : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)}
    {
    }

    OffsetPtr& operator=(const OffsetPtr& other) noexcept
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
class NullableOffsetPtr
{
  public:
    NullableOffsetPtr(T const* const pointer) noexcept
        : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)}, is_nullptr_{pointer == nullptr}
    {
    }

    NullableOffsetPtr(const NullableOffsetPtr& other) noexcept
        : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)},
          is_nullptr_{other.get() == nullptr}
    {
    }

    NullableOffsetPtr& operator=(const NullableOffsetPtr& other) noexcept
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

}  // namespace score::shm

#endif  // SCORE_SHM_OFFSET_PTR_H
