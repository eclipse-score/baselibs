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

#ifndef SCORE_SERIALIZATION_DETAILS_VISIT_H
#define SCORE_SERIALIZATION_DETAILS_VISIT_H

#include "score/serialization/details/serialization_visitor.h"

#include "static_reflection_with_serialization/visitor/visit.h"
#include "static_reflection_with_serialization/visitor/visit_as_struct.h"

#include <score/type_traits.hpp>

#include <type_traits>
#include <utility>

namespace score
{
namespace serialization
{

namespace helper
{

/// @brief Helper function to correctly set meta-information about elements in structure
// Suppress "AUTOSAR C++14 M3-2-2", The rule states: "The One Definition Rule shall not be violated".
// MarkMetaInformationAboutElement function is templated to handle different types of visitors (serialization and
// deserialization). This avoids code duplication and improves performance without runtime overhead.
template <typename Structure>
// coverity[autosar_cpp14_m3_2_2_violation]
void MarkMetaInformationAboutElement(CommonVisitorLogic& visitor, std::size_t counter)
{
    using visitable = ::score::common::visitor::struct_visitable<Structure>;
    if (counter == static_cast<std::size_t>(0U))
    {
        visitor.MarkAsFirstElementInStruct();
    }
    else if (counter == visitable::fields - static_cast<size_t>(1U))
    {
        // Suppress "AUTOSAR C++14 M0-1-2" rule violations. The rule states "A project shall not contain infeasible
        // paths.". This is false positive, it is required case to be executed and is not dead code.
        // coverity[autosar_cpp14_m0_1_2_violation : FALSE]
        visitor.MarkAsLastElementInStruct();
    }
    else
    {
        visitor.MarkAsElementInStruct();
    }
}

}  // namespace helper

/// @brief This file implements the necessary visitor functions according to the API specification of
/// static_reflection_with_serialization/visitor. We implement these overloads which are found via ADL for the
/// `SerializationVisitor`

/// @brief Main visit function for all types besides structs
///
/// @details This function only participates in overload resolution if the underlying `Strategy` implements a
/// `Serialize` method for this type. If this is not the case, then this function will not be available and compilation
/// will fail with a message that the specific type was not marked as visitable.
///
/// @tparam Container The container into which the data should be serialized
/// @tparam Strategy The serialization strategy to apply (how to serialize)
/// @tparam TypeToSerialize The type that should be serialized into bytes
/// @param visitor The visitor that we provide and holds context information
/// @param value The values of the type we want to serialize
/// @return void
template <typename Container, typename Strategy, typename TypeToSerialize>
inline auto visit_as(SerializationVisitor<Container, Strategy>& visitor, TypeToSerialize&& value)
    -> std::enable_if_t<std::is_void<decltype(visitor.Serialize(std::forward<TypeToSerialize>(value)))>::value, void>
{
    visitor.Serialize(std::forward<TypeToSerialize>(value));
}

/// @brief Main visit function for all structs that are marked as SCORE_STRUCT_VISITABLE()
///
/// @tparam Container The container into which the data should be serialized
/// @tparam Strategy The serialization strategy to apply (how to serialize)
/// @tparam Structure The type that should be serialized into bytes
/// @tparam Args The types for each field of the struct
/// @param visitor The visitor that we provide and holds context information
/// @param fields The actual values of the fields of the struct
template <typename Container, typename Strategy, typename Structure, typename... Args>
inline void visit_as_struct(SerializationVisitor<Container, Strategy>& visitor, Structure&&, Args&&... fields)
{

    // pre-C++17 parameter pack folding idiom
    // std::size_t[] is used as dummy array type to swallow initializer_list. There is no unsafe usage of
    // C-array like decay to pointer, loss of size or other problem mentioned in AUTOSAR Rule A18-1-1.
    // NOLINTNEXTLINE(modernize-avoid-c-arrays): Dummy array type not used as array at all, ergo safe
    using expander = std::size_t[];
    std::size_t counter{};
    // Suppress "AUTOSAR C++14 M5-2-10", The rule states: "The increment (++) and decrement (--) operators shall not be
    // mixed with other operators in an expression." The rule against increment and decriment operators mixed with
    // others on the same line seems to be more applicable to arithmetic operators. This rule is absent from the new
    // MISRA 2023 standard, thus it is in general reasonable to deviate from it.
    // Suppress "AUTOSAR C++14 A6-2-2", The rule states: "Expression statements shall not be explicit calls to
    // constructors of temporary objects only.". This is False positive. The temporary object in this expression is
    // intentionally used to execute side effects (MarkMetaInformationAboutElement, visit, and counter increment) during
    // fold expression iteration over a parameter pack.
    // coverity[autosar_cpp14_a6_2_2_violation]
    // coverity[autosar_cpp14_m5_2_10_violation]
    score::cpp::ignore = (expander{(helper::MarkMetaInformationAboutElement<Structure>(visitor, counter),
                                    ::score::common::visitor::visit(visitor, std::forward<Args>(fields)),
                                    // Suppress "AUTOSAR C++14 M0-1-9" rule violations. The rule states "There shall be
                                    // no dead code.". This is expander expression and is not dead code.
                                    // coverity[autosar_cpp14_m0_1_9_violation : FALSE]
                                    ++counter)...});

    visitor.MarkAsElementInStruct();  // neither first / nor last
}

template <typename Container, typename Strategy, typename TypeToSerialize>
// Suppress "AUTOSAR C++14 A13-3-1" rule violations. The rule states "A function that contains "forwarding reference" as
// its argument shall not be overloaded.". This method and its type of 1st arg `visitor` is different in the overloads
// of Serialization and Deserialization and hence no issues are caused.
// coverity[autosar_cpp14_a13_3_1_violation]
inline auto visit_as(DeSerializationVisitor<Container, Strategy>& visitor, TypeToSerialize&& value)
    -> std::enable_if_t<std::is_void<decltype(visitor.DeSerialize(std::forward<TypeToSerialize>(value)))>::value, void>
{
    visitor.DeSerialize(std::forward<TypeToSerialize>(value));
}

template <typename Container, typename Strategy, typename Structure, typename... Args>
// coverity[autosar_cpp14_a13_3_1_violation]
inline void visit_as_struct(DeSerializationVisitor<Container, Strategy>& visitor, Structure&&, Args&&... fields)
{

    // pre-C++17 parameter pack folding idiom
    // std::size_t[] is used as dummy array type to swallow initializer_list. There is no unsafe usage of
    // C-array like decay to pointer, loss of size or other problem mentioned in AUTOSAR Rule A18-1-1.
    // NOLINTNEXTLINE(modernize-avoid-c-arrays): Dummy array type not used as array at all, ergo safe
    using expander = std::size_t[];
    std::size_t counter{};
    // coverity[autosar_cpp14_a6_2_2_violation]
    // coverity[autosar_cpp14_m5_2_10_violation]
    // Route each field through Next() so the visitor's latched-error guard stops traversal after the first failure.
    score::cpp::ignore = (expander{(helper::MarkMetaInformationAboutElement<Structure>(visitor, counter),
                                    visitor.Next(std::forward<Args>(fields)),
                                    // coverity[autosar_cpp14_m0_1_9_violation : FALSE]
                                    ++counter)...});

    visitor.MarkAsElementInStruct();  // neither first / nor last
}

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_DETAILS_VISIT_H
