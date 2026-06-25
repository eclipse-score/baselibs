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
 *        \brief  Contains tests concerning the JsonData class.
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdio>
#include <string>
#include <utility>

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/span.hpp"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

/*!
 * Test that JsonData can open a file.
 */
TEST(UT__JsonOps__JsonData, ConstructFromFile)
{
    std::string const file{"/tmp/vajson_test.json"};
    std::FILE* const fp{std::fopen(file.c_str(), "w")};
    ASSERT_NE(fp, nullptr);
    ASSERT_EQ(std::fclose(fp), 0);

    const Result<JsonData> result{JsonData::FromFile(file)};
    ASSERT_TRUE(result.has_value());
}

/*!
 * Test that JsonData can open a StringView character buffer.
 */
TEST(UT__JsonOps__JsonData, ConstructFromStringView)
{
    const StringView view{"doesntmatter"};

    const Result<JsonData> result{JsonData::FromBuffer(view)};

    ASSERT_TRUE(result.has_value());
}

/*!
 * Test that JsonData can open a CStringView character buffer.
 *
 * \trace           score::json::vajson::JsonData::FromBuffer
 */
TEST(UT__JsonOps__JsonData, ConstructFromCStringView)
{
    const CStringView view{"doesntmatter"};

    const Result<JsonData> result{JsonData::FromBuffer(view)};

    ASSERT_TRUE(result.has_value());
}

/*!
 * Test that JsonData can open a span of characters.
 */
TEST(UT__JsonOps__JsonData, ConstructFromSpan)
{
    std::string const data{"doesntmatter"};
    const score::cpp::span<const char> view{data.data(), data.size()};

    const Result<JsonData> result{JsonData::FromBuffer(view)};

    ASSERT_TRUE(result.has_value());
}

/*!
 * Test that JsonData returns a JsonErrc if the file cannot be opened.
 */
TEST(UT__JsonOps__JsonData, ConstructFromNonExistingFile)
{
    const Result<JsonData> result{JsonData::FromFile("doesnotexist")};

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kStreamFailure);
}

/*!
 * Test that a document with an UTF-8 BOM can be parsed.
 *
 * - Create a document that contains a UTF-8 BOM.
 * - Parse the document.
 * - Assert that the UTF-8 BOM is recognized.
 */
TEST(UT__JsonOps__JsonData, ReadsUtf8Bom)
{
    // The UTF-8 BOM.
    std::string const data{'\xEF', '\xBB', '\xBF'};

    Result<JsonData> result{JsonData::FromBuffer(std::string_view{data})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    ASSERT_EQ(doc.GetEncoding(), EncodingType::kUtf8);
}

/*!
 * Test that the parser correctly handles a document without a UTF-8 BOM.
 *
 * - Create a document without a BOM.
 * - Assert that encoding is kNone.
 */
TEST(UT__JsonOps__JsonData, FailsOnUnknownBom)
{
    // The UTF-16 BE BOM (not UTF-8, should not be recognized).
    std::string const data{'\xFE', '\xFF'};

    Result<JsonData> result{JsonData::FromBuffer(std::string_view{data})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    ASSERT_EQ(doc.GetEncoding(), EncodingType::kNone);
}

/*!
 * Test that JsonData returns the underlying DepthCounter.
 *
 * - Assert that initially the DepthCounter is empty.
 */
TEST(UT__JsonOps__JsonData, GetState)
{
    Result<JsonData> result{JsonData::FromBuffer(std::string_view{""})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    internal::DepthCounter& state{doc.GetState()};
    ASSERT_FALSE(state.CheckEndOfFile().has_value());
    ASSERT_EQ(state.CheckEndOfFile().error(), JsonErrc::kInvalidJson);
}

/*!
 * Test that JsonData can store/retrieve the current key.
 *
 * - Assert that initially the current key is empty.
 * - Store a new key.
 * - Assert that the new key is returned.
 */
TEST(UT__JsonOps__JsonData, CurrentKey)
{
    Result<JsonData> result{JsonData::FromBuffer(std::string_view{""})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    const CStringView curr{doc.GetCurrentKey()};
    ASSERT_TRUE(curr.empty());

    const StringView new_key{"key"};
    doc.StoreCurrentKey(new_key);
    ASSERT_EQ(new_key, doc.GetCurrentKey());
}

/*!
 * Test that JsonData can store/retrieve the current string buffer.
 *
 * - Assert that initially the string buffer is empty.
 * - Request the string buffer and add data to it.
 * - Assert that the string buffer contains the data.
 * - Assert that GetClearedStringBuffer empties the string buffer.
 */
TEST(UT__JsonOps__JsonData, CurrentStringBuffer)
{
    Result<JsonData> result{JsonData::FromBuffer(std::string_view{""})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    const CStringView curr{doc.GetCurrentString()};
    ASSERT_TRUE(curr.empty());

    const StringView new_string{"string"};
    doc.GetStringBuffer().append(new_string);
    ASSERT_EQ(new_string, doc.GetCurrentString());

    ASSERT_TRUE(doc.GetClearedStringBuffer().empty());
}

/*!
 * Test that JsonData can take a snapshot and restore it.
 *
 * - Assert that a snapshot can be created.
 * - Assert that the snapshot can be restored.
 */
TEST(UT__JsonOps__JsonData, SnapAndRestore)
{
    Result<JsonData> result{JsonData::FromBuffer(std::string_view{""})};
    ASSERT_TRUE(result.has_value());
    JsonData doc{std::move(result.value())};

    ASSERT_TRUE(doc.Snap().has_value());
    ASSERT_TRUE(doc.Restore().has_value());
}

/*!
 * Test that JsonData restore fails if no backup is available.
 */
TEST(UT__JsonOps__JsonData, RestoreFailsWithoutBackup)
{
    Result<JsonData> result{JsonData::FromBuffer(std::string_view{""})};
    ASSERT_TRUE(result.has_value());
    JsonData doc{std::move(result.value())};

    const auto restore_result = doc.Restore();
    ASSERT_FALSE(restore_result.has_value());
    ASSERT_EQ(restore_result.error(), JsonErrc::kStreamFailure);
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
