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
#include "score/language/safecpp/safe_atomics/error.h"

#include "gtest/gtest.h"

namespace score::safe_atomics
{
namespace
{
class ErrorDomainTest : public ::testing::Test
{
  protected:
    ErrorDomain message_obj;

    auto verifyErrorMessage(ErrorCode errorCode, std::string_view errorOutput)
    {
        const auto errorCodeTest = message_obj.MessageFor(static_cast<score::result::ErrorCode>(errorCode));
        ASSERT_EQ(errorCodeTest, errorOutput);
    }
};

TEST_F(ErrorDomainTest, kUnexpectedError)
{
    verifyErrorMessage(ErrorCode::kUnexpectedError, "Unexpected Error was propagated up by a dependent library");
}

TEST_F(ErrorDomainTest, kExceedsNumericLimits)
{
    verifyErrorMessage(ErrorCode::kExceedsNumericLimits, "Operation exceeds numeric limits");
}

TEST_F(ErrorDomainTest, kMaxRetriesReached)
{
    verifyErrorMessage(ErrorCode::kMaxRetriesReached, "Max retries reached");
}

TEST_F(ErrorDomainTest, kUnknown)
{
    verifyErrorMessage(ErrorCode::kUnknown, "Unknown error");
}

TEST_F(ErrorDomainTest, defaultValue)
{
    // Errorcode has enum until value from 0, to use default case use -1.
    int ValueOutOfRange = -1;
    verifyErrorMessage(static_cast<ErrorCode>(ValueOutOfRange), "Unknown error");
}
}  // namespace
}  // namespace score::safe_atomics
