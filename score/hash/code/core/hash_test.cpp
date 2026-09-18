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

#include "score/hash/code/core/hash.h"

#include <gtest/gtest.h>

namespace score
{
namespace hash
{
namespace
{

class HashFixture : public ::testing::Test
{
  public:
    Hash unit_{HashAlgorithm::kSha256, {0x01}};
};

TEST_F(HashFixture, CanCompareEqual)
{
    EXPECT_EQ(unit_, (Hash{HashAlgorithm::kSha256, {0x01}}));
}

TEST_F(HashFixture, DoesNotCoompareEqualOnDifferentAlgorithm)
{
    EXPECT_FALSE(unit_ == (Hash{HashAlgorithm::kCrc32, {0x01}}));
}

TEST_F(HashFixture, DoesNotCoompareEqualOnDifferentContent)
{
    EXPECT_TRUE(unit_ != (Hash{HashAlgorithm::kSha256, {0x02}}));
}

TEST_F(HashFixture, GetBytesAsSpan)
{
    Hash::ByteVector i1 = {0x01};
    score::cpp::span<const std::uint8_t> expected_result(i1);
    auto result = unit_.GetBytes();
    EXPECT_TRUE(std::equal(result.begin(), result.end(), expected_result.begin(), expected_result.end()));
}

TEST(HashTest, CanCreateFromValidString)
{
    const score::cpp::pmr::string crc32{"39a34f41"};
    Hash::ByteVector expected_bytes{0x39, 0xA3, 0x4F, 0x41};

    Result<Hash> crc32_hash_result = Hash::FromString(HashAlgorithm::kCrc32, crc32);
    ASSERT_TRUE(crc32_hash_result.has_value());

    Hash crc32_hash = crc32_hash_result.value();
    auto result = crc32_hash.GetBytes();
    EXPECT_TRUE(std::equal(result.begin(), result.end(), expected_bytes.begin(), expected_bytes.end()));

    score::cpp::pmr::string result_str = crc32_hash.ToString();
    EXPECT_EQ(result_str, crc32);
}

TEST(HashTest, ToStringEmptyValueTest)
{
    Hash unit{HashAlgorithm::kNone, {0x01}};
    score::cpp::pmr::string result_str = unit.ToString();
    ASSERT_TRUE(result_str.empty());
}

TEST(HashTest, CanCreateFromValidStringCompleteSetOfAlgorithms)
{
    using StringPtr = const char* const;
    using TestData = const std::tuple<HashAlgorithm, StringPtr, Hash::ByteVector>;

    static TestData complete_suite[]{
        std::make_tuple(HashAlgorithm::kCrc32, "39a34f41", Hash::ByteVector{0x39, 0xa3, 0x4f, 0x41}),
        std::make_tuple(HashAlgorithm::kSha256,
                        "ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af3872c351",
                        Hash::ByteVector{0xFF, 0x18, 0x25, 0x62, 0x92, 0xF5, 0xF2, 0xBA, 0x52, 0x61, 0xB5,
                                         0x59, 0x40, 0xCD, 0xF1, 0x12, 0x5C, 0xE3, 0x0E, 0x97, 0xC2, 0x2A,
                                         0xCE, 0x6A, 0xFA, 0x54, 0xB7, 0xAF, 0x38, 0x72, 0xC3, 0x51}),
    };

    for (auto test_tuple : complete_suite)
    {
        auto algorithm = std::get<0>(test_tuple);
        auto str_repr = std::get<1>(test_tuple);
        auto bin_repr = std::get<2>(test_tuple);

        std::cout << str_repr << std::endl;
        Result<Hash> result = Hash::FromString(algorithm, score::cpp::pmr::string(str_repr));
        ASSERT_TRUE(result.has_value());

        Hash hash = result.value();
        auto actual_bytes = hash.GetBytes();
        auto algorithm_result = hash.GetAlgorithm();
        EXPECT_TRUE(std::equal(actual_bytes.begin(), actual_bytes.end(), bin_repr.begin(), bin_repr.end()));
        EXPECT_EQ(algorithm_result, algorithm);

        auto actual_str = hash.ToString();
        EXPECT_EQ(actual_str, str_repr);
    }
}

TEST(HashTest, ValidateAlgorithm)
{
    const score::cpp::pmr::string sha256{"ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af3872c351"};

    Result<Hash> hash_result = Hash::FromString(HashAlgorithm::kLast, sha256);
    ASSERT_FALSE(hash_result.has_value());
}

TEST(HashTest, ValidateSize)
{
    const score::cpp::pmr::string short_sha256{"ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af3872c35"};
    const score::cpp::pmr::string long_sha256{"ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af3872c3510"};

    Result<Hash> short_hash_result = Hash::FromString(HashAlgorithm::kSha256, short_sha256);
    ASSERT_FALSE(short_hash_result.has_value());

    Result<Hash> long_hash_result = Hash::FromString(HashAlgorithm::kSha256, long_sha256);
    ASSERT_FALSE(long_hash_result.has_value());
}

TEST(HashTest, ValidateContents)
{
    const score::cpp::pmr::string invalid_sha256{"ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af387BOGUS"};
    ASSERT_EQ(invalid_sha256.size(), 64U);

    Result<Hash> hash_result = Hash::FromString(HashAlgorithm::kSha256, invalid_sha256);
    ASSERT_FALSE(hash_result.has_value());
}

}  // namespace
}  // namespace hash
}  // namespace score
