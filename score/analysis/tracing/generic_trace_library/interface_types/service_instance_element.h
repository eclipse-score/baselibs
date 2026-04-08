/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_SERVICE_INSTANCE_ELEMENT_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_SERVICE_INSTANCE_ELEMENT_H

#include <score/utility.hpp>
#include <score/variant.hpp>

#include <cstdint>
#include <variant>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief ServiceInstanceElement class
///
/// Class used to identify service instance element
class ServiceInstanceElement
{
  public:
    using ServiceIdType = std::uint32_t;   ///< Type used to store Service Id
    using InstanceIdType = std::uint32_t;  ///< Type used to store Instance Id
    using EventIdType = std::uint32_t;     ///< Type used to store Event Id
    using FieldIdType = std::uint32_t;     ///< Type used to store Field Id
    using MethodIdType = std::uint32_t;    ///< Type used to store Method Id
    using VariantType = score::cpp::variant<EventIdType, FieldIdType, MethodIdType>;

    /// @brief Variant index constants for element_id (needed since all types are uint32_t)
    static constexpr std::size_t kEventIdIndex = 0U;   ///< Index for EventIdType in VariantType
    static constexpr std::size_t kFieldIdIndex = 1U;   ///< Index for FieldIdType in VariantType
    static constexpr std::size_t kMethodIdIndex = 2U;  ///< Index for MethodIdType in VariantType

    /// @brief Type used to store Event Id (distinct struct for type-safe variant)
    struct EventId
    {
        std::uint32_t value;
    };
    /// @brief Type used to store Field Id (distinct struct for type-safe variant)
    struct FieldId
    {
        std::uint32_t value;
    };
    /// @brief Type used to store Method Id (distinct struct for type-safe variant)
    struct MethodId
    {
        std::uint32_t value;
    };

    using StdVariantType = std::variant<EventId, FieldId, MethodId>;

    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    ServiceIdType service_id;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::uint32_t major_version;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::uint32_t minor_version;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    InstanceIdType instance_id;
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    VariantType element_id;
};

/// @brief Comparison operator for ServiceInstanceElement
bool operator==(const ServiceInstanceElement& lhs, const ServiceInstanceElement& rhs);

/// @brief Comparison operator for EventId
bool operator==(const ServiceInstanceElement::EventId& lhs, const ServiceInstanceElement::EventId& rhs);

/// @brief Comparison operator for FieldId
bool operator==(const ServiceInstanceElement::FieldId& lhs, const ServiceInstanceElement::FieldId& rhs);

/// @brief Comparison operator for MethodId
bool operator==(const ServiceInstanceElement::MethodId& lhs, const ServiceInstanceElement::MethodId& rhs);

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_SERVICE_INSTANCE_ELEMENT_H
