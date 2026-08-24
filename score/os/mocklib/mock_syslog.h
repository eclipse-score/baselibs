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
#ifndef SCORE_LIB_OS_MOCKLIB_MOCK_SYSLOG_H
#define SCORE_LIB_OS_MOCKLIB_MOCK_SYSLOG_H

#include "score/os/syslog.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

namespace score
{
namespace os
{

class MockSyslog : public Syslog
{
  public:
    MOCK_METHOD(void,
                openlog,
                (const char* ident, std::int32_t option, std::int32_t facility),
                (const, noexcept, override));

    MOCK_METHOD(void, closelog, (), (const, noexcept, override));

    // Googletest does not support variadic arguments. Therefore we pass through the formatted string so that we can
    // use it with MOCK_METHOD.
    MOCK_METHOD(void, MockedSyslog, (std::int32_t priority, const std::string& message), (const, noexcept));

    void syslog(std::int32_t priority, const char* format, ...) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MOCK_SYSLOG_H
