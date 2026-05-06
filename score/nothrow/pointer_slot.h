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
#ifndef SCORE_NOTHROW_POINTER_SLOT_H
#define SCORE_NOTHROW_POINTER_SLOT_H

#include <cstdint>

namespace score::nothrow
{

/// @brief Stores a pointer as offset from the `OffsetSlot` object address.
///
/// Deviation from raw pointers: persisted representation is relative, which is
/// suitable for relocatable/shared-memory layouts in one process.
template <typename T>
class OffsetSlot
{
  public:
    OffsetSlot(T const* const pointer) noexcept
        : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)}
    {
    }

    OffsetSlot(const OffsetSlot& other) noexcept : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)} {}

    OffsetSlot& operator=(const OffsetSlot& other) noexcept
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
class NullableOffsetSlot
{
  public:
    /// @brief Like `OffsetSlot`, but preserves a `nullptr` state explicitly.
    NullableOffsetSlot(T const* const pointer) noexcept
        : offset_from_this_{PointerToInteger(pointer) - PointerToInteger(this)}, is_nullptr_{pointer == nullptr}
    {
    }

    NullableOffsetSlot(const NullableOffsetSlot& other) noexcept
        : offset_from_this_{PointerToInteger(other.get()) - PointerToInteger(this)},
          is_nullptr_{other.get() == nullptr}
    {
    }

    NullableOffsetSlot& operator=(const NullableOffsetSlot& other) noexcept
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
class RawSlot
{
  public:
    /// @brief Minimal wrapper that stores a raw pointer unchanged.
    RawSlot(T const* const pointer) noexcept : pointer_{pointer} {}

    RawSlot(const RawSlot& other) noexcept = default;

    RawSlot& operator=(const RawSlot& other) noexcept = default;

    T* get() noexcept { return const_cast<T*>(pointer_); }

    const T* get() const noexcept { return pointer_; }

  private:
    const T* pointer_{nullptr};
};

struct OffsetSlotPolicy
{
    /// Offset-encoded non-null pointer storage.
    template <typename T>
    using Ptr = OffsetSlot<T>;

    /// Offset-encoded nullable pointer storage.
    template <typename T>
    using NullablePtr = NullableOffsetSlot<T>;
};

struct RawSlotPolicy
{
    /// Raw pointer storage for non-null pointers.
    template <typename T>
    using Ptr = RawSlot<T>;

    /// Raw pointer storage for nullable pointers.
    template <typename T>
    using NullablePtr = RawSlot<T>;
};

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_POINTER_SLOT_H
