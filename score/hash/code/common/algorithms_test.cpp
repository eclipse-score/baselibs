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

#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"

#include <gtest/gtest.h>

namespace score
{
namespace hash
{
namespace
{

using ::testing::_;
using ::testing::AtLeast;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;

const mw::log::SlotHandle kHandle{42};

class AlgorithmLoggerFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        mw::log::SetLogRecorder(&recorder_);

        EXPECT_CALL(recorder_, StartRecord(_, _)).WillRepeatedly(Return(kHandle));
        EXPECT_CALL(recorder_, StopRecord(_)).Times(AtLeast(1));
    }

    void TearDown() override
    {
        score::mw::log::SetLogRecorder(nullptr);
    }

    StrictMock<mw::log::RecorderMock> recorder_{};
};

TEST(HashIdentify, CorrectlyIdentifyHashFromString)
{
    const std::string crc32{"4342711f"};
    const std::string sha1{"526eac4f80dd6e6e73c7a501dff1abc83f0b7ccc"};
    const std::string sha256{"78e599a1555f8dec1d91833c01523e14ea8373cb92345600b24a3cbe987ba400"};
    const std::string sha384{
        "ca796ef303046f9773b9ca35b092be4a86b22d72f0ed91a0e49208d8defadf9f4de3bf63320c32d6890e80b7159afcdf"};
    const std::string sha512{
        "85b7400acc3064a0009ffbd99e2ace4b0d97ac0395f001d0e3ef8fe7f84fe91dfcb25366c6195684fab5e92c44218ed6bcfa50d7eea349"
        "2d39f5800a06c47e91"};

    EXPECT_EQ(IdentifyHash(crc32), HashAlgorithm::kCrc32);
    EXPECT_EQ(IdentifyHash(sha1), HashAlgorithm::kSha1);
    EXPECT_EQ(IdentifyHash(sha256), HashAlgorithm::kSha256);
    EXPECT_EQ(IdentifyHash(sha384), HashAlgorithm::kSha384);
    EXPECT_EQ(IdentifyHash(sha512), HashAlgorithm::kSha512);
}

TEST(HashIdentify, CanNotIdentifyBadHashesFromString)
{
    const std::string bad_hash_0{""};
    const std::string bad_hash_1{"a"};
    const std::string bad_hash_41{"526eac4f80dd6e6e73c7a501dff1abc83f0b7ccca"};

    EXPECT_EQ(IdentifyHash(bad_hash_0), HashAlgorithm::kNone);
    EXPECT_EQ(IdentifyHash(bad_hash_1), HashAlgorithm::kNone);
    EXPECT_EQ(IdentifyHash(bad_hash_41), HashAlgorithm::kNone);
}

TEST(HashIdentify, CorrectlyIdentifyHashFromSize)
{
    const std::vector<uint8_t> crc32{0x43, 0x42, 0x71, 0x1f};
    const std::vector<uint8_t> sha1{0x52, 0x6e, 0xac, 0x4f, 0x80, 0xdd, 0x6e, 0x6e, 0x73, 0xc7,
                                    0xa5, 0x01, 0xdf, 0xf1, 0xab, 0xc8, 0x3f, 0x0b, 0x7c, 0xcc};
    const std::vector<uint8_t> sha256{0x78, 0xe5, 0x99, 0xa1, 0x55, 0x5f, 0x8d, 0xec, 0x1d, 0x91, 0x83,
                                      0x3c, 0x01, 0x52, 0x3e, 0x14, 0xea, 0x83, 0x73, 0xcb, 0x92, 0x34,
                                      0x56, 0x00, 0xb2, 0x4a, 0x3c, 0xbe, 0x98, 0x7b, 0xa4, 0x00};
    const std::vector<uint8_t> sha384{0xca, 0x79, 0x6e, 0xf3, 0x03, 0x04, 0x6f, 0x97, 0x73, 0xb9, 0xca, 0x35,
                                      0xb0, 0x92, 0xbe, 0x4a, 0x86, 0xb2, 0x2d, 0x72, 0xf0, 0xed, 0x91, 0xa0,
                                      0xe4, 0x92, 0x08, 0xd8, 0xde, 0xfa, 0xdf, 0x9f, 0x4d, 0xe3, 0xbf, 0x63,
                                      0x32, 0x0c, 0x32, 0xd6, 0x89, 0x0e, 0x80, 0xb7, 0x15, 0x9a, 0xfc, 0xdf};
    const std::vector<uint8_t> sha512{0x85, 0xb7, 0x40, 0x0a, 0xcc, 0x30, 0x64, 0xa0, 0x00, 0x9f, 0xfb, 0xd9, 0x9e,
                                      0x2a, 0xce, 0x4b, 0x0d, 0x97, 0xac, 0x03, 0x95, 0xf0, 0x01, 0xd0, 0xe3, 0xef,
                                      0x8f, 0xe7, 0xf8, 0x4f, 0xe9, 0x1d, 0xfc, 0xb2, 0x53, 0x66, 0xc6, 0x19, 0x56,
                                      0x84, 0xfa, 0xb5, 0xe9, 0x2c, 0x44, 0x21, 0x8e, 0xd6, 0xbc, 0xfa, 0x50, 0xd7,
                                      0xee, 0xa3, 0x49, 0x2d, 0x39, 0xf5, 0x80, 0x0a, 0x06, 0xc4, 0x7e, 0x91

    };

    EXPECT_EQ(IdentifyHash(crc32.size()), HashAlgorithm::kCrc32);
    EXPECT_EQ(IdentifyHash(sha1.size()), HashAlgorithm::kSha1);
    EXPECT_EQ(IdentifyHash(sha256.size()), HashAlgorithm::kSha256);
    EXPECT_EQ(IdentifyHash(sha384.size()), HashAlgorithm::kSha384);
    EXPECT_EQ(IdentifyHash(sha512.size()), HashAlgorithm::kSha512);
}

TEST(HashIdentify, CanNotIdentifyBadHashesFromSize)
{
    const std::vector<uint8_t> bad_hash_0{};
    const std::vector<uint8_t> bad_hash_1{0xa};
    const std::vector<uint8_t> bad_hash_41{0x52, 0x6e, 0xac, 0x4f, 0x80, 0xdd, 0x6e, 0x6e, 0x73, 0xc7, 0xa5,
                                           0x01, 0xdf, 0xf1, 0xab, 0xc8, 0x3f, 0x0b, 0x7c, 0xcc, 0xa};

    EXPECT_EQ(IdentifyHash(bad_hash_0.size()), HashAlgorithm::kNone);
    EXPECT_EQ(IdentifyHash(bad_hash_1.size()), HashAlgorithm::kNone);
    EXPECT_EQ(IdentifyHash(bad_hash_41.size()), HashAlgorithm::kNone);
}

TEST(HashIdentify, CorrectSizes)
{
    const auto expected_sizes_bytes = std::map<const HashAlgorithm, const std::uint8_t>{
        {HashAlgorithm::kCrc32, 4},
        {HashAlgorithm::kCrc32Autosar, 4},
        {HashAlgorithm::kSha1, 20},
        {HashAlgorithm::kSha256, 32},
        {HashAlgorithm::kSha384, 48},
        {HashAlgorithm::kSha512, 64},
    };

    for (const auto [key, expected_size] : expected_sizes_bytes)
    {
        const auto hash_size_in_bytes = score::hash::HashSizeInBytes(key);
        const auto hash_size_in_characters = score::hash::HashSizeInCharacters(key);

        EXPECT_TRUE(hash_size_in_bytes.has_value());
        EXPECT_TRUE(hash_size_in_characters.has_value());
        EXPECT_EQ(hash_size_in_bytes.value(), expected_size);
        EXPECT_EQ(hash_size_in_characters.value(), expected_size * 2);
    }

    EXPECT_FALSE(score::hash::HashSizeInBytes(HashAlgorithm::kNone).has_value());
    EXPECT_FALSE(score::hash::HashSizeInBytes(HashAlgorithm::kLast).has_value());
    EXPECT_FALSE(score::hash::HashSizeInCharacters(HashAlgorithm::kNone).has_value());
    EXPECT_FALSE(score::hash::HashSizeInCharacters(HashAlgorithm::kLast).has_value());
}

TEST_F(AlgorithmLoggerFixture, CanLogAlgorithms)
{
    InSequence seq;
    EXPECT_CALL(recorder_, LogStringView(_, {"CRC32:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Crc32"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"CRC32 AUTOSAR:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Crc32Autosar"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"SHA-1:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Sha1"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"SHA-256:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Sha256"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"SHA-384:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Sha384"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"SHA-512:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Sha512"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"None:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"None"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"Last:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Last"}));

    EXPECT_CALL(recorder_, LogStringView(_, {"Invalid:"}));
    EXPECT_CALL(recorder_, LogStringView(_, {"HashAlgorithm{"}));
    EXPECT_CALL(recorder_, LogUint16(_, {static_cast<std::uint16_t>(42)}));
    EXPECT_CALL(recorder_, LogStringView(_, {"}"}));

    mw::log::LogInfo() << "CRC32:" << HashAlgorithm::kCrc32;
    mw::log::LogInfo() << "CRC32 AUTOSAR:" << HashAlgorithm::kCrc32Autosar;
    mw::log::LogInfo() << "SHA-1:" << HashAlgorithm::kSha1;
    mw::log::LogInfo() << "SHA-256:" << HashAlgorithm::kSha256;
    mw::log::LogInfo() << "SHA-384:" << HashAlgorithm::kSha384;
    mw::log::LogInfo() << "SHA-512:" << HashAlgorithm::kSha512;
    mw::log::LogInfo() << "None:" << HashAlgorithm::kNone;
    mw::log::LogInfo() << "Last:" << HashAlgorithm::kLast;
    mw::log::LogInfo() << "Invalid:" << HashAlgorithm{42};
}

}  // namespace
}  // namespace hash
}  // namespace score
