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
/// @file
/// @brief  Contains error handling related types.
///
/// @details  Contains the error domain and error codes.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_ERROR_DOMAIN_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_ERROR_DOMAIN_H

#include <string_view>

#include "score/result/error_code.h"
#include "score/result/error_domain.h"
#include "score/result/result.h"
#include "score/utility.hpp"

namespace score
{
namespace json
{
namespace vajson
{
/// @brief           Unqualified access to ErrorDomain
using ErrorDomain = score::result::ErrorDomain;

/// @brief           Unqualified access to ErrorCode
using ErrorCode = score::result::ErrorCode;

/// @brief           All error codes used by vaJson
enum class JsonErrc : ErrorCode
{
    kInvalidJson = 1,
    kStreamFailure,
};

/// @brief           An error domain for all JSON related errors
class JsonErrorDomain final : public ErrorDomain
{
  public:
    /// @brief           Implements the Errc interface-type
    using Errc = ErrorCode;

    /// @brief           Constructs a JsonErrorDomain type
    /// @pre             -
    /// @context         ANY
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    constexpr JsonErrorDomain() noexcept : ErrorDomain() {}

    /// @brief           Converts an error code into a message
    /// @param[in]       error_code
    ///                  to transform.
    /// @return          The transformed message.
    /// @pre             -
    /// @context         ANY
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @internal
    /// - Return the message of the matching error code, or "Unknown error." for any other code.
    /// @endinternal
    auto MessageFor(const Errc& error_code) const noexcept -> std::string_view override final
    {
        std::string_view message{"Unknown error."};
        switch (error_code)
        {
            case score::cpp::to_underlying(JsonErrc::kInvalidJson):
                message = "kInvalidJson: Invalid JSON was encountered.";
                break;
            case score::cpp::to_underlying(JsonErrc::kStreamFailure):
                message = "kStreamFailure: The underlying file or character stream encountered an error.";
                break;
            default:
                // Any other code is unknown to this domain.
                break;
        }
        return message;
    }
};

namespace detail
{
/// @brief           JsonErrorDomain singleton
constexpr JsonErrorDomain kJsonErrorDomain;
}  // namespace detail

/// @brief           Creates an error of the vaJson error domain
/// @param[in]       error_code
///                  The vaJson error code.
/// @param[in]       user_message
///                  Additional context for the error. Must outlive the returned error.
/// @return          The error.
inline score::result::Error MakeError(const JsonErrc error_code, const std::string_view user_message = "") noexcept
{
    return {static_cast<ErrorCode>(error_code), detail::kJsonErrorDomain, user_message};
}

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_ERROR_DOMAIN_H
