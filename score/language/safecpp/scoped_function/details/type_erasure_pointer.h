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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_TYPE_ERASURE_POINTER_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_TYPE_ERASURE_POINTER_H

#include <score/assert.hpp>
#include <score/memory.hpp>

#include <cstdint>
#include <memory>

namespace score::safecpp::details
{

class TypeErasurePointerDeleter
{
  public:
    template <class Implementation>
    explicit TypeErasurePointerDeleter(const score::cpp::pmr::polymorphic_allocator<Implementation>& allocator) noexcept
        // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
        // Rationale: There is not decltype or typeid.
        // coverity[autosar_cpp14_a5_1_7_violation]
        : memory_resource_{allocator.resource()}, size_{sizeof(Implementation)}, align_{alignof(Implementation)}
    {
    }

    TypeErasurePointerDeleter(const TypeErasurePointerDeleter& rhs) = delete;
    TypeErasurePointerDeleter& operator=(const TypeErasurePointerDeleter& rhs) = delete;

    TypeErasurePointerDeleter(TypeErasurePointerDeleter&& rhs) noexcept = default;
    TypeErasurePointerDeleter& operator=(TypeErasurePointerDeleter&& rhs) noexcept;

    ~TypeErasurePointerDeleter() = default;

    // Suppress "AUTOSAR C++14 A8-4-10" rule finding.
    // This rule states:"A parameter shall be passed by reference if it can't be NULL.".
    // Rationale: The signature is defined by the standard library for the deleter concept.
    template <typename Interface>
    // coverity[autosar_cpp14_a8_4_10_violation]
    void operator()(Interface* const interface_pointer) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(memory_resource_ != nullptr,
                                                          "Deallocation failed with nullptr memory resource");
        interface_pointer->~Interface();
        memory_resource_->deallocate(interface_pointer, size_, align_);
    }

  private:
    score::cpp::pmr::memory_resource* memory_resource_{nullptr};
    std::size_t size_{0U};
    std::size_t align_{0U};
};

template <class T>
// Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
// Rationale: There is not decltype or typeid.
// coverity[autosar_cpp14_a5_1_7_violation]
using TypeErasurePointer = std::unique_ptr<T, TypeErasurePointerDeleter>;

template <class Implementation, class... Args>
TypeErasurePointer<Implementation> MakeTypeErasurePointer(
    score::cpp::pmr::polymorphic_allocator<Implementation> allocator,
    Args&&... args)
{
    constexpr std::size_t number_of_elements{1U};
    Implementation* const implementation = allocator.allocate(number_of_elements);
    allocator.construct(implementation, std::forward<Args>(args)...);

    return TypeErasurePointer<Implementation>{implementation, TypeErasurePointerDeleter{allocator}};
}

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_TYPE_ERASURE_POINTER_H
