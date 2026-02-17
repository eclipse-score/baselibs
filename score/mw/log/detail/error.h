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
#ifndef SCORE_MW_LOG_ERROR_H
#define SCORE_MW_LOG_ERROR_H

#include "score/result/error.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

enum class Error : score::result::ErrorCode
{
    kUnknownError = 0,
    kInvalidLogLevelString,
    kInvalidLogModeString,
    kConfigurationFilesNotFound,
    kConfigurationOptionalJsonKeyNotFound,
    kMaximumNumberOfRecordersExceeded,
    kRecorderFactoryUnsupportedLogMode,
    kNoLogModeSpecified,
    kReceiverInitializationError,
    kUnlinkSharedMemoryError,
    kFailedToSendMessageToDatarouter,
    kFailedToSetLoggerThreadName,
    kSetSharedMemoryPermissionsError,
    kShutdownDuringInitialization,
    kSloggerError,
    kLogFileCreationFailed,
    kBlockingTerminationSignalFailed,
    kMemoryResourceError,
    kFailedToCreateMessagePassingClient,
};

class ErrorDomain final : public score::result::ErrorDomain
{
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override;
};

score::result::Error MakeError(const score::mw::log::detail::Error code, const std::string_view user_message = "") noexcept;

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_ERROR_H
