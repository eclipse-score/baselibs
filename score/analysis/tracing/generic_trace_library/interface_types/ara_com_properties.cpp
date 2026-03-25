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
#include "ara_com_properties.h"

namespace score
{
namespace analysis
{
namespace tracing
{
AraComProperties::AraComProperties(const TracePointType trace_point_type,
                                   const ServiceInstanceElement service_instance_element,
                                   std::optional<TracePointDataId> opt_trace_point_data_id)
    : trace_point_id{trace_point_type, service_instance_element}, trace_point_data_id{opt_trace_point_data_id}
{
}

bool operator==(const AraComProperties& lhs, const AraComProperties& rhs) noexcept
{
    return (lhs.trace_point_id == rhs.trace_point_id) && (lhs.trace_point_data_id == rhs.trace_point_data_id);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
