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
#include "score/mw/log/backend_table.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

TEST(ConsoleRegistrantTest, ConsoleBackendIsNotRegisteredWhenDisabled)
{
    RecordProperty("Description", "The console backend registrant shall be registered for LogMode::kConsole");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_FALSE(IsBackendAvailable(LogMode::kConsole));
}

TEST(ConsoleRegistrantTest, CreateRecorderForModeReturnsNullptrWhenConsoleLoggingDisabled)
{
    RecordProperty(
        "Description",
        "CreateRecorderForMode shall return nullptr for LogMode::kConsole when console logging is disabled.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const Configuration config;
    auto recorder = CreateRecorderForMode(LogMode::kConsole, config, score::cpp::pmr::get_default_resource());

    EXPECT_EQ(recorder, nullptr);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
