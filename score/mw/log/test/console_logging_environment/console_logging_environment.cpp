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
#include "score/mw/log/test/console_logging_environment/console_logging_environment.h"

#if (!defined KUSE_STUB_IMPLEMENTATION_ONLY)
#include "score/mw/log/detail/common/recorder_factory.h"
#else
#include "score/mw/log/detail/recorder_factory_stub.h"
#endif

#include "score/mw/log/runtime.h"

namespace score
{
namespace mw
{
namespace log
{

void ConsoleLoggingEnvironment::SetUp()
{
#if (!defined KUSE_STUB_IMPLEMENTATION_ONLY)
    score::mw::log::detail::Configuration config{};
    config.SetLogMode({score::mw::LogMode::kConsole});
    config.SetDefaultConsoleLogLevel(score::mw::log::LogLevel::kVerbose);
    recorder_ = score::mw::log::detail::RecorderFactory().CreateRecorderFromLogMode(score::mw::LogMode::kConsole, config);
#else
    recorder_ = score::mw::log::detail::RecorderFactory().CreateWithConsoleLoggingOnly(nullptr);
#endif

    score::mw::log::detail::Runtime::SetRecorder(recorder_.get());
}

void ConsoleLoggingEnvironment::TearDown()
{
    score::mw::log::detail::Runtime::SetRecorder(nullptr);
    recorder_.reset();
}

}  // namespace log
}  // namespace mw
}  // namespace score
