/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_AWARE_ERASED_TYPE_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_AWARE_ERASED_TYPE_H

#include "score/language/safecpp/scoped_function/details/type_erasure_pointer.h"

#include <score/memory.hpp>

namespace score::safecpp::details
{

template <class Interface>
class AllocatorAwareErasedType
{
  protected:
    constexpr AllocatorAwareErasedType() noexcept = default;
    constexpr AllocatorAwareErasedType(const AllocatorAwareErasedType&) = default;
    constexpr AllocatorAwareErasedType(AllocatorAwareErasedType&&) noexcept = default;
    constexpr AllocatorAwareErasedType& operator=(const AllocatorAwareErasedType&) = default;
    constexpr AllocatorAwareErasedType& operator=(AllocatorAwareErasedType&&) noexcept = default;

  public:
    virtual ~AllocatorAwareErasedType() noexcept = default;

    [[nodiscard]] virtual TypeErasurePointer<Interface> Copy(
        score::cpp::pmr::polymorphic_allocator<Interface>) const = 0;

    [[nodiscard]] virtual TypeErasurePointer<Interface> Move(
        score::cpp::pmr::polymorphic_allocator<Interface>) noexcept = 0;
};

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_ALLOCATOR_AWARE_ERASED_TYPE_H
