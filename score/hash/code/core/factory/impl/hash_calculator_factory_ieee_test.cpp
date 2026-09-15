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
#include "score/hash/code/common/algorithms.h"
#include "score/hash/code/common/error.h"
#include "score/hash/code/core/factory/impl/hash_calculator_factory.h"
#include "score/hash/code/core/factory/impl/safe_hash_calculator_factory.h"
#include "score/hash/code/openssl/openssl_wrapper/openssl_lib_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

namespace score
{
namespace hash
{
namespace
{

template <typename Factory>
class HashCalculatorFactoryCreationTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(HashCalculatorFactoryCreationTest);

TYPED_TEST_P(HashCalculatorFactoryCreationTest, HashCalculatorFactorySuccessTest)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__hash__factory_interface");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that the factory creates a hash calculator instance for a hash algorithm (SHA-256) "
                         "and a checksum algorithm (CRC-32) without the caller depending on the concrete "
                         "implementation type.");

    for (auto algo : {HashAlgorithm::kSha256, HashAlgorithm::kCrc32})
    {
        TypeParam unit{};
        auto digest_result{unit.CreateHashCalculator(algo)};
        ASSERT_TRUE(digest_result.has_value());
    }
}

struct DataAndDigest
{
    std::vector<std::uint8_t> data;
    score::cpp::static_vector<std::uint8_t, kMaxDigestSize> sha256;
};

const std::vector<DataAndDigest> data_and_digest{
    // simple example
    {
        {'1', '2', '3', 'a', 'b', 'c'},
        {221, 19, 10, 132, 157, 123, 41,  229, 84,  27,  5,   210, 247, 248, 106, 74,
         205, 79, 30, 197, 152, 193, 201, 67,  135, 131, 245, 107, 196, 240, 255, 128},
    },
    // dataset with >64 bytes
    {
        {'J', 'K', 'L', 'j', 'z', 'd', 'r', 'f', 'o', 'i', 'd', 'k', 'O', 'J', 'J', 'h', 'u', 'f', 'd',
         's', 'h', 'O', 'L', 'K', '8', '6', '8', '7', 'H', 'J', 'K', 'h', 'd', 'i', '7', 'f', '6', 'z',
         ')', '(', 'N', 'H', '9', '8', '7', '6', 'z', '8', 'f', '7', 'd', '9', 'z', 'U', 'M', 'J', '&',
         '7', 'b', 'f', '9', 'K', 'J', 'G', 'H', 'I', 'o', 'l', 'i', 'm', 'f', 'd', '9', '0', 'o', 'O',
         'P', 'I', 'u', 'z', '9', '(', '6', '(', '/', 'z', '8', '/', 'n', 'm', 'z', ';'},
        {
            4,  253, 206, 109, 160, 180, 165, 188, 62, 228, 33, 142, 219, 118, 132, 48,
            98, 76,  38,  39,  126, 183, 176, 200, 36, 220, 23, 105, 62,  100, 73,  111,
        },
    },
    // dataset with >56 and <64 bytes
    {
        {'z', 'd', 's', 'u', 'o', 'i', 'a', 'j', 'l', 'i', 'L', ';', 'O', '(', '/', '(', ')', 'm', 'j', 'I',
         'D', 'k', 'j', 'd', 'Z', 'I', 'B', 'R', 'E', '5', '3', 'W', '%', '$', 'v', '%', 'B', '/', '&', 'N',
         'g', 'm', '8', 'a', 'i', 'U', 'I', 'Z', '7', ' ', 'B', 'G', 'I', 'U', 'J', 'h', '&', '/', 'i'},
        {206, 225, 74,  237, 91,  78,  33,  176, 22, 97, 21,  103, 146, 189, 123, 230,
         164, 162, 252, 229, 211, 169, 207, 84,  30, 69, 233, 73,  88,  174, 246, 75},
    },
};

TYPED_TEST_P(HashCalculatorFactoryCreationTest, CalculateSha256)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface, comp_req__hash__sha_algorithms");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that a calculator created for SHA-256 computes the correct digest for inputs of "
                         "varying length, including multi-block inputs.");

    TypeParam unit{};

    for (const auto& dataset : data_and_digest)
    {
        auto hash_result_sha256 = unit.CalculateHash(HashAlgorithm::kSha256, dataset.data);
        EXPECT_EQ(hash_result_sha256.value(), (Hash{HashAlgorithm::kSha256, dataset.sha256}));
    }
}

TYPED_TEST_P(HashCalculatorFactoryCreationTest, CalculateCrc32)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface, comp_req__hash__crc32_algorithm");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that a calculator created for CRC-32 computes the correct checksum.");

    TypeParam unit{};

    std::vector<std::uint8_t> test_input{'1', '2', '3', 'a', 'b', 'c'};
    score::cpp::span<const std::uint8_t> data(test_input);
    const score::cpp::static_vector<std::uint8_t, kMaxDigestSize> expected_crc32{210, 81, 254, 141};

    auto hash_result_crc32 = unit.CalculateHash(HashAlgorithm::kCrc32, data);

    EXPECT_EQ(hash_result_crc32.value(), (Hash{HashAlgorithm::kCrc32, expected_crc32}));
}

TYPED_TEST_P(HashCalculatorFactoryCreationTest, Crc32AutosarNotSupportedInIeeeVariant)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__hash__crc32_algorithm");
    this->RecordProperty("TestType", "fault-injection");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check that the IEEE-variant factory rejects the CRC-32-Autosar algorithm identifier, "
                         "since that variant is not supported by this factory.");

    TypeParam unit{};
    std::vector<std::uint8_t> test_input{'1', '2', '3', 'a', 'b', 'c'};
    score::cpp::span<const std::uint8_t> data(test_input);

    auto result = unit.CalculateHash(HashAlgorithm::kCrc32Autosar, data);

    EXPECT_FALSE(result.has_value());
}

REGISTER_TYPED_TEST_SUITE_P(HashCalculatorFactoryCreationTest,
                            HashCalculatorFactorySuccessTest,
                            CalculateSha256,
                            CalculateCrc32,
                            Crc32AutosarNotSupportedInIeeeVariant);
using Factories = ::testing::Types<HashCalculatorFactory, SafeHashCalculatorFactory>;

INSTANTIATE_TYPED_TEST_SUITE_P(WorkingDigests, HashCalculatorFactoryCreationTest, Factories, );

TEST(HashCalculatorFactory, hashCalculatorFactoryFailTest)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__factory_interface, comp_req__hash__safe_computation");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that the factory returns an error, instead of a calculator, when asked to create a "
                   "calculator for HashAlgorithm::kNone.");

    HashCalculatorFactory unit{};
    auto hash_factory = unit.CreateHashCalculator(HashAlgorithm::kNone);
    ASSERT_FALSE(hash_factory.has_value());
}

TEST(HashCalculatorFactory, HashCalculatorSpanInput)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that HashCalculatorFactory::CalculateHash() computes the correct digest for a span of "
                   "bytes in a single call.");

    HashCalculatorFactory unit{};
    std::vector<std::uint8_t> test_input{'1', '2', '3', 'a', 'b', 'c'};
    score::cpp::span<const std::uint8_t> data(test_input);
    const score::cpp::static_vector<std::uint8_t, kMaxDigestSize> expected_sha256{
        221, 19, 10, 132, 157, 123, 41,  229, 84,  27,  5,   210, 247, 248, 106, 74,
        205, 79, 30, 197, 152, 193, 201, 67,  135, 131, 245, 107, 196, 240, 255, 128};

    auto hash_result_sha256 = unit.CalculateHash(HashAlgorithm::kSha256, data);

    EXPECT_EQ(hash_result_sha256.value(), (Hash{HashAlgorithm::kSha256, expected_sha256}));
}

TEST(HashCalculatorFactory, HashCalculatorStreamInput)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that HashCalculatorFactory::CalculateHash() computes the correct digest for input "
                   "supplied via a std::istream.");

    HashCalculatorFactory unit{};
    std::istringstream test_input("123abc");
    const score::cpp::static_vector<std::uint8_t, kMaxDigestSize> expected_sha256{
        221, 19, 10, 132, 157, 123, 41,  229, 84,  27,  5,   210, 247, 248, 106, 74,
        205, 79, 30, 197, 152, 193, 201, 67,  135, 131, 245, 107, 196, 240, 255, 128};

    auto hash_result_sha256 = unit.CalculateHash(HashAlgorithm::kSha256, test_input);

    EXPECT_EQ(hash_result_sha256.value(), (Hash{HashAlgorithm::kSha256, expected_sha256}));
}

TEST(HashCalculatorFactory, FailedHashObjCreation)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that CalculateHash() returns an error, instead of a value, for HashAlgorithm::kNone with "
                   "span input.");

    HashCalculatorFactory unit{};
    std::vector<std::uint8_t> test_input{'1', '2', '3', 'a', 'b', 'c'};
    score::cpp::span<const std::uint8_t> data(test_input);

    auto hash_result = unit.CalculateHash(HashAlgorithm::kNone, data);

    EXPECT_FALSE(hash_result.has_value());
}

TEST(HashCalculatorFactory, FailedHashObjCreationStream)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that CalculateHash() returns an error, instead of a value, for HashAlgorithm::kNone with "
                   "stream input.");

    HashCalculatorFactory unit{};
    std::istringstream test_input("123abc");

    auto hash_result = unit.CalculateHash(HashAlgorithm::kNone, test_input);

    EXPECT_FALSE(hash_result.has_value());
}

TEST(HashCalculatorFactory, InvalidHashCalculatorSpanInput)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that CalculateHash() returns an error, instead of a value, for an empty (default "
                   "constructed) span.");

    HashCalculatorFactory unit{};
    score::cpp::span<const std::uint8_t> data;

    auto hash_result_sha256 = unit.CalculateHash(HashAlgorithm::kSha256, data);

    EXPECT_FALSE(hash_result_sha256.has_value());
}

TEST(HashCalculatorFactory, InvalidhashCalculatorStreamInput)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that CalculateHash() returns an error, instead of a value, when the input stream is "
                   "already in a failed state.");

    HashCalculatorFactory unit{};
    std::istringstream test_input{};
    test_input.setstate(std::ios::failbit);

    auto hash_result_sha256 = unit.CalculateHash(HashAlgorithm::kSha256, test_input);

    EXPECT_FALSE(hash_result_sha256.has_value());
}

TEST(HashCalculatorFactory, FailHashObjCreationWithMaxRead)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that CalculateHash() with a max-read limit still returns an error for "
                   "HashAlgorithm::kNone.");

    HashCalculatorFactory unit{};
    std::istringstream test_input("123abcefg");

    auto hash_result_none = unit.CalculateHash(HashAlgorithm::kNone, test_input, 6);

    EXPECT_FALSE(hash_result_none.has_value());
}

TEST(HashCalculatorFactory, HashCalculatorStreamInputWithMaxRead)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__calculation_interface");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that CalculateHash() computes the digest only over the first max-read bytes of a "
                   "stream, ignoring the remainder.");

    HashCalculatorFactory unit{};
    std::istringstream test_input("123abcefg");
    const score::cpp::static_vector<std::uint8_t, kMaxDigestSize> expected_sha256{
        221, 19, 10, 132, 157, 123, 41,  229, 84,  27,  5,   210, 247, 248, 106, 74,
        205, 79, 30, 197, 152, 193, 201, 67,  135, 131, 245, 107, 196, 240, 255, 128};

    auto hash_result_sha256 = unit.CalculateHash(HashAlgorithm::kSha256, test_input, 6);

    EXPECT_EQ(hash_result_sha256.value(), (Hash{HashAlgorithm::kSha256, expected_sha256}));
}

TEST(HashCalculatorFactory, InstantiateWithFailingOpenSslLib)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__factory_interface, comp_req__hash__safe_computation");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "error-guessing");
    RecordProperty("Description",
                   "Check that the factory propagates an instantiation error, instead of throwing, when the "
                   "underlying OpenSSL library fails to resolve the digest algorithm.");

    openssl::OpensslLibMock open_ssl_lib_mock{};
    EXPECT_CALL(open_ssl_lib_mock, DigestAlgoSha1()).WillOnce(::testing::Return(nullptr));
    HashCalculatorFactory unit{std::cref(open_ssl_lib_mock)};
    const auto create_result{unit.CreateHashCalculator(HashAlgorithm::kSha1)};
    ASSERT_FALSE(create_result.has_value());
    EXPECT_EQ(create_result.error(), ErrorCode::kCouldNotCreateDigest);
}

TEST(HashCalculatorFactory, InstantiateWithSuccess)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__factory_interface");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description",
                   "Check that the factory successfully creates a calculator by driving the underlying OpenSSL "
                   "digest lookup, context creation and initialization calls in the expected order.");

    openssl::OpensslLibMock open_ssl_lib_mock{};
    {
        openssl::StructDigestCtx* digest_context{reinterpret_cast<openssl::StructDigestCtx*>(0xC0DEBEEF)};
        const openssl::StructDigest* digest{reinterpret_cast<const openssl::StructDigest*>(0xDEADBEEF)};

        ::testing::InSequence seq{};
        EXPECT_CALL(open_ssl_lib_mock, DigestAlgoSha1()).WillOnce(::testing::Return(digest));
        EXPECT_CALL(open_ssl_lib_mock, CreateDigestCtx()).WillOnce(::testing::Return(digest_context));
        EXPECT_CALL(open_ssl_lib_mock, InitDigestCtx(digest_context, digest, nullptr)).WillOnce(::testing::Return(37));
    }

    HashCalculatorFactory unit{std::cref(open_ssl_lib_mock)};
    const auto create_result{unit.CreateHashCalculator(HashAlgorithm::kSha1)};
    EXPECT_TRUE(create_result.has_value());
}

TEST(SafeHashCalculatorFactory, TryToInstantiateNonexistentHash)
{
    RecordProperty("PartiallyVerifies", "comp_req__hash__factory_interface, comp_req__hash__safe_computation");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that SafeHashCalculatorFactory returns an error, instead of a calculator, for an "
                   "algorithm it does not support (SHA-1).");

    SafeHashCalculatorFactory unit{};
    EXPECT_FALSE(unit.CreateHashCalculator(HashAlgorithm::kSha1).has_value());
}

}  // namespace
}  // namespace hash
}  // namespace score
