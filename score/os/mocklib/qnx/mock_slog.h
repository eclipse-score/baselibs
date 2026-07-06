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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SLOG_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SLOG_H

#include "score/os/qnx/slog.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

namespace score
{
namespace os
{
namespace qnx
{

class MockSlog : public Slog
{
  public:
    // gmock can't mock variadics, so slogf() formats the message and forwards it here.
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                MockedSlogf,
                (SlogCode code, SlogSeverity severity, const std::string& message),
                (const, noexcept));

    score::cpp::expected<std::int32_t, score::os::Error> slogf(const SlogCode code,
                                                               const SlogSeverity severity,
                                                               const char* const format,
                                                               ...) const noexcept override;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SLOG_H
