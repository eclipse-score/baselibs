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

TEST(ConsoleRegistrantTest, ConsoleBackendIsRegisteredAfterStaticInitialization)
{
    RecordProperty("Description", "The console backend registrant shall be registered for LogMode::kConsole");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Analysis of functional dependencies");

    EXPECT_TRUE(IsBackendAvailable(LogMode::kConsole));
}

TEST(ConsoleRegistrantTest, ConsoleBackendCreatorReturnsNonNullRecorder)
{
    RecordProperty("Description",
                   "The registered console backend shall return a non-null Recorder given valid configuration.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of functional dependencies");

    ASSERT_TRUE(IsBackendAvailable(LogMode::kConsole));

    const Configuration config;
    auto recorder = CreateRecorderForMode(LogMode::kConsole, config, score::cpp::pmr::get_default_resource());

    EXPECT_NE(recorder, nullptr);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
