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
#include "score/containers/test/allocator_test_type_helpers.h"

namespace score::containers
{

std::size_t NonMoveableAndCopyableElementType::non_moveable_element_dtor_count{0U};

std::size_t SpecialMemberFunctionRecordingType::constructor_count{0U};
std::size_t SpecialMemberFunctionRecordingType::destructor_count{0U};
std::size_t SpecialMemberFunctionRecordingType::copy_constructor_count{0U};
std::size_t SpecialMemberFunctionRecordingType::copy_assignment_count{0U};
std::size_t SpecialMemberFunctionRecordingType::move_constructor_count{0U};
std::size_t SpecialMemberFunctionRecordingType::move_assignment_count{0U};

auto operator==(const TriviallyConstructibleDestructibleType& lhs,
                const TriviallyConstructibleDestructibleType& rhs) noexcept -> bool
{
    return (lhs.i == rhs.i && lhs.j == rhs.j);
}

}  // namespace score::containers
