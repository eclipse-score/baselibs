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
#include "score/result/error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

namespace score
{
namespace result
{
namespace
{

using ::testing::_;
using ::testing::Return;

enum class MyErrorCode : score::result::ErrorCode
{
    kFirstError,
    kSecondError,
};

enum class MyErrorCode2 : score::result::ErrorCode
{
    kFirstError,
    kSecondError,
};

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

class MyErrorDomain2 final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode&) const noexcept override
    {
        return "Unknown Error!";
    }
};

constexpr MyErrorDomain my_error_domain;
constexpr MyErrorDomain2 my_error_domain_2;

score::result::Error MakeError(MyErrorCode code, std::string_view user_message = "") noexcept
{
    return {static_cast<score::result::ErrorCode>(code), my_error_domain, user_message};
}

score::result::Error MakeError(MyErrorCode2 code, std::string_view user_message = "") noexcept
{
    return {static_cast<score::result::ErrorCode>(code), my_error_domain_2, user_message};
}

TEST(Error, CorrectConstructionAndAssignmentGuarantees)
{
    static_assert(std::is_trivially_copy_assignable_v<Error>);
    static_assert(std::is_trivially_copy_constructible_v<Error>);
    static_assert(std::is_trivially_move_assignable_v<Error>);
    static_assert(std::is_trivially_move_constructible_v<Error>);
    static_assert(std::is_nothrow_move_assignable_v<Error>);
    static_assert(std::is_nothrow_move_constructible_v<Error>);
}

TEST(Error, RetrieveMessage)
{
    // Given an constructed error with user message
    const score::result::Error unit{MyErrorCode::kFirstError, "We had a parsing failure"};

    // When trying to read the Message()
    const auto message = unit.Message();

    // That the message equals the expected one
    EXPECT_EQ(message, "First Error!");
}

TEST(Error, RetrieveUserMessage)
{
    // Given an constructed error with user message
    const score::result::Error unit{MyErrorCode::kFirstError, "We had a parsing failure"};

    // When trying to read the UserMessage()
    const auto user_message = unit.UserMessage();

    // That the message equals the expected one
    EXPECT_EQ(user_message, "We had a parsing failure");
}

TEST(Error, RetrieveUserMessageWhenNotProvided)
{
    // Given an constructed error with user message
    const score::result::Error unit{MyErrorCode::kFirstError};

    // When trying to read the UserMessage()
    const auto user_message = unit.UserMessage();

    // That the message equals the expected one
    EXPECT_EQ(user_message, "");
}

TEST(Error, CanCompareForEqual)
{
    // Given an constructed error
    score::result::Error unit{MyErrorCode::kFirstError};

    // When comparing it equal
    // That it returns true if it is equal
    EXPECT_EQ(unit, MyErrorCode::kFirstError);
}

TEST(Error, CanCompareForNotEqualOnDifferentDomains)
{
    // Given an constructed error
    score::result::Error unit{MyErrorCode::kFirstError};

    // When comparing it equal
    // That it returns true if is not equal
    EXPECT_NE(unit, MyErrorCode2::kFirstError);
}

TEST(Error, CanCompareForNotEqualOnDifferentValues)
{
    // Given an constructed error
    score::result::Error unit{MyErrorCode::kFirstError};

    // When comparing it equal
    // That it returns true if is not equal
    EXPECT_NE(unit, MyErrorCode::kSecondError);
}

TEST(Error, CanDereferenceToUnderlyingType)
{
    // Given an constructed error
    score::result::Error unit{MyErrorCode::kFirstError};

    // When comparing it equal
    // That it returns true if is not equal
    EXPECT_EQ(*unit, 0);  // e.g. helpful in switch-case statement with down-cast.
}

TEST(Error, ConstructViaMakeFunction)
{
    // When constructing the error via make function
    auto unit = MakeError(MyErrorCode::kSecondError);

    // Then the error is as expected
    EXPECT_NE(unit, MyErrorCode::kFirstError);
}

TEST(Error, CanLogCustomMessageToOstream)
{
    // Given an error with a custom message
    auto unit = MakeError(MyErrorCode::kSecondError, "Foo");

    // When logging it into a ostream
    std::stringstream stream{};
    stream << unit;

    // Then the message contains the custom message
    EXPECT_EQ(stream.str(), "Error Second Error! occurred with message Foo");
}

TEST(Error, WithUserMessagePreservesCodeAndDomain)
{
    // Given an error with a known code/domain
    const score::result::Error original{MyErrorCode::kSecondError, "original message"};

    // When creating a copy with a new user message
    const score::result::Error updated = original.WithUserMessage("updated message");

    // Then code/domain are preserved while the user message is replaced
    EXPECT_EQ(updated, original);
    EXPECT_EQ(*updated, *original);
    EXPECT_EQ(updated.Message(), original.Message());
    EXPECT_EQ(updated.UserMessage(), "updated message");
}

TEST(Error, WithUserMessageCanClearUserMessage)
{
    // Given an error with an existing user message
    const score::result::Error original{MyErrorCode::kFirstError, "has message"};

    // When creating a copy with an empty user message
    const score::result::Error updated = original.WithUserMessage("");

    // Then the user message is cleared
    EXPECT_EQ(updated, original);
    EXPECT_TRUE(updated.UserMessage().empty());
}

}  // namespace
}  // namespace result
}  // namespace score
