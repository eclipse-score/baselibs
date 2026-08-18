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
#ifndef SCORE_LIB_OS_MOCKLIB_STATMOCK_H
#define SCORE_LIB_OS_MOCKLIB_STATMOCK_H

#include "score/os/stat.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class StatMock : public Stat
{
  public:
    MOCK_METHOD(score::cpp::expected_blank<Error>, stat, (const char*, StatBuffer&, bool), (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>,
                fstat,
                (const std::int32_t, StatBuffer&),
                (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>, mkdir, (const char*, const Stat::Mode), (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>,
                chmod,
                (const char* path, const Mode mode),
                (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>,
                fchmod,
                (const std::int32_t, const Stat::Mode),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<Stat::Mode, Error>), umask, (const Stat::Mode), (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>,
                fchmodat,
                (const std::int32_t, const char*, const Mode, const bool),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_STATMOCK_H
