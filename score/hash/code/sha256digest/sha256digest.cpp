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

#include "score/hash/code/sha256digest/sha256digest.h"
#include "score/hash/code/common/error.h"

#include <score/utility.hpp>

#include <cstdint>

namespace score
{
namespace hash
{

namespace
{

// Suppress "UNUSED C++14 A16-0-1" rule finding: "The pre-processor shall only be used for unconditional and
// conditional file inclusion and include guards, and using the following directives: (1) #ifndef, #ifdef, (3) #if, (4)
// #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Checks if byte order forcely requested to be little endian
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__BYTE_ORDER__)
constexpr bool kIsLittleEndian{__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__};
// coverity[autosar_cpp14_a16_0_1_violation]
#else  // defined(__BYTE_ORDER__)
// coverity[autosar_cpp14_a16_0_1_violation]
#error __BYTE_ORDER__ is not defined
// coverity[autosar_cpp14_a16_0_1_violation]
#endif  // defined(__BYTE_ORDER__)

// Suppress "UNUSED C++14 M8-5-2" rule find: "Braces shall be used to indicate and match the structure in the non-zero
// initialization of arrays and structures.".
// False positive as we here use initialization list.
// coverity[autosar_cpp14_m8_5_2_violation]
constexpr std::array<uint32_t, 64> kSha256Constants{
    0X428A2F98U, 0X71374491U, 0XB5C0FBCFU, 0XE9B5DBA5U, 0X3956C25BU, 0X59F111F1U, 0X923F82A4U, 0XAB1C5ED5U,
    0XD807AA98U, 0X12835B01U, 0X243185BEU, 0X550C7DC3U, 0X72BE5D74U, 0X80DEB1FEU, 0X9BDC06A7U, 0XC19BF174U,
    0XE49B69C1U, 0XEFBE4786U, 0X0FC19DC6U, 0X240CA1CCU, 0X2DE92C6FU, 0X4A7484AAU, 0X5CB0A9DCU, 0X76F988DAU,
    0X983E5152U, 0XA831C66DU, 0XB00327C8U, 0XBF597FC7U, 0XC6E00BF3U, 0XD5A79147U, 0X06CA6351U, 0X14292967U,
    0X27B70A85U, 0X2E1B2138U, 0X4D2C6DFCU, 0X53380D13U, 0X650A7354U, 0X766A0ABBU, 0X81C2C92EU, 0X92722C85U,
    0XA2BFE8A1U, 0XA81A664BU, 0XC24B8B70U, 0XC76C51A3U, 0XD192E819U, 0XD6990624U, 0XF40E3585U, 0X106AA070U,
    0X19A4C116U, 0X1E376C08U, 0X2748774CU, 0X34B0BCB5U, 0X391C0CB3U, 0X4ED8AA4AU, 0X5B9CCA4FU, 0X682E6FF3U,
    0X748F82EEU, 0X78A5636FU, 0X84C87814U, 0X8CC70208U, 0X90BEFFFAU, 0XA4506CEBU, 0XBEF9A3F7U, 0XC67178F2U,
};

// Suppress "UNUSED C++14 M8-5-2" rule find: "Braces shall be used to indicate and match the structure in the non-zero
// initialization of arrays and structures.".
// False positive as we here use initialization list.
// coverity[autosar_cpp14_m8_5_2_violation]
constexpr std::array<uint32_t, 8> kSha256StartingValues{
    0X6A09E667U,
    0XBB67AE85U,
    0X3C6EF372U,
    0XA54FF53AU,
    0X510E527FU,
    0X9B05688CU,
    0X1F83D9ABU,
    0X5BE0CD19U,
};

constexpr std::uint32_t RotateRight32(const std::uint32_t val, const std::uint32_t amount)
{
    constexpr auto sizeof_val_bits = sizeof(val) * 8U;
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(amount < sizeof_val_bits);
    return (val >> amount) | (val << (sizeof_val_bits - amount));
}

constexpr std::uint32_t Ch(const std::uint32_t x, const std::uint32_t y, const std::uint32_t z)
{
    return (x & y) ^ (~x & z);
}

constexpr std::uint32_t Maj(const std::uint32_t x, const std::uint32_t y, const std::uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

constexpr std::uint32_t Bsig0(const std::uint32_t x)
{
    return RotateRight32(x, 2U) ^ RotateRight32(x, 13U) ^ RotateRight32(x, 22U);
}

constexpr std::uint32_t Bsig1(const std::uint32_t x)
{
    return RotateRight32(x, 6U) ^ RotateRight32(x, 11U) ^ RotateRight32(x, 25U);
}

constexpr std::uint32_t Ssig0(const std::uint32_t x)
{
    return RotateRight32(x, 7U) ^ RotateRight32(x, 18U) ^ (x >> static_cast<std::uint32_t>(3U));
}

constexpr std::uint32_t Ssig1(const std::uint32_t x)
{
    return RotateRight32(x, 17U) ^ RotateRight32(x, 19U) ^ (x >> static_cast<std::uint32_t>(10U));
}
}  // namespace

Sha256Digest::Sha256Digest() noexcept : IHashCalculator(), buffer_{}, hash_(kSha256StartingValues), message_len_{0U} {}

ResultBlank Sha256Digest::Update(const score::cpp::span<const std::uint8_t> data) noexcept
{
    if (data.empty() || (data.data() == nullptr))
    {
        return MakeUnexpected(ErrorCode::kInvalidParameters);
    }

    using SpanSizeType = score::cpp::span<const std::uint8_t>::size_type;

    SpanSizeType start_index = 0U;
    while (start_index < data.size())
    {
        const score::cpp::span<const std::uint8_t> chunk = data.last(data.size() - start_index);
        // Suppress "UNUSED C++14 A4-7-1" rule finding. This rule states:
        // "An integer expression shall not lead to data loss."
        // No data loss as your converting from uint8_t to size_t
        // coverity[autosar_cpp14_a4_7_1_violation]
        start_index += buffer_.fill_from(chunk);
        if (buffer_.size() == kBufferSize)
        {
            UpdateHash();
            buffer_.clear();
        }
    }
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(data.size() < std::numeric_limits<SpanSizeType>::max() / 8U);
    const auto count = data.size() * 8U;
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(message_len_ < std::numeric_limits<std::uint64_t>::max() - count);
    message_len_ += count;

    return {};
}

Hash Sha256Digest::Finalize() noexcept
{
    buffer_.push_back(128U);
    if (buffer_.size() > static_cast<MessageBuffer::SizeType>(56U))
    {
        while (buffer_.size() < static_cast<MessageBuffer::SizeType>(64U))
        {
            buffer_.push_back(0U);
        }
        UpdateHash();
        buffer_.clear();
    }
    while (buffer_.size() < static_cast<MessageBuffer::SizeType>(56U))
    {
        buffer_.push_back(0U);
    }

    SCORE_LANGUAGE_FUTURECPP_ASSERT(buffer_.size() == static_cast<MessageBuffer::SizeType>(56U));

    for (std::uint64_t shift = 64U; shift > 0U; shift -= 8U)
    {
        // This is ok since we will exit the loop if 'shift' becomes negative
        const auto shift_reduced = shift - 8U;
        constexpr std::uint64_t mask = 0xFFU;
        const auto final_byte = (message_len_ >> shift_reduced) & mask;
        SCORE_LANGUAGE_FUTURECPP_ASSERT(final_byte <= std::numeric_limits<std::uint8_t>::max());
        buffer_.push_back(static_cast<std::uint8_t>(final_byte));
    }

    SCORE_LANGUAGE_FUTURECPP_ASSERT(buffer_.size() == static_cast<MessageBuffer::SizeType>(64U));

    UpdateHash();
    buffer_.clear();

    // We currently only support little endian due to the absence of test machines with big endianness. Support of
    // big endian should be a matter of removing word rotation below, though.
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(kIsLittleEndian);
    for (auto& word : hash_)  // Todo: Necessary on little-endian machines only?
    {
        // wwxxyyzz -> zzyyxxww -> ww >> 24 + xx >> 16 + yy << 8 + zz << 24
        word = (word >> 24U) | ((word >> 8U) & 0xFF00U) | ((word << 8U) & 0xFF0000U) | ((word << 24U) & 0xFF000000U);
    }

    // This is not a violation of the strict aliasing rules plus crc_calculator needs a std::uint8 buffer */
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) rationale above.
    // Suppress "UNUSED C++14 A5-2-4" rule finding: "Reinterpret_cast shall not be used"
    // coverity[autosar_cpp14_a5_2_4_violation]
    const auto* const hash_begin = reinterpret_cast<const std::uint8_t*>(&*hash_.begin());
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) rationale above.
    // This is not a violation of the strict aliasing rules plus crc_calculator needs a std::uint8 buffer */
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) rationale above.
    // Suppress "UNUSED C++14 A5-2-4" rule finding: "Reinterpret_cast shall not be used"
    // coverity[autosar_cpp14_a5_2_4_violation]
    const auto* const hash_end = reinterpret_cast<const std::uint8_t*>(&*hash_.end());
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) rationale above.
    return Hash{HashAlgorithm::kSha256, score::cpp::static_vector<std::uint8_t, kMaxDigestSize>{hash_begin, hash_end}};
}

// Suppress "UNUSED C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() is implicitly called from SCORE_LANGUAGE_FUTURECPP_PRECONDITION
// coverity[autosar_cpp14_a15_5_3_violation]
void Sha256Digest::UpdateHash() noexcept
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(static_cast<std::size_t>(buffer_.size()) == kBufferSize);

    // The remainder of this function is kept as similar as possible to the RFC6234 so that it is easy to compare both
    // code snippets to easily verify correctness of the code below.

    std::uint32_t a{hash_[0]};
    std::uint32_t b{hash_[1]};
    std::uint32_t c{hash_[2]};
    std::uint32_t d{hash_[3]};
    std::uint32_t e{hash_[4]};
    std::uint32_t f{hash_[5]};
    std::uint32_t g{hash_[6]};
    std::uint32_t h{hash_[7]};

    using Array = std::array<std::uint32_t, kBufferSize>;
    Array message{};
    for (Array::size_type t = 0U; t < 16U; ++t)
    {
        // MessageBuffer has no other access method, also bounds checked in for loop
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index) rationale above
        message[t] = (static_cast<std::uint32_t>(buffer_[t * 4U]) << 24U) |
                     (static_cast<std::uint32_t>(buffer_[(t * 4U) + 1U]) << 16U) |
                     (static_cast<std::uint32_t>(buffer_[(t * 4U) + 2U]) << 8U) |
                     (static_cast<std::uint32_t>(buffer_[(t * 4U) + 3U]));
        // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index) rationale above
    }
    /* t is >= 16 and < 64. Therefore, subtracting 16 is fine, which is the maximum in the below term */
    for (Array::size_type t = 16U; t < 64U; ++t)
    {
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index) rationale above
        //
        // Explanation for Coverity suppression for UNUSED A4-7-1 [An integer expression shall not lead to data loss.]:
        // Unsigned integer overflow in the loop below is actually intended
        // In the latest revision of Secure Hash Standard specification from NIST (FIPS 180-4) addition is always modulo
        // 2^W, where W stands for word size (32-bit in case of SHA-256). According to C++ standard in paragraph 6.8.1
        // arithmetic for unsigned types is defined as modulo 2^N, which means the code below is working as intended
        //
        // coverity[autosar_cpp14_a4_7_1_violation]
        message[t] = Ssig1(message[t - 2U]) + message[t - 7U] + Ssig0(message[t - 15U]) + message[t - 16U];
        // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index) rationale above
    }

    static_assert(std::tuple_size<decltype(kSha256Constants)>::value == std::tuple_size<decltype(message)>::value);

    for (Array::size_type round = 0U; round < 64U; ++round)
    {
        /* whether they're references or values */
        /* round is always less than kSha256Constants and message size, thus no out-of-bounds access should occur */
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index) rationale above
        //
        // Explanation for Coverity suppression for UNUSED A4-7-1 [An integer expression shall not lead to data loss.]:
        // Unsigned integer overflow in the loop below is actually intended
        // In the latest revision of Secure Hash Standard specification from NIST (FIPS 180-4) addition is always modulo
        // 2^W, where W stands for word size (32-bit in case of SHA-256). According to C++ standard in paragraph 6.8.1
        // arithmetic for unsigned types is defined as modulo 2^N, which means the code below is working as intended
        //
        // coverity[autosar_cpp14_a4_7_1_violation]
        const std::uint32_t t1 = h + Bsig1(e) + Ch(e, f, g) + kSha256Constants[round] + message[round];
        // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index) rationale above
        // coverity[autosar_cpp14_a4_7_1_violation]
        const std::uint32_t t2 = Bsig0(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        // coverity[autosar_cpp14_a4_7_1_violation]
        e = d + t1;
        d = c;
        c = b;
        b = a;
        // coverity[autosar_cpp14_a4_7_1_violation]
        a = t1 + t2;
    }

    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[0] += a;
    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[1] += b;
    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[2] += c;
    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[3] += d;
    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[4] += e;
    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[5] += f;
    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[6] += g;
    // coverity[autosar_cpp14_a4_7_1_violation]
    hash_[7] += h;
}

}  // namespace hash
}  // namespace score
