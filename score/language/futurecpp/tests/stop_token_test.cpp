/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \file
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

#include <score/stop_token.hpp>
#include <score/stop_token.hpp> // check include guard

#include <score/jthread.hpp>

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>

#include <gtest/gtest.h>

namespace
{

/// \Test [stopsource.constr].1, [stopsource.constr].2, [stopsource.mem].2
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, default_constructor)
{
    score::cpp::stop_source unit{};

    EXPECT_TRUE(unit.stop_possible());
    EXPECT_FALSE(unit.stop_requested());
}

/// \Test [stopsource.constr].4, [stopsource.mem].2
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, nostopstate_t_constructor)
{
    score::cpp::stop_source unit{score::cpp::nostopstate};

    EXPECT_FALSE(unit.stop_possible());
    EXPECT_FALSE(unit.stop_requested());
}

/// \Test [stopsource.constr].5, [stopsource.cmp].1, [stopsource.cmp].2
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, copy_constuction)
{
    score::cpp::stop_source unit_with_state{};
    score::cpp::stop_source unit_without_state{score::cpp::nostopstate};

    score::cpp::stop_source unit_with_state_copy{unit_with_state};
    score::cpp::stop_source unit_without_state_copy{unit_without_state};

    EXPECT_EQ(unit_with_state, unit_with_state_copy);
    EXPECT_EQ(unit_without_state, unit_without_state_copy);
    EXPECT_NE(unit_with_state, unit_without_state);
}

/// \Test [stopsource.constr].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, move_construction)
{
    score::cpp::stop_source unit{};

    score::cpp::stop_source unit_move_constructed{std::move(unit)};

    EXPECT_FALSE(unit.stop_possible());
    EXPECT_TRUE(unit_move_constructed.stop_possible());
}

/// \Test [stopsource.assign].1, [stopsource.assign].2, [stoptoken.mem].2
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, copy_assignment_construct)
{
    score::cpp::stop_source unit_with_state{};
    score::cpp::stop_source unit_without_state{score::cpp::nostopstate};

    score::cpp::stop_source unit_with_state_copy;
    // a stop_token from later copy target, which references its stop_state
    score::cpp::stop_token token = unit_with_state_copy.get_token();
    // expect, that the shared stop_state this token refers to, returns, that stop is possible
    EXPECT_TRUE(token.stop_possible());
    score::cpp::stop_source unit_without_state_copy;
    score::cpp::stop_source unit_without_initial_state{score::cpp::nostopstate};

    unit_with_state_copy = unit_with_state;
    unit_without_state_copy = unit_without_state;
    unit_without_initial_state = unit_with_state;

    EXPECT_EQ(unit_with_state, unit_with_state_copy);
    EXPECT_EQ(unit_without_state, unit_without_state_copy);
    EXPECT_EQ(unit_with_state, unit_without_initial_state);
    EXPECT_FALSE(unit_with_state == unit_without_state);
    // expect, that after copy assign of unit_with_state_copy, stop on its old stop_state isn't possible anymore.
    EXPECT_FALSE(token.stop_possible());
}

/// \Test [stopsource.assign].3, [stopsource.assign].4, [stoptoken.mem].2
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, move_assignment_construction)
{
    score::cpp::stop_source unit{};

    score::cpp::stop_source unit_move_constructed;
    // a stop_token from later move target, which references its stop_state
    score::cpp::stop_token token = unit_move_constructed.get_token();
    // expect, that the shared stop_state this token refers to, returns, that stop is possible
    EXPECT_TRUE(token.stop_possible());
    unit_move_constructed = std::move(unit);

    EXPECT_FALSE(unit.stop_possible());
    EXPECT_TRUE(unit_move_constructed.stop_possible());
    // expect, that after move assign of unit_move_constructed, stop on its old stop_state isn't possible anymore.
    EXPECT_FALSE(token.stop_possible());
}

/// \Test [stopsource.swap].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, swap)
{
    score::cpp::stop_source unit{};
    score::cpp::stop_source unit_without_state{score::cpp::nostopstate};

    unit.swap(unit_without_state);

    EXPECT_TRUE(unit_without_state.stop_possible());
    EXPECT_FALSE(unit.stop_possible());
}

/// \Test [stopsource.special].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, algorithm_swap)
{
    score::cpp::stop_source unit{};
    score::cpp::stop_source unit_without_state{score::cpp::nostopstate};

    swap(unit, unit_without_state);

    EXPECT_TRUE(unit_without_state.stop_possible());
    EXPECT_FALSE(unit.stop_possible());
}

/// \Test [stopsource.mem].4
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, request_stop_without_state)
{
    score::cpp::stop_source unit{score::cpp::nostopstate};

    EXPECT_FALSE(unit.request_stop());
}

/// \Test [stopsource.mem].4, [stopsource.mem].5, [stopsource.mem].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, request_stop_second_time)
{
    score::cpp::stop_source unit{};

    EXPECT_TRUE(unit.request_stop());
    EXPECT_FALSE(unit.request_stop());
}

/// \Test [stoptoken.constr].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, default_constructor)
{
    score::cpp::stop_token unit{};

    EXPECT_FALSE(unit.stop_possible());
    EXPECT_FALSE(unit.stop_requested());
}

/// \Test [stopsource.mem].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, get_token)
{
    score::cpp::stop_source unit{};

    const auto token = unit.get_token();
    EXPECT_NE(token, score::cpp::stop_token{});
}

/// \Test [stopsource.mem].3
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148
TEST(stop_source, stop_requested)
{
    score::cpp::stop_source unit{};

    ASSERT_FALSE(unit.stop_requested());
    unit.request_stop();
    EXPECT_TRUE(unit.stop_requested());
}

/// \Test [stoptoken.constr].2, [stoptoken.cmp].1, [stoptoken.cmp].2
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, copy_construct)
{
    score::cpp::stop_token unit_without_state{};

    score::cpp::stop_source source{};
    score::cpp::stop_token unit_with_state = source.get_token();

    score::cpp::stop_token unit_without_state_copy{unit_without_state};
    score::cpp::stop_token unit_with_state_copy{unit_with_state};

    EXPECT_EQ(unit_without_state, unit_without_state_copy);
    EXPECT_EQ(unit_with_state, unit_with_state_copy);
    EXPECT_NE(unit_with_state, unit_without_state);
}

/// \Test [stoptoken.constr].3
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, move_construct)
{
    score::cpp::stop_source source{};
    score::cpp::stop_token unit_with_state = source.get_token();

    score::cpp::stop_token unit{std::move(unit_with_state)};

    EXPECT_FALSE(unit_with_state.stop_possible());
}

/// \Test [stoptoken.assign].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, copy_assign)
{
    score::cpp::stop_token unit_without_state{};

    score::cpp::stop_source source{};
    score::cpp::stop_token unit_with_state = source.get_token();

    score::cpp::stop_token unit_without_state_copy;
    score::cpp::stop_token unit_with_state_copy;

    unit_without_state_copy = unit_without_state;
    unit_with_state_copy = unit_with_state;

    EXPECT_EQ(unit_without_state, unit_without_state_copy);
    EXPECT_EQ(unit_with_state, unit_with_state_copy);
}

/// \Test [stoptoken.assign].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, move_assign)
{
    score::cpp::stop_source source{};
    score::cpp::stop_token unit_with_state = source.get_token();

    score::cpp::stop_token unit;
    unit = std::move(unit_with_state);

    EXPECT_FALSE(unit_with_state.stop_possible());
}

/// \Test [stoptoken.swap].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, swap)
{
    score::cpp::stop_source source{};
    score::cpp::stop_token unit_with_state = source.get_token();
    score::cpp::stop_token unit_without_state{};

    unit_with_state.swap(unit_without_state);

    EXPECT_TRUE(unit_without_state.stop_possible());
    EXPECT_FALSE(unit_with_state.stop_possible());
}

/// \Test [stoptoken.special].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, algorithm_swap)
{
    score::cpp::stop_source source{};
    score::cpp::stop_token unit_with_state = source.get_token();
    score::cpp::stop_token unit_without_state{};

    swap(unit_with_state, unit_without_state);

    EXPECT_TRUE(unit_without_state.stop_possible());
    EXPECT_FALSE(unit_with_state.stop_possible());
}

/// \Test [stoptoken.mem].2, [stopsource.destr].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, stop_possible)
{
    score::cpp::stop_token unit_without_state{};
    ASSERT_FALSE(unit_without_state.stop_possible());

    score::cpp::stop_token unit_with_state;
    {
        score::cpp::stop_source source{};
        unit_with_state = source.get_token();
        ASSERT_TRUE(unit_with_state.stop_possible());
    }
    EXPECT_FALSE(unit_with_state.stop_possible());
}

/// \Test [stoptoken.mem].2, [stopsource.destr].1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462147
TEST(stop_token, stop_possible_copy_ref_count)
{
    score::cpp::stop_token unit_without_state{};
    ASSERT_FALSE(unit_without_state.stop_possible());

    score::cpp::stop_token unit_with_state;
    score::cpp::stop_source source_copied;
    {
        score::cpp::stop_source source{};
        unit_with_state = source.get_token();
        source_copied = source;
        ASSERT_TRUE(unit_with_state.stop_possible());
    }
    EXPECT_TRUE(unit_with_state.stop_possible());
}

/// \Test [stoptoken.mem].1, [stopsource.mem].4, [stopsource.mem].5, [stopsource.mem].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462148, CB-#9462147
TEST(stop_source_stop_token, stop_requested)
{
    score::cpp::stop_source source{};
    score::cpp::stop_token token = source.get_token();

    EXPECT_FALSE(token.stop_requested());
    EXPECT_TRUE(source.request_stop());
    EXPECT_TRUE(token.stop_requested());
}

/// \Test [stopcallback.constr].3
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, callback_executed_after_stop_requested)
{
    std::atomic<bool> was_executed{false};
    score::cpp::stop_source source{};
    score::cpp::stop_callback unit{source.get_token(), [&was_executed]() { was_executed = true; }};
    source.request_stop();
    EXPECT_TRUE(was_executed);
}

/// \Test [stopcallback.constr].3
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, multiple_callback_executed_after_stop_requested)
{
    std::atomic<std::int32_t> was_executed{0};
    score::cpp::stop_source source{};
    score::cpp::stop_callback unit{source.get_token(), [&was_executed]() { was_executed++; }};
    score::cpp::stop_callback unit2{source.get_token(), [&was_executed]() { was_executed++; }};
    score::cpp::stop_callback unit3{source.get_token(), [&was_executed]() { was_executed++; }};
    source.request_stop();
    EXPECT_EQ(was_executed, 3);
}

/// \Test [stopcallback.constr].3
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, callback_executed_immediately)
{
    std::atomic<bool> was_executed{false};
    score::cpp::stop_source source{};
    source.request_stop();
    score::cpp::stop_callback unit{source.get_token(), [&was_executed]() { was_executed = true; }};
    EXPECT_TRUE(was_executed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, callback_executed_immediately_by_reference_constructor)
{
    std::atomic<bool> was_executed{false};
    score::cpp::stop_source source{};
    source.request_stop();
    auto token = source.get_token();
    score::cpp::stop_callback unit{token, [&was_executed]() { was_executed = true; }};
    EXPECT_TRUE(was_executed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, create_callback_and_request_stop_in_parallel)
{
    std::atomic<bool> was_executed{false};
    score::cpp::stop_source source{};
    auto token = source.get_token();
    score::cpp::jthread t{[token, &was_executed]() {
        score::cpp::stop_callback unit{token, [&was_executed]() { was_executed = true; }};
        while (!was_executed)
        {
        }
    }};
    source.request_stop();
    t.join();
    EXPECT_TRUE(was_executed);
}

/// \Test [stopcallback.constr].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, callback_not_executed)
{
    std::atomic<bool> was_executed{false};
    score::cpp::stop_source source{};

    {
        score::cpp::stop_callback unit{source.get_token(), [&was_executed]() { was_executed = true; }};
    }

    source.request_stop();
    EXPECT_FALSE(was_executed);
}

/// \Test [stopcallback.constr].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, callback_not_executed_for_stateless_stop_source)
{
    std::atomic<bool> was_executed{false};
    score::cpp::stop_source source{score::cpp::nostopstate};

    {
        score::cpp::stop_callback unit{static_cast<score::cpp::stop_token&&>(source.get_token()), [&was_executed]() {
                                    was_executed = true;
                                }}; // static_cast to silence pessimizing-move compiler warning
    }

    source.request_stop();
    EXPECT_FALSE(was_executed);
}

/// \Test [stopcallback.constr].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, multiple_callbacks_not_executed)
{
    std::atomic<bool> was_executed{false};
    score::cpp::stop_source source{};

    {
        score::cpp::stop_callback unit{source.get_token(), [&was_executed]() { was_executed = true; }};
        score::cpp::stop_callback unit2{source.get_token(), [&was_executed]() { was_executed = true; }};
        score::cpp::stop_callback unit3{source.get_token(), [&was_executed]() { was_executed = true; }};
    }

    source.request_stop();
    EXPECT_FALSE(was_executed);
}

/// \Test [stopcallback.constr].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, destructor_blocking_when_invoked_from_different_thread)
{
    std::atomic<bool> run{true};
    std::atomic<bool> callback_executed_at_least_once{false};
    std::atomic<bool> start_destruction{false};
    score::cpp::stop_source source{};

    auto unit = std::make_unique<score::cpp::stop_callback>(source.get_token(), [&run, &callback_executed_at_least_once]() {
        callback_executed_at_least_once = true;
        while (run)
        {
            std::this_thread::yield();
        }
    });

    score::cpp::jthread t{[&source]() { source.request_stop(); }};

    std::promise<void> p{};
    auto f = p.get_future();

    score::cpp::jthread t2{[unit = std::move(unit), &p, &start_destruction]() mutable {
        while (!start_destruction)
        {
            std::this_thread::yield();
        }
        unit.reset();
        p.set_value();
    }};

    while (!callback_executed_at_least_once)
    {
        std::this_thread::yield();
    }
    start_destruction = true;
    // Testing that ~stop_callback() actually blocks.
    EXPECT_EQ(f.wait_for(std::chrono::milliseconds{500}), std::future_status::timeout);
    run = false;

    t.join();
    t2.join();
}

/// \Test [stopcallback.constr].6
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, destructor_not_blocking_when_invoked_from_same_thread)
{
    score::cpp::stop_source source{};
    bool executed = false;
    class destruction_observer
    {
    public:
        destruction_observer(bool& executed) : executed_{executed} {}
        ~destruction_observer() { executed_ = true; }

    private:
        bool& executed_;
    };

    destruction_observer observer{executed};

    std::shared_ptr<score::cpp::stop_callback> unit;
    unit = std::make_shared<score::cpp::stop_callback>(source.get_token(),
                                                [&unit, observer = std::move(observer)]() { unit.reset(); });

    source.request_stop();

    EXPECT_TRUE(executed);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, destruct_without_associated_stop_source)
{
    score::cpp::stop_token token{};
    score::cpp::stop_callback unit{token, []() {}};
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, parallel_register_and_request_stop)
{
    // Since the test below is a probabilistic one which verifies timing behaviour, we perform it plenty of times.
    // This is esp. useful when used together with sanitizers since these are yielding different execution behaviors.
    constexpr std::uint64_t max_num_iterations = 1'000U;

    for (std::uint64_t num_iteration = 0U; num_iteration < max_num_iterations; ++num_iteration)
    {
        score::cpp::stop_source source{};
        std::atomic<std::uint32_t> num_invoked_callbacks{0U};

        std::promise<void> request_stop_started{};
        std::vector<std::unique_ptr<score::cpp::stop_callback>> callbacks{};
        score::cpp::jthread t{[&source, &callbacks, &request_stop_started, &num_invoked_callbacks]() {
            request_stop_started.get_future().wait();
            for (std::uint32_t counter = 0U; counter < 1'000U; ++counter)
            {
                callbacks.emplace_back(std::make_unique<score::cpp::stop_callback>(
                    source.get_token(), [&num_invoked_callbacks]() { ++num_invoked_callbacks; }));
            }
        }};

        request_stop_started.set_value();
        source.request_stop();

        t.join();
        ASSERT_EQ(num_invoked_callbacks, 1'000U);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, parallel_unregister_and_request_stop)
{
    // Since the test below is a probabilistic one which verifies timing behaviour, we perform it plenty of times.
    // This is esp. useful when used together with sanitizers since these are yielding different execution behaviors.
    constexpr std::uint64_t max_num_iterations = 1'000U;

    for (std::uint64_t num_iteration = 0U; num_iteration < max_num_iterations; ++num_iteration)
    {
        score::cpp::stop_source source{};
        std::atomic<std::uint32_t> num_invoked_callbacks{0U};

        std::vector<std::unique_ptr<score::cpp::stop_callback>> callbacks{};
        for (std::uint32_t counter = 0U; counter < 1'000U; ++counter)
        {
            callbacks.emplace_back(std::make_unique<score::cpp::stop_callback>(
                source.get_token(), [&num_invoked_callbacks]() { ++num_invoked_callbacks; }));
        }

        std::promise<void> request_stop_started{};
        score::cpp::jthread t{[&callbacks, &request_stop_started]() {
            request_stop_started.get_future().wait();
            for (auto& callback : callbacks)
            {
                callback.reset();
            }
        }};

        request_stop_started.set_value();
        source.request_stop();

        t.join();
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, register_and_unregister_after_request_stop_while_other_callback_is_currently_executing)
{
    score::cpp::stop_source source{};
    std::atomic<std::uint32_t> num_invoked_callbacks{0U};

    // required since score::cpp::stop_callback's callback capacity is just 32 byte
    struct blocking_data
    {
        std::mutex wait_mutex{};
        std::condition_variable wait_cv{};
        std::promise<void> callback_is_blocked{};
        bool blocked_callback_shall_finish{false};
    };
    blocking_data blocking_function_data{};

    // utility for notifying the blocked callback function to continue with its execution
    const auto notify_blocked_callback_function_to_continue_execution = [&blocking_function_data] {
        std::unique_lock<std::mutex> lock{blocking_function_data.wait_mutex};
        blocking_function_data.blocked_callback_shall_finish = true;
        blocking_function_data.wait_cv.notify_all();
    };

    // setup a callback function which will block execution in case of its first invocation
    const auto blocking_function_upon_first_invocation{[&blocking_function_data, &num_invoked_callbacks]() {
        std::unique_lock<std::mutex> lock{blocking_function_data.wait_mutex};
        if (++num_invoked_callbacks == 1U)
        {
            blocking_function_data.callback_is_blocked.set_value();
            blocking_function_data.wait_cv.wait(
                lock, [&blocking_function_data]() { return blocking_function_data.blocked_callback_shall_finish; });
        }
    }};

    // utility for registering a stop_callback whose underlying callback shall be executed immediately
    const auto register_stop_callback_and_expect_immediate_execution = [&source] {
        bool callback_is_finished{false};

        // when registering the stop_callback
        auto callback = std::make_unique<score::cpp::stop_callback>(
            source.get_token(), [&callback_is_finished]() { callback_is_finished = true; });

        // then it must have gotten executed immediately
        EXPECT_TRUE(callback_is_finished);

        return callback;
    };

    // register some stop_callbacks at stop_source to populate its internal list of stop_callbacks
    score::cpp::stop_callback first_callback{source.get_token(), blocking_function_upon_first_invocation};
    score::cpp::stop_callback second_callback{source.get_token(), blocking_function_upon_first_invocation};
    score::cpp::stop_callback third_callback{source.get_token(), blocking_function_upon_first_invocation};

    // then, in a separate thread, request the stop_state to stop which must block execution due to first_callback
    score::cpp::jthread stop_requester{[&source]() { source.request_stop(); }};

    // wait now in this thread until the blocking function blocks execution within the stop_requester thread
    blocking_function_data.callback_is_blocked.get_future().wait();
    ASSERT_TRUE(source.stop_requested());

    // when exactly during such situation, another stop_callback gets registered
    auto late_callback = register_stop_callback_and_expect_immediate_execution();

    // when unregistering such already executed callback now
    late_callback.reset();

    // and notifying the blocked callback to continue with its execution
    notify_blocked_callback_function_to_continue_execution();

    // as well as waiting for the stop_requester thread to finish
    ASSERT_TRUE(stop_requester.joinable());
    stop_requester.join();

    // then, all three of the initially registered callbacks must have gotten executed
    EXPECT_EQ(3U, num_invoked_callbacks);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
TEST(stop_callback, destroy_callbacks_out_of_order)
{
    score::cpp::stop_source source{};
    bool first_invoked{false};
    bool second_invoked{false};
    bool third_invoked{false};
    bool fourth_invoked{false};

    auto callback_1 =
        std::make_unique<score::cpp::stop_callback>(source.get_token(), [&first_invoked]() { first_invoked = true; });
    auto callback_2 =
        std::make_unique<score::cpp::stop_callback>(source.get_token(), [&second_invoked]() { second_invoked = true; });
    auto callback_3 =
        std::make_unique<score::cpp::stop_callback>(source.get_token(), [&third_invoked]() { third_invoked = true; });
    auto callback_4 =
        std::make_unique<score::cpp::stop_callback>(source.get_token(), [&fourth_invoked]() { fourth_invoked = true; });

    callback_1.reset();
    callback_3.reset();

    source.request_stop();

    EXPECT_FALSE(first_invoked);
    EXPECT_TRUE(second_invoked);
    EXPECT_FALSE(third_invoked);
    EXPECT_TRUE(fourth_invoked);
}

/// @brief Class creating a separate thread which waits repeatedly for the stop_source to get stop requested.
class token_waiter
{
public:
    explicit token_waiter(const std::uint64_t max_num_iterations,
                          const score::cpp::stop_source& stop_source,
                          std::mutex& outer_mutex)
        : max_num_iterations_{max_num_iterations}, stop_source_{stop_source}, outer_mutex_{outer_mutex}
    {
        the_thread_ = score::cpp::jthread{[this] {
            // see the comment in the actual test about why we need this loop within the thread and not outside of it
            for (std::uint64_t num_iteration = 0U; num_iteration < max_num_iterations_; ++num_iteration)
            {
                wait_until_token_waiter_shall_continue();

                { // this is the actual test logic
                    std::mutex own_mutex{};
                    std::condition_variable own_cv{};
                    const score::cpp::stop_token stop_token{stop_source_.get_token()};

                    // notify the stop_requester thread that we will actually start waiting now
                    token_waiter_is_running_.set_value();

                    // when registering an score::cpp::stop_callback for waiting that stop_token gets stop requested
                    score::cpp::stop_callback stop_callback{stop_token, [&own_cv, &own_mutex] {
                                                         // mutex must be acquired here since own_cv.notify_all()
                                                         // must not get called prior to below own_cv.wait()
                                                         // operation really being in waiting state
                                                         std::unique_lock<std::mutex> callback_lock{own_mutex};
                                                         own_cv.notify_all();
                                                     }};

                    // and initiating a wait operation for own_cv
                    std::unique_lock<std::mutex> own_lock{own_mutex};
                    own_cv.wait(own_lock, [&stop_token] { return stop_token.stop_requested(); });

                    // then the wait operation must have ended once stop_token got stop requested
                }

                indicate_that_token_waiter_finished_an_iteration();
            }
        }};
    }

    token_waiter& operator=(const token_waiter&) = delete;
    token_waiter& operator=(token_waiter&&) = delete;
    token_waiter(const token_waiter&) = delete;
    token_waiter(token_waiter&&) = delete;

    ~token_waiter()
    {
        if (the_thread_.joinable())
        {
            the_thread_.join();
        }
    }

    void perform_an_iteration(const std::unique_lock<std::mutex>& lock)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(lock.owns_lock());
        token_waiter_is_running_ = std::promise<void>{};
        token_waiter_has_finished_ = false;
        token_waiter_shall_continue_ = true;
        token_waiter_shall_continue_cv_.notify_all();
    }

    void wait_until_is_running() { token_waiter_is_running_.get_future().wait(); }
    void wait_until_has_finished(std::unique_lock<std::mutex>& lock)
    {
        token_waiter_has_finished_cv_.wait(lock, [this] { return token_waiter_has_finished_; });
    }

private:
    void wait_until_token_waiter_shall_continue()
    {
        std::unique_lock<std::mutex> lock{outer_mutex_};
        token_waiter_shall_continue_cv_.wait(lock, [this] { return token_waiter_shall_continue_; });
    }

    void indicate_that_token_waiter_finished_an_iteration()
    {
        std::unique_lock<std::mutex> lock{outer_mutex_};
        token_waiter_has_finished_ = true;
        token_waiter_shall_continue_ = false;
        token_waiter_has_finished_cv_.notify_all();
    }

    bool token_waiter_has_finished_{false};
    bool token_waiter_shall_continue_{false};
    std::promise<void> token_waiter_is_running_{};
    std::condition_variable token_waiter_has_finished_cv_{};
    std::condition_variable token_waiter_shall_continue_cv_{};
    const std::uint64_t max_num_iterations_;
    const score::cpp::stop_source& stop_source_;
    std::mutex& outer_mutex_;
    score::cpp::jthread the_thread_;
};

/// @brief Class creating a separate thread that repeatedly requests stop at stop_source after waiting for token_waiter.
class stop_requester
{
public:
    explicit stop_requester(const std::uint64_t max_num_iterations,
                            score::cpp::stop_source& stop_source,
                            token_waiter& token_waiter,
                            std::mutex& outer_mutex)
        : max_num_iterations_{max_num_iterations}
        , stop_source_{stop_source}
        , token_waiter_{token_waiter}
        , outer_mutex_{outer_mutex}
    {
        the_thread_ = score::cpp::jthread{[this] {
            // see the comment in the actual test about why we need this loop within the thread and not outside of it
            for (std::uint64_t num_iteration = 0U; num_iteration < max_num_iterations_; ++num_iteration)
            {
                wait_until_stop_requester_shall_continue();

                { // this is the actual test logic
                    // after waiting for the token_waiter thread to actually start waiting
                    token_waiter_.wait_until_is_running();

                    // when calling request_stop() at stop_source around the same time token_waiter started waiting
                    const bool stop_requested = stop_source_.request_stop();

                    // then the operation must have succeeded
                    ASSERT_TRUE(stop_requested);
                }

                indicate_that_stop_requester_finished_an_iteration();
            }
        }};
    }

    stop_requester& operator=(const stop_requester&) = delete;
    stop_requester& operator=(stop_requester&&) = delete;
    stop_requester(const stop_requester&) = delete;
    stop_requester(stop_requester&&) = delete;

    ~stop_requester()
    {
        if (the_thread_.joinable())
        {
            the_thread_.join();
        }
    }

    void perform_an_iteration(const std::unique_lock<std::mutex>& lock)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(lock.owns_lock());
        stop_requester_has_finished_ = false;
        stop_requester_shall_continue_ = true;
        stop_requester_shall_continue_cv_.notify_all();
    }

    void wait_until_has_finished(std::unique_lock<std::mutex>& lock)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(lock.owns_lock());
        stop_requester_has_finished_cv_.wait(lock, [this] { return stop_requester_has_finished_; });
    }

private:
    void wait_until_stop_requester_shall_continue()
    {
        std::unique_lock<std::mutex> lock{outer_mutex_};
        stop_requester_shall_continue_cv_.wait(lock, [this] { return stop_requester_shall_continue_; });
    }
    void indicate_that_stop_requester_finished_an_iteration()
    {
        std::unique_lock<std::mutex> lock{outer_mutex_};
        stop_requester_has_finished_ = true;
        stop_requester_shall_continue_ = false;
        stop_requester_has_finished_cv_.notify_all();
    }

    bool stop_requester_has_finished_{false};
    bool stop_requester_shall_continue_{false};
    std::condition_variable stop_requester_has_finished_cv_{};
    std::condition_variable stop_requester_shall_continue_cv_{};
    const std::uint64_t max_num_iterations_;
    score::cpp::stop_source& stop_source_;
    token_waiter& token_waiter_;
    std::mutex& outer_mutex_;
    score::cpp::jthread the_thread_;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9462172
/// @note This test originates from a unit test in ddad_platform which discovered the data race in stop_callback.
/// @note For further details, see broken_link_j/Ticket-89170.
TEST(stop_callback, parallel_stop_callback_usage_and_request_stop)
{
    // Since the test below is a probabilistic one which verifies timing behaviour, we perform it plenty of times.
    // This is esp. useful when used together with sanitizers since these are yielding different execution behaviors.
#ifdef __SANITIZE_ADDRESS__
    constexpr std::uint64_t max_num_iterations = 50'000U;
#else
    constexpr std::uint64_t max_num_iterations = 100'000U;
#endif

    score::cpp::stop_source source{};
    std::mutex mutex{};
    token_waiter token_waiter{max_num_iterations, source, mutex};
    stop_requester stop_requester{max_num_iterations, source, token_waiter, mutex};

    // In case you are wondering why we need to supervise token_waiter as well as token_waiter from outside to perform
    // a single iteration, the reason is that the qemu-aarch64 emulator consumes constantly increasing amounts of memory
    // when creating a high number of threads. That's why we cannot create new threads upon each test iteration and join
    // them in a loop. Instead, for mitigation, we only create two threads here and synchronize their loop iterations to
    // always continue at the same time again.

    // utilities for synchronizing a single iteration of token_waiter as well as stop_requester thread
    const auto notify_threads_to_perform_an_iteration = [&] {
        std::unique_lock<std::mutex> lock{mutex};
        // token_waiter must be notified prior to stop_requester since it resets its std::promise
        token_waiter.perform_an_iteration(lock);
        stop_requester.perform_an_iteration(lock);
    };
    const auto wait_for_threads_to_finish_such_iteration = [&] {
        std::unique_lock<std::mutex> lock{mutex};
        token_waiter.wait_until_has_finished(lock);
        stop_requester.wait_until_has_finished(lock);

        // refresh stop_source with initial state so that threads can perform the test once more
        source = score::cpp::stop_source{};
    };

    // when repeatedly advising the two threads created above to perform a single iteration each
    for (std::uint64_t num_iteration = 0U; num_iteration < max_num_iterations; ++num_iteration)
    {
        // when letting both of the above threads perform one of their iteration
        notify_threads_to_perform_an_iteration();

        // then each one of them must perform a single iteration successfully
        wait_for_threads_to_finish_such_iteration();
    }
}

} // namespace
