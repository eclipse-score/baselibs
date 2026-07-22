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

#include "score/result/rust/result_example_cpp.h"

#include <charconv>

namespace
{
enum class ConversionErrorCode : score::result::ErrorCode
{
    kConversionFailed = 17,  // Use a random number different from 0 to distinguish from special cases like nullptr
};

class ConversionErrorDomain final : public score::result::ErrorDomain
{
  public:
    [[nodiscard]] std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (static_cast<ConversionErrorCode>(code))
        {
            case ConversionErrorCode::kConversionFailed:
                return "Conversion Failed";
            default:
                return "Unknown Error";
        }
    }
};

constexpr ConversionErrorDomain conversion_error_domain;

score::result::Error MakeError(ConversionErrorCode code, std::string_view user_message = {}) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), conversion_error_domain, user_message};
}
}  // namespace

score::Result<std::int32_t> ExecuteCppFunction(rust::Str input)
{
    std::int32_t result;
    if (const auto ec = std::from_chars(input.data(), input.data() + input.size(), result).ec; ec == std::errc{})
    {
        return result;
    }
    else
    {
        return score::MakeUnexpected(ConversionErrorCode::kConversionFailed, "Dummy message");
    }
}

UniquePtrResult CreateUniquePtrResult()
{
    return std::make_unique<OpaqueInt>(OpaqueInt{15});
}

SharedPtrResult CreateSharedPtrResult()
{
    return std::make_shared<OpaqueInt>(OpaqueInt{55});
}
