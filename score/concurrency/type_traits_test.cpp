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
#include "score/concurrency/type_traits.h"
#include "score/concurrency/test_types.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <chrono>
#include <mutex>
#include <shared_mutex>

using namespace score;

namespace
{

class HasLockOnly
{
  public:
    void lock() {}
};

class HasUnlockOnly
{
  public:
    void unlock() {}
};

class HasLockAndUnlockMismatchedSignature
{
  public:
    void lock(int&) {}
    void unlock(int*) {}
};

class LockWithVoidOwnsLock
{
  public:
    void lock() {}
    void unlock() {}
    void owns_lock() const {}
};

class LockWithIntOwnsLock
{
  public:
    void lock() {}
    void unlock() {}
    int owns_lock() const
    {
        return 1;
    }
};

class TimedLockableArchetype : public test::BasicLockableArchetype
{
  public:
    template <typename Rep, typename Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>&)
    {
        return true;
    }
};

class TimedLockableUntilArchetype : public test::BasicLockableArchetype
{
  public:
    template <typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>&)
    {
        return true;
    }
};

class TimedLockWithoutBasicLocking
{
  public:
    bool try_lock_for(const std::chrono::milliseconds&)
    {
        return true;
    }

    bool try_lock_until(const std::chrono::steady_clock::time_point&)
    {
        return true;
    }
};

}  // namespace

TEST(TypeTraitsTest, IsBasicLockableCompileTimeChecks)
{
    // Negative cases
    EXPECT_TRUE((!is_basic_lockable_v<HasLockOnly>)) << "HasLockOnly should not be basic_lockable";
    EXPECT_TRUE((!is_basic_lockable_v<HasUnlockOnly>)) << "HasUnlockOnly should not be basic_lockable";
    EXPECT_TRUE((!is_basic_lockable_v<HasLockAndUnlockMismatchedSignature>))
        << "HasLockAndUnlockMismatchedSignature should not be basic_lockable";

    // Positive cases
    EXPECT_TRUE((is_basic_lockable_v<test::BasicLockableArchetype>))
        << "BasicLockableArchetype should be basic_lockable";

    // Standard library types
    EXPECT_TRUE((is_basic_lockable_v<std::mutex>)) << "std::mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::timed_mutex>)) << "std::timed_mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::recursive_mutex>)) << "std::recursive_mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::recursive_timed_mutex>))
        << "std::recursive_timed_mutex should be basic_lockable";

    EXPECT_TRUE((is_basic_lockable_v<std::shared_mutex>)) << "std::shared_mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::shared_timed_mutex>)) << "std::shared_timed_mutex should be basic_lockable";

    EXPECT_TRUE((is_basic_lockable_v<std::unique_lock<std::mutex>>))
        << "std::unique_lock<std::mutex> should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::shared_lock<std::shared_mutex>>))
        << "std::shared_lock<std::shared_mutex> should be basic_lockable";
}

TEST(TypeTraitsTest, LockWithOwnsLockCompileTimeChecks)
{
    EXPECT_FALSE((LockWithOwnsLock<test::BasicLockableArchetype>::value))
        << "a basic lockable without owns_lock should not satisfy LockWithOwnsLock";
    EXPECT_FALSE((LockWithOwnsLock<LockWithVoidOwnsLock>::value))
        << "a lock with void owns_lock should not satisfy LockWithOwnsLock";
    EXPECT_FALSE((LockWithOwnsLock<LockWithIntOwnsLock>::value))
        << "a lock with non-bool owns_lock should not satisfy LockWithOwnsLock";
    EXPECT_TRUE((LockWithOwnsLock<test::LockableWithOwnsLock>::value))
        << "LockableWithOwnsLock should satisfy LockWithOwnsLock";
    EXPECT_FALSE((LockWithOwnsLock<std::mutex>::value)) << "std::mutex should not satisfy LockWithOwnsLock";
    EXPECT_TRUE((LockWithOwnsLock<std::unique_lock<std::mutex>>::value))
        << "std::unique_lock<std::mutex> should satisfy LockWithOwnsLock";
    EXPECT_TRUE((LockWithOwnsLock<std::shared_lock<std::shared_mutex>>::value))
        << "std::shared_lock<std::shared_mutex> should satisfy LockWithOwnsLock";
}

TEST(TypeTraitsTest, IsTimedLockableForCompileTimeChecks)
{
    using FractionalDuration = std::chrono::duration<double, std::ratio<1, 3>>;

    EXPECT_TRUE((is_timed_lockable_for_v<TimedLockableArchetype>));
    EXPECT_TRUE((is_timed_lockable_for_v<TimedLockableArchetype, FractionalDuration>));
    EXPECT_FALSE((is_timed_lockable_for_v<test::BasicLockableArchetype>));
    EXPECT_FALSE((is_timed_lockable_for_v<TimedLockWithoutBasicLocking>));

    EXPECT_TRUE((is_timed_lockable_for_v<std::timed_mutex>));
    EXPECT_TRUE((is_timed_lockable_for_v<std::recursive_timed_mutex>));
    EXPECT_TRUE((is_timed_lockable_for_v<std::shared_timed_mutex>));
    EXPECT_FALSE((is_timed_lockable_for_v<std::mutex>));
    EXPECT_FALSE((is_timed_lockable_for_v<std::recursive_mutex>));
    EXPECT_FALSE((is_timed_lockable_for_v<std::shared_mutex>));
}

TEST(TypeTraitsTest, IsTimedLockableUntilCompileTimeChecks)
{
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

    EXPECT_TRUE((is_timed_lockable_until_v<TimedLockableUntilArchetype>));
    EXPECT_TRUE((is_timed_lockable_until_v<TimedLockableUntilArchetype, TimePoint>));
    EXPECT_FALSE((is_timed_lockable_until_v<test::BasicLockableArchetype>));
    EXPECT_FALSE((is_timed_lockable_until_v<TimedLockWithoutBasicLocking>));

    EXPECT_TRUE((is_timed_lockable_until_v<std::timed_mutex>));
    EXPECT_TRUE((is_timed_lockable_until_v<std::recursive_timed_mutex>));
    EXPECT_TRUE((is_timed_lockable_until_v<std::shared_timed_mutex>));
    EXPECT_FALSE((is_timed_lockable_until_v<std::mutex>));
    EXPECT_FALSE((is_timed_lockable_until_v<std::recursive_mutex>));
    EXPECT_FALSE((is_timed_lockable_until_v<std::shared_mutex>));
}
