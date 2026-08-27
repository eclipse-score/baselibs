/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#include "score/utils/static_destruction_guard.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

namespace score
{
namespace utils
{
namespace
{

/// \brief A type with an observable lifecycle (construction/destruction counters, a mutable value) used to verify
/// both the nifty-counter idiom's lifetime guarantees and that StaticDestructionGuard::GetStorage() consistently
/// refers to one and the same live object across calls (i.e. exercises the std::launder'ed access path).
struct LifecycleTracker
{
    static constexpr std::int32_t kInitialValue{42};

    LifecycleTracker() noexcept
    {
        ++construction_count;
        value = kInitialValue;
    }

    ~LifecycleTracker() noexcept
    {
        ++destruction_count;
    }

    LifecycleTracker(const LifecycleTracker&) = delete;
    LifecycleTracker& operator=(const LifecycleTracker&) = delete;
    LifecycleTracker(LifecycleTracker&&) = delete;
    LifecycleTracker& operator=(LifecycleTracker&&) = delete;

    static void Reset() noexcept
    {
        construction_count = 0;
        destruction_count = 0;
    }

    std::int32_t value{};

    static std::uint8_t construction_count;
    static std::uint8_t destruction_count;
};

std::uint8_t LifecycleTracker::construction_count{0U};
std::uint8_t LifecycleTracker::destruction_count{0U};

using Guard = StaticDestructionGuard<LifecycleTracker>;

// The counter and storage guarded by Guard have static storage duration for the whole test binary (that is the
// point of the nifty-counter idiom), so every test must leave it fully destroyed (all guards out of scope) by the
// time it ends, and reset the observation counters in SetUp(), to keep tests independent of each other and of order.
class StaticDestructionGuardTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        LifecycleTracker::Reset();
    }
};

TEST_F(StaticDestructionGuardTest, FirstGuardConstructsGuardedInstanceExactlyOnce)
{
    // Given a first guard has just been constructed
    Guard guard_1{};
    EXPECT_EQ(LifecycleTracker::construction_count, 1U);

    // When a second guard is constructed while the first is still alive
    Guard guard_2{};

    // Then the guarded instance must not have been constructed a second time
    EXPECT_EQ(LifecycleTracker::construction_count, 1U);
}

TEST_F(StaticDestructionGuardTest, GuardedInstanceIsDestroyedOnceAllGuardsAreGone)
{
    // Given two nested guards are alive
    {
        Guard guard_1{};
        {
            Guard guard_2{};

            // Then the guarded instance must still be alive
            EXPECT_EQ(LifecycleTracker::destruction_count, 0U);
        }
        // When the inner guard goes out of scope while the outer one is still alive

        // Then the guarded instance must still not have been destroyed
        EXPECT_EQ(LifecycleTracker::destruction_count, 0U);
    }
    // When the last remaining guard also goes out of scope

    // Then the guarded instance must have been destroyed exactly once
    EXPECT_EQ(LifecycleTracker::destruction_count, 1U);
}

TEST_F(StaticDestructionGuardTest, GuardedInstanceOutlivesEveryGuardExceptTheLast)
{
    // Given two independently-owned guards are alive
    auto guard_1 = std::make_unique<Guard>();
    auto guard_2 = std::make_unique<Guard>();
    EXPECT_EQ(LifecycleTracker::construction_count, 1U);

    // When the first guard is destroyed while the second is still alive
    guard_1.reset();

    // Then the guarded instance must not have been destroyed yet
    EXPECT_EQ(LifecycleTracker::destruction_count, 0U);

    // When the last remaining guard is also destroyed
    guard_2.reset();

    // Then the guarded instance must have been destroyed exactly once
    EXPECT_EQ(LifecycleTracker::destruction_count, 1U);
}

TEST_F(StaticDestructionGuardTest, GetStorageReturnsFreshlyConstructedValue)
{
    // Given a guard has just been constructed
    Guard guard{};

    // When/Then the guarded instance is accessed via GetStorage(), it must hold its freshly constructed value
    EXPECT_EQ(Guard::GetStorage().value, LifecycleTracker::kInitialValue);
}

TEST_F(StaticDestructionGuardTest, GetStorageAlwaysRefersToTheSameLiveObject)
{
    // Given a guard has just been constructed
    Guard guard{};

    // GetStorage() used to reinterpret_cast the raw StorageType bytes to T& directly, without std::launder.
    // Per C++17 object-lifetime rules ([basic.life]), a reference obtained that way is not guaranteed
    // by the standard to refer to the T that was placement-new'd into that storage, even though it has the same
    // address - which is exactly what std::launder is required to fix.

    // When the guarded instance is mutated through one call to GetStorage()
    Guard::GetStorage().value = 1337;

    // Then that mutation must be visible through a later call, and both calls must yield the same address, proving
    // every access refers to the one live object
    EXPECT_EQ(Guard::GetStorage().value, 1337);
    EXPECT_EQ(&Guard::GetStorage(), &Guard::GetStorage());
}

}  // namespace
}  // namespace utils
}  // namespace score
