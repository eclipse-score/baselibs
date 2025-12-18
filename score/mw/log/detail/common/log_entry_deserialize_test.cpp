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
#include "score/mw/log/detail/common/log_entry_deserialize.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace log_entry_deserialization
{
namespace test
{

TEST(LogEntryDeserialize, PayloadSpanShallBeAccessible)
{
    const std::array<uint8_t, 16> data = {};
    LogEntryDeserializationReflection unit{
        LoggingIdentifier{"ABSD"}, LoggingIdentifier{"DFLT"}, {{data.begin(), data.size()}}, 0UL, LogLevel::kOff};

    const auto result = unit.GetPayload();
    EXPECT_EQ(result.size(), sizeof(data));
    EXPECT_EQ(result.data(), data.begin());
}

}  // namespace test
}  // namespace log_entry_deserialization
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
