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

/// @file mw_log_demo_test.cpp
/// @brief Demonstrates logging through the mw::log frontend.

#include "score/mw/log/logging.h"

#include <gtest/gtest.h>

#include <cstdint>

TEST(MwLogDemo, LogsWithDefaultAndExplicitContext)
{
    const std::int32_t answer{42};
    score::mw::log::LogInfo() << "The answer is" << answer;
    score::mw::log::LogWarn("DEMO") << "Logged with an explicit context id";
}
