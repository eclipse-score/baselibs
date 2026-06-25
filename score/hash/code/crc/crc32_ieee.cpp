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
#include "score/hash/code/crc/lookup_table.h"

#include <cstdint>
#include <numeric>

namespace score
{
namespace hash
{

namespace
{

constexpr std::uint_fast32_t kAllOnes{0xFFFFFFFFU};

constexpr internal::LookupTable<0xEDB88320U> kIeeeCrc32LookupTable{};

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
