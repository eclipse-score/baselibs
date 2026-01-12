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
#ifndef SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSTRING_VIEW_H
#define SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSTRING_VIEW_H

#include "score/language/safecpp/string_view/details/zspan.h"

#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(readability-identifier-naming): STL-style notation is intended here
// in order to facilitate interoperability with other STL-like containters/algorithms

namespace score::safecpp
{

///
/// @brief non-modifiable view type over guaranteed null-terminated contiguous sequence of characters
/// @note This implementation was originally motivated by the following C++ standard draft paper:
///       https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p3655r2.html
///       However, we decided to prioritize safety over following exactly a future standard proposal.
///       As a result, we do not strictly follow the proposed API but intentionally deviate from it in some parts
///       e.g. by not providing certain constructors or by performing additional bounds checks upon element access.
///       Further note: the above-mentioned draft paper got meanwhile superseded by a revised one which itself is also
///       subject to further changes at any time. The current draft version can be found here: https://wg21.link/p3655.
///
template <typename CharType>
class basic_zstring_view : private details::zspan<std::add_const_t<CharType>>
{
    using base = const details::zspan<std::add_const_t<CharType>>;

    template <typename T, typename S, typename = void>
    class is_null_terminated_string_type : public std::false_type
    {
    };
    template <typename T, typename S>
    class is_null_terminated_string_type<
        T,
        S,
        std::void_t<decltype(std::declval<T>().c_str()), decltype(std::declval<T>().size())>>
        : public std::is_convertible<decltype(*std::declval<T>().c_str()), S>
    {
    };

  public:
    using violation_policies = typename base::violation_policies;

    using value_type = typename base::value_type;
    using size_type = typename base::size_type;

    /// @brief Constructs a `basic_zstring_view` as view over character array.
    /// @details will invoke `std::abort()` in case such character array is not null-terminated at its last element
    template <std::size_t N>
    // NOLINTNEXTLINE(google-explicit-constructor, modernize-avoid-c-arrays) allow implicit creation from C-style arrays
    constexpr basic_zstring_view(const CharType (&str)[N]) noexcept : base(str, typename violation_policies::abort{})
    {
    }

    /// @brief Constructs a `basic_zstring_view` as view over null-terminated \p StringType;.
    /// @details type \p StringType; is required to guarantee null-termination of its underlying buffer
    template <typename StringType,
              std::enable_if_t<is_null_terminated_string_type<StringType, CharType>::value, bool> = true>
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversions from `StringType` which is null-terminated
    constexpr basic_zstring_view(const StringType& str) noexcept
        // `StringType` guarantees null-termination of `c_str()` and hence we use `set_empty` as dummy policy here
        : base(str.c_str(), str.size() + 1U, typename violation_policies::set_empty{})
    {
    }

    /// @brief Constructs a `basic_zstring_view` as view over character range pointed to by pointer \p str;.
    /// @details will invoke `std::abort()` in case such character range is not null-terminated at index \p len; - 1
    constexpr basic_zstring_view(const CharType* str, size_type len) noexcept
        : base(str, len, typename violation_policies::abort{})
    {
    }

    /// @note Constructor from char ptr w/o length info is omitted since its null-termination cannot be checked safely.
    /// @details We can also not mark the corresponding constructor as `= delete` since that would lead to ambiguities.
    ///          Hence, a constructor like the below one is simply not provided at all.
    // constexpr basic_zstring_view(CharType*) = delete;

    /// @brief Constructs a `basic_zstring_view` from base type (i.e. `zspan<const CharType>`)
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversions from base type `zspan` (are always safe)
    constexpr basic_zstring_view(base other) noexcept : base(other) {}

    /// @brief Prohibits construction of a `basic_zstring_view` from `std::basic_string_view`
    constexpr basic_zstring_view(std::basic_string_view<CharType>) = delete;

    /// @brief Prohibits construction from `std::nullptr_t`
    constexpr basic_zstring_view(std::nullptr_t) = delete;

    /// @brief Default constructs a `basic_zstring_view`.
    constexpr basic_zstring_view() noexcept = default;

    using base::base;

    using base::at;
    using base::back;
    using base::data;
    using base::empty;
    using base::front;
    using base::size;
    using base::operator[];

    /// @brief Returns a pointer to the null-terminated underlying character sequence.
    [[nodiscard]] constexpr auto c_str() const noexcept
    {
        return data();
    }

    /// @brief Returns the number of `char_type` elements in the view (not accounting the trailing null-terminator).
    [[nodiscard]] constexpr size_type length() const noexcept
    {
        return size();
    }

    ///
    /// @brief implicit conversion operator from `safecpp::basic_zstring_view` to `std::basic_string_view`
    ///
    // NOLINTNEXTLINE(google-explicit-constructor) allow impl. conversions to `std::basic_string_view` (const view type)
    [[nodiscard]] constexpr operator std::basic_string_view<CharType>() const noexcept
    {
        return std::basic_string_view<CharType>{data(), size()};
    }

    ///
    /// @brief `std::ostream` output operator for `safecpp::basic_zstring_view`
    ///
    friend std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os,
                                                    safecpp::basic_zstring_view<CharType> sv)
    {
        return os << std::basic_string_view<CharType>{sv};
    }

    ///
    /// @brief swap operator for `safecpp::basic_zstring_view`
    ///
    friend constexpr void swap(safecpp::basic_zstring_view<CharType>& lhs,
                               safecpp::basic_zstring_view<CharType>& rhs) noexcept
    {
        auto tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }
};

/// @brief alias definition for `zstring_view`
using zstring_view = basic_zstring_view<char>;

namespace literals
{
///
/// @brief user-defined literal operator for character type `const char`
/// @note invoking this literal operator manually is strictly prohibited and will result in undefined behavior!
///
[[nodiscard]] constexpr zstring_view operator""_zsv(const char* str, std::size_t len) noexcept
{
    // since string literals are guaranteed to be null-terminated, we can safely apply `len + 1U`
    return details::zspan<const char>{str, len + 1U, null_termination_violation_policies::set_empty{}};
}
}  // namespace literals

}  // namespace score::safecpp

// NOLINTEND(readability-identifier-naming)

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_ZSTRING_VIEW_H
