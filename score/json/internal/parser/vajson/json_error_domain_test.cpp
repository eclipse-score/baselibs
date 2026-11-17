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
/// @file
/// @brief  Contains tests concerning the vaJson error domain.
#include "score/json/internal/parser/vajson/json_error_domain.h"

#include <string_view>

#include "gtest/gtest.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

/// Test vaJson error domain messages.
TEST(UT__ErrorDomain, General)
{
    const JsonErrorDomain domain{};

    // Unknown error code (below range)
    ASSERT_EQ(domain.MessageFor(-1), std::string_view{"Unknown error."});

    // Ordinary error codes
    ASSERT_EQ(domain.MessageFor(static_cast<ErrorCode>(JsonErrc::kInvalidJson)),
              std::string_view{"kInvalidJson: Invalid JSON was encountered."});
    ASSERT_EQ(domain.MessageFor(static_cast<ErrorCode>(JsonErrc::kStreamFailure)),
              std::string_view{"kStreamFailure: The underlying file or character stream encountered an error."});

    // Too large error code
    ASSERT_EQ(domain.MessageFor(static_cast<ErrorCode>(JsonErrc::kStreamFailure) + 1),
              std::string_view{"Unknown error."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
