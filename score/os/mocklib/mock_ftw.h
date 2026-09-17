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
#ifndef SCORE_LIB_OS_MOCKLIB_MOCK_FTW_H
#define SCORE_LIB_OS_MOCKLIB_MOCK_FTW_H

#include "score/os/interface/ftw.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

class MockFtw : public Ftw
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                ftw,
                (const char* path,
                 std::int32_t (*fn)(const char* fname, const struct stat* sbuf, std::int32_t flag),
                 std::int32_t ndirs),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MOCK_FTW_H
