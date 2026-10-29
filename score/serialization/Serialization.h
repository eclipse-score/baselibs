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

#ifndef SCORE_PLATFORM_SERIALIZATION_H
#define SCORE_PLATFORM_SERIALIZATION_H

#include "score/serialization/serializer.h"
#include "score/serialization/strategies/unsigned_integral_only_strategy.h"

#include <score/assert.hpp>
#include <score/span.hpp>
#include <score/static_vector.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace score
{

namespace helper
{

using VectorBytes_t = std::vector<std::uint8_t>;

template <class T>
struct is_amp_static_vector : std::false_type
{
};

template <std::size_t N>
struct is_amp_static_vector<score::cpp::static_vector<std::uint8_t, N>> : std::true_type
{
};
}  // namespace helper

/// In-place serialization for vectors is not supported!
/// @tparam T The type of the value to serialize
/// @param buffer The buffer to serialize into
/// @param variable The value to serialize
/// @param offset The offset where the serialized value will be put at
/// @param swap_bytes Whether to swap the order of the bytes

template <typename T,
          typename Container,
          typename = std::enable_if_t<std::is_same<Container, helper::VectorBytes_t>::value>>
void serialize_in_place(Container&, const T&, std::int64_t&, bool = true)
{
    static_assert(!std::is_same<Container, helper::VectorBytes_t>::value,
                  "In place serialization is not supported for vectors");
}

/// Serializes a given value at a given offset into a byte-array. Previous values at these bytes will be overwritten.
/// The size of the array must be sufficient to store the serialized value at the offset. Otherwise, a termination
/// occurs.
/// @tparam T The type of the value to serialize
/// @param buffer The buffer to serialize into
/// @param variable The value to serialize
/// @param offset The offset where the serialized value will be put at - updated to the new offset for the next value
/// @param swap_bytes Whether to swap the order of the bytes
template <typename T>
void serialize_in_place(score::cpp::span<std::uint8_t> buffer,
                        const T& variable,
                        std::int64_t& offset,
                        bool swap_bytes = true)
{
    serialization::Serializer<serialization::UnsignedIntegralOnlyStrategy> serializer{
        serialization::UnsignedIntegralOnlyStrategy{swap_bytes}};
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(buffer.size() >= static_cast<std::size_t>(offset));
    auto offset_corrected_view{buffer.subspan(static_cast<std::size_t>(offset))};
    const auto result = serializer.Serialize(variable, offset_corrected_view);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(result.has_value());

    constexpr auto max_int64_t = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(result.value() < max_int64_t);
    offset += static_cast<std::int64_t>(result.value());
}

/// Serializes a given value and appends it at the end of the vector.
/// @tparam T The type of the value to serialize
/// @param buffer The buffer to serialize into
/// @param variable The value to serialize
/// @param swap_bytes Whether to swap the order of the bytes
template <typename Container,
          typename T,
          typename = std::enable_if_t<std::is_same<Container, std::vector<std::uint8_t>>::value ||
                                      helper::is_amp_static_vector<Container>::value>>
[[nodiscard]] score::Result<std::size_t> serialize_and_allocate(Container& buffer,
                                                                const T& variable,
                                                                bool swap_bytes = true)
{
    serialization::Serializer<serialization::UnsignedIntegralOnlyStrategy> serializer{
        serialization::UnsignedIntegralOnlyStrategy{swap_bytes}};
    return serializer.Serialize(variable, buffer);
}

/// Deserializes a value of a given type at an offset from a byte-array.
/// @tparam T The type of the value to deserialize to
/// @param bytes The buffer to deserialize from
/// @param offset The offset where to start deserialization
/// @param swap_bytes Whether to swap the order of the bytes
/// @return The deserialized value
template <typename T>
auto deserialize(const score::cpp::span<const std::uint8_t> bytes, std::size_t& offset, bool swap_bytes = true) -> T
{
    serialization::Serializer<serialization::UnsignedIntegralOnlyStrategy> serializer{
        serialization::UnsignedIntegralOnlyStrategy{swap_bytes}};

    T value{};
    // coverity[autosar_cpp14_m5_0_4_violation]
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(offset < std::numeric_limits<score::cpp::span<std::uint8_t>::size_type>::max());
    score::cpp::span<const std::uint8_t> view =
        bytes.subspan(static_cast<score::cpp::span<std::uint8_t>::size_type>(offset));

    const auto result = serializer.DeSerialize(view, value);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(result.has_value());
    offset += result.value();

    return value;
}

}  // namespace score

#endif  // SCORE_PLATFORM_SERIALIZATION_H
