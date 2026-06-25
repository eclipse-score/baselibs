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

#include "score/hash/code/core/typed_hash.h"

#include "score/hash/code/common/error.h"
#include "score/json/i_json_parser.h"

#include <gtest/gtest.h>

namespace score
{
namespace hash
{
namespace
{

class TypedHashFixture : public ::testing::Test
{
  public:
    TypedHash<HashAlgorithm::kSha256> unit_{{0x01}};
};

TEST_F(TypedHashFixture, CanCompareEqual)
{
    EXPECT_EQ(unit_, (TypedHash<HashAlgorithm::kSha256>{{0x01}}));
}

TEST_F(TypedHashFixture, DoesNotCompareEqualOnDifferentContent)
{
    EXPECT_NE(unit_, (TypedHash<HashAlgorithm::kSha256>{{0x02}}));
}

TEST_F(TypedHashFixture, GetBytesAsSpan)
{
    Hash::ByteVector i1 = {0x01};
    score::cpp::span<const std::uint8_t> expected_result(i1);
    auto result = unit_.GetBytes();
    EXPECT_TRUE(std::equal(result.begin(), result.end(), expected_result.begin(), expected_result.end()));
}

TEST(TypedHashTest, CanCreateFromValidString)
{
    const score::cpp::pmr::string sha1{"89fdde0b28373dc4f361cfb810b35342cc2c3232"};
    Hash::ByteVector expected_bytes = {0x89, 0xFD, 0xDE, 0x0B, 0x28, 0x37, 0x3D, 0xC4, 0xF3, 0x61,
                                       0xCF, 0xB8, 0x10, 0xB3, 0x53, 0x42, 0xCC, 0x2C, 0x32, 0x32};

    Result<TypedHash<HashAlgorithm::kSha1>> sha1_hash_result = TypedHash<HashAlgorithm::kSha1>::FromString(sha1);
    ASSERT_TRUE(sha1_hash_result.has_value());

    TypedHash<HashAlgorithm::kSha1> sha1_hash = sha1_hash_result.value();
    auto result = sha1_hash.GetBytes();
    EXPECT_TRUE(std::equal(result.begin(), result.end(), expected_bytes.begin(), expected_bytes.end()));

    score::cpp::pmr::string result_str = sha1_hash.ToString();
    EXPECT_EQ(result_str, sha1);

    Hash sha1_hash_nontyped{HashAlgorithm::kSha1, expected_bytes};
    EXPECT_EQ(sha1_hash_nontyped, sha1_hash.ToHash());
}

TEST(TypedHashTest, ToStringEmptyValueTest)
{
    TypedHash<HashAlgorithm::kNone> unit{{0x01}};
    score::cpp::pmr::string result_str = unit.ToString();
    ASSERT_TRUE(result_str.empty());
}

TEST(TypedHashTest, CanSerializeToJsonAny)
{
    const TypedHash<HashAlgorithm::kSha256> hash{{0xFF, 0x18, 0x25, 0x62, 0x92, 0xF5, 0xF2, 0xBA, 0x52, 0x61, 0xB5,
                                                  0x59, 0x40, 0xCD, 0xF1, 0x12, 0x5C, 0xE3, 0x0E, 0x97, 0xC2, 0x2A,
                                                  0xCE, 0x6A, 0xFA, 0x54, 0xB7, 0xAF, 0x38, 0x72, 0xC3, 0x51}};
    const auto json{hash.ToAny()};
    const auto json_string{json.As<std::string>()};
    ASSERT_TRUE(json_string.has_value());
    ASSERT_EQ(json_string->get(), "ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af3872c351");

    const TypedHash<HashAlgorithm::kNone> hash1{{}};
    const auto json1{hash1.ToAny()};
    const auto json_string1{json1.As<std::string>()};
    ASSERT_TRUE(json_string1.has_value());
    ASSERT_EQ(json_string1->get(), "");
}

TEST(TypedHashTest, CanDeserializeFromJsonAny)
{
    const TypedHash<HashAlgorithm::kSha256> expected_hash{
        {0xFF, 0x18, 0x25, 0x62, 0x92, 0xF5, 0xF2, 0xBA, 0x52, 0x61, 0xB5, 0x59, 0x40, 0xCD, 0xF1, 0x12,
         0x5C, 0xE3, 0x0E, 0x97, 0xC2, 0x2A, 0xCE, 0x6A, 0xFA, 0x54, 0xB7, 0xAF, 0x38, 0x72, 0xC3, 0x51}};
    const auto hash_result{TypedHash<HashAlgorithm::kSha256>::FromAny(
        json::Any{std::string{"ff18256292f5f2ba5261b55940cdf1125ce30e97c22ace6afa54b7af3872c351"}})};
    ASSERT_TRUE(hash_result.has_value());
    ASSERT_EQ(hash_result.value(), expected_hash);

    const auto hash_result1{TypedHash<HashAlgorithm::kNone>::FromAny(json::Any{std::string{""}})};
    ASSERT_FALSE(hash_result1.has_value());
    ASSERT_EQ(hash_result1.error(), score::hash::ErrorCode::kInvalidParameters);
    ASSERT_EQ(hash_result1.error().UserMessage(), "Unsupported algorithm");

    const auto hash_result2{TypedHash<HashAlgorithm::kSha256>::FromAny(json::Any{5.4})};
    ASSERT_FALSE(hash_result2.has_value());
    ASSERT_EQ(hash_result2.error(), score::json::Error::kParsingError);
    ASSERT_EQ(hash_result2.error().UserMessage(), "Expected hex string");
}

}  // namespace
}  // namespace hash
}  // namespace score
