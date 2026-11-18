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

#include "score/json/error.h"

#include <score/utility.hpp>

std::string_view score::json::ErrorDomain::MessageFor(const score::result::ErrorCode& code) const noexcept
{

    using ResultErrorCode = std::decay_t<decltype(code)>;
    static_assert(std::is_same_v<ResultErrorCode, std::underlying_type_t<score::json::Error>>);

    std::string_view result;
    switch (code)
    {
        case score::cpp::to_underlying(Error::kWrongType):
            result = "You tried to cast a Any JSON value into a type that it cannot be represented in!";
            break;
        case score::cpp::to_underlying(Error::kKeyNotFound):
            result = "Your requested key was not found.";
            break;
        case score::cpp::to_underlying(Error::kParsingError):
            result = "An error occurred during parsing.";
            break;
        case score::cpp::to_underlying(Error::kInvalidFilePath):
            result = "The JSON file path is incorrect.";
            break;
        case score::cpp::to_underlying(Error::kUnknownError):
            result = "Unknown Error";
            break;
        default:
            result = "Unknown Error";
            break;
    }
    return result;
}

namespace
{
constexpr score::json::ErrorDomain json_error_domain;
}

score::result::Error score::json::MakeError(const score::json::Error code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), json_error_domain, user_message};
}
