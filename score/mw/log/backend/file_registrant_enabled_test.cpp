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

TEST(FileRegistrantTest, FileBackendIsRegisteredAfterStaticInitialization)
{
    RecordProperty("Description",
                   "The file backend registrant shall register a creator for LogMode::kFile during static init.");
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("DerivationTechnique", "Analysis of functional dependencies");

    EXPECT_TRUE(IsBackendAvailable(LogMode::kFile));
}

TEST(FileRegistrantTest, FileBackendCreatorReturnsNonNullRecorder)
{
    RecordProperty("Description",
                   "The registered file backend creator shall return a non-null Recorder given valid configuration.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of functional dependencies");

    ASSERT_TRUE(IsBackendAvailable(LogMode::kFile));

    Configuration config;
    config.SetLogFilePath("/tmp");
    config.SetAppId("TEST");
    config.SetEcuId("ECU1");

    auto recorder = CreateRecorderForMode(LogMode::kFile, config, score::cpp::pmr::get_default_resource());

    EXPECT_NE(recorder, nullptr);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
