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

#include "score/mw/log/log_stream_factory.h"
#include "score/mw/log/detail/thread_local_guard.h"

#include <gtest/gtest.h>

#include <sstream>

namespace score
{
namespace mw
{
namespace log
{

namespace
{

TEST(LogStreamFactoryTest, GetStreamOutsideLogging)
{
    auto log_level = LogLevel::kVerbose;
    const std::string_view context_id = "1234";

    auto stream = detail::LogStreamFactory::GetStream(log_level, context_id);
    EXPECT_NO_FATAL_FAILURE(stream << "test");

    // ThreadLocalGuard used inside LogStreamFactory::GetStream() to determine local context
    // simulate situation when LogStreamFactory::GetStream() is called from LogStreamFactory::GetStream()
    // it is enouth just to create instance of ThreadLocalGuard() as it statically tracks context
    detail::ThreadLocalGuard guard{};
    auto stream_inside = detail::LogStreamFactory::GetStream(log_level, context_id);

    // expect no crashes/other failures due to using logging inside logging
    EXPECT_NO_FATAL_FAILURE(stream_inside << "test");
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace score
