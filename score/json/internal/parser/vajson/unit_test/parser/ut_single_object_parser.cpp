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
 *        \brief  Contains tests concerning the SingleObjectParser class.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/single_object_parser.h"
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

class DynamicSingleObjectParser : public v2::SingleObjectParser
{
  public:
    using v2::SingleObjectParser::SingleObjectParser;
};

}  // namespace

/*!
 * Tests the dynamic interface version of the SingleObjectParser with an empty (comma-filled) object.
 * \trace           score::json::vajson::v1::SingleObjectParser
 */
TEST(UT__Parser__SingleObjectParser, Dynamic__EmptyObject)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({,,,,,,})"});
    ASSERT_TRUE(data.has_value());

    DynamicSingleObjectParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
}

/*!
 * Test that the dynamic interface version of the SingleObjectParser fails if an element is not parsed.
 * \trace           score::json::vajson::v1::SingleObjectParser
 */
TEST(UT__Parser__SingleObjectParser, Dynamic__Unexpected)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"unexpected":)"});
    ASSERT_TRUE(data.has_value());

    DynamicSingleObjectParser parser{data.value()};
    const auto result = parser.Parse();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kUserValidationFailed);
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
