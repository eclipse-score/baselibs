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
#include "score/mw/log/detail/dlt_argument_counter.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

TEST(DltArgumentCounterShould, IncreaseCounter)
{
    RecordProperty("Requirement", "SCR-1633144");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "DltArgumentCounter should increase counter when an argument is added");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    uint8_t counter = 0;
    DltArgumentCounter sut{counter};
    EXPECT_EQ(AddArgumentResult::Added, sut.TryAddArgument([]() noexcept {
        return AddArgumentResult::Added;
    }));
    EXPECT_EQ(counter, 1);
}

TEST(DltArgumentCounterShould, NotIncreaseCounterBecauseArgumentNotAdded)
{
    RecordProperty("Requirement", "SCR-1633144");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "DltArgumentCounter should not increase counter when no argument is added");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    uint8_t counter = 0;
    DltArgumentCounter sut{counter};
    EXPECT_EQ(AddArgumentResult::NotAdded, sut.TryAddArgument([]() noexcept {
        return AddArgumentResult::NotAdded;
    }));
    EXPECT_EQ(counter, 0);
}

TEST(DltArgumentCounterShould, NotIncreaseCounterBecauseMaxCounterReached)
{
    RecordProperty("Requirement", "SCR-1633144");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "DltArgumentCounter should not increase counter when the counter has maximum value");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    uint8_t counter = 255;
    DltArgumentCounter sut{counter};
    EXPECT_EQ(AddArgumentResult::NotAdded, sut.TryAddArgument([]() noexcept {
        return AddArgumentResult::Added;
    }));
    EXPECT_EQ(counter, 255);
}

TEST(DltArgumentCounterShould, NotIncreaseCounterBecauseMaxCounterReachedAndNoArgumentAdded)
{
    RecordProperty("Requirement", "SCR-1633144");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "DltArgumentCounter should not increase counter when the counter has maximum value and no argument is added");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    uint8_t counter = 255;
    DltArgumentCounter sut{counter};
    EXPECT_EQ(AddArgumentResult::NotAdded, sut.TryAddArgument([]() noexcept {
        return AddArgumentResult::NotAdded;
    }));
    EXPECT_EQ(counter, 255);
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
