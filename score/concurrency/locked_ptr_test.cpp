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
#include "score/concurrency/locked_ptr.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "score/concurrency/test_types.h"
#include "score/concurrency/unlock_guard.h"

#include <functional>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <utility>

using score::concurrency::LockedPtr;
using score::concurrency::UnlockGuard;

namespace test
{

namespace
{
template <typename Lock>
using LPtr2int = LockedPtr<int, Lock>;

struct IntWrapper
{
    int value;
};

using LPtr2IntW = LockedPtr<IntWrapper, std::unique_lock<MockMutex>>;
}  // namespace

TEST(LockedPtrTest, ConstructionWithTypes)
{
    EXPECT_TRUE((std::is_constructible_v<LockedPtr<int, BasicLockableArchetype>, int*, BasicLockableArchetype>))
        << "LockedPtr should be constructible with BasicLockableArchetype";
    EXPECT_TRUE(
        (std::is_constructible_v<LockedPtr<const int, BasicLockableArchetype>, const int*, BasicLockableArchetype>))
        << "LockedPtr should be constructible with BasicLockableArchetype";
    EXPECT_TRUE(
        (std::is_constructible_v<LockedPtr<int, std::unique_lock<std::mutex>>, int*, std::unique_lock<std::mutex>>))
        << "LockedPtr should be constructible with std::unique_lock<std::mutex>";
    EXPECT_TRUE(
        (std::is_constructible_v<LockedPtr<int, std::shared_lock<std::mutex>>, int*, std::shared_lock<std::mutex>>))
        << "LockedPtr should be constructible with std::shared_lock<std::mutex>";

    EXPECT_TRUE((!std::is_copy_constructible_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should not be copy-constructible";
    EXPECT_TRUE((!std::is_copy_assignable_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should not be copy-assignable";
    EXPECT_TRUE((std::is_move_constructible_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should be move-constructible";
    EXPECT_TRUE((std::is_move_assignable_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr should be move-assignable";
}

TEST(LockedPtrTest, SwappingWithTypes)
{
    EXPECT_TRUE((std::is_swappable_v<LockedPtr<int, BasicLockableArchetype>>))
        << "LockedPtr with BasicLockableArchetype should be swappable";
    EXPECT_TRUE((std::is_swappable_v<LockedPtr<int, std::unique_lock<std::mutex>>>))
        << "LockedPtr with unique_lock should be swappable";
    EXPECT_TRUE((std::is_swappable_v<LockedPtr<int, std::shared_lock<std::shared_mutex>>>))
        << "LockedPtr with shared_lock should be swappable";
}

TEST(LockedPtrTest, DereferenceTests)
{
    int x = 10;
    LPtr2int<BasicLockableArchetype> lp_basic{&x, BasicLockableArchetype{}};

    EXPECT_EQ(*lp_basic, 10);
    EXPECT_EQ(*std::as_const(lp_basic), 10);

    *lp_basic = 20;
    EXPECT_EQ(*lp_basic, 20);
    EXPECT_EQ(*std::as_const(lp_basic), 20);

    decltype(auto) x_ref = *lp_basic;
    EXPECT_TRUE((std::is_same_v<decltype(x_ref), int&>)) << "LockedPtr::operator* should return int&";

    decltype(auto) cx_ref = *std::as_const(lp_basic);
    EXPECT_TRUE((std::is_same_v<decltype(cx_ref), const int&>)) << "LockedPtr::operator* should return const int&";
}

TEST(LockedPtrTest, PtrUsageTests)
{
    IntWrapper obj{42};
    MockMutex mut{};

    LPtr2IntW lp_obj{&obj, std::unique_lock<MockMutex>{mut}};
    EXPECT_EQ(lp_obj->value, 42);
    EXPECT_EQ(std::as_const(lp_obj)->value, 42);

    lp_obj->value = 100;
    EXPECT_EQ(lp_obj->value, 100);
    EXPECT_EQ(std::as_const(lp_obj)->value, 100);
}

TEST(LockedPtrTest, GetTests)
{
    IntWrapper obj{42};
    MockMutex mut{};

    LPtr2IntW lp_obj{&obj, std::unique_lock<MockMutex>{mut}};
    EXPECT_EQ(lp_obj.get(), &obj);
    EXPECT_EQ(std::as_const(lp_obj).get(), &obj);

    decltype(auto) x_ptr = lp_obj.get();
    EXPECT_TRUE((std::is_same_v<decltype(x_ptr), IntWrapper*>)) << "LockedPtr::get() should return IntWrapper*";

    decltype(auto) cx_ptr = std::as_const(lp_obj).get();
    EXPECT_TRUE((std::is_same_v<decltype(cx_ptr), const IntWrapper*>))
        << "LockedPtr::get() should return const IntWrapper*";
}

TEST(LockedPtrTest, UnlockGuardBasicLockableArchetypeTests)
{
    int x = 42;

    LPtr2int<BasicLockableArchetype> lp_basic{&x, BasicLockableArchetype{}};

    {
        auto ug_basic = lp_basic.unlock_guard();
        EXPECT_TRUE((std::is_same_v<decltype(ug_basic), UnlockGuard<BasicLockableArchetype>>))
            << "unlock_guard() should return UnlockGuard<BasicLockableArchetype>";
    }

    {
        auto cug_basic = std::as_const(lp_basic).unlock_guard();
        EXPECT_TRUE((std::is_same_v<decltype(cug_basic), UnlockGuard<BasicLockableArchetype>>))
            << "unlock_guard() should return UnlockGuard<BasicLockableArchetype>";
    }
}

TEST(LockedPtrTest, UnlockGuardUniqueLockTests)
{
    int x = 42;

    std::mutex mut{};
    LPtr2int<std::unique_lock<std::mutex>> lp_mut{&x, std::unique_lock{mut}};

    {
        auto ug_mut = lp_mut.unlock_guard();
        EXPECT_TRUE((std::is_same_v<decltype(ug_mut), UnlockGuard<std::unique_lock<std::mutex>>>))
            << "unlock_guard() should return UnlockGuard<std::unique_lock<std::mutex>>";
    }

    {
        auto cug_mut = std::as_const(lp_mut).unlock_guard();
        EXPECT_TRUE((std::is_same_v<decltype(cug_mut), UnlockGuard<std::unique_lock<std::mutex>>>))
            << "unlock_guard() should return UnlockGuard<std::unique_lock<std::mutex>>";
    }
}

TEST(LockedPtrTest, UnlockGuardSharedLockTests)
{
    int x = 42;

    std::shared_mutex sh_mut{};
    LPtr2int<std::shared_lock<std::shared_mutex>> lp_sh{&x, std::shared_lock{sh_mut}};

    {
        auto ug_sh = lp_sh.unlock_guard();
        EXPECT_TRUE((std::is_same_v<decltype(ug_sh), UnlockGuard<std::shared_lock<std::shared_mutex>>>))
            << "unlock_guard() should return UnlockGuard<std::shared_lock<std::shared_mutex>>";
    }

    {
        auto cug_sh = std::as_const(lp_sh).unlock_guard();
        EXPECT_TRUE((std::is_same_v<decltype(cug_sh), UnlockGuard<std::shared_lock<std::shared_mutex>>>))
            << "unlock_guard() should return UnlockGuard<std::shared_lock<std::shared_mutex>>";
    }
}

TEST(LockedPtrTest, NonConstWithUniqueLock)
{
    IntWrapper obj{42};
    std::mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::unique_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::unique_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::unique_lock{mut})));
    }
}

TEST(LockedPtrTest, ConstWithUniqueLock)
{
    const IntWrapper obj{42};
    std::mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::unique_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::unique_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::unique_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::unique_lock{mut})));
    }
}

TEST(LockedPtrTest, NonConstWithSharedLock)
{
    IntWrapper obj{42};
    std::shared_mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::shared_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::shared_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::shared_lock{mut})));
    }
}

TEST(LockedPtrTest, ConstWithSharedLock)
{
    const IntWrapper obj{42};
    std::shared_mutex mut;

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}), &obj);
    }

    {
        EXPECT_EQ(LockedPtr(&obj, std::shared_lock{mut}).get(), &obj);
    }

    {
        EXPECT_EQ(&(*LockedPtr(&obj, std::shared_lock{mut})).value, &obj.value);
    }

    {
        EXPECT_TRUE(static_cast<bool>(LockedPtr(&obj, std::shared_lock{mut})));
    }

    {
        constexpr IntWrapper* nullp = nullptr;
        EXPECT_FALSE(static_cast<bool>(LockedPtr(nullp, std::shared_lock{mut})));
    }
}

TEST(LockedPtrTest, Swap)
{
    {
        IntWrapper obj1{11};
        IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto lp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto lp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(lp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        lp1.swap(lp2);
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, true)));
        EXPECT_THAT(std::pair(lp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }

    {
        const IntWrapper obj1{11};
        const IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto clp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto clp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(clp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        clp1.swap(clp2);
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, true)));
        EXPECT_THAT(std::pair(clp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }
}

TEST(LockedPtrTest, Move)
{
    {
        IntWrapper obj1{11};
        IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto lp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto lp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(lp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        lp1 = std::move(lp2);
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, false)));
        EXPECT_FALSE(lp2);
        EXPECT_TRUE(mut2.is_locked());
    }

    {
        const IntWrapper obj1{11};
        const IntWrapper obj2{22};
        MockMutex mut1;
        MockMutex mut2;

        auto clp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        auto clp2 = LockedPtr(&obj2, std::unique_lock{mut2});
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
        EXPECT_THAT(std::pair(clp2.get(), mut2.is_locked()), ::testing::Eq(std::pair(&obj2, true)));

        clp1 = std::move(clp2);
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj2, false)));
        EXPECT_FALSE(clp2);
        EXPECT_TRUE(mut2.is_locked());
    }

    {
        IntWrapper obj1{11};
        MockMutex mut1;

        auto lp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        EXPECT_THAT(std::pair(lp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));

        auto lp2(std::move(lp1));
        EXPECT_FALSE(lp1);
        EXPECT_THAT(std::pair(lp2.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }

    {
        const IntWrapper obj1{11};
        MockMutex mut1;

        auto clp1 = LockedPtr(&obj1, std::unique_lock{mut1});
        EXPECT_THAT(std::pair(clp1.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));

        auto clp2 = std::move(clp1);
        EXPECT_FALSE(clp1);
        EXPECT_THAT(std::pair(clp2.get(), mut1.is_locked()), ::testing::Eq(std::pair(&obj1, true)));
    }
}

TEST(LockedPtrTest, UnlockGuard)
{
    IntWrapper obj{42};
    MockMutex mut;

    auto lp = LockedPtr(&obj, std::unique_lock{mut});
    EXPECT_TRUE(mut.is_locked());

    {
        auto ug = lp.unlock_guard();
        EXPECT_FALSE(mut.is_locked());
    }

    EXPECT_TRUE(mut.is_locked());
}

}  // namespace test
