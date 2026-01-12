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
#ifndef SCORE_LIB_RESULT_ERROR_H
#define SCORE_LIB_RESULT_ERROR_H

#include "score/result/error_code.h"
#include "score/result/error_domain.h"

#include <ostream>
#include <string_view>

namespace score
{
namespace result
{

namespace details
{
// Below predicate is required as a workaround to avoid undefined behavior in C++ prior 20 where std::underlying_type<T>
// will result in UB if the given type `T` is not an enum. Therefore, we first check whether the given type `Code` is
// indeed an enum type before invoking std::underlying_type.
template <typename Code>
// Function is neither in an anonymous namespace, nor is it a static function with internal linkage,
// nor is it a private member function. Hence, the rule does not apply.
// coverity[autosar_cpp14_a0_1_3_violation]
constexpr bool IsValidErrorCodeEnum() noexcept
{
    // false-positive; this is the correct syntax for a constexpr-if-statement. Reordering is not allowed by standard.
    // coverity[autosar_cpp14_a7_1_8_violation]
    // coverity[autosar_cpp14_m6_4_1_violation] false-positive: "if constexpr" is a valid statement since C++17.
    if constexpr (std::is_enum_v<Code>)
    {
        return std::is_same_v<ErrorCode, std::underlying_type_t<Code>>;
    }
    // coverity[autosar_cpp14_m6_4_1_violation] false-positive: "if constexpr" is a valid statement since C++17.
    else
    {
        return false;
    }
}
}  // namespace details

/// \brief Goal of this class is to provide a common error class that provides a customization endpoint via a mixin to
/// provide easy user and domain specific usage. The main idea is that this is returned as score::cpp::expected unexpected
/// type.
class Error final
{
  public:
    /// \brief Constructs an Error based on user defined error code.
    ///
    /// \details Be aware that this function will only work, once a MakeError(Code, std::string_view) function is
    /// provided. The compiler will use ADL to figure out the correct function. The custom MakeError() function then
    /// needs to construct the Error class using the also the user-defined ErrorDomain. For an example please see
    /// related unit-tests. \param code The error code this error shall represent. It must be an enum class that is of
    /// type ErrorCode. \param user_message The user-message (optional) that shall give additional information
    template <typename Code, std::enable_if_t<details::IsValidErrorCodeEnum<Code>(), bool> = true>
    // Constructor intentionally provides implicit conversion from Code type to take advantage of ADL of MakeError().
    // Intentional delegation to copy constructor by MakeError() design and to comply with AUTOSAR A12-1-5.
    // NOLINTNEXTLINE(google-explicit-constructor,cppcoreguidelines-pro-type-member-init): Intended, see lines above
    constexpr Error(Code code, std::string_view user_message = "") : Error(MakeError(code, user_message))
    {
    }

    /// \brief Constructs an Error
    /// \param code The error code this error shall represent
    /// \param domain The error domain the respective error code is used
    /// \param user_message The user-message (optional) that shall give additional information
    constexpr Error(const ErrorCode code, const ErrorDomain& domain, const std::string_view user_message)
        : code_{code}, domain_{&domain}, user_messages_{user_message}
    {
    }

    /// \brief Dereferences the Error class to its underlying code (e.g. usage within switch-statement)
    /// \return The underlying error code as integral type
    score::result::ErrorCode operator*() const noexcept
    {
        return code_;
    }

    /// \brief Translates the error code into a human readable message
    ///
    /// \details This function will call the underlying error domain to translate the error code
    /// \return The human readable representation of the error code
    std::string_view Message() const noexcept
    {
        return domain_->MessageFor(code_);
    }

    /// \brief Exposes the provided user-message on request
    /// \return empty string if no message is provided on construction, the user-message otherwise
    std::string_view UserMessage() const noexcept
    {
        return user_messages_;
    }

  private:
    score::result::ErrorCode code_;
    const score::result::ErrorDomain* domain_;
    std::string_view user_messages_;

    friend bool operator==(const score::result::Error&, const score::result::Error&) noexcept;
    friend bool operator!=(const score::result::Error&, const score::result::Error&) noexcept;
};

/// \brief Compares two Errors for being equal. Be aware that you can also compare the underlying code type using this
/// function
bool operator==(const score::result::Error& lhs, const score::result::Error& rhs) noexcept;

/// \brief Compares two Errors for being unequal. Be aware that you can also compare the underlying code type using this
/// function
bool operator!=(const score::result::Error& lhs, const score::result::Error& rhs) noexcept;

/// \brief Global overload of a stream operator to enable translation into any std::ostream converts error
/// to human readable representation)
std::ostream& operator<<(std::ostream& out, const score::result::Error& value) noexcept;

}  // namespace result
}  // namespace score

#endif  // SCORE_LIB_RESULT_ERROR_H
