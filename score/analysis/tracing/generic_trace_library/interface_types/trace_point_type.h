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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_TRACE_POINT_TYPE_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_TRACE_POINT_TYPE_H

#include <cstdint>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief TracePointType enumerator
enum class TracePointType : std::uint8_t
{
    kSkelEventSnd = 0,
    kSkelEventSndA = 1,
    kSkelEventSub = 2,
    kSkelFieldUpd = 3,
    kSkelFieldSetCall = 4,
    kSkelFieldSetCallResult = 5,
    kSkelFieldSub = 6,
    kSkelMethodCall = 7,
    kSkelMethodCallResultOk = 8,
    kSkelMethodCallResultError = 9,
    kSkelEventUnsub = 10,
    kSkelFieldUpdA = 11,
    kSkelFieldGetCall = 12,
    kSkelFieldGetCallResult = 13,
    kSkelFieldUnsub = 14,
    kProxyEventSub = 128,
    kProxyEventUnsub = 129,
    kProxyEventSetRechdl = 130,
    kProxyEventRechdl = 131,
    kProxyEventUnsetRechdl = 132,
    kProxyEventSetChghdl = 133,
    kProxyEventChghdl = 134,
    kProxyEventGetSamples = 135,
    kProxyEventSampleCb = 136,
    kProxyFieldSub = 137,
    kProxyFieldUnsub = 138,
    kProxyFieldSetRechdl = 139,
    kProxyFieldRechdl = 140,
    kProxyFieldUnsetRechdl = 141,
    kProxyFieldSetChghdl = 142,
    kProxyFieldChghdl = 143,
    kProxyFieldGetSamples = 144,
    kProxyFieldSampleCb = 145,
    kProxyFieldSet = 146,
    kProxyFieldSetResult = 147,
    kProxyFieldGet = 148,
    kProxyFieldGetResult = 149,
    kProxyMethodCall = 150,
    kProxyMethodCallResultOk = 151,
    kProxyMethodCallResultError = 152,
    kProxyEventSubstateChange = 153,
    kProxyEventUnsetChghdl = 154,
    kProxyFieldSubstateChange = 155,
    kProxyFieldUnsetChghdl = 156,
    kTracePointUndefined = 255
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_TRACE_POINT_TYPE_H
