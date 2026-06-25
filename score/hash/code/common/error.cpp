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

#include "score/hash/code/common/error.h"

namespace score
{
namespace hash
{

namespace
{

class HashErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        std::string_view errorMessage = "Unknown Error!";
        // Suppress "UNUSED C++14 A7-2-1" rule finding: "An expression with enum underlying type shall only have
        // values corresponding to the enumerators of the enumeration"
        // The value of "code" can be converted back and forth because the underlying type of
        // "ErrorCode" is "score::result::ErrorCode" (aka std::int32_t), and the enumeration value is
        // guaranteed to be in the range of "score::hash::ErrorCode"
        // coverity[autosar_cpp14_a7_2_1_violation]
        switch (static_cast<ErrorCode>(code))
        {
            case ErrorCode::kCouldNotCreateDigest:
                errorMessage = "Could not create digest algorithm";
                break;
            case ErrorCode::kCouldNotUpdateDigest:
                errorMessage = "Could not update digest";
                break;
            case ErrorCode::kCouldNotUpdateDigestFromStream:
                errorMessage = "Could not update digest from stream";
                break;
            case ErrorCode::kCouldNotFinalizeDigest:
                errorMessage = "Could not finalize digest";
                break;
            case ErrorCode::kInvalidParameters:
                errorMessage = "Invalid parameters";
                break;
            case ErrorCode::kNotFound:
                errorMessage = "Object not found";
                break;
            case ErrorCode::kStreamError:
                errorMessage = "Stream error";
                break;
            case ErrorCode::kUnknownError:
            default:
                break;
        }

        return errorMessage;
    }
};

constexpr HashErrorDomain hash_error_domain;
}  // namespace

score::result::Error MakeError(const score::hash::ErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), hash_error_domain, user_message};
}

}  // namespace hash
}  // namespace score
