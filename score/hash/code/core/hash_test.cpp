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
    EXPECT_FALSE(unit_ == (Hash{HashAlgorithm::kSha1, {0x01}}));
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
    const score::cpp::pmr::string sha1{"89fdde0b28373dc4f361cfb810b35342cc2c3232"};
    Hash::ByteVector expected_bytes = {0x89, 0xFD, 0xDE, 0x0B, 0x28, 0x37, 0x3D, 0xC4, 0xF3, 0x61,
                                       0xCF, 0xB8, 0x10, 0xB3, 0x53, 0x42, 0xCC, 0x2C, 0x32, 0x32};

    Result<Hash> sha1_hash_result = Hash::FromString(HashAlgorithm::kSha1, sha1);
    ASSERT_TRUE(sha1_hash_result.has_value());

    Hash sha1_hash = sha1_hash_result.value();
    auto result = sha1_hash.GetBytes();
    EXPECT_TRUE(std::equal(result.begin(), result.end(), expected_bytes.begin(), expected_bytes.end()));

    score::cpp::pmr::string result_str = sha1_hash.ToString();
    EXPECT_EQ(result_str, sha1);
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
        std::make_tuple(HashAlgorithm::kSha1,
                        "89fdde0b28373dc4f361cfb810b35342cc2c3232",
                        Hash::ByteVector{0x89, 0xFD, 0xDE, 0x0B, 0x28, 0x37, 0x3D, 0xC4, 0xF3, 0x61,
                                         0xCF, 0xB8, 0x10, 0xB3, 0x53, 0x42, 0xCC, 0x2C, 0x32, 0x32}),
        std::make_tuple(HashAlgorithm::kSha256,
                        "ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af3872c351",
                        Hash::ByteVector{0xFF, 0x18, 0x25, 0x62, 0x92, 0xF5, 0xF2, 0xBA, 0x52, 0x61, 0xB5,
                                         0x59, 0x40, 0xCD, 0xF1, 0x12, 0x5C, 0xE3, 0x0E, 0x97, 0xC2, 0x2A,
                                         0xCE, 0x6A, 0xFA, 0x54, 0xB7, 0xAF, 0x38, 0x72, 0xC3, 0x51}),
        std::make_tuple(
            HashAlgorithm::kSha384,
            "40f73e53971550af0189f05b090c96f8c0280e6cc15f6932ca0c143e301c98a6873f4394a24dc62d4df619d1890b5879",
            Hash::ByteVector{0x40, 0xF7, 0x3E, 0x53, 0x97, 0x15, 0x50, 0xAF, 0x01, 0x89, 0xF0, 0x5B,
                             0x09, 0x0C, 0x96, 0xF8, 0xC0, 0x28, 0x0E, 0x6C, 0xC1, 0x5F, 0x69, 0x32,
                             0xCA, 0x0C, 0x14, 0x3E, 0x30, 0x1C, 0x98, 0xA6, 0x87, 0x3F, 0x43, 0x94,
                             0xA2, 0x4D, 0xC6, 0x2D, 0x4D, 0xF6, 0x19, 0xD1, 0x89, 0x0B, 0x58, 0x79}),
        std::make_tuple(
            HashAlgorithm::kSha512,
            "53f09563ec1e30bd40808202e14bff32968015397026532c0f9f7eb81fc2f099e7ba505eb1e8e69392a7afae6e8d95f286652b6dfa"
            "2ab183e42ebd8dc54dd2aa",
            Hash::ByteVector{0x53, 0xF0, 0x95, 0x63, 0xEC, 0x1E, 0x30, 0xBD, 0x40, 0x80, 0x82, 0x02, 0xE1,
                             0x4B, 0xFF, 0x32, 0x96, 0x80, 0x15, 0x39, 0x70, 0x26, 0x53, 0x2C, 0x0F, 0x9F,
                             0x7E, 0xB8, 0x1F, 0xC2, 0xF0, 0x99, 0xE7, 0xBA, 0x50, 0x5E, 0xB1, 0xE8, 0xE6,
                             0x93, 0x92, 0xA7, 0xAF, 0xAE, 0x6E, 0x8D, 0x95, 0xF2, 0x86, 0x65, 0x2B, 0x6D,
                             0xFA, 0x2A, 0xB1, 0x83, 0xE4, 0x2E, 0xBD, 0x8D, 0xC5, 0x4D, 0xD2, 0xAA}),
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
    const score::cpp::pmr::string sha1 = "89fdde0b28373dc4f361cfb810b35342cc2c3232";

    Result<Hash> sha1_hash_result = Hash::FromString(HashAlgorithm::kLast, sha1);
    ASSERT_FALSE(sha1_hash_result.has_value());
}

TEST(HashTest, ValidateSize)
{
    const score::cpp::pmr::string short_sha1 = "89fdde0b28373dc4f361cfb810b35342cc2c323";
    const score::cpp::pmr::string long_sha1 = "89fdde0b28373dc4f361cfb810b35342cc2c3232A";

    Result<Hash> sha1_hash_result = Hash::FromString(HashAlgorithm::kSha1, short_sha1);
    ASSERT_FALSE(sha1_hash_result.has_value());

    Result<Hash> sha1_hash_result2 = Hash::FromString(HashAlgorithm::kSha1, long_sha1);
    ASSERT_FALSE(sha1_hash_result2.has_value());
}

TEST(HashTest, ValidateContents)
{
    const score::cpp::pmr::string invalid_sha1 = "89fdde0b28373dc4f361cfb810b35342cc2BOGUS";

    Result<Hash> sha1_hash_result = Hash::FromString(HashAlgorithm::kSha1, invalid_sha1);
    ASSERT_FALSE(sha1_hash_result.has_value());
}

}  // namespace
}  // namespace hash
}  // namespace score
