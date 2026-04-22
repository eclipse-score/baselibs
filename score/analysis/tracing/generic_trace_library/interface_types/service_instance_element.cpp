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

ServiceInstanceElement::ServiceInstanceElement(ServiceIdType input_service_id,
                                               std::uint32_t input_major_version,
                                               std::uint32_t input_minor_version,
                                               InstanceIdType input_instance_id,
                                               VariantType input_element_id)
    : service_id{input_service_id},
      major_version{input_major_version},
      minor_version{input_minor_version},
      instance_id{input_instance_id},
      element_id{input_element_id}
{
}

ServiceInstanceElement::ServiceInstanceElement(ServiceIdType input_service_id,
                                               std::uint32_t input_major_version,
                                               std::uint32_t input_minor_version,
                                               InstanceIdType input_instance_id,
                                               StdVariantType input_element_id)
    : service_id{input_service_id},
      major_version{input_major_version},
      minor_version{input_minor_version},
      instance_id{input_instance_id}
{
    if (std::holds_alternative<EventId>(input_element_id))
    {
        element_id = VariantType{score::cpp::in_place_type<EventIdType>, std::get<EventId>(input_element_id).value};
    }
    else if (std::holds_alternative<FieldId>(input_element_id))
    {
        element_id = VariantType{score::cpp::in_place_type<FieldIdType>, std::get<FieldId>(input_element_id).value};
    }
    else
    {
        element_id = VariantType{score::cpp::in_place_type<MethodIdType>, std::get<MethodId>(input_element_id).value};
    }
}

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
