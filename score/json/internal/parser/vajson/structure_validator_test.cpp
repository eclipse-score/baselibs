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
#include "score/json/internal/parser/vajson/structure_validator.h"

#include <ostream>
#include <string>
#include <string_view>

#include "gtest/gtest.h"
#include "score/json/error.h"

namespace score::json::vajson::unit_test
{
namespace
{

/// Reports each token to the validator and then the end of the document, and returns the first error.
/// Tokens: 'v' value, 'k' key, '[' ']' '{' '}' container start and end, ',' comma.
auto Validate(const std::string_view tokens) -> score::Result<void>
{
    internal::StructureValidator validator{};
    score::Result<void> result{};
    for (const char token : tokens)
    {
        switch (token)
        {
            case 'v':
                result = validator.AddValue();
                break;
            case 'k':
                result = validator.AddKey();
                break;
            case '[':
                result = validator.OpenArray();
                break;
            case ']':
                result = validator.CloseArray();
                break;
            case '{':
                result = validator.OpenObject();
                break;
            case '}':
                result = validator.CloseObject();
                break;
            case ',':
                result = validator.AddComma();
                break;
            default:
                ADD_FAILURE() << "Unknown token " << token;
                break;
        }
        if (!result.has_value())
        {
            return result;
        }
    }
    return validator.CheckEndOfDocument();
}

struct RejectedParam
{
    std::string_view name;
    std::string_view tokens;
    std::string_view message;
};

// Found by gtest via ADL to print GetParam() in failure messages.
void PrintTo(const RejectedParam& param, std::ostream* os)
{
    *os << param.tokens;
}

class StructureValidatorRejectsTest : public testing::TestWithParam<RejectedParam>
{
};

}  // namespace

TEST(StructureValidatorTest, AcceptsWellFormedDocuments)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that a single top-level value, empty and nested arrays and objects, and comma separated "
                   "elements and members are accepted.");

    for (const std::string_view tokens : {"v", "[]", "{}", "[v,v]", "{kv,kv}", "[{kv},[v],v]", "{k[v],k{k{}}}"})
    {
        SCOPED_TRACE(tokens);
        EXPECT_TRUE(Validate(tokens).has_value());
    }
}

TEST(StructureValidatorTest, AcceptsTrailingCommas)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__trailing_commas");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that a comma before the end of an array or object is accepted.");

    for (const std::string_view tokens : {"[v,]", "{kv,}", "[[v,],{kv,},]"})
    {
        SCOPED_TRACE(tokens);
        EXPECT_TRUE(Validate(tokens).has_value());
    }
}

TEST_P(StructureValidatorRejectsTest, ReportsViolatedRule)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that a token at a position the JSON grammar does not allow is rejected as invalid JSON, "
                   "with the same message for the same rule regardless of the token.");

    const auto result{Validate(GetParam().tokens)};

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::kParsingError);
    EXPECT_EQ(result.error().UserMessage(), GetParam().message);
}

INSTANTIATE_TEST_SUITE_P(
    StructureValidatorTest,
    StructureValidatorRejectsTest,
    testing::Values(RejectedParam{"SecondTopLevelValue", "vv", "Multiple top-level values."},
                    RejectedParam{"ContainerAfterTopLevelValue", "[]{", "Multiple top-level values."},
                    RejectedParam{"KeyAfterTopLevelValue", "{}k", "Multiple top-level values."},
                    RejectedParam{"ValueWithoutComma", "[vv", "Missing comma."},
                    RejectedParam{"ContainerWithoutComma", "{kv[", "Missing comma."},
                    RejectedParam{"KeyWithoutComma", "{k[]k", "Missing comma."},
                    RejectedParam{"ValueWithoutKey", "{v", "Expected a key."},
                    RejectedParam{"ContainerWithoutKey", "{kv,[", "Expected a key."},
                    RejectedParam{"KeyAfterKey", "{kk", "Expected a value after a key."},
                    RejectedParam{"CommaAfterKey", "{k,", "Expected a value after a key."},
                    RejectedParam{"CloseAfterKey", "{k}", "Expected a value after a key."},
                    RejectedParam{"TopLevelKey", "k", "Key outside of an object."},
                    RejectedParam{"KeyInArray", "[k", "Key outside of an object."},
                    RejectedParam{"TopLevelComma", ",", "Comma outside of an array or object."},
                    RejectedParam{"CommaAfterTopLevelValue", "[],", "Comma outside of an array or object."},
                    RejectedParam{"TopLevelArrayClose", "]", "Unexpected closing bracket."},
                    RejectedParam{"ArrayCloseInObject", "{k]", "Unexpected closing bracket."},
                    RejectedParam{"ObjectCloseInArray", "[}", "Unexpected closing brace."},
                    RejectedParam{"EmptyDocument", "", "Document is empty."},
                    RejectedParam{"UnclosedArray", "{k[v", "Expected closing bracket."},
                    RejectedParam{"UnclosedObject", "[{kv", "Expected closing brace."}),
    [](const testing::TestParamInfo<RejectedParam>& param_info) {
        return std::string{param_info.param.name};
    });

}  // namespace score::json::vajson::unit_test
