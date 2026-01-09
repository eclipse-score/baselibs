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
#include "score/os/utils/interprocess/interprocess_mutex.h"

#include "gtest/gtest.h"

#include <mutex>

namespace score
{
namespace os
{
namespace test
{

TEST(InterprocessMutex, locks)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InterprocessMutex locks");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::os::InterprocessMutex unit{};

    unit.lock();
    ASSERT_FALSE(unit.try_lock());
    unit.unlock();
}

TEST(InterprocessMutex, LocksAndFrees)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InterprocessMutex Locks And Frees");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::os::InterprocessMutex unit{};

    unit.lock();
    unit.unlock();
}

TEST(InterprocessMutex, DoubleTryLockFails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InterprocessMutex Double Try Lock Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::os::InterprocessMutex unit{};

    ASSERT_TRUE(unit.try_lock());
    ASSERT_FALSE(unit.try_lock());
    unit.unlock();
}

TEST(InterprocessMutex, FulfillsBasicLockableRequirements)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InterprocessMutex Fulfills Basic Lockable Requirements");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::os::InterprocessMutex unit{};

    std::lock_guard<score::os::InterprocessMutex> lock{unit};
}

}  // namespace test
}  // namespace os
}  // namespace score
