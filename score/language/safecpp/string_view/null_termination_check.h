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
#ifndef SCORE_LANGUAGE_SAFECPP_STRING_VIEW_NULL_TERMINATION_CHECK_H
#define SCORE_LANGUAGE_SAFECPP_STRING_VIEW_NULL_TERMINATION_CHECK_H

#include "score/language/safecpp/string_view/null_termination_violation_policies.h"

#include "score/language/safecpp/string_view/details/zspan.h"
#include "score/language/safecpp/string_view/zstring_view.h"

#include <functional>
#include <type_traits>
#include <utility>

namespace score::safecpp
{

namespace details
{

template <typename T, typename = void>
class is_string_view : public std::false_type
{
};

template <typename T>
class is_string_view<T,
                     std::void_t<decltype(std::declval<T>().back()),
                                 decltype(std::declval<T>().data()),
                                 decltype(std::declval<T>().front()),
                                 decltype(std::declval<T>().size()),
                                 decltype(std::declval<T>().remove_prefix(std::size_t{})),
                                 decltype(std::declval<T>().remove_suffix(std::size_t{}))>> : public std::true_type
{
};

template <typename T, typename = void>
class is_string_view_like : public std::false_type
{
};

template <typename T>
class is_string_view_like<T,
                          std::void_t<typename T::value_type,
                                      decltype(std::declval<T>().back()),
                                      decltype(std::declval<T>().data()),
                                      decltype(std::declval<T>().empty())>>
    : public std::conjunction<std::is_pointer<decltype(std::declval<T>().data())>,
                              std::is_convertible<decltype(std::declval<T>().back()), decltype(kNullByte)>>
{
};

}  // namespace details

template <typename T>
[[nodiscard]] constexpr bool IsNullTerminatedViewType() noexcept
{
    return std::disjunction_v<
        details::is_null_terminated_string_type<T>,
        std::is_convertible<T, safecpp::details::zspan<std::add_const_t<typename T::value_type>>>>;
}

///
/// @brief Safely retrieve a pointer to the null-terminated underlying buffer of a string-view like object.
///
/// This function template ensures that the provided view object entails a null-terminated underlying buffer
/// before returning a pointer to it. It performs various checks and handles different types of string views:
///  - For guaranteed null-terminated view types (`zspan`, `zstring_view`), returns the data pointer directly
///  - For `std::string_view` types, emits a deprecation warning but returns the pointer (migration required)
///  - For other string-view like types, verifies null-termination by checking the last element in view's range
///
/// @tparam ViewType The type of the string-view like object (must satisfy the `is_string_view_like` trait)
/// @tparam ViolationPolicy Policy type for handling null-termination violations (defaults to `abort`)
///
/// @param view string-view like object from which to extract the pointer to the underlying buffer
/// @param violation_policy Policy object to handle violations when buffer is not null-terminated
///
/// @return Pointer to the null-terminated underlying buffer, probably nullptr if validation fails
///
/// @throws May throw depending on the `ViolationPolicy`'s noexcept specification
///
/// @note This function encourages migration from `std::string_view` to `safecpp::zstring_view`
///       for guaranteed null-termination safety!
///
template <typename ViewType,
          typename ViolationPolicy = null_termination_violation_policies::default_policy,
          typename VT = std::remove_cv_t<std::remove_reference_t<ViewType>>,
          std::enable_if_t<details::is_string_view_like<VT>::value &&
                               null_termination_violation_policies::is_valid_one<ViolationPolicy>(),
                           bool> = true>
[[nodiscard]] constexpr auto GetPtrToNullTerminatedUnderlyingBufferOf(
    ViewType&& view,
    ViolationPolicy violation_policy = {}) noexcept(noexcept(std::invoke(violation_policy, "reason")))
    -> decltype(std::declval<ViewType>().data())
{
    if (view.data() == nullptr)  // LCOV_EXCL_BR_LINE: tooling issue since both branches are covered (see below)
    {
        std::invoke(violation_policy, "score::safecpp: provided view object does not entail any underlying buffer");
        return nullptr;
    }
    else
    {
        // dummy statement to prove this branch as covered
        (void)view.size();
    }

    if constexpr (safecpp::IsNullTerminatedViewType<VT>())
    {
        // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage) `ViewType` guarantees null-termination
        return view.data();
    }
    else if constexpr (details::is_string_view<VT>::value)
    {
        // TODO (Ticket-214240): static_assert here once the codebase got fully migrated to `safecpp::zstring_view`
        class EmitCompilerWarningFor
        {
          public:
            [[deprecated(
                "CAUTION: The underlying buffer of a string_view does NOT guarantee any null-termination! "
                "Instead, migrate your code to make use of 'safecpp::zstring_view' in order to obtain "
                "such guarantee")]] constexpr static void
            DoNotExpectNullTerminationOfStringViewsUnderlyingBuffer() noexcept
            {
            }
        };
        // below line is required as such so that the emitted warning also prints the source location of our caller
        EmitCompilerWarningFor::DoNotExpectNullTerminationOfStringViewsUnderlyingBuffer();
        // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage) this code will get removed, see Ticket-214240
        return view.data();
    }
    else
    {
        if (view.empty())
        {
            std::invoke(violation_policy, "score::safecpp: provided view object is empty");
            return nullptr;
        }

        if (view.back() != details::kNullByte)
        {
            std::invoke(violation_policy, "score::safecpp: provided view's underlying sequence is not null-terminated");
            return nullptr;
        }

        // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage) underlying sequence is null-terminated at its end
        return view.data();
    }
}

}  // namespace score::safecpp

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_NULL_TERMINATION_CHECK_H
