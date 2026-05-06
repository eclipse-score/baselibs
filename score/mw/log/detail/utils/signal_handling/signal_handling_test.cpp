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
#include "score/mw/log/detail/utils/signal_handling/signal_handling.h"
#include "score/os/errno.h"
#include "score/os/utils/mocklib/signalmock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <future>
#include <iostream>
namespace score::mw::log::detail
{
namespace
{
using ::testing::_;
using ::testing::Return;

class SignalHandlingTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        signal_mock_ = new score::os::SignalMock();
    }

    void TearDown() override
    {
        delete signal_mock_;
    }

    score::os::SignalMock* signal_mock_;
};

// ============================================================
// PThreadBlockSigTerm tests
// ============================================================

TEST_F(SignalHandlingTest, PThreadBlockSigTerm_SigEmptySetFails_ReturnsError)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, _)).Times(0);
    EXPECT_CALL(*signal_mock_, PthreadSigMask(_, _)).Times(0);

    auto result = SignalHandling::PThreadBlockSigTerm(*signal_mock_);
    EXPECT_FALSE(result.has_value());
}

TEST_F(SignalHandlingTest, PThreadBlockSigTerm_SigAddSetFails_ReturnsError)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, SIGTERM))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    EXPECT_CALL(*signal_mock_, PthreadSigMask(_, _)).Times(0);

    auto result = SignalHandling::PThreadBlockSigTerm(*signal_mock_);
    EXPECT_FALSE(result.has_value());
}

TEST_F(SignalHandlingTest, PThreadBlockSigTerm_PthreadSigMaskFails_ReturnsError)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, SIGTERM)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, PthreadSigMask(SIG_BLOCK, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    auto result = SignalHandling::PThreadBlockSigTerm(*signal_mock_);
    EXPECT_FALSE(result.has_value());
}

TEST_F(SignalHandlingTest, PThreadBlockSigTerm_AllSucceed_ReturnsSuccess)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, SIGTERM)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, PthreadSigMask(SIG_BLOCK, _))
        .WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    auto result = SignalHandling::PThreadBlockSigTerm(*signal_mock_);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}

// ============================================================
// PThreadUnblockSigTerm tests
// ============================================================

TEST_F(SignalHandlingTest, PThreadUnblockSigTerm_SigEmptySetFails_ReturnsError)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, _)).Times(0);
    EXPECT_CALL(*signal_mock_, PthreadSigMask(_, _)).Times(0);

    auto result = SignalHandling::PThreadUnblockSigTerm(*signal_mock_);
    EXPECT_FALSE(result.has_value());
}

TEST_F(SignalHandlingTest, PThreadUnblockSigTerm_SigAddSetFails_ReturnsError)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, SIGTERM))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    EXPECT_CALL(*signal_mock_, PthreadSigMask(_, _)).Times(0);

    auto result = SignalHandling::PThreadUnblockSigTerm(*signal_mock_);
    EXPECT_FALSE(result.has_value());
}

TEST_F(SignalHandlingTest, PThreadUnblockSigTerm_PthreadSigMaskFails_ReturnsError)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, SIGTERM)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, PthreadSigMask(SIG_UNBLOCK, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    auto result = SignalHandling::PThreadUnblockSigTerm(*signal_mock_);
    EXPECT_FALSE(result.has_value());
}

TEST_F(SignalHandlingTest, PThreadUnblockSigTerm_AllSucceed_ReturnsSuccess)
{
    EXPECT_CALL(*signal_mock_, SigEmptySet(_)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, SigAddSet(_, SIGTERM)).WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    EXPECT_CALL(*signal_mock_, PthreadSigMask(SIG_UNBLOCK, _))
        .WillOnce(Return(score::cpp::expected<std::int32_t, score::os::Error>{0}));

    auto result = SignalHandling::PThreadUnblockSigTerm(*signal_mock_);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}

}  // namespace

}  // namespace score::mw::log::detail
