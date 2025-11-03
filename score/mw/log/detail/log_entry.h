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
#ifndef SCORE_MW_LOG_DETAIL_LOG_ENTRY_H
#define SCORE_MW_LOG_DETAIL_LOG_ENTRY_H

#include "score/mw/log/detail/logging_identifier.h"
#include "score/mw/log/log_level.h"
#include "visitor/visit_as_struct.h"

#include <vector>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

using Byte = char;
using ByteVector = std::vector<Byte>;
/*
Deviation from Rule M11-0-1:
- Member data in non-POD class types shall be private.
Justification:
- Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
  particular struct. The Type is simple and does not require invariance (interface OR custom behavior) as per the
  design. Moreover the type is used with STRUCT_TRACEABLE for serialization purpose
*/
// coverity[autosar_cpp14_m11_0_1_violation]
struct LogEntry
{
    // coverity[autosar_cpp14_m11_0_1_violation]
    LoggingIdentifier app_id{""};
    // coverity[autosar_cpp14_m11_0_1_violation]
    LoggingIdentifier ctx_id{""};
    // coverity[autosar_cpp14_m11_0_1_violation]
    ByteVector payload{};
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint64_t timestamp_steady_nsec{};
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint64_t timestamp_system_nsec{};
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint8_t num_of_args{};
    // coverity[autosar_cpp14_m11_0_1_violation]
    ByteVector header_buffer{};
    // coverity[autosar_cpp14_m11_0_1_violation]
    LogLevel log_level{};
/*
The pre-processor shall only be used for unconditional and conditional file inclusion and include guards.
ifdef not used for file inclusion.
*/
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined __QNX__
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint16_t slog2_code{static_cast<std::uint16_t>(0)};
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
};

constexpr std::uint8_t GetLogLevelU8FromLogEntry(const LogEntry& entry)
{
    return static_cast<std::uint8_t>(entry.log_level);
}

// NOLINTBEGIN(score-struct-usage-compliance) justified by design
// Forward declaration for struct_visitable_impl is required for implementation
// std::forward<T>(s) added due to CB-#10171555
/*
Deviation from Rule A0-1-1:
- A project shall not contain instances of non-volatile variables being given values that are not subsequently used.

Justification:
- we need to use STRUCT_VISITABLE here and create an instance of LogEntry
- to get the size of LogEntry for serialization purpose
*/
// coverity[autosar_cpp14_a18_9_4_violation]
// coverity[autosar_cpp14_m3_2_3_violation]
// coverity[autosar_cpp14_a11_0_2_violation]
// coverity[autosar_cpp14_a0_1_1_violation]
// coverity[autosar_cpp14_a2_10_4_violation]
STRUCT_VISITABLE(LogEntry,
                 app_id,
                 ctx_id,
                 payload,
                 //  timestamp_steady_nsec,
                 //  timestamp_system_nsec,
                 num_of_args,
                 //  header_buffer,
                 log_level)
// NOLINTEND(score-struct-usage-compliance) justified by design

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_LOG_ENTRY_H
