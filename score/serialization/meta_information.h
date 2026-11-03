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

#ifndef SCORE_SERIALIZATION_META_INFORMATION_H
#define SCORE_SERIALIZATION_META_INFORMATION_H

namespace score
{
namespace serialization
{

/// @brief Sometimes during serialization (depending on the format) it is necessary to have meta information about the
/// element that is serialized. This class shall act as POD to exchange this information between the visitor and the
/// serialization algorithm
// Suppress "AUTOSAR C++14 M11-0-1" rule findings. This rule states: "Member data in non-POD class types shall
// be private.". We need these data elements to exchange this information between the visitor and the serialization
// algorithm
struct MetaInformation
{
    // coverity[autosar_cpp14_m11_0_1_violation]
    bool is_first_element_in_struct_{false};
    // coverity[autosar_cpp14_m11_0_1_violation]
    bool is_last_element_in_struct_{false};
};

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_META_INFORMATION_H
