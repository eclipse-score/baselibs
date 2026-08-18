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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SLOG2_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SLOG2_H

#include "score/os/qnx/slog2.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{

class MockSlog2 : public Slog2
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                slog2_register,
                (const slog2_buffer_set_config_t* config, slog2_buffer_t* handles, std::uint32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                slog2c,
                (slog2_buffer_t buffer, std::uint16_t code, std::uint8_t severity, const char* data),
                (const, noexcept, override));

    MOCK_METHOD((std::int32_t),
                slog2_set_verbosity,
                (const slog2_buffer_t buffer, const uint8_t verbosity),
                (const, noexcept, override));

    MOCK_METHOD((std::int32_t), slog2_reset, (), (const, noexcept, override));
    // Googletest does not support variadic arguments. Therefore we pass through the formatted string so that we can use
    // it with MOCK_METHOD.
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                MockedSlog2f,
                (slog2_buffer_t buffer, std::uint16_t code, std::uint8_t severity, const std::string& message),
                (const, noexcept));

    score::cpp::expected<std::int32_t, score::os::Error> slog2f(slog2_buffer_t buffer,
                                                                std::uint16_t code,
                                                                std::uint8_t severity,
                                                                const char* format,
                                                                ...) const noexcept override;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SLOG2_H
