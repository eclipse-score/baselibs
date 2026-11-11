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
/// @brief  Contains tests concerning the JsonData class.
#include "score/json/internal/parser/vajson/json_data.h"

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>
#include <utility>

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/json_error_domain.h"
#include "score/json/internal/parser/vajson/json_ops.h"
#include "score/json/internal/parser/vajson/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

constexpr std::size_t kNoSizeLimit{std::numeric_limits<std::size_t>::max()};

/// Test that JsonData can open a file.
TEST(UT__JsonOps__JsonData, ConstructFromFile)
{
    const std::string file{"/tmp/vajson_test.json"};
    std::FILE* const fp{std::fopen(file.c_str(), "w")};
    ASSERT_NE(fp, nullptr);
    ASSERT_EQ(std::fclose(fp), 0);

    const Result<JsonData> result{JsonData::FromFile(file, kNoSizeLimit)};
    ASSERT_TRUE(result.has_value());
}

/// Test that JsonData can open a StringView character buffer.
TEST(UT__JsonOps__JsonData, ConstructFromStringView)
{
    const StringView view{"doesntmatter"};

    const Result<JsonData> result{JsonData::FromBuffer(view)};

    ASSERT_TRUE(result.has_value());
}

/// Test that JsonData returns a JsonErrc if the file cannot be opened.
TEST(UT__JsonOps__JsonData, ConstructFromNonExistingFile)
{
    const Result<JsonData> result{JsonData::FromFile("doesnotexist", kNoSizeLimit)};

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kStreamFailure);
}

class JsonDataFileSizeTest : public ::testing::TestWithParam<std::size_t>
{
  protected:
    void SetUp() override
    {
        RecordProperty("PartiallyVerifies", "comp_req__json__max_file_size");
        RecordProperty("TestType", "requirements-based");
        RecordProperty("DerivationTechnique", "boundary-values");

        std::ofstream file{file_path_, std::ios::binary};
        file << std::string(GetParam(), ' ');
        file.close();
        ASSERT_FALSE(file.fail());
    }

    void TearDown() override
    {
        static_cast<void>(std::remove(file_path_.c_str()));
    }

    const std::string file_path_{::testing::TempDir() + "size_limit.json"};
};

TEST_P(JsonDataFileSizeTest, AcceptsFileAtSizeLimit)
{
    RecordProperty("Description", "Check that FromFile accepts a file whose size equals the limit.");

    const Result<JsonData> result{JsonData::FromFile(file_path_, GetParam())};

    EXPECT_TRUE(result.has_value());
}

TEST_P(JsonDataFileSizeTest, RejectsFileAboveSizeLimit)
{
    RecordProperty("Description", "Check that FromFile rejects a file one byte larger than the limit.");

    const Result<JsonData> result{JsonData::FromFile(file_path_, GetParam() - 1U)};

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), JsonErrc::kStreamFailure);
}

INSTANTIATE_TEST_SUITE_P(ChunkBoundaries, JsonDataFileSizeTest, ::testing::Values(1U, 65535U, 65536U, 65537U, 200000U));

TEST(UT__JsonOps__JsonData, AppliesSizeLimitToBytesReadBeyondReportedSize)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__max_file_size");
    RecordProperty("Description", "Check that FromFile limits the bytes read from a file that reports size 0.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");
#if defined(__linux__)
    // Procfs reports size 0 for this regular file, although it has content.
    const std::string file{"/proc/self/cmdline"};

    EXPECT_TRUE(JsonData::FromFile(file, kNoSizeLimit).has_value());
    const Result<JsonData> result{JsonData::FromFile(file, 1U)};

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), JsonErrc::kStreamFailure);
#else
    GTEST_SKIP() << "Requires Linux procfs";
#endif
}

/// Test that a UTF-8 BOM is skipped.
///
/// - Create a document that contains a UTF-8 BOM followed by a character.
/// - Assert that the first character read is the one following the BOM.
TEST(UT__JsonOps__JsonData, SkipsUtf8Bom)
{
    // The UTF-8 BOM.
    const std::string data{'\xEF', '\xBB', '\xBF', 'x'};

    Result<JsonData> result{JsonData::FromBuffer(std::string_view{data})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    internal::JsonOps ops{doc};
    ASSERT_EQ(ops.TryTake(), 'x');
}

/// Test that a document without a UTF-8 BOM is read from its beginning.
///
/// - Create a document that starts with a BOM other than UTF-8.
/// - Assert that the first character read is the first byte of the document.
TEST(UT__JsonOps__JsonData, KeepsUnknownBom)
{
    // The UTF-16 BE BOM (not UTF-8, should not be recognized).
    const std::string data{'\xFE', '\xFF'};

    Result<JsonData> result{JsonData::FromBuffer(std::string_view{data})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    internal::JsonOps ops{doc};
    ASSERT_EQ(ops.TryTake(), '\xFE');
}

/// Test that JsonData returns the underlying DepthCounter.
///
/// - Assert that initially the DepthCounter is empty.
TEST(UT__JsonOps__JsonData, GetState)
{
    Result<JsonData> result{JsonData::FromBuffer(std::string_view{""})};
    ASSERT_TRUE(result.has_value());

    JsonData doc{std::move(result.value())};
    internal::DepthCounter& state{doc.GetState()};
    ASSERT_FALSE(state.CheckEndOfFile().has_value());
    ASSERT_EQ(state.CheckEndOfFile().error(), JsonErrc::kInvalidJson);
}

/// Test that JsonData can store/retrieve the current string buffer.
///
/// - Assert that initially the string buffer is empty.
/// - Request the string buffer and add data to it.
/// - Assert that the string buffer contains the data.
/// - Assert that GetClearedStringBuffer empties the string buffer.
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

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
