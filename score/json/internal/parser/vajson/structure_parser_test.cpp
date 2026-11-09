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
/// @brief  Contains tests concerning the StructureParser class.

#include "score/json/internal/parser/vajson/structure_parser.h"

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/json_data.h"
#include "score/json/internal/parser/vajson/json_error_domain.h"
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

/// @brief Parser that accepts the first num_accepted events and fails on the next one.
class FailingParser final : public internal::StructureParser
{
  public:
    explicit FailingParser(JsonData& doc, std::size_t num_accepted) noexcept
        : internal::StructureParser(doc), num_accepted_{num_accepted}
    {
    }

    auto GetCount() const noexcept -> std::size_t
    {
        return this->count_;
    }

  private:
    auto OnNull() noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnBool(bool) noexcept -> ParserResult override  // NOLINT(readability/casting)
    {
        return this->OnEvent();
    }

    auto OnNumber(JsonNumber) noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnString(CStringView) noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnKey(CStringView) noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnStartObject() noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnEndObject(std::size_t) noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnStartArray() noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnEndArray(std::size_t) noexcept -> ParserResult override
    {
        return this->OnEvent();
    }

    auto OnEvent() noexcept -> ParserResult
    {
        if (this->count_ >= this->num_accepted_)
        {
            ++this->count_;
            return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed);
        }
        ++this->count_;
        return ParserResult{ParserState::kRunning};
    }

    std::size_t num_accepted_;
    std::size_t count_{0};
};

}  // namespace

/// Test that an error returned by any callback aborts parsing with that error.
/// @trace           score::json::vajson::internal::StructureParser
TEST(UT__Parser, Static__CallbackError)
{
    constexpr std::string_view doc{R"({"key": [123, "string", null, true]})"};
    const std::size_t num_events{9};

    for (std::size_t idx{0}; idx < num_events; ++idx)
    {
        SCOPED_TRACE(idx);
        auto result{JsonData::FromBuffer(doc)};
        ASSERT_TRUE(result.has_value());
        JsonData data{std::move(result).value()};

        FailingParser parser{data, idx};
        const auto parse_result{parser.Parse()};
        ASSERT_FALSE(parse_result.has_value());
        ASSERT_EQ(parse_result.error(), JsonErrc::kUserValidationFailed);
        EXPECT_EQ(parser.GetCount(), idx + 1U);
    }

    auto result{JsonData::FromBuffer(doc)};
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    FailingParser parser{data, num_events};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.GetCount(), num_events);
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
