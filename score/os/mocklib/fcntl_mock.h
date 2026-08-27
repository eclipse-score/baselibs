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
#ifndef SCORE_LIB_OS_MOCKLIB_FCNTL_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_FCNTL_MOCK_H

#include "score/os/fcntl.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class FcntlMock : public Fcntl
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                fcntl,
                (const std::int32_t, const Fcntl::Command, const Fcntl::Open),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<Fcntl::Open, Error>),
                fcntl,
                (const std::int32_t, const Fcntl::Command),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                open,
                (const char*, Fcntl::Open),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                open,
                (const char*, Fcntl::Open, Stat::Mode),
                (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>,
                posix_fallocate,
                (const std::int32_t, const off_t, const off_t),
                (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>,
                flock,
                (const std::int32_t, const Fcntl::Operation),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_FCNTL_MOCK_H
