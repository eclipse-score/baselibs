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
#include "score/os/qnx/slog.h"
#include "score/os/qnx/slog_impl.h"

#include "score/memory.hpp"

#include <gtest/gtest.h>

#include <string>

namespace
{

struct SlogImplTest : ::testing::Test
{
    score::os::qnx::SlogImpl slog_{};
};

TEST_F(SlogImplTest, SlogfSucceedsWithSimpleMessage)
{
    const auto result =
        slog_.slogf(score::os::qnx::SlogCode::Network, score::os::qnx::SlogSeverity::Info, "simple message");

    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), -1);
}

TEST_F(SlogImplTest, SlogfSucceedsWithFormatArguments)
{
    const auto result =
        slog_.slogf(score::os::qnx::SlogCode::Network, score::os::qnx::SlogSeverity::Debug1, "value=%d str=%s", 42, "abc");

    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), -1);
}

TEST_F(SlogImplTest, SlogfTruncatesOverlongMessageAndSucceeds)
{
    const std::string overlong_message(8192U, 'x');

    const auto result = slog_.slogf(
        score::os::qnx::SlogCode::Network, score::os::qnx::SlogSeverity::Notice, "%s", overlong_message.c_str());

    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), -1);
}

TEST(SlogDefaultTest, DefaultReturnsUsableWrapper)
{
    const auto slog = score::os::qnx::Slog::Default(score::cpp::pmr::get_default_resource());
    ASSERT_NE(slog, nullptr);

    const auto result =
        slog->slogf(score::os::qnx::SlogCode::Network, score::os::qnx::SlogSeverity::Info, "from default");

    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), -1);
}

}  // namespace
