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
 *        \brief  Contains tests concerning the StrictParser and VirtualParser classes.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/virtual_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include <utility>

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

class StaticTestParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnKey(StringView) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto GetDoc() const noexcept -> const JsonData&
    {
        return internal::VirtualParser::GetJsonDocument();
    }

    auto GetKey() const noexcept -> CStringView
    {
        return internal::VirtualParser::GetCurrentKey();
    }
};

class DynamicTestParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnKey(StringView) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto GetDoc() const noexcept -> const JsonData&
    {
        return internal::VirtualParser::GetJsonDocument();
    }

    auto GetKey() const noexcept -> CStringView
    {
        return internal::VirtualParser::GetCurrentKey();
    }
};

class EmptyParser final : public internal::VirtualParser
{
  public:
    explicit EmptyParser(JsonData& doc, std::size_t num_ignore) noexcept
        : internal::VirtualParser(doc), num_ignore_{num_ignore}
    {
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        if (this->count_ >= this->num_ignore_)
        {
            ++this->count_;
            return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed);
        }
        ++this->count_;
        return ParserResult{ParserState::kRunning};
    }

  private:
    std::size_t num_ignore_;
    std::size_t count_{0};
};

}  // namespace

/*!
 * Test that GetJsonDocument with const qualifier returns the current JsonData object.
 * \trace           score::json::vajson::internal::StrictParser::GetJsonDocument
 */
TEST(UT__Parser, Static__GetJsonDocumentConst)
{
    auto result = JsonData::FromBuffer(std::string_view{""});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    const StaticTestParser parser{data};
    ASSERT_EQ(&data, &parser.GetDoc());
}

/*!
 * Test that GetJsonDocument with const qualifier returns the current JsonData object of the VirtualParser.
 * \trace           score::json::vajson::internal::StrictParser::GetJsonDocument
 */
TEST(UT__Parser, Dynamic__GetJsonDocumentConst)
{
    auto result = JsonData::FromBuffer(std::string_view{""});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    const DynamicTestParser parser{data};
    ASSERT_EQ(&data, &parser.GetDoc());
}

/*!
 * Test that GetCurrentKey with const qualifier returns the last parsed key.
 * \trace           score::json::vajson::internal::StrictParser::GetCurrentKey
 */
TEST(UT__Parser, Static__GetCurrentKeyConst)
{
    auto result = JsonData::FromBuffer(std::string_view{R"({"key":123})"});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    StaticTestParser parser{data};
    ASSERT_TRUE(parser.Parse().has_value());

    ASSERT_STREQ(parser.GetKey().c_str(), "key");
}

/*!
 * Test that GetCurrentKey with const qualifier returns the last parsed key of the VirtualParser.
 * \trace           score::json::vajson::internal::StrictParser::GetCurrentKey
 */
TEST(UT__Parser, Dynamic__GetCurrentKeyConst)
{
    auto result = JsonData::FromBuffer(std::string_view{R"({"key":123})"});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    DynamicTestParser parser{data};
    ASSERT_TRUE(parser.Parse().has_value());

    ASSERT_STREQ(parser.GetKey().c_str(), "key");
}

/*!
 * Test that SubParse succeeds.
 * \trace           score::json::vajson::internal::StrictParser::SubParse
 */
TEST(UT__Parser, Static__SubParse)
{
    auto result = JsonData::FromBuffer(std::string_view{R"({"key":123})"});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    StaticTestParser parser{data};
    const auto sub_result = parser.SubParse();
    ASSERT_TRUE(sub_result.has_value());
    ASSERT_EQ(sub_result.value(), ParserState::kRunning);
}

/*!
 * Test that a parser that does not overwrite any methods uses the default methods which return an error.
 * \trace           score::json::vajson::internal::StrictParser
 */
TEST(UT__Parser, Static__Empty)
{
    constexpr std::string_view doc{R"({"key": [123, "string", null, true]})"};
    std::size_t const num_elements{9};

    for (std::size_t idx{0}; idx < num_elements; ++idx)
    {
        auto result = JsonData::FromBuffer(doc);
        ASSERT_TRUE(result.has_value());
        JsonData data{std::move(result).value()};

        EmptyParser parser{data, idx};
        const auto parse_result = parser.Parse();
        ASSERT_FALSE(parse_result.has_value());
        ASSERT_EQ(parse_result.error(), JsonErrc::kUserValidationFailed);
    }
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
