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
/// @brief Runs the nst/JSONTestSuite parsing corpus through JsonParser.

#include "score/json/json_parser.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace score
{
namespace json
{
namespace
{

/// The corpus file names start with y_ (must be accepted), n_ (must be rejected) or i_ (implementation defined).
constexpr std::string_view kAcceptPrefix{"y_"};
constexpr std::string_view kRejectPrefix{"n_"};

/// Valid per RFC 8259, but comp_req__json__duplicate_member_names requires rejection.
constexpr std::array<std::string_view, 2U> kDuplicateMemberNameFiles{
    "y_object_duplicated_key.json",
    "y_object_duplicated_key_and_value.json",
};

/// Files for which the parser currently returns the opposite of the expected outcome.
/// The test asserts the opposite outcome, so fixing the parser requires removing the file from this list.
constexpr std::array<std::string_view, 47U> kKnownDeviations{
    // Unicode escapes (\u) are rejected, see StructureParser.
    "y_object_escaped_null_in_key.json",
    "y_object_string_unicode.json",
    "y_string_1_2_3_bytes_UTF-8_sequences.json",
    "y_string_accepted_surrogate_pair.json",
    "y_string_accepted_surrogate_pairs.json",
    "y_string_escaped_control_character.json",
    "y_string_escaped_noncharacter.json",
    "y_string_last_surrogates_1_and_2.json",
    "y_string_nbsp_uescaped.json",
    "y_string_null_escape.json",
    "y_string_one-byte-utf-8.json",
    "y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
    "y_string_three-byte-utf-8.json",
    "y_string_two-byte-utf-8.json",
    "y_string_uEscape.json",
    "y_string_uescaped_newline.json",
    "y_string_unicode.json",
    "y_string_unicodeEscapedBackslash.json",
    "y_string_unicode_U+10FFFE_nonchar.json",
    "y_string_unicode_U+1FFFE_nonchar.json",
    "y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
    "y_string_unicode_U+2064_invisible_plus.json",
    "y_string_unicode_U+FDD0_nonchar.json",
    "y_string_unicode_U+FFFE_nonchar.json",
    "y_string_unicode_escaped_double_quote.json",
    // Missing values and trailing commas are accepted.
    "n_array_comma_and_number.json",
    "n_array_double_comma.json",
    "n_array_double_extra_comma.json",
    "n_array_extra_comma.json",
    "n_array_just_comma.json",
    "n_array_missing_value.json",
    "n_array_number_and_comma.json",
    "n_array_number_and_several_commas.json",
    "n_object_lone_continuation_byte_in_key_and_trailing_comma.json",
    "n_object_several_trailing_commas.json",
    "n_object_trailing_comma.json",
    "n_object_two_commas_in_a_row.json",
    // Numbers outside the RFC 8259 grammar are accepted.
    "n_number_0.e1.json",
    "n_number_2.e+3.json",
    "n_number_2.e-3.json",
    "n_number_2.e3.json",
    "n_number_hex_1_digit.json",
    "n_number_hex_2_digits.json",
    "n_number_neg_real_without_int_part.json",
    // Unescaped control characters in strings are accepted.
    "n_string_unescaped_ctrl_char.json",
    "n_string_unescaped_newline.json",
    "n_string_unescaped_tab.json",
};

/// Deeply nested files that currently crash the test binary with a segmentation fault, so they are skipped.
/// Remove a file from this list once the parser handles it.
constexpr std::array<std::string_view, 2U> kCrashingFiles{
    "n_structure_100000_opening_arrays.json",
    "n_structure_open_array_object.json",
};

template <typename Container>
bool Contains(const Container& container, const std::string_view name)
{
    return std::find(container.begin(), container.end(), name) != container.end();
}

/// Reads the space separated runfiles paths that the BUILD file passes in the given environment variable.
std::vector<std::string> CorpusPaths()
{
    std::vector<std::string> paths{};
    const char* const source_dir{std::getenv("TEST_SRCDIR")};
    const char* const files{std::getenv("JSON_TEST_SUITE_FILES")};
    if ((source_dir == nullptr) || (files == nullptr))
    {
        return paths;
    }
    std::istringstream stream{files};
    std::string runfiles_path{};
    while (stream >> runfiles_path)
    {
        // Bazel shell-quotes paths containing special characters such as '#'.
        if ((runfiles_path.size() >= 2U) && (runfiles_path.front() == '\'') && (runfiles_path.back() == '\''))
        {
            runfiles_path = runfiles_path.substr(1U, runfiles_path.size() - 2U);
        }
        paths.push_back(std::string{source_dir} + "/" + runfiles_path);
    }
    return paths;
}

std::string FileName(const std::string& path)
{
    return path.substr(path.find_last_of('/') + 1U);
}

std::string ReadFile(const std::string& path)
{
    std::ifstream file{path, std::ios::binary};
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

bool StartsWith(const std::string& text, const std::string_view prefix)
{
    return text.compare(0U, prefix.size(), prefix) == 0;
}

class JsonTestSuiteTest : public ::testing::TestWithParam<std::string>
{
};

TEST(JsonTestSuiteCorpusTest, CorpusIsAvailable)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that the JSONTestSuite corpus is provided to the test and is not empty.");

    const auto paths{CorpusPaths()};
    ASSERT_FALSE(paths.empty());
    for (const auto& path : paths)
    {
        EXPECT_TRUE(std::ifstream{path}.good()) << path;
    }
}

TEST_P(JsonTestSuiteTest, FromBufferMatchesExpectedOutcome)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description",
                   "Check that the parser accepts JSONTestSuite y_ files, rejects n_ files and handles i_ files "
                   "without crashing.");

    const std::string name{FileName(GetParam())};
    if (Contains(kCrashingFiles, name))
    {
        GTEST_SKIP() << name << " crashes the parser, see kCrashingFiles";
    }
    const std::string content{ReadFile(GetParam())};

    const auto result{JsonParser{}.FromBuffer(content)};

    if (StartsWith(name, kAcceptPrefix) || StartsWith(name, kRejectPrefix))
    {
        const bool expect_accepted{StartsWith(name, kAcceptPrefix) && !Contains(kDuplicateMemberNameFiles, name)};
        const bool is_known_deviation{Contains(kKnownDeviations, name)};
        EXPECT_EQ(result.has_value(), expect_accepted != is_known_deviation)
            << name << (expect_accepted ? " is valid JSON" : " is invalid JSON") << " and was "
            << (result.has_value() ? "accepted" : "rejected")
            << (is_known_deviation ? ", but it is listed in kKnownDeviations; remove it from the list"
                                   : ", which is unexpected; fix the parser or add it to kKnownDeviations");
    }
}

std::string TestName(const ::testing::TestParamInfo<std::string>& info)
{
    constexpr std::string_view kHexDigits{"0123456789abcdef"};
    const std::string file_name{FileName(info.param)};
    std::string name{};
    for (const char c : file_name.substr(0U, file_name.size() - std::string_view{".json"}.size()))
    {
        const auto byte{static_cast<unsigned char>(c)};
        if (std::isalnum(byte) || (c == '_'))
        {
            name.push_back(c);
        }
        else
        {
            // Escape instead of replacing so that, e.g., 1.0e+ and 1.0e- stay distinct test names.
            name.push_back('_');
            name.push_back(kHexDigits[byte >> 4U]);
            name.push_back(kHexDigits[byte & 0xFU]);
        }
    }
    return name;
}

INSTANTIATE_TEST_SUITE_P(Corpus, JsonTestSuiteTest, ::testing::ValuesIn(CorpusPaths()), TestName);

}  // namespace
}  // namespace json
}  // namespace score
