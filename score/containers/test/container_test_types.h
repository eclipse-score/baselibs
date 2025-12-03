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
#ifndef SCORE_LIB_CONTAINERS_TEST_CONTAINER_TEST_TYPES_H
#define SCORE_LIB_CONTAINERS_TEST_CONTAINER_TEST_TYPES_H

#include <cstddef>
#include <cstdint>

namespace score::containers
{

class NonTrivialType
{
  public:
    NonTrivialType() : member_1_{42U}, member_2_{1.0f} {}
    NonTrivialType(std::uint32_t member_1, float member_2) : member_1_{member_1}, member_2_{member_2} {}

    std::uint32_t member_1_;
    float member_2_;
};

using TrivialType = std::uint32_t;

/// \brief A more elaborated/complex trivial type compared to the TrivialType above.
struct TriviallyConstructibleDestructibleType
{
    char i;
    std::uint64_t j;
};

auto operator==(const TriviallyConstructibleDestructibleType& lhs,
                const TriviallyConstructibleDestructibleType& rhs) noexcept -> bool;

struct NonMoveableAndCopyableElementType
{
    NonMoveableAndCopyableElementType() = default;
    ~NonMoveableAndCopyableElementType()
    {
        non_moveable_element_dtor_count++;
    }

    NonMoveableAndCopyableElementType(const NonMoveableAndCopyableElementType&) = delete;
    NonMoveableAndCopyableElementType& operator=(const NonMoveableAndCopyableElementType&) = delete;
    NonMoveableAndCopyableElementType(NonMoveableAndCopyableElementType&&) = delete;
    NonMoveableAndCopyableElementType& operator=(NonMoveableAndCopyableElementType&&) = delete;

    static void ResetDestructorCount()
    {
        non_moveable_element_dtor_count = 0U;
    }

    static std::size_t GetDestructorCount()
    {
        return non_moveable_element_dtor_count;
    }

    int i_;
    static std::size_t non_moveable_element_dtor_count;
};

struct SpecialMemberFunctionRecordingType
{
    SpecialMemberFunctionRecordingType()
    {
        constructor_count++;
    }
    ~SpecialMemberFunctionRecordingType()
    {
        destructor_count++;
    }

    SpecialMemberFunctionRecordingType(const SpecialMemberFunctionRecordingType&)
    {
        copy_constructor_count++;
    }

    SpecialMemberFunctionRecordingType& operator=(const SpecialMemberFunctionRecordingType&)
    {
        copy_assignment_count++;
        return *this;
    }

    SpecialMemberFunctionRecordingType(SpecialMemberFunctionRecordingType&&)
    {
        move_constructor_count++;
    }

    SpecialMemberFunctionRecordingType& operator=(SpecialMemberFunctionRecordingType&&)
    {
        move_assignment_count++;
        return *this;
    }

    static void Reset()
    {
        constructor_count = 0U;
        destructor_count = 0U;
        copy_constructor_count = 0U;
        copy_assignment_count = 0U;
        move_constructor_count = 0U;
        move_assignment_count = 0U;
    }

    static std::size_t constructor_count;
    static std::size_t destructor_count;
    static std::size_t copy_constructor_count;
    static std::size_t copy_assignment_count;
    static std::size_t move_constructor_count;
    static std::size_t move_assignment_count;
};

}  // namespace score::containers

#endif  // SCORE_LIB_CONTAINERS_TEST_CONTAINER_TEST_TYPES_H
