#include "score/utils/base64.h"
#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <vector>

namespace score::utils
{
namespace test
{

TEST(Base64Test, EncodeEmptyString)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::EncodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input string is correctly encoded.");
    std::vector<std::uint8_t> input = {};
    std::string expected_output = "";
    EXPECT_EQ(EncodeBase64(input), expected_output);
}

TEST(Base64Test, EncodeSingleByte)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::EncodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input string is correctly encoded.");
    std::vector<std::uint8_t> input = {'A'};
    std::string expected_output = "QQ==";
    EXPECT_EQ(EncodeBase64(input), expected_output);
}

TEST(Base64Test, EncodeTwoBytes)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::EncodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input string is correctly encoded.");
    std::vector<std::uint8_t> input = {'A', 'B'};
    std::string expected_output = "QUI=";
    EXPECT_EQ(EncodeBase64(input), expected_output);
}

TEST(Base64Test, EncodeMultipleBytes)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::EncodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input string is correctly encoded.");
    std::vector<std::uint8_t> input = {'S', 'y', 's', 'f', 'c', 'n', 'U', 't', 'i', 'l', 's'};
    std::string expected_output = "U3lzZmNuVXRpbHM=";
    EXPECT_EQ(EncodeBase64(input), expected_output);
}

TEST(Base64Test, DecodeEmptyString)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::DecodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input is correctly decoded.");
    std::string input = "";
    std::vector<std::uint8_t> expected_output = {};
    EXPECT_EQ(DecodeBase64(input), expected_output);
}

TEST(Base64Test, DecodeSingleByte)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::DecodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input is correctly decoded.");
    std::string input = "QQ==";
    std::vector<std::uint8_t> expected_output = {'A'};
    EXPECT_EQ(DecodeBase64(input), expected_output);
}

TEST(Base64Test, DecodeTwoBytes)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::DecodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input is correctly decoded.");
    std::string input = "QUI=";
    std::vector<std::uint8_t> expected_output = {'A', 'B'};
    EXPECT_EQ(DecodeBase64(input), expected_output);
}

TEST(Base64Test, DecodeMultipleBytes)
{
    ::testing::Test::RecordProperty("TestType", "control-flow-analysis");
    ::testing::Test::RecordProperty("Verifies", "::score::utils::DecodeBase64()");
    ::testing::Test::RecordProperty("Description", "This test ensures that a input is correctly decoded.");
    std::vector<std::uint8_t> input(1000, 'A');
    std::string encoded_input = EncodeBase64(input);
    auto result = DecodeBase64(encoded_input);
    EXPECT_EQ(result.size(), 1000);
    EXPECT_EQ(result, input);
}

}  // namespace test
}  // namespace score::utils
