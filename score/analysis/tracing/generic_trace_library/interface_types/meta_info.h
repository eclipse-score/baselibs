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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_META_INFO_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_META_INFO_H

#include "ara_com_properties.h"
#include <score/optional.hpp>
#include <bitset>
#include <cstdint>
#include <type_traits>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MetaInfoBase class
///
/// Base class used to store meta info data
// No harm to declare the class in that format
//  coverity[autosar_cpp14_a12_8_6_violation]
class MetaInfoBase
{
  public:
    using StatusBits = std::bitset<1>;  ///< Type used to store trace status bits
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    StatusBits trace_status{0U};  ///< Trace status bits field. Used to store i.e. information about the data loss
    // Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
    // Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
    // coverity[autosar_cpp14_a15_5_3_violation]
    void SetDataLossBit() noexcept
    {
        // No harm from ignorning the returned value here
        //  coverity[autosar_cpp14_a0_1_2_violation]
        trace_status.set(0U);
    }
};

/// @brief MetaInfo class template
///
/// MetaInfo class is used to store meta info of the data that need to be traced
///
/// @param T Type of properties that should be stored: AraComMetaInfo or DltMetaInfo
template <typename Properties>
class MetaInfo : public MetaInfoBase
{
  public:
    explicit MetaInfo(Properties p) noexcept : MetaInfoBase(), properties(p) {}
    // No harm to declare the members as public
    //  coverity[autosar_cpp14_m11_0_1_violation]
    Properties properties;  ///< Properties of the meta data specific to the protocol that needs to be traced
};

template <typename Properties>
bool operator==(const MetaInfo<Properties>& lhs, const MetaInfo<Properties>& rhs) noexcept
{
    return lhs.properties == rhs.properties;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_META_INFO_H
