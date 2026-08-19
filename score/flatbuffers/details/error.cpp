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

#include "score/flatbuffers/error.hpp"

namespace score
{
namespace flatbuffers
{

namespace
{

class FlatbuffersErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (code)
        {
            case static_cast<score::result::ErrorCode>(ErrorCode::kNullDataPointer):
                return "Data pointer or derived root table pointer is null";
            case static_cast<score::result::ErrorCode>(ErrorCode::kVerificationFailed):
                return "Structural buffer verification failed";
            case static_cast<score::result::ErrorCode>(ErrorCode::kVersionInfoNotPresent):
                return "No version info available in buffer";
            case static_cast<score::result::ErrorCode>(ErrorCode::kVersionMismatch):
                return "Buffer version does not match expected version";
            default:
                return "Unknown FlatBuffers error";
        }
    }
};

constexpr FlatbuffersErrorDomain flatbuffers_error_domain;

}  // namespace

score::result::Error MakeError(const ErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), flatbuffers_error_domain, user_message};
}

}  // namespace flatbuffers
}  // namespace score
