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

    // No harm from defining the == operator as member function
    // coverity[autosar_cpp14_a13_5_5_violation]
    bool operator==(const ServiceInstanceElement& other) const
    {
        return ((((service_id == other.service_id) && (major_version == other.major_version)) &&
                 ((minor_version == other.minor_version) && (instance_id == other.instance_id))) &&
                (element_id == other.element_id));
    }
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_SERVICE_INSTANCE_ELEMENT_H
