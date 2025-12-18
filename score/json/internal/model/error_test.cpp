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
#include "score/json/internal/model/error.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace score
{
namespace json
{
namespace
{

using ::testing::Eq;
using ::testing::Ne;

TEST(Error, CanMakeError)
{
    RecordProperty("Verifies", "::score::json::MakeError");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Use MakeError() to create Error instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    score::result::Error error = score::json::MakeError(Error::kParsingError);

    EXPECT_THAT(*error, Eq(static_cast<result::ErrorCode>(Error::kParsingError)));
}

TEST(Error, CanMakeErrorWithUserMessage)
{
    RecordProperty("Verifies", "::score::json::MakeError");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Use MakeError() with error message to create Error instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    std::string_view userMessage{"User message"};
    score::result::Error error = score::json::MakeError(Error::kUnknownError, userMessage);

    EXPECT_THAT(error.UserMessage(), Eq(userMessage));
}

TEST(Error, CanGetMessageForWrongType)
{
    RecordProperty("Verifies", "::score::result::Error::Message");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get error message from WrongType error.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    score::result::Error error = score::json::MakeError(Error::kWrongType);

    std::string_view message;
    EXPECT_NO_FATAL_FAILURE(message = error.Message());
    EXPECT_THAT(message, Ne(std::string_view{}));
}

TEST(Error, CanGetMessageForKeyNotFound)
{
    RecordProperty("Verifies", "::score::result::Error::Message");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get error message from KeyNotFound error.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    score::result::Error error = score::json::MakeError(Error::kKeyNotFound);

    std::string_view message;
    EXPECT_NO_FATAL_FAILURE(message = error.Message());
    EXPECT_THAT(message, Ne(std::string_view{}));
}

TEST(Error, CanGetMessageForParsingError)
{
    RecordProperty("Verifies", "::score::result::Error::Message");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get error message from ParsingError error.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    score::result::Error error = score::json::MakeError(Error::kParsingError);

    std::string_view message;
    EXPECT_NO_FATAL_FAILURE(message = error.Message());
    EXPECT_THAT(message, Ne(std::string_view{}));
}

TEST(Error, CanGetMessageForInvalidFilePath)
{
    RecordProperty("Verifies", "::score::result::Error::Message");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get error message from InvalidFilePath error.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    score::result::Error error = score::json::MakeError(Error::kInvalidFilePath);

    std::string_view message;
    EXPECT_NO_FATAL_FAILURE(message = error.Message());
    EXPECT_THAT(message, Ne(std::string_view{}));
}

TEST(Error, CanGetMessageForUnknownError)
{
    RecordProperty("Verifies", "::score::result::Error::Message");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get error message from UnknownError.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    score::result::Error error = score::json::MakeError(Error::kUnknownError);

    std::string_view message;
    EXPECT_NO_FATAL_FAILURE(message = error.Message());
    EXPECT_THAT(message, Ne(std::string_view{}));
}

TEST(Error, CanGetMessageForUndefinedError)
{
    RecordProperty("Verifies", "::score::result::Error::Message");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get error message from UndefinedError.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    score::result::Error error = score::json::MakeError(static_cast<Error>(-1));

    std::string_view message;
    EXPECT_NO_FATAL_FAILURE(message = error.Message());
    EXPECT_THAT(message, Ne(std::string_view{}));
}

}  // namespace
}  // namespace json
}  // namespace score
