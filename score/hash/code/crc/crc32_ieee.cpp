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

#include <cstdint>
#include <numeric>

namespace score
{
namespace hash
{

namespace
{

constexpr std::uint_fast32_t kAllOnes{0xFFFFFFFFU};

template <std::uint_fast32_t ReversePolynomial>
class LookupTable final
{
  public:
    constexpr LookupTable() : table_{}
    {
        for (std::uint_fast32_t table_index = 0U; table_index < kTableSize; ++table_index)
        {
            auto checksum = table_index;

            for (auto round = 0U; round < 8U; ++round)
            {
                if (static_cast<bool>(checksum & 0x1U))
                {
                    checksum = (checksum >> 1U) ^ ReversePolynomial;
                }
                else
                {
                    checksum = (checksum >> 1U) ^ 0U;
                }
            }
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) can't use .at() in constexpr fn
            table_[table_index] = checksum;
        }
    }
    constexpr std::uint_fast32_t operator[](size_t i) const noexcept
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) can't use .at() in constexpr fn
        return table_[i];
    }

  private:
    static constexpr auto kTableSize = 256U;

    /* Doesn't impose security issues when used C-style array */
    /* Using C-style array for filling array constexpr function (this can't be done with an array or a vector)*/
    // NOLINTNEXTLINE(modernize-avoid-c-arrays) Using C-style array for filling array constexpr function
    std::uint_fast32_t table_[kTableSize];
};

constexpr LookupTable<0xEDB88320U> kIeeeCrc32LookupTable{};

template <typename InputIterator>
constexpr std::uint_fast32_t CalculateIeeeCrc32(std::uint_fast32_t start, InputIterator first, InputIterator last)
{
    return static_cast<std::uint_fast32_t>(0xFFFFFFFFU) &
           std::accumulate(
               first, last, start & kAllOnes, [](auto checksum, const std::uint_fast8_t value) -> std::uint_fast32_t {
                   return kIeeeCrc32LookupTable[(checksum ^ value) & 0xFFU] ^ (checksum >> 8U);
               });
}

}  // namespace

Crc32IeeeHashCalculator::Crc32IeeeHashCalculator() noexcept : ICrc32HashCalculator(), checksum_{kAllOnes} {}

std::uint_fast32_t Crc32IeeeHashCalculator::GetChecksum() const noexcept
{
    return ~checksum_ & kAllOnes;
}

ResultBlank Crc32IeeeHashCalculator::Update(const score::cpp::span<const std::uint8_t> data) noexcept
{
    checksum_ = CalculateIeeeCrc32(checksum_, data.begin(), data.end());
    return {};
}

Hash Crc32IeeeHashCalculator::Finalize() noexcept
{
    const auto checksum = ~checksum_;
    const score::cpp::static_vector<std::uint8_t, kMaxDigestSize> data{static_cast<std::uint8_t>(checksum & 0xFFU),
                                                                static_cast<std::uint8_t>((checksum >> 8U) & 0xFFU),
                                                                static_cast<std::uint8_t>((checksum >> 16U) & 0xFFU),
                                                                static_cast<std::uint8_t>((checksum >> 24U) & 0xFFU)};
    return Hash{HashAlgorithm::kCrc32, data};
}

}  // namespace hash
}  // namespace score
