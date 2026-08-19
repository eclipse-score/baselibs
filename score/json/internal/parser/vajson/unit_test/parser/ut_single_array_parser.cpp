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
/*!        \file
 *        \brief  Contains tests concerning the SingleArrayParser class.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/composition_parser_impl.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{
namespace
{

class DynamicSingleArrayParser : public v2::SingleArrayParser
{
  public:
    using v2::SingleArrayParser::SingleArrayParser;

    MOCK_METHOD(ParserResult, OnElement, (), (noexcept, override));
};

struct InputData
{
    std::string_view json;
    std::string name;
    std::size_t index;
};

auto operator<<(std::ostream& os, const InputData& in) noexcept -> std::ostream&
{
    return os << in.name;
}

const InputData ordinary_array{R"([1,2,3])", "OrdinaryArray", 3};
const InputData empty_elements{R"([,,,,,1,,,,2])", "EmptyElements", 2};
const InputData trailing_commas{R"([1,,,])", "TrailingCommas", 1};
const InputData empty_array{R"([])", "EmptyArray", 0};
const InputData empty_elements_only{R"([,,,])", "EmptyElementsOnly", 0};

class UT__Parser__SingleArrayParser__Dynamic : public ::testing::TestWithParam<InputData>
{
  public:
    void SetUp() noexcept override
    {
        auto result = JsonData::FromBuffer(GetParam().json);
        ASSERT_TRUE(result.has_value());
        doc_ = std::make_unique<JsonData>(std::move(result).value());
        parser_ = std::make_unique<testing::NiceMock<DynamicSingleArrayParser>>(*doc_);

        ON_CALL(*parser_, OnElement()).WillByDefault([this]() noexcept {
            return parser_->Number<std::uint8_t>([](std::uint8_t) noexcept {});
        });
    }

    std::unique_ptr<JsonData> doc_;
    std::unique_ptr<testing::NiceMock<DynamicSingleArrayParser>> parser_;
};

}  // namespace

/*!
 * Tests the dynamic interface version of the SingleArrayParser.
 * \trace           score::json::vajson::v2::SingleArrayParser::GetIndex
 */
TEST_P(UT__Parser__SingleArrayParser__Dynamic, Success)
{
    const auto result = parser_->Parse();
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(parser_->GetIndex(), GetParam().index);
}

INSTANTIATE_TEST_SUITE_P(
    SingleArrayParser,
    UT__Parser__SingleArrayParser__Dynamic,
    ::testing::Values(ordinary_array, empty_elements, trailing_commas, empty_array, empty_elements_only),
    [](const testing::TestParamInfo<InputData>& input) noexcept {
        return input.param.name;
    });

/*!
 * Verify that v2 OnStartArray returns kStreamFailure when the underlying stream is already in a failed state.
 *
 * The test constructs a parser from an input stream, marks that stream as failed, and then checks that
 * entering the array reports the stream failure to the caller.
 *
 * \trace            score::json::vajson::v2::SingleArrayParser::OnStartArray
 */
TEST(UT__Parser__SingleArrayParser__Dynamic, OnStartArray__SteamErrorTell)
{
    std::istringstream dummy_stream{R"({"key":[]})"};
    JsonData json_data{dummy_stream};

    DynamicSingleArrayParser parser{json_data};
    ON_CALL(parser, OnElement()).WillByDefault([&parser]() noexcept {
        return parser.Number<std::uint8_t>([](std::uint8_t) noexcept {});
    });

    dummy_stream.setstate(std::ios_base::failbit);
    auto result = parser.OnStartArray();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), json::vajson::JsonErrc::kStreamFailure);
}

/*!
 * Verify that v2 OnUnexpectedEvent returns kStreamFailure when the stream enters a failed state while parsing
 * array elements.
 *
 * The test first transitions the parser into array-processing mode via OnStartArray(), then marks the backing
 * stream as failed, and finally checks that OnUnexpectedEvent propagates the stream failure.
 *
 * \trace            score::json::vajson::v2::SingleArrayParser::OnUnexpectedEvent
 */
TEST(UT__Parser__SingleArrayParser__Dynamic, OnUnexpectedEvent__SteamErrorSeek)
{

    std::istringstream dummy_stream{R"({"key":[]})"};

    JsonData json_data{dummy_stream};
    DynamicSingleArrayParser parser{json_data};
    ON_CALL(parser, OnElement()).WillByDefault([&parser]() noexcept {
        return parser.Number<std::uint8_t>([](std::uint8_t) noexcept {});
    });

    auto result = parser.OnStartArray();
    ASSERT_TRUE(result.has_value());

    dummy_stream.setstate(std::ios_base::failbit);
    auto unexpected_event_result = parser.OnUnexpectedEvent();
    ASSERT_FALSE(unexpected_event_result.has_value());
    EXPECT_EQ(unexpected_event_result.error(), vajson::JsonErrc::kStreamFailure);
}

/*!
 * Verify that v2 OnUnexpectedEvent forwards an error returned by OnElement().
 *
 * The test enters array-processing mode with OnStartArray(), configures OnElement() to fail with
 * kUserValidationFailed, and then checks that OnUnexpectedEvent returns the same error to the caller.
 *
 * \trace            score::json::vajson::v2::SingleArrayParser::OnUnexpectedEvent
 */
TEST(UT__Parser__SingleArrayParser__Dynamic, OnUnexpectedEvent__OnElementFails)
{

    std::istringstream dummy_stream{R"({"key":[]})"};
    JsonData json_data{dummy_stream};
    DynamicSingleArrayParser parser{json_data};
    EXPECT_CALL(parser, OnElement()).WillRepeatedly(testing::Return(MakeUnexpected(JsonErrc::kUserValidationFailed)));

    auto result = parser.OnStartArray();
    ASSERT_TRUE(result.has_value());
    auto unexpected_event_result = parser.OnUnexpectedEvent();
    ASSERT_FALSE(unexpected_event_result.has_value());
    EXPECT_EQ(unexpected_event_result.error(), JsonErrc::kUserValidationFailed);
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
