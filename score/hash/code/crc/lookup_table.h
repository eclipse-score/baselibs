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

#ifndef SCORE_HASH_CODE_CRC_LOOKUP_TABLE_H
#define SCORE_HASH_CODE_CRC_LOOKUP_TABLE_H

#include <cstdint>

namespace score
{
namespace hash
{
namespace internal
{

/// @brief Compile-time CRC32 lookup table parameterised by a reflected polynomial.
///
/// This is an implementation detail shared by CRC32 calculator classes.
/// It is not part of the public API.
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
    /* Using C-style array for filling array constexpr function (this can't be done with an array or a vector) */
    // NOLINTNEXTLINE(modernize-avoid-c-arrays) Using C-style array for filling array constexpr function
    std::uint_fast32_t table_[kTableSize];
};

}  // namespace internal
}  // namespace hash
}  // namespace score

#endif  // SCORE_HASH_CODE_CRC_LOOKUP_TABLE_H
