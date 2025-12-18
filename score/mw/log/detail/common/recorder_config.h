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
#ifndef SCORE_MW_LOG_DETAIL_COMMON_RECORDER_CONFIG_H
#define SCORE_MW_LOG_DETAIL_COMMON_RECORDER_CONFIG_H

#include "score/mw/log/detail/empty_recorder_factory.h"

#if defined(KCONSOLE_LOGGING)
#include "score/mw/log/detail/text_recorder/console_recorder_factory.h"
#include "score/mw/log/detail/text_recorder/text_recorder.h"
#endif

#if defined(KFILE_LOGGING)
#include "score/mw/log/detail/file_recorder/file_recorder_factory.h"
#endif

#if defined(KREMOTE_LOGGING)
#include "score/mw/log/detail/data_router/data_router_recorder.h"
#include "score/mw/log/detail/data_router/remote_dlt_recorder_factory.h"
#endif

#if defined(__QNXNTO__)
#include "score/mw/log/detail/slog/slog_recorder_factory.h"
#endif

#if defined(KCUSTOM_LOGGING)
#include "custom_recorder.h"
#endif

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
/*
Deviation from Rule A16-0-1:
- Rule A16-0-1 (required, implementation, automated)
The pre-processor shall only be used for unconditional and conditional file
inclusion and include guards, and using the following directives: (1) #ifndef,
#ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9)
#include.
Justification:
- Feature Flag to enable/disable Logging Modes in Production SW.
*/
// coverity[autosar_cpp14_a16_0_1_violation] see above
#if defined(KCONSOLE_LOGGING)
using ConsoleRecorderType = TextRecorder;
using ConsoleRecorderFactoryType = ConsoleRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#else
using ConsoleRecorderType = EmptyRecorder;
using ConsoleRecorderFactoryType = EmptyRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

// coverity[autosar_cpp14_a16_0_1_violation] see above
#if defined(KFILE_LOGGING)
using FileRecorderType = FileRecorder;
using FileRecorderFactoryType = FileRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#else
using FileRecorderType = EmptyRecorder;
using FileRecorderFactoryType = EmptyRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

// coverity[autosar_cpp14_a16_0_1_violation] see above
#if defined(KREMOTE_LOGGING)
using RemoteRecorderType = DataRouterRecorder;
using RemoteRecorderFactoryType = RemoteDltRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#else
using RemoteRecorderType = EmptyRecorder;
using RemoteRecorderFactoryType = EmptyRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

// coverity[autosar_cpp14_a16_0_1_violation] see above
#if defined(__QNXNTO__)
using SystemRecorderType = TextRecorder;
using SystemRecorderFactoryType = SlogRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#else
using SystemRecorderType = EmptyRecorder;
using SystemRecorderFactoryType = EmptyRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

// coverity[autosar_cpp14_a16_0_1_violation] see above
#if defined(KCUSTOM_LOGGING)
using CustomRecorderType = CustomRecorder;
using CustomRecorderFactoryType = CustomRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#else
using CustomRecorderType = EmptyRecorder;
using CustomRecorderFactoryType = EmptyRecorderFactory;
// coverity[autosar_cpp14_a16_0_1_violation] see above
#endif

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_COMMON_RECORDER_CONFIG_H
