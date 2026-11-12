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

#ifndef SCORE_LIB_HASH_CODE_COMMON_ALGORITHM_H
#define SCORE_LIB_HASH_CODE_COMMON_ALGORITHM_H

#include "score/mw/log/log_stream.h"

#include <score/string.hpp>

#include <cstdint>

namespace score
{
namespace hash
{

constexpr std::size_t kMaxDigestSize{64U};

constexpr std::uint8_t kSha256Size{32U};
constexpr std::uint8_t kSha384Size{48U};
constexpr std::uint8_t kSha512Size{64U};
constexpr std::uint8_t kCrc32Size{4U};
constexpr std::uint8_t kCrc32AutosarSize{4U};

/// Cryptographic hash algorithm
enum class HashAlgorithm : std::uint8_t
{
    kNone = 0,
    // Value 1 was SHA-1 and remains reserved as unsupported.
    kSha256 = 2,
    kSha384 = 3,
    kSha512 = 4,
    kCrc32 = 5,
    kCrc32Autosar = 6,
    kLast = 7
};

// Suppress "UNUSED C++14 A13-2-2" rule finding: "A binary arithmetic operator and a bitwise operator shall return
// a “prvalue”."
// The code here is present in single line to avoid '<<' is not a left shift operator but an overload for logging the
// respective types. code analysis tools tend to assume otherwise hence a false positive.
// coverity[autosar_cpp14_a13_2_2_violation]
inline score::mw::log::LogStream& operator<<(score::mw::log::LogStream& stream, const HashAlgorithm algorithm)
{
    score::mw::log::LogStream& modifiedStream = stream;

    switch (algorithm)
    {
        case HashAlgorithm::kCrc32:
            modifiedStream << "Crc32";
            break;
        case HashAlgorithm::kCrc32Autosar:
            modifiedStream << "Crc32Autosar";
            break;
        case HashAlgorithm::kSha256:
            modifiedStream << "Sha256";
            break;
        case HashAlgorithm::kSha384:
            modifiedStream << "Sha384";
            break;
        case HashAlgorithm::kSha512:
            modifiedStream << "Sha512";
            break;
        case HashAlgorithm::kNone:
            modifiedStream << "None";
            break;
        case HashAlgorithm::kLast:
            modifiedStream << "Last";
            break;
        default:
            modifiedStream << "HashAlgorithm{" << static_cast<std::uint16_t>(algorithm) << "}";
            break;
    }

    return modifiedStream;
}

score::cpp::optional<std::uint8_t> HashSizeInBytes(const HashAlgorithm algorithm) noexcept;
score::cpp::optional<std::uint8_t> HashSizeInCharacters(const HashAlgorithm algorithm) noexcept;

/// @brief identify the hash algorithm from the \p hash_string
/// @param[in] hash_string  - the hash as a string
/// @return HashAlgorithm   - the \p hash_string algorithm type
HashAlgorithm IdentifyHash(std::string_view hash_string) noexcept;

/// @brief identify hash algorithm from \p hash_size_in_bytes, for example vector.size()
///
/// @param[in] hash_vector  - hash size in bytes
/// @return HashAlgorithm   -the \p hash_size_in_bytes algorithm type
HashAlgorithm IdentifyHash(std::size_t hash_size_in_bytes) noexcept;

}  // namespace hash
}  // namespace score
#endif  // SCORE_LIB_HASH_CODE_COMMON_ALGORITHM_H
