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
/// \file
/// \brief Serializer for length tags.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_LENGTH_SERIALIZER_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_LENGTH_SERIALIZER_H

#include <array>
#include <cstdint>

#include "score/json/internal/writer/vajson/writer/serializers/structures/serializer.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief Serializes a length value as big endian
/// \details
/// - Copy each byte of the value to byte buffer, big-endian byte order,
/// - Write byte buffer to the stream.
/// \param[in] os Stream to write to.
/// \param[in] length to serialize.
inline void SerializeLength(WriterType os, const std::uint32_t length) noexcept
{
    constexpr std::size_t kPrefixSize{sizeof(std::uint32_t)};

    std::array<std::uint8_t, kPrefixSize> arr{};
    arr[0] = static_cast<std::uint8_t>((length >> 24) & 0xffu);
    arr[1] = static_cast<std::uint8_t>((length >> 16) & 0xffu);
    arr[2] = static_cast<std::uint8_t>((length >> 8) & 0xffu);
    arr[3] = static_cast<std::uint8_t>(length & 0xffu);

    os.get().write(reinterpret_cast<const char*>(arr.data()), kPrefixSize);
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_LENGTH_SERIALIZER_H
