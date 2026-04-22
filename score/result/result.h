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
#ifndef SCORE_LIB_RESULT_RESULT_H
#define SCORE_LIB_RESULT_RESULT_H

#include "score/result/error.h"

#include "score/result/details/expected/expected.h"
#include "score/result/details/expected/extensions.h"

#include <score/expected.hpp>

#include <optional>
#include <string_view>
#include <type_traits>

namespace score
{

template <typename T>
// Suppress "AUTOSAR C++14 A5-1-7" rule finding. This rule states: "A lambda shall not be an operand to decltype or
// typeid". False-positive, at this point "decltype" is not used with lambda.
// coverity[autosar_cpp14_a5_1_7_violation : FALSE]
using Result = details::expected<T, score::result::Error>;

// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used". False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
inline constexpr details::unexpect_t unexpect{};

using Unexpected = details::unexpected<score::result::Error>;

/// \brief Blank and ResultBlank can be used interchangeably with void and Result<void>.
using Blank = void;
using ResultBlank = Result<Blank>;

template <typename Code>
// Overload of MakeUnexpected; the overloads have different signatures
// and their first parameters are of different type
// coverity[autosar_cpp14_a2_10_4_violation]
auto MakeUnexpected(Code code, const std::string_view user_message = "") noexcept -> Unexpected
{
    return Unexpected{MakeError(code, user_message)};
}

template <typename T>
auto MakeUnexpected(score::result::Error error) noexcept -> Result<T>
{
    return Result<T>{details::unexpect, error};
}

template <typename T, typename F>
auto ResultToOptionalOrElse(const Result<T>& result, F&& error_handling) -> std::optional<T>
{
    return details::expected_value_to_optional_or_else(result, std::forward<F>(error_handling));
}

template <typename T, typename F>
// coverity[autosar_cpp14_a13_3_1_violation]
auto ResultToOptionalOrElse(Result<T>&& result, F&& error_handling) -> std::optional<T>
{
    return details::expected_value_to_optional_or_else(std::forward<Result<T>>(result),
                                                       std::forward<F>(error_handling));
}

template <typename T>
struct IsResult : public std::false_type
{
};

template <typename T>
struct IsResult<Result<T>> : public std::true_type
{
};

template <typename T>
constexpr bool IsResultV = IsResult<T>::value;

}  // namespace score

#endif  // SCORE_LIB_RESULT_RESULT_H
