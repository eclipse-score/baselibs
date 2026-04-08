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
/*!        \file
 *        \brief  Contains error handling related types.
 *
 *      \details  Contains Result alias, error domain & error codes.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_JSON_ERROR_DOMAIN_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_JSON_ERROR_DOMAIN_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <iostream>
#include <utility>

#include "amsr/json/util/types.h"
#include "score/result/error_code.h"
#include "score/result/error_domain.h"
#include "score/result/result.h"

namespace amsr {
namespace json {
/*!
 * \brief           Null-terminated C-string
 *
 * \vprivate        Vector component internal API
 */
using CStr = char const*;

/*!
 * \brief           Unqualified access to Result
 * \tparam          T
 *                  Type of value.
 * \vprivate        Vector component internal API
 */
template <typename T>
using Result = score::Result<T>;

using ResultBlank = score::ResultBlank;

/*!
 * \brief           Unqualified access to ErrorDomain
 *
 * \vprivate        Vector product internal API
 */
using ErrorDomain = score::result::ErrorDomain;

/*!
 * \brief           Unqualified access to ErrorCode
 *
 * \vprivate        Vector product internal API
 */
using ErrorCode = score::result::ErrorCode;

/*!
 * \brief           All error codes used by vaJson
 *
 * \vpublic
 */
enum class JsonErrc : ErrorCode {
  kNotInitialized = 1,
  kInvalidJson,
  kUserValidationFailed,
  kStreamFailure,
  kInvalidApiUsage,
};

/*!
 * \brief           An error domain for all JSON related errors
 *
 * \vpublic
 */
class JsonErrorDomain final : public ErrorDomain {
 public:
  /*!
   * \brief           Implements the Errc interface-type
   * \vpublic
   */
  using Errc = ErrorCode;

  /*!
   * \brief           Constructs a JsonErrorDomain type
   *
   * \pre             -
   * \context         ANY
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \vprivate        Vector product internal API
   *
   * \spec
   * requires true;
   * \endspec
   */
  constexpr JsonErrorDomain() noexcept : ErrorDomain() {}

  /*!
   * \brief           Converts an error code into a message
   * \param[in]       error_code
   *                  to transform.
   * \return          The transformed message.
   *
   * \pre             -
   * \context         ANY
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \vpublic
   *
   * \spec
   * requires true;
   * \endspec
   *
   * \internal
   * - If the error code is within bounds of the messages array:
   *   - Return the message from the array.
   * - Otherwise:
   *   - Return "unknown error".
   * \endinternal
   */
  auto MessageFor(const Errc& error_code) const noexcept -> std::string_view final {
    Errc errc{error_code};
    constexpr static std::array<std::string_view, 6> kMessages{{
        "Unknown error.",
        "kNotInitialized: Result was not initialized.",
        "kInvalidJson: Invalid JSON was encountered.",
        "kUserValidationFailed: The user aborted due to a validation failure.",
        "kStreamFailure: The underlying file or character stream encountered an error.",
        "kInvalidApiUsage: Invalid usage of vaJson APIs, see vaJson Technical Reference",
    }};

    if ((error_code < 1) || (error_code > 5)) {
      errc = 0;
    }

    return kMessages[static_cast<std::size_t>(errc)];
  }

};

namespace detail {
/*!
 * \brief           JsonErrorDomain singleton
 * \vprivate        Vector product internal API
 */
constexpr JsonErrorDomain kJsonErrorDomain;
}  // namespace detail


inline score::result::Error MakeError(const ErrorCode code, const std::string_view user_message = "") noexcept {
    return {static_cast<score::result::ErrorCode>(code), detail::kJsonErrorDomain, user_message};
}

inline score::result::Error MakeError(amsr::json::JsonErrc const error_code, const std::string_view user_message = "") noexcept {
  return MakeError(static_cast<ErrorCode>(error_code), user_message);
}

template<typename T>
inline score::Result<T> MakeErrorResult(ErrorCode const error_code, std::string_view const user_message = "") noexcept {
  return score::Result<T>{score::unexpect, MakeError(error_code, user_message)};
}

template<typename T>
inline score::Result<T> MakeErrorResult(JsonErrc const error_code, std::string_view const user_message = "") noexcept {
  return MakeErrorResult<T>(static_cast<ErrorCode>(error_code), user_message);
}

/*!
 * \brief           Filters a Result based on a predicate
 * \tparam          T  Value type of the Result.
 * \tparam          Pred  Predicate type.
 * \param[in,out]   result  The Result to filter.
 * \param[in]       pred    Predicate: if false, the result becomes an error.
 * \param[in]       error   Error to use if the predicate returns false.
 * \return          The filtered Result.
 */
template <typename T, typename Pred>
auto Filter(score::Result<T> result, Pred pred, score::result::Error error) noexcept -> score::Result<T> {
  if (result.has_value() && !pred(result.value())) {
    return score::Result<T>{score::unexpect, error};
  }
  return result;
}

/*!
 * \brief           Discards the value of a Result (converts to ResultBlank)
 * \tparam          T  Value type of the Result.
 * \param[in]       result  The Result to drop.
 * \return          ResultBlank with same error state.
 */
template <typename T>
auto Drop(score::Result<T> result) noexcept -> score::ResultBlank {
  if (result.has_value()) {
    return score::ResultBlank{score::Blank{}};
  }
  return score::ResultBlank{score::unexpect, result.error()};
}

/*!
 * \brief           Inspects (peeks at) a Result without transforming it
 * \tparam          T   Value type of the Result.
 * \tparam          Fn  Side-effect function type.
 * \param[in]       result  The Result to inspect.
 * \param[in]       fn      Side-effect function called with the value if present.
 * \return          The original Result unchanged.
 */
template <typename T, typename Fn>
auto Inspect(score::Result<T> result, Fn fn) noexcept -> score::Result<T> {
  if (result.has_value()) {
    fn(result.value());
  }
  return result;
}

/*!
 * \brief           Returns the second Result if the first has a value, otherwise propagates the error
 * \tparam          T  Value type of the first Result.
 * \tparam          U  Value type of the second Result.
 * \param[in]       first   The first Result.
 * \param[in]       second  The second Result.
 * \return          The second Result if first has a value, else the first error.
 */
template <typename T, typename U>
auto And(score::Result<T> first, score::Result<U> second) noexcept -> score::Result<U> {
  if (!first.has_value()) {
    return score::Result<U>{score::unexpect, first.error()};
  }
  return second;
}
/*!
 * \brief           Gets the error domain singleton
 * \return          The error domain singleton.
 * \pre             -
 * \context         ANY
 * \threadsafe      TRUE, for different this pointer
 * \vprivate        Vector component internal API
 *
 * \spec
 * requires true;
 * \endspec
 */

inline constexpr auto GetJsonDomain() noexcept -> ErrorDomain const& { return detail::kJsonErrorDomain; }

/*!
 * \brief           Creates a Result
 * \tparam          T
 *                  Type of Result.
 * \param[in]       value
 *                  to put into the Result.
 * \return          The Result.
 *
 * \pre             -
 * \context         ANY
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \vprivate        Vector component internal API
 *
 * \spec
 * requires true;
 * \endspec
 *
 * \internal
 * - Return a Result created from the given value.
 * \endinternal
 */
template <typename T>
constexpr auto Ok(T value) noexcept -> Result<T> {
  return Result<T>{value};
}

/*!
 * \brief           Creates a Result from a boolean value
 * \param[in]       value
 *                  to check.
 * \param[in]       error
 *                  Specific error code.
 * \return          The empty Result if true, or the specified error.
 *
 * \pre             -
 * \context         ANY
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \vprivate        Vector component internal API
 *
 * \spec
 * requires true;
 * \endspec
 *
 * \internal
 * - If the value is true:
 *   - Return an empty Result.
 * - Otherwise:
 *   - Return an Error created from the given arguments.
 * \endinternal
 */
inline auto MakeResult(bool value, ErrorCode error) noexcept -> score::ResultBlank {
  return value ? score::ResultBlank{} : score::MakeUnexpected<score::Blank>(MakeError(error, ""));
}

inline auto MakeResult(bool value, score::result::Error error) noexcept -> score::ResultBlank {
  return value ? score::ResultBlank{} : score::MakeUnexpected<score::Blank>(error);
}


/*!
 * \brief           Assert that a condition holds
 * \param[in]       value
 *                  to check.
 * \param[in]       message
 *                  Optional message for this error.
 *
 * \pre             -
 * \context         ANY
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \vprivate        Vector component internal API
 *
 * \spec
 * requires true;
 * \endspec
 *
 * \internal
 * - If the value is false:
 *   - Abort printing the passed message.
 * \endinternal
 */
inline void AssertCondition(bool value, CStr message = "") noexcept {
  if (!value) {
    std::cerr << message << std::endl;
    std::abort();
  }
}

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_JSON_ERROR_DOMAIN_H_
