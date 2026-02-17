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

#include <score/latch.hpp>
#include <score/latch.hpp> // check include guard

#include <score/assert_support.hpp>
#include <score/jthread.hpp>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenCountIsOne_WhenCountDown_ThenFinalCountIsZero)
{
    score::cpp::latch latch{1};

    EXPECT_FALSE(latch.try_wait());

    latch.count_down();

    EXPECT_TRUE(latch.try_wait());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenCountIsFortyTwo_WhenCountDownFortyTwo_ThenFinalCountIsZero)
{
    score::cpp::latch latch{42};

    EXPECT_FALSE(latch.try_wait());

    latch.count_down(42);

    EXPECT_TRUE(latch.try_wait());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenCountIsOne_WhenCountDownOne_ThenWaitDoesNotBlock)
{
    score::cpp::latch latch{1};

    EXPECT_FALSE(latch.try_wait());

    latch.count_down();

    latch.wait();
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenCountIsThree_WhenCountDownByThreeThreads_ThenFinalCountIsZero)
{
    score::cpp::latch latch{3};

    EXPECT_FALSE(latch.try_wait());

    auto count_down = [&] { latch.count_down(); };

    score::cpp::jthread t0{count_down};
    score::cpp::jthread t1{count_down};

    latch.count_down();
    latch.wait();

    t0.join();
    t1.join();

    EXPECT_TRUE(latch.try_wait());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenCtor_ThenPreConditionsHold)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::latch latch{3};);
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::latch latch{0};);
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::latch latch{score::cpp::latch::max()});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenCtor_ThenPreConditionsViolated) { SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::latch latch{-3};); }

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenCountDownEQInternalCount_ThenPreConditionHolds)
{
    const std::ptrdiff_t count{3};
    score::cpp::latch latch{count};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(latch.count_down(count));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenCountDownGTInternalCount_ThenPreConditionViolated)
{
    const std::ptrdiff_t count{3};
    score::cpp::latch latch{count};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(latch.count_down(count + 1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenCountDownNegative_ThenPreConditionViolated)
{
    score::cpp::latch latch{3};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(latch.count_down(-1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenCountDownZero_ThenPreConditionHolds)
{
    score::cpp::latch latch{3};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(latch.count_down(0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenCountDownPositiveLTInternalCounter_ThenPreConditionHolds)
{
    score::cpp::latch latch{3};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(latch.count_down(1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenArriveAndWaitNegative_ThenPreConditionViolated)
{
    score::cpp::latch latch{3};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(latch.arrive_and_wait(-1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenLatch_WhenArriveAndWaitGTInternalCount_ThenPreConditionViolated)
{
    const std::ptrdiff_t count{3};
    score::cpp::latch latch{count};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(latch.arrive_and_wait(count + 1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7993747
TEST(LatchTest, GivenCountIsThree_WhenArrivedByThreeThreads_ThenAllThreadsObserveTheSameSharedState)
{
    score::cpp::latch latch{3};

    EXPECT_FALSE(latch.try_wait());

    std::atomic<bool> shared_state{false};

    auto count_down = [&] {
        latch.arrive_and_wait();
        EXPECT_TRUE(shared_state);
    };

    score::cpp::jthread t0{count_down};
    score::cpp::jthread t1{count_down};

    shared_state = true;
    latch.arrive_and_wait();

    t0.join();
    t1.join();

    EXPECT_TRUE(shared_state);
    EXPECT_TRUE(latch.try_wait());
}

} // namespace
