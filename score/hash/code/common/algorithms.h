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

/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR:constexpr must be initialized */
/* KW_SUPPRESS_START:UNUSED.STYLE.SINGLE_STMT_PER_LINE:this is a declaration with a literal expression*/
constexpr std::size_t kMaxDigestSize{64U};
/* KW_SUPPRESS_END:UNUSED.STYLE.SINGLE_STMT_PER_LINE */
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR */

constexpr std::uint8_t kSha1Size{20U};
constexpr std::uint8_t kSha256Size{32U};
constexpr std::uint8_t kSha384Size{48U};
constexpr std::uint8_t kSha512Size{64U};
constexpr std::uint8_t kCrc32Size{4U};
constexpr std::uint8_t kCrc32UnusedSize{4U};

/// Cryptographic hash algorithm
/* KW_SUPPRESS_START:UNUSED.STYLE.SINGLE_STMT_PER_LINE:this is a declaration of an enum; putting this on a single */
/* line harms readability */
enum class HashAlgorithm : std::uint8_t
{
    kNone = 0,
    kSha1,
    kSha256,
    kSha384,
    kSha512,
    kCrc32,
    kCrc32Unused,
    kLast
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
        case HashAlgorithm::kCrc32Unused:
            modifiedStream << "Crc32Unused";
            break;
        case HashAlgorithm::kSha1:
            modifiedStream << "Sha1";
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
/// @example if input is sha1sum as string "526eac4f80dd6e6e73c7a501dff1abc83f0b7ccc" the return will be
/// HashAlgorithm::kSha1
///
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
