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

#ifndef SCORE_SERIALIZATION_STRATEGIES_UDS_STRATEGY_H
#define SCORE_SERIALIZATION_STRATEGIES_UDS_STRATEGY_H

#include "score/memory/endianness.h"
#include "score/serialization/types/length_prefixed_vector.h"

#include "score/serialization/strategies/unsigned_integral_only_strategy.h"

#include <score/static_vector.hpp>
#include <score/string.hpp>
#include <score/vector.hpp>

#include <array>
#include <iterator>

namespace score
{
namespace serialization
{

namespace helper
{
constexpr auto fixed_string_size = 255U;
}  // namespace helper

/// @brief This class should implement the serialization / deserialization logic for the UDS strategy.
///
/// @details As of now the UDS serialization strategy only supports:
/// * unsigned integral types (all sizes)
/// * enums with underlying type of integral types (all sizes)
/// * score::cpp::pmr::string (on wire: first byte size of string, then string, then padding until length of 255 is
/// achieved)
/// * score::cpp::static_vector (on wire: first byte number of actually used elements, then the elements)
/// * score::cpp::pmr::vector (on wire: one element after another, until end of buffer reached)
/// * std::array (on wire one element after another for the size of the array)
/// * score::serialization::types::LengthPrefixedVector (on wire first n-bytes number of elements, then the elements)
///
/// Everything is stored only in big-endian format on wire.
class UDSStrategy final : public UnsignedIntegralOnlyStrategy
{
  public:
    // in theory, it is possible that UDS jobs also want little endian format, but until no I did not see such kind
    UDSStrategy() noexcept
        : UnsignedIntegralOnlyStrategy(memory::SwitchingBytesNecessary(memory::Endianness::kBigEndian))
    {
    }

    using UnsignedIntegralOnlyStrategy::DeSerialize;
    using UnsignedIntegralOnlyStrategy::Serialize;

    template <typename Visitor, typename Container, typename UnderlyingType, typename LengthType>
    void Serialize(Visitor& visitor,
                   SerializeIterator<Container> iterator,
                   const LengthPrefixedVector<UnderlyingType, LengthType>& value) const
    {
        Serialize(visitor, iterator, value.size());
        for (const auto& sub_value : value)
        {
            visitor.Next(sub_value);
        }
    }

    template <typename Visitor, typename Container, typename UnderlyingType, typename LengthType>
    Result<void> DeSerialize(Visitor& visitor,
                             DeSerializeIterator<Container>&,
                             LengthPrefixedVector<UnderlyingType, LengthType>& value) const
    {
        value.clear();  // a reused destination must hold only the decoded value, not previous contents
        LengthType counter{};
        visitor.Next(counter);
        if (!visitor.GetResult().has_value())
        {
            return MakeUnexpected(SerializationError::kOutOfBoundsWrite);
        }

        for (; counter > static_cast<decltype(counter)>(0); counter--)
        {
            UnderlyingType underlying_type{};
            visitor.Next(underlying_type);
            if (!visitor.GetResult().has_value())
            {
                return MakeUnexpected(SerializationError::kOutOfBoundsWrite);
            }
            value.push_back(underlying_type);
        }

        return {};
    }

    template <typename Visitor, typename Container>
    void Serialize(Visitor& visitor, SerializeIterator<Container> iterator, const score::cpp::pmr::string& value) const
    {
        if (value.size() > helper::fixed_string_size)
        {
            iterator = MakeUnexpected(SerializationError::kStringTooBig);
        }
        else
        {
            Serialize(visitor, iterator, static_cast<std::uint8_t>(value.size()));
            for (auto& character : value)
            {
                Serialize(visitor, iterator, static_cast<std::uint8_t>(character));
            }

            for (std::size_t counter{value.size()}; counter < helper::fixed_string_size; counter++)
            {
                // padding for fixed size 255
                constexpr std::uint8_t padding_filler{static_cast<std::uint8_t>(0x00)};
                Serialize(visitor, iterator, padding_filler);
            }
        }
    }

    template <typename Visitor, typename Container>
    Result<void> DeSerialize(Visitor&, DeSerializeIterator<Container>& iterator, score::cpp::pmr::string& value) const
    {
        value.clear();  // a reused destination must hold only the decoded value, not previous contents
        if (iterator == iterator.End())
        {
            return {};
        }

        const auto size = *iterator;
        auto counter = size;

        // Suppress "AUTOSAR C++14 M5-2-10" rule finding. This rule states: "The increment (++) and decrement (--)
        // operators shall not be mixed with other operators in an expression.".
        // Justification: Apart from using the increment operator, the assignment operator is used to ignore the
        // increment result, which also eliminates another rule violation.
        // coverity[autosar_cpp14_m5_2_10_violation]
        score::cpp::ignore = ++iterator;

        // Guard against manipulated input where the length byte exceeds the actually available bytes.
        if (static_cast<std::ptrdiff_t>(size) > std::distance(iterator, iterator.End()))
        {
            return MakeUnexpected(SerializationError::kOutOfBoundsWrite);
        }

        for (; counter > static_cast<decltype(counter)>(0); counter--)
        {
            value.push_back(static_cast<char>(*iterator));
            // coverity[autosar_cpp14_m5_2_10_violation]: same reason as above
            score::cpp::ignore = ++iterator;
        }

        std::advance(iterator,
                     static_cast<std::ptrdiff_t>(helper::fixed_string_size) - static_cast<std::ptrdiff_t>(size));

        return {};
    }

    template <typename Visitor, typename Container, typename UnderlyingType, std::size_t MaxSize>
    void Serialize(Visitor& visitor,
                   SerializeIterator<Container> iterator,
                   const score::cpp::static_vector<UnderlyingType, MaxSize>& value) const
    {
        static_assert(MaxSize <= std::numeric_limits<std::uint8_t>::max(),
                      "The size of the static_vector exceeds the maximum value that can be stored in a uint8_t.");
        // Suppress"AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
        // loss." In this context, there is no data loss because the static assert above ensures the value is in range
        // of unit8.
        // coverity[autosar_cpp14_a4_7_1_violation]
        Serialize(visitor, iterator, static_cast<std::uint8_t>(value.size()));
        for (const auto& sub_value : value)  // LCOV_EXCL_BR_LINE (tooling issue SWP-188259)
        {
            visitor.Next(sub_value);
        }
    }

    template <typename Visitor, typename Container, typename UnderlyingType, std::size_t MaxSize>
    Result<void> DeSerialize(Visitor& visitor,
                             DeSerializeIterator<Container>& iterator,
                             score::cpp::static_vector<UnderlyingType, MaxSize>& value) const
    {
        value.clear();  // a reused destination must hold only the decoded value, not previous contents
        if (iterator == iterator.End())
        {
            return {};
        }

        const auto size = *iterator;

        auto counter = size;

        if (size > value.capacity())
        {
            return MakeUnexpected(SerializationError::kOutOfBoundsWrite);
        }

        value.resize(static_cast<std::size_t>(size));

        // Suppress "AUTOSAR C++14 M5-2-10" rule finding. This rule states: "The increment (++) and decrement (--)
        // operators shall not be mixed with other operators in an expression.".
        // Justification: Apart from using the increment operator, the assignment operator is used to ignore the
        // increment result, which also eliminates another rule violation.
        // coverity[autosar_cpp14_m5_2_10_violation]
        score::cpp::ignore = ++iterator;
        auto value_iterator = value.begin();
        for (; counter > static_cast<decltype(counter)>(0); counter--)
        {
            visitor.Next(*value_iterator);
            // coverity[autosar_cpp14_m5_2_10_violation]: same reason as above for "score::cpp::ignore = ++iterator;"
            score::cpp::ignore = ++value_iterator;
        }
        return {};
    }

    template <typename Visitor, typename Container, typename UnderlyingType>
    void Serialize(Visitor& visitor,
                   SerializeIterator<Container>,
                   const score::cpp::pmr::vector<UnderlyingType>& value) const
    {
        for (const auto& sub_value : value)
        {
            visitor.Next(sub_value);
        }
    }

    template <typename Visitor, typename Container, typename UnderlyingType>
    Result<void> DeSerialize(Visitor& visitor,
                             DeSerializeIterator<Container>& iterator,
                             score::cpp::pmr::vector<UnderlyingType>& value) const
    {
        value.clear();  // a reused destination must hold only the decoded value, not previous contents
        const auto end = iterator.End();
        while (iterator != end)
        {
            const auto position_before = iterator;
            UnderlyingType underlying_type{};
            visitor.Next(underlying_type);

            // A latched failure turns Next into a no-op and an element consuming no input never advances;
            // either case would loop forever, so bail out before appending when the decode failed or made no progress.
            if ((not visitor.GetResult().has_value()) || (iterator == position_before))
            {
                return MakeUnexpected(SerializationError::kOutOfBoundsWrite);
            }
            value.push_back(underlying_type);
        }

        return {};
    }

    template <typename Visitor, typename Container, typename UnderlyingType, std::size_t MaxSize>
    void Serialize(Visitor& visitor,
                   SerializeIterator<Container>,
                   const std::array<UnderlyingType, MaxSize>& value) const
    {
        for (const auto& sub_value : value)
        {
            visitor.Next(sub_value);
        }
    }

    template <typename Visitor, typename Container, typename UnderlyingType, std::size_t MaxSize>
    Result<void> DeSerialize(Visitor& visitor,
                             DeSerializeIterator<Container>&,
                             std::array<UnderlyingType, MaxSize>& value) const
    {
        // Suppress "AUTOSAR C++14 M6-5-4" and "AUTOSAR C++14 M6-5-4" rule findings. The rule states: "The loop-counter
        // shall be modified by one of: , ++, = n, or + = n; where n remains constant for the duration of the loop." and
        // "If loop-counter is not modified by or ++, then, within condition, the loop-counter shall only be used as an
        // operand to <=, <, > or >=." respectively. Increment operators cannot be used due to sev1 rule "AUTOSAR C++14
        // M5-0-15".
        // coverity[autosar_cpp14_m6_5_2_violation]
        for (auto value_iterator = value.begin();  // LCOV_EXCL_BR_LINE (tooling issue SWP-191502)
             value_iterator != value.end();
             // coverity[autosar_cpp14_m6_5_4_violation]
             value_iterator = std::next(value_iterator))
        {
            visitor.Next(*value_iterator);
        }

        return {};
    }
};

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_STRATEGIES_UDS_STRATEGY_H
