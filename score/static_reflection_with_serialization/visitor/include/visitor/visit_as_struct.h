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
// NOLINT(score-header-guard) False positive - Include guard provided
#ifndef SCORE_COMMON_VISITOR_INCLUDE_VISITOR_VISIT_AS_STRUCT_H
#define SCORE_COMMON_VISITOR_INCLUDE_VISITOR_VISIT_AS_STRUCT_H

#include <array>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>

namespace score
{

namespace common
{

namespace visitor
{

namespace detail
{

template <typename T>
using no_cref_t = std::remove_const_t<std::remove_reference_t<T>>;

/// \brief Left-shift the end to the next non-whitespace character.
/// \details GCC may produce a trailing whitespace on templated structs in __PRETTY_FUNCTION__.
/// In order to avoid that trailing whitespace in the typename, we shift the end to the left until there is no
/// whitespace.
/// Example value of __PRETTY_FUNCTION__:
/// ```
/// static constexpr auto& \ignore this line break due to clang-format.
/// some::namespace::struct_visitable_impl<some::namespace::SomeDataType<299> >::namedata()'
/// ```                                                                                ^
///                                             trailing whitespace to be removed above^

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays) Array is safe to use when dealing with compile time constructs
template <std::size_t N>
constexpr inline std::size_t strip_trailing_spaces(const char (&pretty_name)[N], std::size_t begin, std::size_t end)
// NOLINTEND(cppcoreguidelines-avoid-c-arrays) Array is safe to use when dealing with compile time constructs
{
    if ((end == 0UL) || (end > N))
    {
        return end;
    }

    std::size_t last_character_index = (end - 1UL);

    while (last_character_index > begin)
    {
        const char last_character = pretty_name[last_character_index];

        if (last_character == ' ')
        {
            // Strip trailing whitespace by shifting the end to the left.
            last_character_index--;
        }
        else
        {
            // Last character is not whitespace, we are done.
            break;
        }
    }

    // The end is one increment past the last character.
    end = (last_character_index + 1UL);

    return end;
}

template <std::size_t N>
constexpr inline std::pair<std::size_t, std::size_t> visitor_extract_type_span(const char (&pretty_name)[N])
{
    // ============== COMMON_ARGUMENTATION ==============
    // The FALSE case for the below 'for' loop are already tested with 'extract_type' test case in test_detail.cpp
    // with the below assertions:
    //     static_assert(check_type_span("qwerty", 0, 6), "backup logic - without angle brackets");
    //     static_assert(check_type_span("q<erty", 2, 6), "abnormal logic - with left bracket");
    // lcov complains about FALSE case didn't taken, it's false positive.
    size_t first = 0UL;
    size_t second = (N - 1UL);
    for (std::size_t i = 0UL; i < (N - 1UL); ++i)  // LCOV_EXCL_BR_LINE: COMMON_ARGUMENTATION
    {
        if (pretty_name[i] == '<')
        {
            first = (i + 1UL);
            for (std::size_t j = (N - 1UL); j >= first; --j)  // LCOV_EXCL_BR_LINE: COMMON_ARGUMENTATION
            {
                if (pretty_name[j] == '>')
                {
                    second = strip_trailing_spaces<N>(pretty_name, first, j);
                    break;
                }
            }
            break;
        }
    }
    return {first, second};
}

template <typename Output, std::size_t N>
constexpr inline Output visitor_extract_type(const char (&pretty_name)[N])
{
    auto pair = visitor_extract_type_span(pretty_name);
    return {&pretty_name[pair.first], &pretty_name[pair.second]};
}

template <typename... Args>
constexpr inline auto pack_values(Args&&... args)
{
    return std::make_tuple(std::forward<Args>(args)...);
}

template <typename... Ts, std::size_t... Is>
constexpr std::array<const char*, sizeof...(Ts)> tuple_to_array_impl(const std::tuple<Ts...>& tup,
                                                                     std::index_sequence<Is...>)
{
    return {std::get<Is>(tup)...};
}

template <typename... Ts>
constexpr std::array<const char*, sizeof...(Ts)> tuple_to_array(const std::tuple<Ts...>& tup)
{
    return tuple_to_array_impl(tup, std::make_index_sequence<sizeof...(Ts)>{});
}

}  // namespace detail

//=== need to be exposed because of fasinfo hack
template <typename S, typename T>
using if_same_struct = std::enable_if_t<std::is_same<S, detail::no_cref_t<T>>::value, std::int32_t>;

template <typename T>
/*
    - rule of zero applied here, because we only have static function.
    so if we assign it or copy it, the compiler by default will generate these functions.

    - Rule M3-2-3 (required, implementation, automated)
      A type, object or function that is used in multiple translation units shall
      be declared in one and only one file.
      - false positive , because it was defined only once in this file.
*/
// coverity[autosar_cpp14_a12_8_6_violation]
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class struct_visitable_base
{
  public:
    template <typename Output = std::string>
    static constexpr Output name()
    {
        return detail::visitor_extract_type<Output>(T::namedata());
    }
};
//===

// Koenig lookup, SFINAE
template <typename T>
/*
    - Rule M3-2-3 (required, implementation, automated)
      A type, object or function that is used in multiple translation units shall
      be declared in one and only one file.
      - false positive , because it was defined only once in this file.
*/
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
inline auto get_struct_visitable() -> decltype(get_struct_visitable_(static_cast<detail::no_cref_t<T>*>(nullptr)));

template <typename T>
using struct_visitable = decltype(get_struct_visitable<T>());

}  // namespace visitor

}  // namespace common

}  // namespace score

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) Macros are used to access field names
// Macros are used to access field names
// Macros are used to concatenate names and can't use parenthesis
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define STRUCT_VISITABLE_FULL_DEFINITION(S, TUPLE_OF_NAMES, ...)                                                       \
    template <typename T>                                                                                              \
    class struct_visitable_impl;                                                                                       \
    template <>                                                                                                        \
    class struct_visitable_impl<S> : public ::score::common::visitor::struct_visitable_base<struct_visitable_impl<S>>    \
    {                                                                                                                  \
      public:                                                                                                          \
        static constexpr auto& namedata()                                                                              \
        {                                                                                                              \
            return __PRETTY_FUNCTION__;                                                                                \
        }                                                                                                              \
        static constexpr std::size_t fields = static_cast<std::size_t>(COUNT_VARARGS(__VA_ARGS__));                    \
        static constexpr const char* field_name(const size_t i)                                                        \
        {                                                                                                              \
            constexpr std::array<const char*, fields> names =                                                          \
                ::score::common::visitor::detail::tuple_to_array(TUPLE_OF_NAMES);                                        \
            /* NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index) Index is evaluated in macros and thus    \
             * not given for user to control. */                                                                       \
            return ((i < fields) ? names[i] : nullptr);                                                                  \
            /* NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index) Index is evaluated in macros and thus not  \
             * given for user to control. */                                                                           \
        }                                                                                                              \
        template <typename V, typename T>                                                                              \
        static auto visit(V&& v, T&& s)                                                                                \
        {                                                                                                              \
            return visit_as_struct(std::forward<V>(v), std::forward<T>(s), __VA_ARGS__);                               \
        }                                                                                                              \
    };                                                                                                                 \
    inline auto get_struct_visitable_(S* const /*unused*/)                                                             \
    {                                                                                                                  \
        return struct_visitable_impl<S>();                                                                             \
    }                                                                                                                  \
    template <typename V, typename T, ::score::common::visitor::if_same_struct<S, T> = 0>                                \
    inline auto visit_as(V&& v, T&& t)                                                                                 \
    {                                                                                                                  \
        /* workaround for clang warning -Wunneeded-internal-declaration */                                             \
        static_cast<void>(get_struct_visitable_(static_cast<::score::common::visitor::detail::no_cref_t<S>*>(nullptr))); \
        return struct_visitable_impl<S>::visit(std::forward<V>(v), std::forward<T>(t));                                \
    }

/// \public STRUCT_VISITABLE and STRUCT_TRACEABLE are public API.
// used as public API
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define STRUCT_TRACEABLE(...) STRUCT_VISITABLE(__VA_ARGS__)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE64(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, F60, F61, F62, F63, F64) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57, #F58, #F59, #F60, #F61, #F62, #F63, #F64), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57, s.F58, s.F59, s.F60, s.F61, s.F62, s.F63, s.F64) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE63(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, F60, F61, F62, F63) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57, #F58, #F59, #F60, #F61, #F62, #F63), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57, s.F58, s.F59, s.F60, s.F61, s.F62, s.F63) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE62(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, F60, F61, F62) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57, #F58, #F59, #F60, #F61, #F62), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57, s.F58, s.F59, s.F60, s.F61, s.F62) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE61(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, F60, F61) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57, #F58, #F59, #F60, #F61), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57, s.F58, s.F59, s.F60, s.F61) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE60(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, F60) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57, #F58, #F59, #F60), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57, s.F58, s.F59, s.F60) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE59(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57, F58, F59) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57, #F58, #F59), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57, s.F58, s.F59) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE58(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57, F58) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57, #F58), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57, s.F58) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE57(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56, F57) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56, #F57), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56, s.F57) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE56(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55, F56) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55, #F56), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55, s.F56) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE55(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54, F55) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54, #F55), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54, s.F55) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE54(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53, F54) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53, #F54), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53, s.F54) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE53(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52, F53) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52, #F53), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52, s.F53) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE52(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51, F52) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51, #F52), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51, s.F52) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE51(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50, F51) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50, #F51), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50, s.F51) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE50(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, F50) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49, #F50), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49, s.F50) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE49(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48, F49) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48, #F49), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48, s.F49) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE48(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47, F48) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47, #F48), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47, s.F48) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE47(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46, F47) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46, #F47), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46, s.F47) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE46(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45, F46) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45, #F46), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45, s.F46) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE45(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44, F45) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44, #F45), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44, s.F45) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE44(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43, F44) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43, #F44), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43, s.F44) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE43(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42, F43) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42, #F43), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42, s.F43) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE42(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41, F42) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41, #F42), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41, s.F42) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE41(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40, F41) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40, #F41), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40, s.F41) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE40(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, F40) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39, #F40), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39, s.F40) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE39(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38, F39) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38, #F39), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38, s.F39) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE38(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37, F38) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37, #F38), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37, s.F38) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE37(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36, F37) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36, #F37), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36, s.F37) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE36(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35, F36) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35, #F36), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35, s.F36) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE35(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34, F35) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34, #F35), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34, s.F35) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE34(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33, F34) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33, #F34), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33, s.F34) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE33(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32, F33) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32, #F33), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32, s.F33) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE32(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31, F32) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31, #F32), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31, s.F32) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE31(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30, F31) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30, #F31), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30, s.F31) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE30(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, F30) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29, #F30), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29, s.F30) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE29(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28, F29) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28, #F29), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28, s.F29) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE28(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27, F28) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27, #F28), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27, s.F28) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE27(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26, F27) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26, #F27), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26, s.F27) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE26(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, F26) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25, #F26), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25, s.F26) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE25(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24, #F25), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24, s.F25) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE24(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23, #F24), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23, s.F24) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE23(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22, #F23), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22, s.F23) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE22(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21, #F22), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21, s.F22) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE21(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20, #F21), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20, s.F21) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE20(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19, #F20), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19, s.F20) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE19(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18, #F19), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18, s.F19) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE18(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17, #F18), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17, s.F18) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE17(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16, #F17), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16, s.F17) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE16(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15, #F16), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15, s.F16) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE15(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14, #F15), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14, s.F15) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE14(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13, #F14), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13, s.F14) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE13(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12, #F13), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12, s.F13) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE12(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11, #F12), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11, s.F12) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE11(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10, #F11), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10, s.F11) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE10(S, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9, #F10), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9, s.F10) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE9(S, F1, F2, F3, F4, F5, F6, F7, F8, F9) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8, #F9), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8, s.F9) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE8(S, F1, F2, F3, F4, F5, F6, F7, F8) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7, #F8), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7, s.F8) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE7(S, F1, F2, F3, F4, F5, F6, F7) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6, #F7), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6, s.F7) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE6(S, F1, F2, F3, F4, F5, F6) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5, #F6), s.F1, s.F2, s.F3, s.F4, s.F5, s.F6) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE5(S, F1, F2, F3, F4, F5) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4, #F5), s.F1, s.F2, s.F3, s.F4, s.F5) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE4(S, F1, F2, F3, F4) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3, #F4), s.F1, s.F2, s.F3, s.F4) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE3(S, F1, F2, F3) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2, #F3), s.F1, s.F2, s.F3) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE2(S, F1, F2) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1, #F2), s.F1, s.F2) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros of Field names and can't use parenthesis.
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
// coverity[autosar_cpp14_m16_3_1_violation]
#define STRUCT_VISITABLE1(S, F1) \
    STRUCT_VISITABLE_FULL_DEFINITION(S, \
    ::score::common::visitor::detail::pack_values(#F1), s.F1) \

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros for pass number of Arguments.
// Macros are used to pass length of args and can't use parenthesis
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, N, ...) N
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros for counting number of args.
// Macros are used to pass count args length and can't use parenthesis
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define COUNT_VARARGS(...) GET_NTH_ARG(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros for Concatenation.
// Macros are used to concatenate names and can't use parenthesis
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define DDAD_CONCATENATE2(A, B) A##B
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros for Concatenation.
// Macros are used to concatenate names and can't use parenthesis
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define DDAD_CONCATENATE(A, B) DDAD_CONCATENATE2(A, B)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
// Macros for Concatenation.
// Macros are used to concatenate names and can't use parenthesis
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define STRUCT_VISITABLE(S, ...) DDAD_CONCATENATE(STRUCT_VISITABLE, COUNT_VARARGS(__VA_ARGS__))(S, __VA_ARGS__)


#endif  // SCORE_COMMON_VISITOR_INCLUDE_VISITOR_VISIT_AS_STRUCT_H
