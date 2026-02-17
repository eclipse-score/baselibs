/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

#include <score/execution.hpp>
#include <score/execution.hpp> //  // test include guard

#include <cstdint>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace
{

struct op_state
{
    using operation_state_concept = operation_state_t;

    enum class action
    {
        init,
        non_const_call,
        const_call,
    };

    void start() & { *a = action::non_const_call; }
    void start() && = delete;
    void start() const& { *a = action::const_call; }
    void start() const&& = delete;

    action* a;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(start_test, start_GivenNonConstOperation_ExpectStartIsCalledOnOperation)
{
    op_state::action a{op_state::action::init};
    op_state op{&a};
    start(op);
    EXPECT_EQ(op_state::action::non_const_call, a);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(start_test, start_GivenConstOperation_ExpectStartIsCalledOnOperation)
{
    op_state::action a{op_state::action ::init};
    const op_state op{&a};
    start(op);
    EXPECT_EQ(op_state::action::const_call, a);
}

} // namespace
} // namespace execution
} // namespace score::cpp
