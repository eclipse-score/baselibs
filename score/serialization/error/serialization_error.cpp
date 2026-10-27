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

#include "serialization_error.h"

#include "score/result/error_domain.h"

namespace score
{
namespace serialization
{

namespace
{

class SerializationErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        // Suppress "AUTOSAR C++14 A7-2-1", The rule states: "An expression with enum underlying type shall only have
        // values corresponding to the enumerators of the enumeration." Any other values do not correspond to
        // one of the enumerators of the enumeration will be handled with the default case.
        // Suppress "AUTOSAR C++14 M6-4-5" and "AUTOSAR C++14 M6-4-3", The rule states: "An unconditional throw or break
        // statement shall terminate every nonempty switch-clause." and "A switch statement shall be a well-formed
        // switch statement." respectively There is return for every switch case, adding a break would be dead code.
        // coverity[autosar_cpp14_a7_2_1_violation]
        // coverity[autosar_cpp14_m6_4_3_violation]
        switch (static_cast<SerializationError>(code))
        {
            // coverity[autosar_cpp14_m6_4_5_violation]
            case SerializationError::kOutOfBoundsWrite:
                return "Tried to write out of bounds!";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case SerializationError::kStringTooBig:
                return "Provided string exceeds maximum values!";
            // coverity[autosar_cpp14_m6_4_5_violation]
            case SerializationError::kNotAllDataDeserialized:
                return "Some data still left in input buffer after deserialization!";
            // coverity[autosar_cpp14_m6_4_5_violation]
            default:
                return "Unknown Error!";
        }
    }
};

constexpr SerializationErrorDomain serialization_error_domain;

}  // namespace

score::result::Error MakeError(const SerializationError code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), serialization_error_domain, user_message};
}

}  // namespace serialization
}  // namespace score
