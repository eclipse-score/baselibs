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
#include "score/mw/log/detail/error.h"

#include "gtest/gtest.h"

#include <cstdint>

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

class LogDetailErrorFixture : public ::testing::TestWithParam<Error>
{
  public:
    static score::result::Error ReturnError(const Error err)
    {
        return MakeError(err, "You did it!");
    }
};

INSTANTIATE_TEST_SUITE_P(ErrorCodes,
                         LogDetailErrorFixture,
                         ::testing::Values(Error::kUnknownError,
                                           Error::kInvalidLogLevelString,
                                           Error::kInvalidLogModeString,
                                           Error::kConfigurationFilesNotFound,
                                           Error::kConfigurationOptionalJsonKeyNotFound,
                                           Error::kMaximumNumberOfRecordersExceeded,
                                           Error::kRecorderFactoryUnsupportedLogMode,
                                           Error::kNoLogModeSpecified,
                                           Error::kReceiverInitializationError,
                                           Error::kUnlinkSharedMemoryError,
                                           Error::kFailedToSendMessageToDatarouter,
                                           Error::kFailedToSetLoggerThreadName,
                                           Error::kSetSharedMemoryPermissionsError,
                                           Error::kShutdownDuringInitialization,
                                           Error::kSloggerError,
                                           Error::kLogFileCreationFailed));

TEST_P(LogDetailErrorFixture, EachErrorShallReturnNonEmptyMessage)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of raising the error codes with a specific error message.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error_code = ReturnError(GetParam());
    EXPECT_GT(error_code.Message().size(), 0);
}

TEST(LgDetailErrorUnknown, TestUnknownError)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of raising the error codes with a specific error message.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error_code_out_of_range =
        LogDetailErrorFixture::ReturnError(static_cast<Error>(std::numeric_limits<std::size_t>::max()));
    EXPECT_GT(error_code_out_of_range.Message().size(), 0);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
