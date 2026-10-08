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
#include "score/concurrency/unlock_guard.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <functional>
#include <mutex>
#include <shared_mutex>

#include "score/concurrency/test_types.h"

namespace test
{

using score::concurrency::UnlockGuard;

TEST(UnlockGuardTest, ConstructionWithTypes)
{
    // UnlockGuard should be instantiable with types adhering to is_basic_lockable
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<BasicLockableArchetype>, BasicLockableArchetype&>))
        << "UnlockGuard should be constructible with a BasicLockableArchetype&";
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<MockMutex>, MockMutex&>))
        << "UnlockGuard should be constructible with a MockMutex&";

    // UnlockGuard should be instantiable with standard mutex types
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<std::mutex>, std::mutex&>))
        << "UnlockGuard should be constructible with a std::mutex&";
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<std::timed_mutex>, std::timed_mutex&>))
        << "UnlockGuard should be constructible with a std::timed_mutex&";
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<std::recursive_mutex>, std::recursive_mutex&>))
        << "UnlockGuard should be constructible with a std::recursive_mutex&";
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<std::recursive_timed_mutex>, std::recursive_timed_mutex&>))
        << "UnlockGuard should be constructible with a std::recursive_timed_mutex&";
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<std::shared_mutex>, std::shared_mutex&>))
        << "UnlockGuard should be constructible with a std::shared_mutex&";
    EXPECT_TRUE((std::is_constructible_v<UnlockGuard<std::unique_lock<std::mutex>>, std::unique_lock<std::mutex>&>))
        << "UnlockGuard should be constructible with a std::unique_lock<std::mutex>&";
}

TEST(UnlockGuardTest, MutexUnlocksOnConstructionLocksOnDestruction)
{
    MockMutex mut;
    mut.lock();
    auto unlocker = std::mem_fn(&MockMutex::unlock);
    std::unique_ptr<MockMutex, decltype(unlocker)> unlock_on_test_exit{&mut, unlocker};

    {
        UnlockGuard guard{mut};
        EXPECT_FALSE(mut.is_locked());
    }

    EXPECT_TRUE(mut.is_locked());
}

TEST(UnlockGuardTest, UniqueLockUnlocksOnConstructionLocksOnDestruction)
{
    std::mutex mut;
    std::unique_lock ul{mut};

    {
        UnlockGuard guard{ul};
        EXPECT_FALSE(ul.owns_lock());
    }

    EXPECT_TRUE(ul.owns_lock());

    std::unique_lock deferred_lock{mut, std::defer_lock};
    {
        UnlockGuard guard{deferred_lock};
        EXPECT_FALSE(deferred_lock.owns_lock());
    }

    EXPECT_FALSE(deferred_lock.owns_lock());
}

}  // namespace test
