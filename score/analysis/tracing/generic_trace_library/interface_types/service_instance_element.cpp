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
#include "score/analysis/tracing/generic_trace_library/interface_types/service_instance_element.h"

namespace score
{
namespace analysis
{
namespace tracing
{

bool operator==(const ServiceInstanceElement::EventId& lhs, const ServiceInstanceElement::EventId& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const ServiceInstanceElement::FieldId& lhs, const ServiceInstanceElement::FieldId& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const ServiceInstanceElement::MethodId& lhs, const ServiceInstanceElement::MethodId& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const ServiceInstanceElement& lhs, const ServiceInstanceElement& rhs)
{
    return ((((lhs.service_id == rhs.service_id) && (lhs.major_version == rhs.major_version)) &&
             ((lhs.minor_version == rhs.minor_version) && (lhs.instance_id == rhs.instance_id))) &&
            (lhs.element_id == rhs.element_id));
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
