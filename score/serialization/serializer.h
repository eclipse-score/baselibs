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

#ifndef SCORE_SERIALIZATION_SERIALIZER_H
#define SCORE_SERIALIZATION_SERIALIZER_H

#include "score/serialization/details/serialization_visitor.h"
#include "score/serialization/details/type_traits.h"
#include "score/serialization/details/visit.h"
#include "score/serialization/meta_information.h"
#include "score/serialization/serialize_iterator.h"

#include "score/result/result.h"

#include <cstdint>
#include <iostream>
#include <vector>

// Keep SCORE_MARK_SERIALIZATION_VISITOR_AS_FRIEND macro on a single line to suppress coverity violation.
// clang-format off
// Suppress "AUTOSAR C++14 A11-3-1", The rule states: "Friend declarations shall not be used".
// In order to avoid boilerplate code, we need to mark the visitor as friend. This way it can access
// the private members and serialize or pollute them.
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// coverity[autosar_cpp14_a11_3_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#define SCORE_MARK_SERIALIZATION_VISITOR_AS_FRIEND template <typename T> friend class struct_visitable_impl
// clang-format on

namespace score
{
namespace serialization
{

/// @brief Interface class to determine a common interface on how to perform serializations
/// @tparam SerializationStrategy The strategy that defines what serializations are possible and _how_ they are
/// performed
template <typename SerializationStrategy>
class Serializer
{
  public:
    /// @brief Construct a serializer with a specific strategy. This enables that strategies are constructed in user
    /// scope and thus come with custom construction options
    explicit Serializer(SerializationStrategy strategy) noexcept : strategy_{std::move(strategy)} {}

    /// @brief Serializes source into a buffer (destination)
    ///
    /// @details The buffer must be a sequence container of value_type std::uint8_t
    /// We serialize from the beginning of the destination to the end. If an offset is required, a score::cpp::span
    /// should be given for the actual buffer to illustrate the available view on the container.
    template <typename SequenceContainer, typename TypeToSerialize>
    Result<std::size_t> Serialize(const TypeToSerialize& source, SequenceContainer& destination) const noexcept
    {
        static_assert(details::is_supported_sequence_container<SequenceContainer>::value,
                      "Provided container is not a sequence container");
        static_assert(details::is_supported_value_type<SequenceContainer>::value,
                      "value_type of sequence container is not one of supported (uint8, int8, char)");

        SerializationVisitor<SequenceContainer, SerializationStrategy> visitor{destination, strategy_};
        visitor.Next(source);

        return visitor.GetResult();
    }

    template <typename SequenceContainer, typename TypeToSerialize>
    Result<std::size_t> DeSerialize(const SequenceContainer& source, TypeToSerialize& destination) const noexcept
    {
        DeSerializationVisitor<SequenceContainer, SerializationStrategy> visitor(source, strategy_);
        DeSerializeData(visitor, destination);
        return visitor.GetResult();
    }

    template <typename SequenceContainer, typename TypeToSerialize>
    Result<std::size_t> DeSerializeCompleteData(const SequenceContainer& source,
                                                TypeToSerialize& destination) const noexcept
    {
        DeSerializationVisitor<SequenceContainer, SerializationStrategy> visitor(source, strategy_);
        DeSerializeData(visitor, destination);
        const auto result = visitor.GetResult();

        if (!result.has_value())
        {
            return result;
        }
        if (visitor.IsSerializedDataRemaining())
        {
            return MakeUnexpected(SerializationError::kNotAllDataDeserialized);
        }
        return result;
    }

  private:
    SerializationStrategy strategy_;

    template <typename SequenceContainer, typename TypeToSerialize>
    void DeSerializeData(DeSerializationVisitor<SequenceContainer, SerializationStrategy>& visitor,
                         TypeToSerialize& destination) const noexcept
    {
        static_assert(details::is_supported_sequence_container<SequenceContainer>::value,
                      "Provided container is not a sequence container");
        static_assert(details::is_supported_value_type<SequenceContainer>::value,
                      "value_type of sequence container is not one of supported (uint8)");

        visitor.Next(destination);
    }
};

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_SERIALIZER_H
