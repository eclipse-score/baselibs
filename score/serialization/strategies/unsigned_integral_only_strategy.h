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

#ifndef SCORE_SERIALIZATION_STRATEGIES_UNSIGNED_INTEGRAL_ONLY_STRATEGY_H
#define SCORE_SERIALIZATION_STRATEGIES_UNSIGNED_INTEGRAL_ONLY_STRATEGY_H

#include "score/serialization/meta_information.h"
#include "score/serialization/serialize_iterator.h"

#include <score/utility.hpp>

#include <array>
#include <cstring>
#include <type_traits>

namespace score
{
namespace serialization
{

namespace helper
{

template <class T, bool = std::is_enum<T>::value>
struct type_or_underlying_type
{
    using type = typename std::underlying_type<T>::type;
};
template <class T>
struct type_or_underlying_type<T, false>
{
    using type = T;
};

template <typename T>
struct is_uint_fix_width
{
    // Suppress "AUTOSAR C++14 A0-1-1" rule findings. This rule states: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used." False positive, value is used as
    // part of an std::enable_if condition in a template.
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    static constexpr bool value =
        (((std::is_same<T, std::uint64_t>::value) || (std::is_same<T, std::uint32_t>::value)) ||
         ((std::is_same<T, std::uint16_t>::value) || (std::is_same<T, std::uint8_t>::value)));
};

}  // namespace helper

class UnsignedIntegralOnlyStrategy
{
  public:
    explicit UnsignedIntegralOnlyStrategy(const bool reverse_byte_order) : reverse_byte_order_{reverse_byte_order} {}
    virtual ~UnsignedIntegralOnlyStrategy() = default;

    // Used by the Serializer constructor (copy-move)
    // coverity[autosar_cpp14_a12_8_6_violation]
    UnsignedIntegralOnlyStrategy(UnsignedIntegralOnlyStrategy&&) noexcept = default;

    UnsignedIntegralOnlyStrategy& operator=(const UnsignedIntegralOnlyStrategy&) = delete;
    UnsignedIntegralOnlyStrategy& operator=(UnsignedIntegralOnlyStrategy&&) = delete;

  protected:
    UnsignedIntegralOnlyStrategy(const UnsignedIntegralOnlyStrategy&) noexcept = default;

  public:
    // Suppress "AUTOSAR C++14 M3-2-2", The rule states: "The One Definition Rule shall not be violated".
    // This is a false positive. Serialize is not defined more than once.
    template <typename Visitor,
              typename Container,
              typename T,
              std::enable_if_t<helper::is_uint_fix_width<typename helper::type_or_underlying_type<T>::type>::value,
                               bool> = true>
    // coverity[autosar_cpp14_m3_2_2_violation]
    void Serialize(Visitor&, SerializeIterator<Container> iterator, const T& value) const
    {
        constexpr auto variable_size = sizeof(T);
        std::array<std::uint8_t, variable_size> bytes{};
        static_assert(std::is_trivially_copyable<T>::value == true, "Serialized 'value' must be TriviallyCopyable");
        // Raw memory operations are essence of Serialization, and safe as long as type being serialized satisfies
        // TriviallyCopyable requirement. Destination is just byte buffer, so does not have to match any requirement.
        // NOLINTNEXTLINE(score-banned-function): std::memcpy() needed for serialization, TriviallyCopyable-ness checked
        score::cpp::ignore = std::memcpy(bytes.data(), &value, static_cast<size_t>(variable_size));

        if (reverse_byte_order_)
        {
            score::cpp::ignore = std::copy(bytes.crbegin(), bytes.crend(), iterator);
        }
        else
        {
            score::cpp::ignore = std::copy(bytes.cbegin(), bytes.cend(), iterator);
        }
    }

    // Suppress "AUTOSAR C++14 M3-2-2", The rule states: "The One Definition Rule shall not be violated".
    // This is a false positive. DeSerialize is not defined more than once.
    template <typename Visitor,
              typename Container,
              typename T,
              std::enable_if_t<helper::is_uint_fix_width<typename helper::type_or_underlying_type<T>::type>::value,
                               bool> = true>
    // coverity[autosar_cpp14_m3_2_2_violation]
    Result<void> DeSerialize(Visitor&, DeSerializeIterator<Container>& iterator, T& value) const
    {
        constexpr auto size = sizeof(T);
        const auto begin = iterator;
        // Suppress "AUTOSAR C++14 M5-0-4", the rule states "An implicit integral conversion shall not change the
        // signedness of the underlying type." No harm from implicit integral conversion here.
        // coverity[autosar_cpp14_m5_0_4_violation]
        std::advance(iterator, size);

        std::array<std::uint8_t, size> raw_bytes{};
        if (reverse_byte_order_)  // LCOV_EXCL_BR_LINE (tooling issue SWP-191502)
        {
            score::cpp::ignore = std::copy(begin, iterator, raw_bytes.rbegin());
        }
        else
        {
            score::cpp::ignore = std::copy(begin, iterator, raw_bytes.begin());
        }

        static_assert(std::is_trivially_copyable<T>::value == true, "Deserialized 'value' must be TriviallyCopyable");
        // Raw memory operations are essence of Serialization, and safe as long as type being deserialized
        // satisfies TriviallyCopyable requirement. Source is just byte buffer.
        // NOLINTNEXTLINE(score-banned-function): std::memcpy() needed for deserialization, TriviallyCopyable-ness
        // checked
        score::cpp::ignore = std::memcpy(&value, raw_bytes.data(), static_cast<std::size_t>(size));

        return {};
    }

  private:
    const bool reverse_byte_order_;
};

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_STRATEGIES_UNSIGNED_INTEGRAL_ONLY_STRATEGY_H
