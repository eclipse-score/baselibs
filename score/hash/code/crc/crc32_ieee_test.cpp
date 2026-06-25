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

#include "score/hash/code/crc/crc32_ieee.h"

#include <score/span.hpp>

#include <gtest/gtest.h>
#include <random>

namespace score
{
namespace hash
{
namespace
{

class Crc32IeeeTest : public ::testing::Test
{
  protected:
    template <typename InputIter>
    static Hash HashFromConstant(InputIter begin, InputIter end)
    {
        return {HashAlgorithm::kCrc32, score::cpp::static_vector<std::uint8_t, kMaxDigestSize>{begin, end}};
    }

    Crc32IeeeHashCalculator unit_{};

    static constexpr std::uint8_t kTest[]{"The quick brown fox jumps over the lazy dog"};
    static constexpr std::uint_fast32_t kExpected{0x414fa339U};
    static constexpr std::uint8_t kExpectedBytes[]{0x39U, 0xa3U, 0x4fU, 0x41U};
};

constexpr std::uint8_t Crc32IeeeTest::kTest[];
constexpr std::uint_fast32_t Crc32IeeeTest::kExpected;
constexpr std::uint8_t Crc32IeeeTest::kExpectedBytes[];

TEST_F(Crc32IeeeTest, Hash)
{
    // Given an input text
    const score::cpp::span<const std::uint8_t> input{kTest, sizeof(kTest) - 1};

    // When calculating the checksum
    EXPECT_TRUE(unit_.Update(input));

    // Then we get the expected values
    ASSERT_EQ(unit_.GetChecksum(), kExpected);
    ASSERT_EQ(HashFromConstant(std::begin(kExpectedBytes), std::end(kExpectedBytes)), unit_.Finalize());
}

TEST_F(Crc32IeeeTest, TwoUpdates)
{
    // Given an input text split in the middle at kPivot
    constexpr auto kPivot{sizeof(kTest) / 2U};
    const score::cpp::span<const std::uint8_t> input1{kTest, kPivot};
    const score::cpp::span<const std::uint8_t> input2{&kTest[kPivot], sizeof(kTest) - kPivot - 1};

    // When calculating the checksum over part 1 and part 2 separately
    EXPECT_TRUE(unit_.Update(input1));
    EXPECT_TRUE(unit_.Update(input2));

    // Then we still get the expected values
    ASSERT_EQ(unit_.GetChecksum(), kExpected);
    ASSERT_EQ(HashFromConstant(std::begin(kExpectedBytes), std::end(kExpectedBytes)), unit_.Finalize());
}

}  // namespace
}  // namespace hash
}  // namespace score
