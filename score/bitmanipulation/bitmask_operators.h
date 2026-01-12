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
#ifndef SCORE_LIB_OS_BITMASK_OPERATORS_H
#define SCORE_LIB_OS_BITMASK_OPERATORS_H

/// @brief Provides bitmask operators for enum classes
///
/// !!!! ATTENTION !!!!
/// These operators only work well on real bitmasks. Meaning, the elements of an enum must always be set to a power of
/// two, and no value shall have the value 0.
///
/// @see https://en.cppreference.com/w/cpp/named_req/BitmaskType
///
/// @details One shall prefer `enum class` over `enum` since this avoids implicit conversions. This comes with the
/// drawback that it is no longer possible to combine flags of an enum (which is a mechanism which is often used in
/// POSIX related APIs).
///
/// By implementing the named requirements for BitmaskType for such `enum class` types, we get the best of both worlds.
///
/// This is in particular needed for the `lib/os`, since not all POSIX options are safety-certified. So we have to
/// ensure that in a safety-use-case, only the certified options are chosen. Thus, our functions cannot take any
/// integer, since this could lead that developers _leak_ not allowed numerical representations of flags into our
/// functional abstractions. By using a specific type for flag combinations (enable_bitmask_operators) we can ensure
/// that only valid flags are combined with each other.

#include <cstdint>
#include <type_traits>

namespace score
{

/// @brief Primary template to enable bitmask operators for scoped enums.
///
/// Scoped enums by default are not bitmasks and therefore don't support operators used for working with bitmasks, like
/// `operator|` and `operator&`. Scoped enums that represent bitmasks specialize this type to support bitmasks
/// operators.
///
/// Example:
///
///     template <>
///     class enable_bitmask_operators<MyType> {
///        public:
///         static constexpr bool value{true};
///     };
///
/// @note A shortcoming of this approach is that the specialization of enable_bitmask_operators must be placed in the
/// same namespace as the primary template (i.e., score::os).
template <typename T>
class enable_bitmask_operators : public std::false_type
{
};

}  // namespace score

namespace details
{
template <typename T>
using IsEnabledEnum =
    typename std::enable_if<std::is_enum<T>::value && score::enable_bitmask_operators<T>::value, T>::type;
}  // namespace details

/// @brief Provides the bitwise OR operator for scoped enums so that individual bits can be set in a bitmask.
///
/// Example:
///
///      MyBitmask bitmask{MyBitmask::a | MyBitmask::b};  // Set a and b in the bitmask
///
/// @see https://www.justsoftwaresolutions.co.uk/cplusplus/using-enum-classes-as-bitfields.html
/// @see https://www.heise.de/select/ix-special/archiv/2020/17/seite-78

// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule declares: "The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Intentionally global to specialize standard library operators. Thanks to SFINAE (using std::enable_if),
// the operator is only enabled when certain conditions are met, ensuring it is available on demand.
template <typename T>
// coverity[autosar_cpp14_m7_3_1_violation]
constexpr ::details::IsEnabledEnum<T> operator|(const T& lhs, const T& rhs)
{
    using U = std::underlying_type_t<T>;
    // Suppress "AUTOSAR C++14 A7-2-1" rule finding. The rule states: "An expression with enum underlying type shall
    // only have values corresponding to the enumerators of the enumeration.".
    // This finding occurs because the underlying value, which is a smaller type, is cast to uint64_t and then back
    // after applying the bitwise operation. This can potentially cause data loss, but this fulfills the result type.
    // coverity[autosar_cpp14_a7_2_1_violation]
    return static_cast<T>(static_cast<std::uint64_t>(static_cast<U>(lhs)) |
                          static_cast<std::uint64_t>(static_cast<U>(rhs)));
}

/// @brief Provides the bitwise AND operator for scoped enums so that individual bits can be read from a bitmask.
///
/// Example:
///
///      void f(MyBitmask bitmask) {
///          if (bitmask & MyBitmask::a) {
///              // a is set in the bitmask
///          }
///          if (bitmask & MyBitmask::b) {
///              // b is set in the bitmask
///          }
///      }
///
/// @details We don't follow the C++ Standard in this operator, to enable easy checking if a bit is set. If an
/// bitwiseAND is needed, please use operator&= instead.
// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule declares: "The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Intentionally global to specialize standard library operators. Thanks to SFINAE (using std::enable_if),
// the operator is only enabled when certain conditions are met, ensuring it is available on demand.
template <typename T>
constexpr typename std::enable_if<std::is_enum<T>::value && score::enable_bitmask_operators<T>::value, bool>::type
// coverity[autosar_cpp14_m7_3_1_violation]
operator&(const T& lhs, const T& rhs)
{
    using U = std::underlying_type_t<T>;
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
    // loss." In this context, there is no data loss because lhs/rhs is promoted to std::uint64_t for the bitwise
    // operation. The result is then checked for non-zero, ensuring the integrity of the original data.
    // coverity[autosar_cpp14_a4_7_1_violation]
    return (static_cast<std::uint64_t>(static_cast<U>(lhs)) & static_cast<std::uint64_t>(static_cast<U>(rhs))) != 0U;
}

/// @brief Provides the bitwise XOR operator for scoped enums.
// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule declares: "The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Intentionally global to specialize standard library operators. Thanks to SFINAE (using std::enable_if),
// the operator is only enabled when certain conditions are met, ensuring it is available on demand.
template <typename T>
// coverity[autosar_cpp14_m7_3_1_violation]
constexpr ::details::IsEnabledEnum<T> operator^(const T& lhs, const T& rhs)
{
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<std::uint64_t>(static_cast<U>(lhs)) ^
                          static_cast<std::uint64_t>(static_cast<U>(rhs)));
}

/// @brief Provides the bitwise NOT operator for scoped enums.
// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule declares: "The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Intentionally global to specialize standard library operators. Thanks to SFINAE (using std::enable_if),
// the operator is only enabled when certain conditions are met, ensuring it is available on demand.
template <typename T>
// coverity[autosar_cpp14_m7_3_1_violation]
constexpr ::details::IsEnabledEnum<T> operator~(const T& other)
{
    using U = std::underlying_type_t<T>;
    // Suppress "AUTOSAR C++14 A7-2-1" rule finding. The rule states: "An expression with enum underlying type shall
    // only have values corresponding to the enumerators of the enumeration.".
    // This finding occurs because the underlying value, which is a smaller type, is cast to uint64_t and then back
    // after applying the bitwise operation. This can potentially cause data loss, but this fulfills the result type.
    // coverity[autosar_cpp14_a7_2_1_violation]
    return static_cast<T>(~static_cast<std::uint64_t>(static_cast<U>(other)));
}

/// @brief Provides the bitwise AND assign operator for scoped enums.
// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule declares: "The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Intentionally global to specialize standard library operators. Thanks to SFINAE (using std::enable_if),
// the operator is only enabled when certain conditions are met, ensuring it is available on demand.
template <typename T>
// coverity[autosar_cpp14_m7_3_1_violation]
constexpr ::details::IsEnabledEnum<T> operator&=(T& lhs, const T& rhs)
{
    using U = std::underlying_type_t<T>;
    // Suppress "AUTOSAR C++14 A7-2-1" rule finding. The rule states: "An expression with enum underlying type shall
    // only have values corresponding to the enumerators of the enumeration.".
    // This finding occurs because the underlying value, which is a smaller type, is cast to uint64_t and then back
    // after applying the bitwise operation. This can potentially cause data loss, but this fulfills the result type.
    // coverity[autosar_cpp14_a7_2_1_violation]
    lhs = static_cast<T>(static_cast<std::uint64_t>(static_cast<U>(lhs)) &
                         static_cast<std::uint64_t>(static_cast<U>(rhs)));

    return lhs;
}

/// @brief Provides the bitwise OR assign operator for scoped enums.
// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule declares: "The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Intentionally global to specialize standard library operators. Thanks to SFINAE (using std::enable_if),
// the operator is only enabled when certain conditions are met, ensuring it is available on demand.
template <typename T>
// coverity[autosar_cpp14_m7_3_1_violation]
constexpr ::details::IsEnabledEnum<T> operator|=(T& lhs, const T& rhs)
{
    using U = std::underlying_type_t<T>;
    // Suppress "AUTOSAR C++14 A7-2-1" rule finding. The rule states: "An expression with enum underlying type shall
    // only have values corresponding to the enumerators of the enumeration.".
    // This finding occurs because the underlying value, which is a smaller type, is cast to uint64_t and then back
    // after applying the bitwise operation. This can potentially cause data loss, but this fulfills the result type.
    // coverity[autosar_cpp14_a7_2_1_violation]
    lhs = static_cast<T>(static_cast<std::uint64_t>(static_cast<U>(lhs)) |
                         static_cast<std::uint64_t>(static_cast<U>(rhs)));
    return lhs;
}

/// @brief Provides the bitwise XOR assign operator for scoped enums.
// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule declares: "The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Intentionally global to specialize standard library operators. Thanks to SFINAE (using std::enable_if),
// the operator is only enabled when certain conditions are met, ensuring it is available on demand.
template <typename T>
// coverity[autosar_cpp14_m7_3_1_violation]
constexpr ::details::IsEnabledEnum<T> operator^=(T& lhs, const T& rhs)
{
    using U = std::underlying_type_t<T>;
    lhs = static_cast<T>(static_cast<std::uint64_t>(static_cast<U>(lhs)) ^
                         static_cast<std::uint64_t>(static_cast<U>(rhs)));
    return lhs;
}

#endif  // SCORE_LIB_OS_BITMASK_OPERATORS_H
