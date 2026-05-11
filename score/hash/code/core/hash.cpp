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
#include "score/hash/code/common/error.h"

#include <score/optional.hpp>

#include <map>
#include <utility>

namespace
{

constexpr uint8_t kInvalidCharSentinel{std::numeric_limits<uint8_t>::max()};

/// @brief Converts byte to the hex representation. Used in ToString() function
/// @return Hex representation of byte
char NumToChar(const std::uint8_t number) noexcept
{
    const std::vector<char> hex_chars = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(number < hex_chars.size());
    return hex_chars.at(static_cast<std::size_t>(number));
}

std::uint8_t Char2Num(const char& character)
{
    const std::map<char, std::uint8_t> char_num_mapping = {
        {'0', 0U},  {'1', 1U},  {'2', 2U},  {'3', 3U},  {'4', 4U},  {'5', 5U},  {'6', 6U},  {'7', 7U},
        {'8', 8U},  {'9', 9U},  {'a', 10U}, {'A', 10U}, {'b', 11U}, {'B', 11U}, {'c', 12U}, {'C', 12U},
        {'d', 13U}, {'D', 13U}, {'e', 14U}, {'E', 14U}, {'f', 15U}, {'F', 15U}};

    const auto find_iterator = char_num_mapping.find(character);
    if (find_iterator != char_num_mapping.end())
    {
        return find_iterator->second;
    }
    else
    {
        return kInvalidCharSentinel;
    }
}

}  // namespace

namespace score
{
namespace hash
{

Hash::Hash(const HashAlgorithm algorithm, ByteVector raw_hash) : algorithm_{algorithm}, raw_hash_{std::move(raw_hash)}
{
}

score::cpp::span<const std::uint8_t> Hash::GetBytes() const& noexcept
{
    return raw_hash_;
}

score::cpp::pmr::string Hash::ToString() const
{
    score::cpp::pmr::string hex_repr{};
    const auto hash_sz = HashSizeInCharacters(algorithm_);

    // As currently the constructor does not enforce neither algorithm validity nor size correctedness, it could be the
    // case that we have an invalid algorithm being used in the object (for example, kNone). As we can still generate a
    // string representation even for the invalid cases, we do not expose this problem to the user.
    if (hash_sz.has_value())
    {
        hex_repr.reserve((static_cast<std::size_t>(HashSizeInCharacters(algorithm_).value()) + 1U));
    }
    else
    {
        return hex_repr;
    }
    // GCOVR is unable to analyze line below. But all decision are taken in for loop declaration.
    for (const auto a_byte : raw_hash_)  // LCOV_EXCL_BR_LINE rationale above
    {
        const auto high_char = static_cast<std::uint8_t>(a_byte / 16U);
        const auto low_char = static_cast<std::uint8_t>(a_byte % 16U);

        hex_repr.push_back(NumToChar(high_char));
        hex_repr.push_back(NumToChar(low_char));
    }

    return hex_repr;
}

Result<Hash> Hash::FromString(const HashAlgorithm algorithm, const score::cpp::string_view& hex_repr)
{
    const score::cpp::optional<std::uint8_t> exp_size = HashSizeInCharacters(algorithm);

    if (!exp_size.has_value())
    {
        return score::MakeUnexpected(score::hash::ErrorCode::kInvalidParameters, "Unsupported algorithm");
    }
    else if (hex_repr.size() != exp_size.value())
    {
        return score::MakeUnexpected(score::hash::ErrorCode::kInvalidParameters,
                                   "String size does not match expected size for the chosen algorithm");
    }
    else
    {
        ByteVector raw_hash{};
        bool is_high_byte = true;
        // Suppress "UNUSED C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
        // being given values that are not subsequently used"
        // False positive, variable is used.
        // coverity[autosar_cpp14_a0_1_1_violation]
        std::uint8_t current_byte = 0U;

        for (const auto character : hex_repr)
        {
            const std::uint8_t current_digit = Char2Num(character);

            if (current_digit == kInvalidCharSentinel)
            {
                return score::MakeUnexpected(score::hash::ErrorCode::kInvalidParameters,
                                           "String does not represent an hexadecimal number");
            }

            if (is_high_byte)
            {
                current_byte = static_cast<uint8_t>(current_digit << 4U);
            }
            else
            {
                // When is_high_byte is true, high byte in current_byte is updated with current_digit.
                // When is_high_byte is false, low byte in current_byte is updated with current_digit.
                // is_high_byte will toggle for each iteration. Char2Num method verifies that the input is within the
                // hex range. Since current_digit is within hexadecimal range, max value of current_byte will be
                // 0xFF.
                // coverity[autosar_cpp14_a4_7_1_violation]: Overflow won't occur.
                current_byte = static_cast<uint8_t>(current_byte + current_digit);
                raw_hash.push_back(current_byte);
            }

            is_high_byte = !is_high_byte;
        }

        return Hash{algorithm, raw_hash};
    }
}

HashAlgorithm Hash::GetAlgorithm() const noexcept
{
    return algorithm_;
}

bool operator==(const Hash& lhs, const Hash& rhs) noexcept
{
    return (lhs.algorithm_ == rhs.algorithm_) && (lhs.raw_hash_ == rhs.raw_hash_);
}

bool operator!=(const Hash& lhs, const Hash& rhs) noexcept
{
    return !operator==(lhs, rhs);
}

}  // namespace hash
}  // namespace score
