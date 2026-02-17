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

#include <gtest/gtest.h>

#include <cstdint>
#include <utility>

namespace score::cpp
{
namespace execution
{
namespace
{

enum class category
{
    const_lvalue,
    const_rvalue,
    lvalue,
    rvalue,
};

struct receiver
{
    using receiver_concept = receiver_t;
};

struct op_state
{
    using operation_state_concept = operation_state_t;

    category sender;
    category receiver;
};

struct sender
{
    using sender_concept = sender_t;

    op_state connect(receiver&) & { return {category::lvalue, category::lvalue}; }
    op_state connect(receiver&&) && { return {category::rvalue, category::rvalue}; }
    op_state connect(const receiver&) const& { return {category::const_lvalue, category::const_lvalue}; }
    op_state connect(const receiver&&) const&& { return {category::const_rvalue, category::const_rvalue}; }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(connect_test, connect_GivenRValueSenderAndReceiver_ExpectConnectsSenderAndReceiverInAnOperationState)
{
    sender s{};
    receiver r{};
    const op_state op{connect(std::move(s), std::move(r))};

    EXPECT_EQ(op.sender, category::rvalue);
    EXPECT_EQ(op.receiver, category::rvalue);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(connect_test, connect_GivenLValueSenderAndReceiver_ExpectConnectsSenderAndReceiverInAnOperationState)
{
    sender s{};
    receiver r{};
    const op_state op{connect(s, r)};

    EXPECT_EQ(op.sender, category::lvalue);
    EXPECT_EQ(op.receiver, category::lvalue);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(connect_test, connect_GivenConstRValueSenderAndReceiver_ExpectConnectsSenderAndReceiverInAnOperationState)
{
    const sender s{};
    const receiver r{};
    const op_state op{connect(std::move(s), std::move(r))};

    EXPECT_EQ(op.sender, category::const_rvalue);
    EXPECT_EQ(op.receiver, category::const_rvalue);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(connect_test, connect_GivenConstLValueSenderAndReceiver_ExpectConnectsSenderAndReceiverInAnOperationState)
{
    const sender s{};
    const receiver r{};
    const op_state op{connect(s, r)};

    EXPECT_EQ(op.sender, category::const_lvalue);
    EXPECT_EQ(op.receiver, category::const_lvalue);
}

} // namespace
} // namespace execution
} // namespace score::cpp
