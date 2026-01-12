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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_ARA_COM_PROPERTIES_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_ARA_COM_PROPERTIES_H

#include "service_instance_element.h"
#include "trace_point_type.h"
#include <score/optional.hpp>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief AraComProperties class
///
/// Class used to store properties specifed to AraCom protocol
class AraComProperties
{
  public:
    using TracePointIdentification = std::pair<TracePointType, ServiceInstanceElement>;
    using TracePointDataId = std::uint32_t;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    TracePointIdentification trace_point_id;
    // No harm from not declaring private members here
    // coverity[autosar_cpp14_m11_0_1_violation]
    score::cpp::optional<TracePointDataId> trace_point_data_id;

    AraComProperties(const TracePointType trace_point_type,
                     const ServiceInstanceElement service_instance_element,
                     score::cpp::optional<TracePointDataId> opt_trace_point_data_id);
};
bool operator==(const AraComProperties& lhs, const AraComProperties& rhs) noexcept;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_ARA_COM_PROPERTIES_H
