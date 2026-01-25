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

// Function has trivial implementation.
// The switch-case is used to map enum class to static strings to avoid memory allocation.
// SCORE_CCM_NO_LINT
std::string_view score::mw::log::detail::ErrorDomain::MessageFor(const score::result::ErrorCode& code) const noexcept
{
    std::string_view error_msg{};
    // boundary check is done by default case in switch
    // coverity[autosar_cpp14_a7_2_1_violation]
    switch (static_cast<score::mw::log::detail::Error>(code))
    {
        case Error::kInvalidLogLevelString:
            error_msg = "The string does not contain a valid log level.";
            break;
        case Error::kInvalidLogModeString:
            error_msg = "The string does not contain a valid log mode.";
            break;
        case Error::kConfigurationFilesNotFound:
            error_msg = "No logging configuration files could be found.";
            break;
        case Error::kConfigurationOptionalJsonKeyNotFound:
            error_msg = "Configuration key not found in JSON file.";
            break;
        case Error::kMaximumNumberOfRecordersExceeded:
            error_msg = "Exceeded the maximum number of active recorders.";
            break;
        case Error::kRecorderFactoryUnsupportedLogMode:
            error_msg = "Unsupported LogMode encountered in the RecorderFactory, using EmptyRecorder instead.";
            break;
        case Error::kNoLogModeSpecified:
            error_msg = "No log mode in configuration, using EmptyRecorder instead.";
            break;
        case Error::kReceiverInitializationError:
            error_msg = "Failed to initialize message passing receiver.";
            break;
        case Error::kUnlinkSharedMemoryError:
            error_msg = "Failed to unlink shared memory file. Memory might be leaked.";
            break;
        case Error::kFailedToSendMessageToDatarouter:
            error_msg = "Failed to send message to Datarouter. Logging is shutting down.";
            break;
        case Error::kFailedToSetLoggerThreadName:
            error_msg = "Failed to set thread name of logger thread.";
            break;
        case Error::kSetSharedMemoryPermissionsError:
            error_msg = "Failed to change ownership of shared memory file.";
            break;
        case Error::kShutdownDuringInitialization:
            error_msg = "Shutdown was requested during initialization of logging library.";
            break;
        case Error::kSloggerError:
            error_msg = "The slogger2 library error_msg =ed an error.";
            break;
        case Error::kLogFileCreationFailed:
            error_msg = "Failed to create the log file.";
            break;
        case Error::kBlockingTerminationSignalFailed:
            error_msg = "Failed to block termination signal.";
            break;
        case Error::kMemoryResourceError:
            error_msg = "Failed to get memory resource.";
            break;
        case Error::kFailedToCreateMessagePassingClient:
            error_msg = "Failed to create message passing client.";
            break;
        case Error::kUnknownError:
            error_msg = "Unknown Error";
            break;
        default:
            error_msg = "Unknown Error";
            break;
    }

    return error_msg;
}

namespace
{
constexpr score::mw::log::detail::ErrorDomain mw_log_error_domain;
}

score::result::Error score::mw::log::detail::MakeError(const score::mw::log::detail::Error code,
                                                   const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), mw_log_error_domain, user_message};
}
