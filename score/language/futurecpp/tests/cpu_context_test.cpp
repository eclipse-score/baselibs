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
#include <score/execution.hpp> // test include guard

#include <score/optional.hpp>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace detail
{
namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#39890557, CB-#40946023
TEST(cpu_context_test, equality_GivenCpuContext_ExpectEqual)
{
    cpu_context ctx{cpu_context::worker_count{1}};

    EXPECT_EQ(ctx.get_scheduler(), ctx.get_scheduler());
    EXPECT_FALSE(ctx.get_scheduler() != ctx.get_scheduler());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#39890557, CB-#40946023
TEST(cpu_context_test, equality_GivenTwoCpuContext_ExpectNotEqual)
{
    cpu_context ctx1{cpu_context::worker_count{1}};
    cpu_context ctx2{cpu_context::worker_count{1}};

    EXPECT_NE(ctx1.get_scheduler(), ctx2.get_scheduler());
    EXPECT_FALSE(ctx1.get_scheduler() == ctx2.get_scheduler());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946023
TEST(cpu_context_test, max_concurrency_GivenCpuContext_ExpectMaxConcurrencyReturnsNumberOfThreads)
{
    const cpu_context ctx1{cpu_context::worker_count{1}};
    const cpu_context ctx2{cpu_context::worker_count{2}};

    EXPECT_EQ(ctx1.max_concurrency(), 1);
    EXPECT_EQ(ctx2.max_concurrency(), 2);
}

struct receiver
{
public:
    using receiver_concept = receiver_t;

    enum class state : std::int32_t
    {
        initial,
        start,
        disable,
    };

    explicit receiver(std::atomic<state>& s) : state_{&s} {}

    void set_value() & = delete;
    void set_value() &&
    {
        EXPECT_EQ(state::initial, *state_);
        *state_ = state::start;
    }
    void set_value() const& = delete;
    void set_value() const&& = delete;

    void set_stopped() & = delete;
    void set_stopped() &&
    {
        EXPECT_EQ(state::initial, *state_);
        *state_ = state::disable;
    }
    void set_stopped() const& = delete;
    void set_stopped() const&& = delete;

private:
    std::atomic<state>* state_;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#39890557, CB-#40946023
TEST(cpu_context_test, start_GivenOperationStateStarted_ExpectSetValueIsCalledOnReceiverEventually)
{
    std::atomic<receiver::state> state{};

    cpu_context ctx{cpu_context::worker_count{1}};
    auto op_state = connect(schedule(ctx.get_scheduler()), receiver{state});
    start(op_state);

    while (state == receiver::state::initial)
    {
        // busy wait
    }

    EXPECT_EQ(state, receiver::state::start);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#39890557, CB-#40946023
TEST(cpu_context_test, start_GivenThreadPoolTerminated_ExpectReceiverIsNotInStateInitial)
{
    std::atomic<receiver::state> state{};
    score::cpp::optional<cpu_scheduler_op_state<receiver>> op_state{};

    {
        cpu_context ctx{cpu_context::worker_count{1}};
        op_state = connect(schedule(ctx.get_scheduler()), receiver{state});
        start(*op_state);
    }

    EXPECT_NE(state, receiver::state::initial);
}

struct counting_receiver
{
public:
    using receiver_concept = receiver_t;

    enum class state : std::int32_t
    {
        initial,
        finished,
    };

    explicit counting_receiver(std::atomic<state>& s) : state_{&s}, count_{0} {}

    void set_value()
    {
        ++count_;
        EXPECT_EQ(state::initial, *state_);
        *state_ = state::finished;
    }

    void set_stopped() { FAIL() << "shall not be called"; }

    std::int32_t get() const { return count_; }

private:
    std::atomic<state>* state_;
    std::int32_t count_;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(cpu_context_test, set_value_GivenReceiver_ExpectCountIsIncrementedAndStateIsFinished)
{
    std::atomic<counting_receiver::state> state{};
    counting_receiver receiver{state};

    EXPECT_EQ(0, receiver.get());
    EXPECT_EQ(state, counting_receiver::state::initial);

    receiver.set_value();

    EXPECT_EQ(1, receiver.get());
    EXPECT_EQ(state, counting_receiver::state::finished);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#39890557, CB-#40946837
TEST(cpu_context_test, start_GivenOperationStateStarted_ExpectSetValueIsCalledOnCopyOfReceiver)
{
    std::atomic<counting_receiver::state> state{};
    counting_receiver receiver{state};

    cpu_context ctx{cpu_context::worker_count{1}};
    auto op_state = connect(schedule(ctx.get_scheduler()), receiver);
    start(op_state);

    while (state == counting_receiver::state::initial)
    {
        // busy wait
    }

    EXPECT_EQ(0, receiver.get());
    EXPECT_EQ(state, counting_receiver::state::finished);
}

} // namespace
} // namespace detail
} // namespace execution
} // namespace score::cpp
