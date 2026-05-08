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

#include "score/hash/code/openssl/openssl_hash_calculator.h"
#include "score/hash/code/common/error.h"
#include "score/hash/code/common/gtest_printers.h"
#include "score/hash/code/openssl/openssl_wrapper/openssl_lib_impl.h"
#include "score/hash/code/openssl/openssl_wrapper/openssl_lib_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <vector>

namespace score
{
namespace hash
{
namespace
{

using namespace ::testing;
using openssl::StructDigest;
using openssl::StructDigestCtx;
using ::testing::_;
using ::testing::InSequence;
using testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

class HashCalculatorTest : public ::testing::Test
{
  public:
    void SetUp() override;
    void TearDown() override;
    void ExpectSha256Call();

  protected:
    score::hash::openssl::OpensslLibImpl openssl_lib_;
    NiceMock<score::hash::openssl::OpensslLibMock> openssl_lib_mock_;
    const StructDigest* digest_type_sha256;
    StructDigestCtx* digest_context_sha256;
    std::int32_t hash_digest_sha256;
};

void HashCalculatorTest::SetUp()
{
    digest_type_sha256 = openssl_lib_.DigestAlgoSha256();
    digest_context_sha256 = openssl_lib_.CreateDigestCtx();
    hash_digest_sha256 = openssl_lib_.InitDigestCtx(digest_context_sha256, digest_type_sha256, nullptr);
}

void HashCalculatorTest::TearDown()
{
    openssl_lib_.ResetDigestCtx(digest_context_sha256);
    digest_context_sha256 = nullptr;
}

void HashCalculatorTest::ExpectSha256Call()
{
    ON_CALL(openssl_lib_mock_, DigestAlgoSha256()).WillByDefault(Return(digest_type_sha256));
    ON_CALL(openssl_lib_mock_, CreateDigestCtx()).WillByDefault(Return(digest_context_sha256));
    ON_CALL(openssl_lib_mock_, InitDigestCtx(_, _, _)).WillByDefault(Return(hash_digest_sha256));
    EXPECT_CALL(openssl_lib_mock_, ResetDigestCtx(_)).Times(1);
}

TEST_F(HashCalculatorTest, HashAlgorithmsSimpleSha1Test)
{
    const StructDigest* digest_type_sha1 = openssl_lib_.DigestAlgoSha1();
    StructDigestCtx* digest_context_sha1 = openssl_lib_.CreateDigestCtx();
    std::int32_t hash_digest_sha1 = openssl_lib_.InitDigestCtx(digest_context_sha1, digest_type_sha1, nullptr);

    EXPECT_CALL(openssl_lib_mock_, DigestAlgoSha1()).WillOnce(Return(digest_type_sha1));
    EXPECT_CALL(openssl_lib_mock_, CreateDigestCtx()).WillOnce(Return(digest_context_sha1));
    EXPECT_CALL(openssl_lib_mock_, InitDigestCtx(_, _, _)).WillOnce(Return(hash_digest_sha1));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha1, openssl_lib_mock_);
    openssl_lib_.ResetDigestCtx(digest_context_sha1);
    digest_context_sha1 = nullptr;

    ASSERT_TRUE(digest.has_value());
}

TEST_F(HashCalculatorTest, HashAlgorithmsSimpleSha256Test)
{
    ExpectSha256Call();

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);

    ASSERT_TRUE(digest.has_value());
}

TEST_F(HashCalculatorTest, HashAlgorithmsSimpleSha384Test)
{
    const StructDigest* digest_type_sha384 = openssl_lib_.DigestAlgoSha384();
    StructDigestCtx* digest_context_sha384 = openssl_lib_.CreateDigestCtx();
    std::int32_t hash_digest_sha384 = openssl_lib_.InitDigestCtx(digest_context_sha384, digest_type_sha384, nullptr);

    ON_CALL(openssl_lib_mock_, DigestAlgoSha384()).WillByDefault(Return(digest_type_sha384));
    ON_CALL(openssl_lib_mock_, CreateDigestCtx()).WillByDefault(Return(digest_context_sha384));
    ON_CALL(openssl_lib_mock_, InitDigestCtx(_, _, _)).WillByDefault(Return(hash_digest_sha384));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha384, openssl_lib_mock_);
    openssl_lib_.ResetDigestCtx(digest_context_sha384);
    digest_context_sha384 = nullptr;

    ASSERT_TRUE(digest.has_value());
}

TEST_F(HashCalculatorTest, HashAlgorithmsSimpleSha512Test)
{
    const StructDigest* digest_type_sha512 = openssl_lib_.DigestAlgoSha512();
    StructDigestCtx* digest_context_sha512 = openssl_lib_.CreateDigestCtx();
    std::int32_t hash_digest_sha512 = openssl_lib_.InitDigestCtx(digest_context_sha512, digest_type_sha512, nullptr);

    ON_CALL(openssl_lib_mock_, DigestAlgoSha512()).WillByDefault(Return(digest_type_sha512));
    ON_CALL(openssl_lib_mock_, CreateDigestCtx()).WillByDefault(Return(digest_context_sha512));
    ON_CALL(openssl_lib_mock_, InitDigestCtx(_, _, _)).WillByDefault(Return(hash_digest_sha512));
    EXPECT_CALL(openssl_lib_mock_, UpdateDigestCtx(_, _, _)).WillOnce(Return(1));

    std::vector<std::uint8_t> test_input{1, 2, 3, 4};
    score::cpp::span<const std::uint8_t> data(test_input.data(), test_input.size());

    auto digest_1 = OpensslHashCalculator::Create(HashAlgorithm::kSha512, openssl_lib_mock_).value();
    auto digest_2 = OpensslHashCalculator::Create(HashAlgorithm::kSha512, openssl_lib_mock_).value();
    // Move assignement operator used for code coverage
    digest_2 = std::move(digest_1);
    auto update_result = digest_2.Update(data);

    openssl_lib_.ResetDigestCtx(digest_context_sha512);
    digest_context_sha512 = nullptr;

    ASSERT_TRUE(update_result.has_value());
}

TEST_F(HashCalculatorTest, NullDigestTypeCreateAlgoTest)
{
    EXPECT_CALL(openssl_lib_mock_, DigestAlgoSha256()).WillOnce(Return(nullptr));
    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);

    EXPECT_EQ(digest.error(), ErrorCode::kCouldNotCreateDigest);
}

TEST_F(HashCalculatorTest, InvalidDigestCtxCreateAlgoTest)
{
    ON_CALL(openssl_lib_mock_, DigestAlgoSha256()).WillByDefault(Return(digest_type_sha256));
    ON_CALL(openssl_lib_mock_, CreateDigestCtx()).WillByDefault(Return(nullptr));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);

    EXPECT_EQ(digest.error(), ErrorCode::kCouldNotCreateDigest);
}

TEST_F(HashCalculatorTest, InvalidDigestInitCreateAlgoTest)
{
    EXPECT_CALL(openssl_lib_mock_, DigestAlgoSha256()).WillOnce(Return(digest_type_sha256));
    EXPECT_CALL(openssl_lib_mock_, CreateDigestCtx()).WillOnce(Return(digest_context_sha256));
    EXPECT_CALL(openssl_lib_mock_, InitDigestCtx(_, _, _)).WillOnce(Return(0));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);

    EXPECT_EQ(digest.error(), ErrorCode::kCouldNotCreateDigest);
}

TEST_F(HashCalculatorTest, InvalidCreateAlgorthimTest)
{
    auto hash_calculator = OpensslHashCalculator::Create(HashAlgorithm::kNone, openssl_lib_mock_);
    ASSERT_FALSE(hash_calculator.has_value());
    EXPECT_EQ(hash_calculator.error(), ErrorCode::kCouldNotCreateDigest);

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kLast, openssl_lib_mock_);
    ASSERT_FALSE(digest.has_value());
    EXPECT_EQ(digest.error(), ErrorCode::kCouldNotCreateDigest);
}

TEST_F(HashCalculatorTest, UpdateStreamTest)
{
    ExpectSha256Call();
    EXPECT_CALL(openssl_lib_mock_, UpdateDigestCtx(_, _, _)).WillOnce(Return(1));

    std::vector<std::uint8_t> test_input{1, 2, 3, 4};
    score::cpp::span<const std::uint8_t> data(test_input.data(), test_input.size());

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->Update(data);

    ASSERT_TRUE(update_result.has_value());
}

TEST_F(HashCalculatorTest, EmptyUpdateTest)
{
    ExpectSha256Call();
    score::cpp::span<const std::uint8_t> data;

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->Update(data);

    ASSERT_FALSE(update_result.has_value());
    EXPECT_EQ(update_result.error(), ErrorCode::kInvalidParameters);
}

TEST_F(HashCalculatorTest, InvalidDigestUpdateTest)
{
    ExpectSha256Call();
    EXPECT_CALL(openssl_lib_mock_, UpdateDigestCtx(_, _, _)).WillOnce(Return(0));

    std::vector<std::uint8_t> test_input{1, 2, 3, 4};
    score::cpp::span<const std::uint8_t> data(test_input.data(), test_input.size());

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->Update(data);

    ASSERT_FALSE(update_result.has_value());
    EXPECT_EQ(update_result.error(), ErrorCode::kCouldNotUpdateDigest);
}

TEST_F(HashCalculatorTest, UpdateFromStreamTest)
{
    ExpectSha256Call();
    EXPECT_CALL(openssl_lib_mock_, UpdateDigestCtx(_, _, _)).WillOnce(Return(1));

    std::vector<char> test_input{1, 2, 3, 4};
    std::istringstream input_stream(std::string(test_input.begin(), test_input.end()));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->UpdateFromStream(input_stream);

    ASSERT_TRUE(update_result.has_value());
}

TEST_F(HashCalculatorTest, InvalidUpdateFromStreamTest)
{
    ExpectSha256Call();

    EXPECT_CALL(openssl_lib_mock_, UpdateDigestCtx(_, _, _)).WillOnce(Return(0));

    std::vector<char> test_input{1, 2, 3, 4};
    std::istringstream input_stream(std::string(test_input.begin(), test_input.end()));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->UpdateFromStream(input_stream);

    ASSERT_FALSE(update_result.has_value());
    EXPECT_EQ(update_result.error(), ErrorCode::kCouldNotUpdateDigest);
}

TEST_F(HashCalculatorTest, UpdateFromStreamGoesBadTest)
{
    ExpectSha256Call();

    std::vector<char> test_input{1, 2, 3, 4};
    std::istringstream input_stream(std::string(test_input.begin(), test_input.end()));

    auto callback = [&](StructDigestCtx*, const void*, std::size_t) -> std::int32_t {
        input_stream.setstate(input_stream.badbit);
        return 1;
    };

    EXPECT_CALL(openssl_lib_mock_, UpdateDigestCtx(_, _, _)).WillOnce(Invoke(callback));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->UpdateFromStream(input_stream);

    ASSERT_FALSE(update_result.has_value());
    EXPECT_EQ(update_result.error(), ErrorCode::kCouldNotUpdateDigestFromStream);
}

TEST_F(HashCalculatorTest, BadInputUpdateFromStreamTest)
{
    ExpectSha256Call();

    std::stringstream input_stream;
    input_stream.clear(input_stream.badbit);

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->UpdateFromStream(input_stream);

    ASSERT_FALSE(update_result.has_value());
    EXPECT_EQ(update_result.error(), ErrorCode::kStreamError);
}

TEST_F(HashCalculatorTest, EmptyInputUpdateFromStreamTest)
{
    std::vector<char> test_input{};
    std::istringstream input_stream(std::string(test_input.begin(), test_input.end()));
    const score::cpp::static_vector<std::uint8_t, kMaxDigestSize> expected_sha256{
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256);
    auto update_result = digest->UpdateFromStream(input_stream);

    ASSERT_TRUE(update_result.has_value());
    const auto hash_result_sha256 = digest->Finalize();

    EXPECT_EQ(hash_result_sha256, (Hash{HashAlgorithm::kSha256, expected_sha256}));
}

TEST_F(HashCalculatorTest, InvalidFinalizeTest)
{
    ExpectSha256Call();

    EXPECT_CALL(openssl_lib_mock_, FinalizeDigestValue(_, _, _)).WillOnce(Return(0));

    auto digest = OpensslHashCalculator::Create(HashAlgorithm::kSha256, openssl_lib_mock_);
    auto update_result = digest->Finalize();

    EXPECT_EQ(update_result, (Hash{HashAlgorithm::kNone, {}}));
}

TEST(HashCalculatorSHATest, HashAlgorithmsSimpleTestSha256)
{
    constexpr std::uint8_t test_input[]{'1', '2', '3', 'a', 'b', 'c'};
    const score::cpp::static_vector<std::uint8_t, kMaxDigestSize> expected_sha256{
        221, 19, 10, 132, 157, 123, 41,  229, 84,  27,  5,   210, 247, 248, 106, 74,
        205, 79, 30, 197, 152, 193, 201, 67,  135, 131, 245, 107, 196, 240, 255, 128};

    auto hash_calculator = OpensslHashCalculator::Create(HashAlgorithm::kSha256);
    ASSERT_TRUE(hash_calculator.has_value());

    EXPECT_TRUE(hash_calculator->Update(test_input).has_value());
    const auto hash_result_sha256 = hash_calculator->Finalize();

    EXPECT_EQ(hash_result_sha256, (Hash{HashAlgorithm::kSha256, expected_sha256}));
}

}  // namespace
}  // namespace hash
}  // namespace score
