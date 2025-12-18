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
#include "score/os/utils/spinlock.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <mutex>
#include <thread>

namespace score
{
namespace os
{
namespace test
{

void f(Spinlock& lock, std::uint32_t& val1, std::uint32_t& val2, std::uint32_t loopCount)
{
    for (std::uint32_t i = 0; i < loopCount; i++)
    {
        lock.lock();
        val1++;
        val2 = val1 + 3;
        lock.unlock();
    }
}

/**
 * @brief Test case spawns a number of concurrent threads, which each independently
 * alters two "global" int variables shared by all threads under a lock in the following way:
 * var1 gets incremented by 1.
 * var2 gets updated to val1 + 3;
 *
 * Initially val1 == 0 and val2 == 0. We started T threads, where each loops N times over these assignments.
 * So AFTER all threads have finished, the expectation is:
 *
 * val1 == T * N
 * val2 == T * N + 3
 *
 * Note: Had run this test WITHOUT the lock/spinlock - and it never fulfilled the expectations in 10 out of 10 runs.
 * So I think this test serves the purpose.
 *
 *
 */
TEST(SpinlockTest, ConcIntManipulation)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SpinlockTest Conc Int Manipulation");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Spinlock lk;
    std::uint32_t val1, val2, loopCount;
    val1 = 0u;
    val2 = 0u;
    loopCount = 100000u;

    std::thread t1(f, std::ref(lk), std::ref(val1), std::ref(val2), loopCount);
    std::thread t2(f, std::ref(lk), std::ref(val1), std::ref(val2), loopCount);
    std::thread t3(f, std::ref(lk), std::ref(val1), std::ref(val2), loopCount);

    t1.join();
    t2.join();
    t3.join();

    EXPECT_EQ(val1, 3 * loopCount);
    EXPECT_EQ(val2, 3 * loopCount + 3);
}

TEST(SpinlockTest, TryLock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SpinlockTest Try Lock");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Spinlock lk;

    lk.lock();
    bool lk_acquired = lk.try_lock();
    EXPECT_EQ(lk_acquired, false);
    lk.unlock();
    lk_acquired = lk.try_lock();
    EXPECT_EQ(lk_acquired, true);
}

/**
 * This test is basically just a compile test.
 * I.e. we validate that our Spinlock impl. fulfills the requirements of
 * <tt>Lockable</tt> (@see https://en.cppreference.com/w/cpp/named_req/Lockable) by wrapping it with a std::lock_guard.
 */
TEST(SpinlockTest, LockGuardSupport)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SpinlockTest Lock Guard Support");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Spinlock spinlock;

    {
        std::lock_guard<Spinlock> lock(spinlock);
    }
}

}  // namespace test
}  // namespace os
}  // namespace score
