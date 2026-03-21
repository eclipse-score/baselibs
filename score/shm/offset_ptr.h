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
    OffsetPtr(T const* const pointer) noexcept : pointer_{pointer} {}

    OffsetPtr(const OffsetPtr& other) noexcept = default;

    OffsetPtr& operator=(const OffsetPtr& other) noexcept = default;

    T* get() noexcept { return const_cast<T*>(pointer_); }

    const T* get() const noexcept { return pointer_; }

  private:
    const T* pointer_{nullptr};
};

template <typename T>
class NullableOffsetPtr
{
  public:
    NullableOffsetPtr(T const* const pointer) noexcept : pointer_{pointer} {}

    NullableOffsetPtr(const NullableOffsetPtr& other) noexcept = default;

    NullableOffsetPtr& operator=(const NullableOffsetPtr& other) noexcept = default;

    T* get() noexcept { return const_cast<T*>(pointer_); }

    const T* get() const noexcept { return pointer_; }

  private:
    const T* pointer_{nullptr};
};

struct ShmPointerPolicy
{
    template <typename T>
    using Ptr = OffsetPtr<T>;

    template <typename T>
    using NullablePtr = NullableOffsetPtr<T>;
};

}  // namespace score::shm

#endif  // SCORE_SHM_OFFSET_PTR_H
