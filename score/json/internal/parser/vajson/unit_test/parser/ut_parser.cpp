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
 *        \brief  Contains tests concerning the VirtualParser class.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/common/json_error_domain.h"
#include "score/json/internal/parser/vajson/reader/internal/parsers/virtual_parser.h"
#include "score/json/internal/parser/vajson/reader/json_data.h"
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
 * Test that a parser that does not overwrite any methods uses the default methods which return an error.
 * \trace           score::json::vajson::internal::VirtualParser
 */
TEST(UT__Parser, Static__Empty)
{
    constexpr std::string_view doc{R"({"key": [123, "string", null, true]})"};
    const std::size_t num_elements{9};

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
