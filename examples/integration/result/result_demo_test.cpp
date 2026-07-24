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

/// @file result_demo_test.cpp
/// @brief Demonstrates defining a custom error domain, returning score::Result<T>
///        from functions, and handling both the success and error paths.

#include "score/result/result.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <string_view>

// ─── Step 1: Define a custom error code enum ────────────────────────────────

enum class MyErrorCode : score::result::ErrorCode
{
    kFirstError,
    kSecondError,
};

// ─── Step 2: Define a custom error domain ───────────────────────────────────

class MyErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (static_cast<MyErrorCode>(code))
        {
            case MyErrorCode::kFirstError:
                return "First Error!";
            case MyErrorCode::kSecondError:
                return "Second Error!";
            default:
                return "Unknown Error!";
        }
    }
};

// ─── Step 3: Provide a MakeError() overload for ADL lookup ──────────────────

constexpr MyErrorDomain my_error_domain;

score::result::Error MakeError(MyErrorCode code, std::string_view user_message = "") noexcept
{
    return {static_cast<score::result::ErrorCode>(code), my_error_domain, user_message};
}

// ─── Step 4: Define functions using score::Result<T> ────────────────────────

score::Result<std::string> MyFancyFunction(std::int32_t number)
{
    if (number == 42)
    {
        return score::MakeUnexpected(MyErrorCode::kFirstError, "You did it!");
    }
    return "Try Again!";
}

// score::Result<void> is used when there is no meaningful return value.
// A default-constructed Result<void>{} signals success.
score::Result<void> MyVoidFunction(std::int32_t number)
{
    if (number == 42)
    {
        return score::MakeUnexpected(MyErrorCode::kSecondError, "Void path failed!");
    }
    return {};
}

// ─── Tests: Result<std::string> ──────────────────────────────────────────────

TEST(ResultDemoTest, SuccessPath)
{
    const auto result = MyFancyFunction(0);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "Try Again!");
}

TEST(ResultDemoTest, ErrorPath)
{
    const auto result = MyFancyFunction(42);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MyErrorCode::kFirstError);
    EXPECT_EQ(result.error().Message(), "First Error!");
    EXPECT_EQ(result.error().UserMessage(), "You did it!");
}

// ─── Tests: Result<void> ─────────────────────────────────────────────────────

TEST(ResultDemoTest, VoidSuccessPath)
{
    const auto result = MyVoidFunction(0);

    EXPECT_TRUE(result.has_value());
}

TEST(ResultDemoTest, VoidErrorPath)
{
    const auto result = MyVoidFunction(42);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MyErrorCode::kSecondError);
    EXPECT_EQ(result.error().Message(), "Second Error!");
    EXPECT_EQ(result.error().UserMessage(), "Void path failed!");
}
