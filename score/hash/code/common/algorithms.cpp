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

namespace score
{
namespace hash
{

score::cpp::optional<std::uint8_t> HashSizeInBytes(const HashAlgorithm algorithm) noexcept
{
    score::cpp::optional<std::uint8_t> hashSizeInBytes = score::cpp::nullopt;

    switch (algorithm)
    {
        case HashAlgorithm::kCrc32:
            hashSizeInBytes = kCrc32Size;
            break;
        case HashAlgorithm::kCrc32Autosar:
            hashSizeInBytes = kCrc32AutosarSize;
            break;
        case HashAlgorithm::kSha1:
            hashSizeInBytes = kSha1Size;
            break;
        case HashAlgorithm::kSha256:
            hashSizeInBytes = kSha256Size;
            break;
        case HashAlgorithm::kSha384:
            hashSizeInBytes = kSha384Size;
            break;
        case HashAlgorithm::kSha512:
            hashSizeInBytes = kSha512Size;
            break;
        case HashAlgorithm::kNone:
        case HashAlgorithm::kLast:
        default:
            break;
    }

    return hashSizeInBytes;
}

score::cpp::optional<std::uint8_t> HashSizeInCharacters(const HashAlgorithm algorithm) noexcept
{
    auto size_in_bytes_optional = HashSizeInBytes(algorithm);
    if (!size_in_bytes_optional.has_value())
    {
        return size_in_bytes_optional;
    }

    return static_cast<std::uint8_t>(size_in_bytes_optional.value() * 2U);
}

HashAlgorithm IdentifyHash(std::string_view hash_string) noexcept
{
    HashAlgorithm identifiedHash = HashAlgorithm::kNone;

    // Reasoning: one digit is one uint8_t for example 0xFF so one digit(byte) is two chars ('f' 'f')
    // e.g. hash: 85b7400acc... first byte is 0x85 --> one byte(uint8_t) --> two chars '8' and '5'
    switch (hash_string.length())
    {
        case kCrc32Size * 2U:
            // there's no way to distinguish between kCrc32 and kCrc32Autosar based on the bytes alone
            identifiedHash = HashAlgorithm::kCrc32;
            break;
        case kSha1Size * 2U:
            identifiedHash = HashAlgorithm::kSha1;
            break;
        case kSha256Size * 2U:
            identifiedHash = HashAlgorithm::kSha256;
            break;
        case kSha384Size * 2U:
            identifiedHash = HashAlgorithm::kSha384;
            break;
        case kSha512Size * 2U:
            identifiedHash = HashAlgorithm::kSha512;
            break;
        default:
            break;
    }

    return identifiedHash;
}

HashAlgorithm IdentifyHash(std::size_t hash_size_in_bytes) noexcept
{
    HashAlgorithm identifiedHash = HashAlgorithm::kNone;

    switch (hash_size_in_bytes)
    {
        case kCrc32Size:
            // there's no way to distinguish between kCrc32 and kCrc32Autosar based on the bytes alone
            identifiedHash = HashAlgorithm::kCrc32;
            break;
        case kSha1Size:
            identifiedHash = HashAlgorithm::kSha1;
            break;
        case kSha256Size:
            identifiedHash = HashAlgorithm::kSha256;
            break;
        case kSha384Size:
            identifiedHash = HashAlgorithm::kSha384;
            break;
        case kSha512Size:
            identifiedHash = HashAlgorithm::kSha512;
            break;
        default:
            break;
    }

    return identifiedHash;
}

}  // namespace hash
}  // namespace score
