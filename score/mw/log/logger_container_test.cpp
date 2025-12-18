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

#include "score/mw/log/logger_container.h"
#include "score/mw/log/logger.h"

#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"

#include <string_view>

#include "gtest/gtest.h"
#include <thread>

namespace score
{
namespace mw
{
namespace log
{

namespace
{
const auto kContext1 = std::string_view{"MYCT"};
const auto kDefaultContext = std::string_view{"DFLT"};
}  // namespace

TEST(LoggerContainerTests, WhenRequestingNonExistingNewLoggerItShallBeInsertedAndReturnToCaller)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies the ability of requesting non-existing new logger shall be inserted and returned to the caller.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    LoggerContainer unit;
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kContext1);
}

TEST(LoggerContainerTests, WhenGetingDefaultLoggerShallGetDLFTContextID)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that when getting a default logger it shall get DFLT context id.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    LoggerContainer unit;
    EXPECT_EQ(unit.GetDefaultLogger().GetContext(), kDefaultContext);
}

TEST(LoggerContainerTests, WhenRequestingAlreadyExistingLoggerShallBeReturnedWithoutInsertingNewLogger)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that when requesting already existing logger shall be returned without inserting new logger.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    LoggerContainer unit;
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kContext1);
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kContext1);
}

TEST(LoggerContainerTests, WhenLoggerContainerIsFullShallGetDefaultContextWhenNewLoggerRequested)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that when logger container is full shall get default context when new logger is requested.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Expecting a log record of level verbose
    LoggerContainer unit;
    std::vector<std::string> contexts_vector(unit.GetCapacity());

    for (size_t i = 0; i < contexts_vector.size(); i++)
    {
        contexts_vector[i] = std::to_string(i);
    }
    // iterate to make the buffer full with 31 different context.
    for (const auto& context : contexts_vector)
    {
        // filling the buffer with different contexts till overrun.
        EXPECT_EQ(unit.GetLogger(context).GetContext(), std::string_view{context});
    }
    // buffer is full, default logger is returned.
    EXPECT_EQ(unit.GetLogger(kContext1).GetContext(), kDefaultContext);
    std::string_view inserted_context = contexts_vector[0];
    EXPECT_EQ(unit.GetLogger(inserted_context).GetContext(), inserted_context);
}

void LoggerRequester1(LoggerContainer& logger_container)
{
    EXPECT_EQ(logger_container.GetLogger(kContext1).GetContext(), kContext1);
}

void LoggerRequester2(LoggerContainer& logger_container)
{
    EXPECT_EQ(logger_container.GetLogger(kContext1).GetContext(), kContext1);
}

TEST(LoggerContainerTests, WhenTwoThreadsRequestSameLoggerShallBeOnlyOneExistingInLoggerContainer)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies that when two threads request the same logger it shall be only one existing in logger container.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    LoggerContainer unit;

    std::thread t1(LoggerRequester1, std::ref(unit));
    t1.join();

    std::thread t2(LoggerRequester2, std::ref(unit));
    t2.join();

    // default logger exists
    EXPECT_EQ(unit.GetLogger(kDefaultContext).GetContext(), kDefaultContext);
}

}  // namespace log
}  // namespace mw
}  // namespace score
