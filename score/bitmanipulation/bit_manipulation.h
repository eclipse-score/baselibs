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
#ifndef SCORE_BIT_MANIPULATION_H
#define SCORE_BIT_MANIPULATION_H

#include <climits>
#include <cstdint>
#include <type_traits>

namespace score
{
namespace platform
{
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
// coverity[autosar_cpp14_m3_2_3_violation]
class HalfByte
{
  public:
    /// \brief Construct a HalfByte from a uint8, only the last 4 bits are considered
    constexpr explicit HalfByte(const std::uint8_t value) : repr{static_cast<std::uint8_t>(value & mask)} {}
    // NOLINTBEGIN(google-explicit-constructor): No potentially dangerous type conversion
    // AUTOSAR Rule A13-5-2 prohibits implicit user defined conversion operators to avoid potential errors in type
    // conversion. But we do not convert underlying type (repr_ is std::uint8_t and return type is std::uint8_t).
    // Implicit 'decay' to std::uint8_t is needed as current HalfByte design permits arithmetic operations on it.
    // coverity[autosar_cpp14_a13_5_2_violation]
    constexpr operator std::uint8_t() const
    {
        return repr;
    }
    // NOLINTEND(google-explicit-constructor): see above for detailed explanation
  private:
    static constexpr std::uint8_t mask{0b0000'1111U};
    std::uint8_t repr;
};

// Change to std::byte in C++17
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
// coverity[autosar_cpp14_m3_2_3_violation]
class Byte
{
  public:
    /// \brief Constructs a Byte taking two half-bytes and concatenating them
    ///
    /// \param upper_half Representing bits 4 to 7
    /// \param lower_half Representing bits 0 to 3
    ///
    /// \details For example 0b00001101 as upper_half. 0b00001001 as lower_half. Would result in: 0b11011001
    constexpr Byte(const HalfByte upper_half, const HalfByte lower_half)
        : repr{static_cast<std::uint8_t>(static_cast<std::uint8_t>(static_cast<std::uint8_t>(upper_half) << 4U) ^
                                         static_cast<std::uint8_t>(lower_half))}
    {
    }

    constexpr explicit Byte(const std::uint8_t value) : repr{value} {}
    // NOLINTBEGIN(google-explicit-constructor): No potentially dangerous type conversion
    // AUTOSAR Rule A13-5-2 prohibits implicit user defined conversion operators to avoid potential errors in type
    // conversion. But we do not convert underlying type (repr_ is std::uint8_t and return type is std::uint8_t).
    // Implicit 'decay' to std::uint8_t is needed as current Byte design permits arithmetic operations on it.
    // coverity[autosar_cpp14_a13_5_2_violation]
    constexpr operator std::uint8_t() const
    {
        return repr;
    }
    // NOLINTEND(google-explicit-constructor): see above for detailed explanation

    /// \brief Obtain the upper half of the byte as a HalfByte type
    ///
    /// \return A HalfByte. For byte 0b11010010, will return 0b00001101
    constexpr HalfByte UpperHalfByte() const
    {
        return HalfByte{static_cast<std::uint8_t>(static_cast<std::uint32_t>(repr) >> 4U)};
    }

    /// \brief Obtain the lower half of the byte as a HalfByte type
    ///
    /// \return A HalfByte. For byte 0b11010010, will return 0b00000010
    constexpr HalfByte LowerHalfByte() const
    {
        return HalfByte{repr};
    }

  private:
    std::uint8_t repr;
};

// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
template <typename T>
// coverity[autosar_cpp14_m3_2_3_violation]
constexpr bool InRange(const T&, std::size_t position)
{
    static_assert(std::is_integral<T>::value, "T is not integral. Bit manipulation would be undefined");
    if (position >= (sizeof(T) * static_cast<std::size_t>(CHAR_BIT)))
    {
        return false;
    }
    return true;
}

/// \brief Will set a bit at given position to 1 (true), in a given value
///
/// \tparam T The integral type where the bit shall be set
/// \param value The value that shall be manipulated
/// \param position The position of the bit (starting at 0) that shall be set
/// \return true, if position was within bounds of value. false otherwise
///
/// \details For value 0b00000000, when setting position 1, the value will be 0b00000010

// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
// coverity[autosar_cpp14_m3_2_3_violation]
template <typename T>
constexpr bool SetBit(T& value, const std::size_t position) noexcept
{
    static_assert(sizeof(T) <= sizeof(std::uint64_t), "T must not exceed std::uint64_t");
    if (!InRange(value, position))
    {
        return false;
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
    // loss." In this context, there is no data loss because "value" is promoted to std::uint64_t for the bitwise
    // operation. The result is then cast back to its original type "T", ensuring the integrity of the original data.
    // coverity[autosar_cpp14_a4_7_1_violation]
    value = static_cast<T>(static_cast<std::uint64_t>(value) | (static_cast<std::uint64_t>(1U) << position));
    return true;
}

/// \brief Will set a bit at given position to 0 (false), in a given value
///
/// \tparam T The integral type where the bit shall be set
/// \param value The value that shall be manipulated
/// \param position The position of the bit (starting at 0) that shall be cleared
/// \return true, if position was within bounds of value. false otherwise
///
/// \details For value 0b00000010, when setting position 1, the value will be 0b00000000
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
// coverity[autosar_cpp14_m3_2_3_violation]
template <typename T>
constexpr bool ClearBit(T& value, const std::size_t position) noexcept
{
    static_assert(sizeof(T) <= sizeof(std::uint64_t), "T must not exceed std::uint64_t");
    if (!InRange(value, position))
    {
        return false;
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
    // loss." In this context, there is no data loss because "value" is promoted to std::uint64_t for the bitwise
    // operation. The result is then cast back to its original type "T", ensuring the integrity of the original data.
    // coverity[autosar_cpp14_a4_7_1_violation]
    value = static_cast<T>(static_cast<std::uint64_t>(value) & ~(static_cast<std::uint64_t>(1U) << position));
    return true;
}

/// \brief Will toggle (0->1 ; 1->0) a bit at given position, in a given value
///
/// \tparam T The integral type where the bit shall be set
/// \param value The value that shall be manipulated
/// \param position The position of the bit (starting at 0) that shall be toggled
/// \return true, if position was within bounds of value. false otherwise
///
/// \details For value 0b00000100, when setting position 1 and 2, the value will be 0b00000010
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
// coverity[autosar_cpp14_m3_2_3_violation]
template <typename T>
constexpr bool ToggleBit(T& value, const std::size_t position) noexcept
{
    static_assert(sizeof(T) <= sizeof(std::uint64_t), "T must not exceed std::uint64_t");
    if (!InRange(value, position))
    {
        return false;
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
    // loss." In this context, there is no data loss because "value" is promoted to std::uint64_t for the bitwise
    // operation. The result is then cast back to its original type "T", ensuring the integrity of the original data.
    // coverity[autosar_cpp14_a4_7_1_violation]
    value = static_cast<T>(static_cast<std::uint64_t>(value) ^ (static_cast<std::uint64_t>(1U) << position));
    return true;
}

/// \brief Will check if a bit at a given position is of value 1 (true)
///
/// \tparam T The integral type where the bit shall be set
/// \param value The value that shall be manipulated
/// \param position The position of the bit (starting at 0) that shall be checked
/// \return true, if position had value 1. false otherwise
///
/// \details For value 0b00000010, when querying position 1, the result would be true
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
// coverity[autosar_cpp14_m3_2_3_violation]
template <typename T>
constexpr bool CheckBit(const T& value, const std::size_t position) noexcept
{
    static_assert(sizeof(T) <= sizeof(std::uint64_t), "T must not exceed std::uint64_t");
    if (!InRange(value, position))
    {
        return false;
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
    // loss." In this context, there is no data loss because "value" is promoted to std::uint64_t for the bitwise
    // operation. The result is then checked for non-zero, ensuring the integrity of the original data.
    // coverity[autosar_cpp14_a4_7_1_violation]
    return 0U != ((static_cast<std::uint64_t>(value) >> position) & static_cast<std::uint64_t>(1U));
}

// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file."
// This is a false positive. The declarations are present in same file.
// coverity[autosar_cpp14_m3_2_3_violation]
template <std::size_t position, typename T>
constexpr std::uint8_t GetByte(const T raw_bytes) noexcept
{
    constexpr auto NUMBER_OF_BIT_IN_BYTE = 8U;
    static_assert(position < sizeof(T), "Position is not included within raw_bytes");
    static_assert(std::is_integral<T>::value == true, "T must be native integer arithmetic type");
    static_assert(sizeof(T) <= sizeof(std::uint64_t), "T must not exceed std::uint64_t");
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
    // loss." In this context, data loss is intentional because the goal of this function is to extract only a specific
    // byte from "raw_bytes". The value is promoted to std::uint64_t for the bitwise operation and then cast to
    // std::uint8_t to provide the desired byte.
    // coverity[autosar_cpp14_a4_7_1_violation]
    return static_cast<std::uint8_t>(static_cast<std::uint64_t>(raw_bytes) >> (NUMBER_OF_BIT_IN_BYTE * position)) &
           0xFFU;
}

}  // namespace platform
}  // namespace score

#endif
