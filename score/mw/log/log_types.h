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

#ifndef SCORE_MW_LOG_TYPES_H
#define SCORE_MW_LOG_TYPES_H

#include <score/assert.hpp>
#include <score/span.hpp>

#include <cstdint>
#include <functional>
#include <iterator>
#include <string_view>
#include <type_traits>
#include <vector>

namespace score
{
namespace mw
{
namespace log
{

/// \brief Helper type to log an uint8 in hexadecimal representation.
/// \public
struct LogHex8
{
    std::uint8_t value;
};

/// \brief Helper type to log an uint16 in hexadecimal representation.
/// \public
struct LogHex16
{
    std::uint16_t value;
};

/// \brief Helper type to log an uint32 in hexadecimal representation.
/// \public
struct LogHex32
{
    std::uint32_t value;
};

/// \brief Helper type to log an uint64 in hexadecimal representation.
/// \public
struct LogHex64
{
    std::uint64_t value;
};

/// \brief Helper type to log an uint8 in binary representation.
/// \public
struct LogBin8
{
    std::uint8_t value;
};

/// \brief Helper type to log an uint16 in binary representation.
/// \public
struct LogBin16
{
    std::uint16_t value;
};

/// \brief Helper type to log an uint32 in binary representation.
/// \public
struct LogBin32
{
    std::uint32_t value;
};

/// \brief Helper type to log an uint64 in binary representation.
/// \public
struct LogBin64
{
    std::uint64_t value;
};

namespace detail
{
/// @brief Determines whether type `RangeType` is a range of `ElementType`.
template <typename RangeType, typename ElementType, typename = void>
class IsRange : public std::false_type
{
};
template <typename RangeType, typename ElementType>
class IsRange<
    RangeType,
    ElementType,
    std::void_t<decltype(std::data(std::declval<RangeType>())), decltype(std::size(std::declval<RangeType>()))>>
    // NOLINTNEXTLINE(score-banned-function): using `std::data()` is valid here since `IsRange` is just a type trait
    : public std::is_same<std::remove_cv_t<std::remove_reference_t<decltype(*std::data(std::declval<RangeType>()))>>,
                          ElementType>
{
};

/// @brief Determines whether type `RangeType` is a range of `ElementType` and not an score::cpp::span or array of such type.
template <typename RangeType, typename ElementType>
constexpr bool IsNonSpanNonArrayRange() noexcept
{
    return std::conjunction_v<IsRange<RangeType, ElementType>,
                              std::negation<score::cpp::is_span<RangeType>>,
                              std::negation<std::is_array<RangeType>>>;
}
}  // namespace detail

/// \brief Helper type serving as view over string-like types.
/// \public
class LogString
{
  public:
    using TraitsType = std::string_view::traits_type;
    using CharType = std::string_view::value_type;
    using CharPtr = std::add_pointer_t<std::add_const_t<CharType>>;

    /// \brief Constructs `LogString` as view over character \p range;.
    ///
    /// This overload participates in overload resolution only if type `RangeType` is iterable via (const) `CharType`
    /// and is not an score::cpp::span or an array of type `CharType`.
    ///
    template <typename RangeType,
              typename RT = std::remove_cv_t<std::remove_reference_t<RangeType>>,
              std::enable_if_t<detail::IsNonSpanNonArrayRange<RT, CharType>(), bool> = true>
    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init): false positive, this constructor delegates to another one
    // NOLINTNEXTLINE(google-explicit-constructor): intended here to allow implicit conversions from range of `CharType`
    constexpr LogString(RangeType&& range) noexcept(noexcept(std::data(range)) && noexcept(std::size(range)))
        // NOLINTNEXTLINE(score-banned-function): using `std::data()` is valid here since used in conj. with `std::size()`
        : LogString(std::data(range), std::size(range))
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)
    {
    }

    /// \brief Constructs `LogString` as view over character \p range;.
    ///
    /// This overload participates in overload resolution only if type `RangeType` is iterable via (const) `CharType`
    /// and is not an score::cpp::span or an array of type `CharType`.
    ///
    template <typename RangeType,
              typename RT = std::remove_cv_t<std::remove_reference_t<RangeType>>,
              std::enable_if_t<detail::IsNonSpanNonArrayRange<RT, CharType>(), bool> = true>
    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init): false positive, this constructor delegates to another one
    // NOLINTNEXTLINE(google-explicit-constructor): intended here to allow implicit conversions from range of `CharType`
    constexpr LogString(std::reference_wrapper<RangeType> range_wrapper) noexcept(noexcept(LogString{
        range_wrapper.get()}))
        : LogString(range_wrapper.get())
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)
    {
    }

    /// \brief Constructs `LogString` as view over a bounded character array.
    template <std::size_t N>
    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init): false positive, this constructor delegates to another one
    // NOLINTBEGIN(google-explicit-constructor, modernize-avoid-c-arrays): allow implicit conversion from char array
    // NOLINTNEXTLINE(score-banned-function): using `std::data()` is valid here since size information is known via `N`
    constexpr LogString(const CharType (&array)[N]) : LogString(std::data(array), N - 1U)
    // NOLINTEND(google-explicit-constructor, modernize-avoid-c-arrays)
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)
    {
        static_assert(N > 0U, "character array must have at least 1 element");
        EnsureIsNullCharacter(array[N - 1U]);
    }

    /// \brief Constructs `LogString` as view over a character sequence pointed-to via `str`.
    constexpr LogString(CharPtr str, const std::size_t size) noexcept : data_{str}, size_{size} {}

    constexpr auto* Data() const noexcept
    {
        return data_;
    }
    constexpr auto Size() const noexcept
    {
        return size_;
    }

  private:
    /// \brief Performs assertion check whether a character value matches the null-character.
    /*
    Deviation from Rule A0-1-3:
    - Every function defined in an anonymous namespace, or static function with internal linkage,
      or private member function shall be used.
    Justification:
    - The function is used.
    */
    // coverity[autosar_cpp14_a0_1_3_violation : false]
    constexpr static void EnsureIsNullCharacter(const CharType character)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(character == '\0', "character array must be null-terminated");
    }

    CharPtr data_;
    std::size_t size_;
};

/// \brief Helper type to log string and forward slog2 code.
/// \brief See QNX slog2f documentation.
/// \public
class LogSlog2Message
{
  public:
    LogSlog2Message() = delete;
    LogSlog2Message(const std::uint16_t code, const std::string_view message) : slog_code_(code), message_(message) {}

    std::uint16_t GetCode() const
    {
        return slog_code_;
    }

    std::string_view GetMessage() const
    {
        return message_;
    }

  private:
    std::uint16_t slog_code_;
    std::string_view message_;
};

/// \brief Convenience method for logging character array
/// \public
/// \note Helps for example to avoid array-to-pointer decay when logging char[] literals or macros (such as __func__).
template <std::size_t N>
[[deprecated(
    "SPP_DEPRECATION: Making use of `mw::log::LogStr()` is no longer required since `mw::log::LogStream` "
    "meanwhile supports logging string literals natively via its `operator<<`.")]] constexpr LogString
// NOLINTNEXTLINE(modernize-avoid-c-arrays): required for avoiding implicit array-to-pointer decay in case of char array
LogStr(const LogString::CharType (&array)[N]) noexcept
{
    const LogString result{std::forward<decltype(array)>(array)};
    return result;
}

/// \brief Helper type to log the raw bytes of a buffer
/// \public
/// \note Maximum supported size for DLT output is less than 64 KB. Bytes exceeding that limit will be cropped.
/// \note Recommended to split the output in chunks of 1400 Bytes to avoid IP fragmentation DLT packets.
using LogRawBuffer = score::cpp::span<const char>;

/// \brief Create a LogRawBuffer from a scalar or array-of-scalars type instance
///
/// \tparam T Type of the data referenced by the value to be dumped.
/// \param values The value that shall be dumped.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input reference.
/*
Deviation from Rule A7-1-8:
- A non-type specifier shall be placed before a type specifier in a declaration.
Justification:
- template function, all non-type specifier are placed before type specifier, hence false positive
*/
template <typename T, typename = std::enable_if_t<std::is_scalar<T>::value>>
// coverity[autosar_cpp14_a7_1_8_violation : FALSE] see above
inline LogRawBuffer MakeLogRawBuffer(const T& value) noexcept
{
    // ----- COMMON_ARGUMENTATION ----
    // We cast to a char to print the byte representation of the given span. Since we know that:
    // - we do not violate the strict aliasing rules and
    // - we do not try to interpret or modify the data
    // the cast is acceptable.
    // --------------------------------

    // COMMON_ARGUMENTATION.
    // coverity[autosar_cpp14_a5_2_4_violation]
    return {reinterpret_cast<const char*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) COMMON_ARGUMENTATION
                &value),
            sizeof(T)};
}

/// \brief Create a LogRawBuffer from an score::cpp::span referencing an array of scalars.
///
/// \tparam T Type of the data referenced by the score::cpp::span. Must be a scalar.
/// \param values The span referencing the array.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input span.
template <typename T>
inline LogRawBuffer MakeLogRawBuffer(const score::cpp::span<T> values)
{
    // It is used by static_assert to check scalar types.
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    constexpr const bool IsEligible = std::is_scalar<T>::value;
    static_assert(IsEligible, "Only scalar types are allowed for dumping for now.");

    using SpanSizeType = typename score::cpp::span<T>::size_type;
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(values.size() >= static_cast<SpanSizeType>(0), "score::cpp::span with negative size refused.");

    // ----- COMMON_ARGUMENTATION ----
    // We cast to a char to print the byte representation of the given span. Since we know that:
    // - the memory is contiguous and
    // - we do not violate the strict aliasing rules and
    // - we do not try to interpret or modify the data
    // the cast is acceptable.
    // -------------------------------

    const auto buffer_size = values.size() * sizeof(T);
    // COMMON_ARGUMENTATION.
    // coverity[autosar_cpp14_a5_2_4_violation]
    return {reinterpret_cast<const char*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) COMMON_ARGUMENTATION
                values.data()),
            buffer_size};
}

/// \brief Create a LogRawBuffer from an std::vector referencing an array of scalars.
///
/// \tparam T Type of the data contained in the std::vector. Must be a scalar.
/// \param values The vector of elements.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input vector.
template <typename T>
inline LogRawBuffer MakeLogRawBuffer(const std::vector<T>& values) noexcept
{
    return MakeLogRawBuffer(
        score::cpp::span<const T>(values.data(), static_cast<typename score::cpp::span<const T>::size_type>(values.size())));
}

/// \brief Create a LogRawBuffer from an std::array consisting of scalars.
///
/// \tparam T Type of the data contained in the std::array. Must be a scalar.
/// \param values The array of elements.
/// \return An instance of LogRawBuffer, whose lifetime is limited by that of the given input array.
template <typename T, std::size_t N>
inline LogRawBuffer MakeLogRawBuffer(const std::array<T, N>& values) noexcept
{
    return MakeLogRawBuffer(score::cpp::span<const T>(values.data(), static_cast<typename score::cpp::span<const T>::size_type>(N)));
}

}  // namespace log
}  // namespace mw
}  // namespace score

#endif
