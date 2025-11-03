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
#ifndef SCORE_MW_LOG_DETAIL_LOG_ENTRY_DESERIALIZE_H
#define SCORE_MW_LOG_DETAIL_LOG_ENTRY_DESERIALIZE_H

#include "score/mw/log/detail/logging_identifier.h"
#include "score/mw/log/log_level.h"
#include "visitor/visit_as_struct.h"

#include <score/span.hpp>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace log_entry_deserialization
{

//  SerializedVectorData is meant to be used as wrapper type to guide template overload resolution
struct SerializedVectorData
{
    // coverity[autosar_cpp14_m11_0_1_violation]
    score::cpp::span<const uint8_t> data;
};

/*
Deviation from clang-tidy:
- warning: 'struct' type shall not provide any special member functions or methods;
  use 'class' type instead [score-struct-usage-compliance]

Justification:
- Only trivial getter (GetPayload) for serialized_vector_data.data to simplify struct usage
*/
// NOLINTBEGIN(score-struct-usage-compliance) See above
//  The type closely resemble score::mw::log::detail::LogEntry for all the member fields that are going to be deserialize
//  with the difference in payload which replaces std::vector with score::cpp::span wrapped in custom structure i.e.
//  SerializedVectorData to allow template function overload to overwrite default deserialization behaviour
/*
Deviation from Rule A11-0-2:
- A type defined as struct shall: (1) provide only public data members, (2)
  not provide any special member functions or methods, (3) not be a base of
  another struct or class, (4) not inherit from another struct or class.
Deviation from Rule M11-0-1:
- Member data in non-POD class types shall be private.

Justification:
- Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
  particular struct. The Type is simple and does not require invariance (interface OR custom behavior) as per the
  design. Moreover the type is used with STRUCT_TRACEABLE for serialization purpose
*/
// coverity[autosar_cpp14_a11_0_2_violation]
struct LogEntryDeserializationReflection
{
    // coverity[autosar_cpp14_m11_0_1_violation]
    score::mw::log::detail::LoggingIdentifier app_id{""};
    // coverity[autosar_cpp14_m11_0_1_violation]
    score::mw::log::detail::LoggingIdentifier ctx_id{""};
    // coverity[autosar_cpp14_m11_0_1_violation]
    SerializedVectorData serialized_vector_data;
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint8_t num_of_args{0U};
    // coverity[autosar_cpp14_m11_0_1_violation]
    score::mw::log::LogLevel log_level{score::mw::log::LogLevel::kOff};

    score::cpp::span<const std::uint8_t> GetPayload() const noexcept
    {
        return serialized_vector_data.data;
    }
};
// NOLINTEND(score-struct-usage-compliance) See above

// LCOV_EXCL_START
// STRUCT_TRACEABLE macro tested in platform/aas/lib/serialization/serializer_test.cpp
// Not required to test this macro with another structure/data
/*
Deviation from Rule A0-1-1:
- A project shall not contain instances of non-volatile variables being given values that are not subsequently used.

Justification:
- we need to use STRUCT_TRACEABLE here and create an instance of LogEntryDeserializationReflection
- to get the size of LogEntryDeserializationReflection for serialization purpose
*/
// coverity[autosar_cpp14_a11_0_2_violation]
// coverity[autosar_cpp14_a0_1_1_violation]
// coverity[autosar_cpp14_a18_9_4_violation]
STRUCT_TRACEABLE(LogEntryDeserializationReflection, app_id, ctx_id, serialized_vector_data, num_of_args, log_level)
// LCOV_EXCL_STOP

}  // namespace log_entry_deserialization
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
#endif  // SCORE_MW_LOG_DETAIL_LOG_ENTRY_DESERIALIZE_H
