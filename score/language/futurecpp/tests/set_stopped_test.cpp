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

#include <score/utility.hpp>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace
{

struct receiver
{
    using receiver_concept = receiver_t;

    enum class action
    {
        init,
        stopped,
    };

    void set_stopped() & = delete;
    void set_stopped() && { *a = action::stopped; }
    void set_stopped() const& = delete;
    void set_stopped() const&& = delete;

    action* a{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(set_stopped_test, set_stopped_GivenRValueReceiver_ExpectSetStoppedCalled)
{
    receiver::action a{receiver::action::init};
    set_stopped(receiver{&a});

    EXPECT_EQ(a, receiver::action::stopped);
}

} // namespace
} // namespace execution
} // namespace score::cpp
