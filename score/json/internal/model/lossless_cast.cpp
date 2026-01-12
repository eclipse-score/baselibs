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

#include "lossless_cast.h"

#include "score/json/internal/model/error.h"

#include <score/assert.hpp>

#include <cfloat>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace
{

template <typename Output, typename Input, typename = std::enable_if_t<std::is_same<Input, Output>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
// coverity[autosar_cpp14_a0_1_3_violation: FALSE]  it's used in the same file here
constexpr bool IsInputTypeContainedInOutputType()
{
    // Same type is fine.
    return true;
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Output>::value, bool>,
          typename = std::enable_if_t<std::is_signed<Input>::value == std::is_signed<Output>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr bool IsInputTypeContainedInOutputType()
{
    // Output and Input are both unsigned or both signed integers.
    return std::numeric_limits<Output>::max() > std::numeric_limits<Input>::max();
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Output>::value, bool>,
          typename = std::enable_if_t<std::is_signed<Input>::value != std::is_signed<Output>::value, bool>,
          typename = std::enable_if_t<!std::is_signed<Input>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr bool IsInputTypeContainedInOutputType()
{
    // Input is unsigned integer, output is signed integer.
    // This is OK iff the output type is greater than the input type.
    return static_cast<uint64_t>(std::numeric_limits<Output>::max()) > std::numeric_limits<Input>::max();
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Output>::value, bool>,
          typename = std::enable_if_t<std::is_signed<Input>::value != std::is_signed<Output>::value, bool>,
          typename = std::enable_if_t<std::is_signed<Input>::value, bool>,
          typename = std::enable_if_t<!std::is_signed<Output>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr bool IsInputTypeContainedInOutputType()
{
    // Input is signed, Output is unsigned.
    // Negatives integers cannot be represented in any unsigned Output type.
    return false;
}

template <typename FloatingPoint, typename = std::enable_if_t<std::is_same<FloatingPoint, float>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr std::uint64_t GetMaximumFloatingPointInteger()
{
    static_assert(std::numeric_limits<float>::radix == 2, "only supported binary float radix");
    static_assert(std::numeric_limits<float>::digits < 64, "Shift count exceeds the width of uint64_t");
    return static_cast<uint64_t>(static_cast<uint64_t>(1) << static_cast<uint8_t>(std::numeric_limits<float>::digits));
}

template <typename FloatingPoint, typename = std::enable_if_t<std::is_same<FloatingPoint, float>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr std::int64_t GetMinimumFloatingPointInteger()
{
    static_assert(std::numeric_limits<float>::radix == 2, "only supported binary float radix");
    // The shifting appied to unsigned long.
    // coverity[autosar_cpp14_m5_0_10_violation: FALSE]
    return -(static_cast<int64_t>(1UL << static_cast<uint32_t>(std::numeric_limits<float>::digits)));
}

template <typename FloatingPoint,
          typename = std::enable_if_t<!std::is_same<FloatingPoint, float>::value, bool>,
          typename = std::enable_if_t<std::is_same<FloatingPoint, double>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr std::uint64_t GetMaximumFloatingPointInteger()
{
    static_assert(std::numeric_limits<float>::radix == 2, "only supported binary float radix");
    static_assert(std::numeric_limits<float>::digits < 64, "Shift count exceeds the width of uint64_t");
    return static_cast<uint64_t>(static_cast<uint64_t>(1) << static_cast<uint8_t>(std::numeric_limits<double>::digits));
}

template <typename FloatingPoint,
          typename = std::enable_if_t<!std::is_same<FloatingPoint, float>::value, bool>,
          typename = std::enable_if_t<std::is_same<FloatingPoint, double>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr std::int64_t GetMinimumFloatingPointInteger()
{
    static_assert(std::numeric_limits<float>::radix == 2, "only supported binary float radix");
    static_assert(std::numeric_limits<float>::digits < 64, "Shift count exceeds the width of uint64_t");
    return -(static_cast<int64_t>(
        static_cast<uint64_t>(static_cast<uint64_t>(1) << static_cast<uint8_t>(std::numeric_limits<double>::digits))));
}

template <typename Output,
          typename Input,
          typename a = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename b = std::enable_if_t<std::is_integral<Input>::value, bool>,
          typename c = std::enable_if_t<!std::is_integral<Output>::value, bool>,
          bool = false,
          bool = false,
          bool = false>
// The compiler will not be confused here it's a template specialization
// coverity[autosar_cpp14_a2_10_4_violation: FASLE]
constexpr bool IsInputTypeContainedInOutputType()
{
    // Input is integral, Output is float.

    // Check that the minimum integer value is contained in float
    constexpr std::int64_t min_value_without_precision_loss = GetMinimumFloatingPointInteger<Output>();
    if (min_value_without_precision_loss > static_cast<int64_t>(std::numeric_limits<Input>::min()))
    {
        return false;
    }

    // Check that the maximum integer value is contained in float
    constexpr std::uint64_t max_value_without_precision_loss = GetMaximumFloatingPointInteger<Output>();
    if (max_value_without_precision_loss < static_cast<uint64_t>(std::numeric_limits<Input>::max()))
    {
        return false;
    }

    // Minimum and Maximum value of the Input integer type can be represented identically in the floating point type.
    return true;
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<std::is_same<Input, double>::value, bool>,
          typename = std::enable_if_t<std::is_same<Output, float>::value, bool>,
          bool = false,
          bool = false>
// The compiler will not be confused be here it's a template specialization
// The function is used inside TryLosslessCast()
// coverity[autosar_cpp14_a2_10_4_violation: FASLE]
// coverity[autosar_cpp14_a0_1_3_violation]
constexpr bool IsInputTypeContainedInOutputType()
{
    // Input is double, but Output is float.
    return false;
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<std::is_same<Input, float>::value, bool>,
          typename = std::enable_if_t<std::is_same<Output, double>::value, bool>,
          bool = false,
          bool = false,
          bool = false,
          bool = false,
          bool = false>
// The compiler will not be confused here it's a template specialization
// The function is used inside TryLosslessCast()
// coverity[autosar_cpp14_a2_10_4_violation: FASLE]
// coverity[autosar_cpp14_a0_1_3_violation]
constexpr bool IsInputTypeContainedInOutputType()
{
    // Input is float, but Output is double.
    return true;
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<!std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Output>::value, bool>,
          bool = false>
// The compiler will not be confused here it's a template specialization.
// The function is used inside TryLosslessCast()
// coverity[autosar_cpp14_a2_10_4_violation: FASLE]
// coverity[autosar_cpp14_a0_1_3_violation]
constexpr bool IsInputTypeContainedInOutputType()
{
    // Input is floating point, output is integer
    return false;
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<!std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Output>::value, bool>>
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// The function is used inside TryLosslessCast()
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
// coverity[autosar_cpp14_a0_1_3_violation]
constexpr bool IsFloatingPointValueContainedInOutputType(Input input)
{
    // Input is a floating point number and output is an integer.
    // Here we actually have to check the concrete value at runtime if a cast results in a precision loss.

    // TODO TicketOld-76807: The implementation below will be obsolete and can be replaced by `return false;` when
    // TicketOld-76807 is resolved.

    // The set of integers that can be represented in a float is determined by the number of bits in the mantissa.
    // For single-precision float this is 23 bits.
    // Due to the implicit 1 prepended to the mantissa in IEEE-754, the effective number of bits is 24 (==
    // FLT_MANT_DIG).
    // The floating point value is determined by the formula: Sign * 2^exponent * mantissa.
    // The value of the exponent is `exponent=E-127` where E corresponds to the exponent bits as defined in IEEE-754.
    // By setting the E=127 we get the effective set of integers according to the formula: {-2^24, -2^24 + 1, ..., 2^24}
    // Note: Beyond this set there are more edge cases of floating point values that represent exactly an integer. Here
    // we restrict us to this set to avoid more expensive checks at runtime.

    // First we need to check if the floating point value is within the range of the integer type.
    // This ensures that the floating point to integer cast is defined behavior.
    // We also discard floating point values that exceed the range in that integers can be represented without precision
    // loss.
    constexpr auto max_input = static_cast<Input>(
        std::min(GetMaximumFloatingPointInteger<Input>(), static_cast<uint64_t>(std::numeric_limits<Output>::max())));

    constexpr auto min_input = static_cast<Input>(
        std::max(GetMinimumFloatingPointInteger<Input>(), static_cast<int64_t>(std::numeric_limits<Output>::min())));

    if ((max_input < input) || (min_input > input))
    {
        // The floating point value is out of the integer type range hence a cast to the integer type would be undefined
        // behavior.
        return false;
    }

    // With the cast to integer we discard the fractional part of the floating point value.
    // We then use a back-to-back check if the output value is identical to the input value.
    const auto input_casted = static_cast<Output>(input);
    const auto input_backcast = static_cast<Input>(input_casted);

    // Only allow the cast iff the back cast yields exactly the same value.
    return (std::equal_to<Input>{}(input, input_backcast));
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<std::is_same<Output, double>::value, bool>,
          typename = std::enable_if_t<std::is_same<Input, float>::value, bool>>
// LCOV_EXCL_START
/* This coverage exclusion is needed as this template func will never be called,
 * since we are always storing our floating-point numbers as a 'double'
 * it's impossible to have this function to be called with 'float' as an Input.
 * But we can't remove this template func specialization entirely
 * as TryLosslessCast specialization wouldn't compile anymore! */
// Discussed during review.Suppressed instead of void casting
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr bool IsFloatingPointValueContainedInOutputType(float)
{
    SCORE_LANGUAGE_FUTURECPP_UNREACHABLE_MESSAGE("Input is float and Output is double - impossible case");
    // Output is double and input is float (impossible case).
    // It is OK to cast a float into a double.
    // Unreachable code exists to prevent a void cast and be specific about the behavior
    // coverity[autosar_cpp14_m0_1_1_violation]
    return true;
}
// LCOV_EXCL_STOP

template <typename FloatingPoint>
constexpr bool IsInf(FloatingPoint value)
{
    static_assert(std::numeric_limits<FloatingPoint>::has_infinity, "value is supposed to be float or double");
    // The '>=' and '<=' operators are used instead of '==' because the compiler raises a build error:
    // "Comparing floating point with == or != is unsafe." Although '==' would be a more appropriate
    // choice in this case (since we are comparing against a specific constant rather than general
    // floating-point values), this approach is necessary to bypass the compiler's restrictions.
    return (value >= std::numeric_limits<FloatingPoint>::infinity()) ||
           (value <= -std::numeric_limits<FloatingPoint>::infinity());
}

template <typename Output,
          typename Input,
          typename = std::enable_if_t<std::is_same<Output, float>::value, bool>,
          typename = std::enable_if_t<std::is_same<Input, double>::value, bool>>
// Discussed during review.Suppressed instead of void casting
// coverity[autosar_cpp14_a2_10_4_violation: FALSE] it's a template specialization
// coverity[autosar_cpp14_a7_1_8_violation: FALSE]  constexpr is in correct order
constexpr bool IsFloatingPointValueContainedInOutputType(double input)
{
    // Output is float and input is double.
    // It is OK to cast a double into a float at the user's request
    // if the number can fit into float type.
    if (IsInf(static_cast<Output>(input)) && !(IsInf(input)))
    {
        return false;
    }
    return true;
}

}  // namespace

template <typename Output, typename Input, class, class, class>
score::Result<Output> score::json::TryLosslessCast(Input input)
{
    // Overload for the case both types are the same.
    static_assert(std::is_same<Input, Output>::value, "must be same");

    return input;
}

template <typename Output, typename Input, class, class, class, class, class>
score::Result<Output> score::json::TryLosslessCast(Input input)
{
    // Overload for the case Input is integer.
    static_assert(std::is_integral<Input>::value, "input must be integer");
    static_assert(std::is_arithmetic<Input>::value, "output must be integer or float");

    if (IsInputTypeContainedInOutputType<Output, Input>())
    {
        return static_cast<Output>(input);
    }
    // Operands do not contain binary operators.
    // coverity[autosar_cpp14_a5_2_6_violation: FALSE]
    else if (std::is_integral<Output>::value && std::is_signed<Output>::value && std::is_unsigned<Input>::value)
    {
        // Justification: The parser will always choose the smallest unsigned type if applicable.
        // If the value is negative, the parser will return the smallest signed type.
        // Here we need to handle the case, where the document contains a non-negative value, but the user expects a
        // signed value.
        // coverity[constant_expression_result]
        if (static_cast<uint64_t>(std::numeric_limits<Output>::max()) >= static_cast<uint64_t>(input))
        {
            // The non-negative input value is contained in the signed output integer, so that the cast is identical.
            return static_cast<Output>(input);
        }
    }
    else
    {
        // Needed for MISRA
    }

    return score::MakeUnexpected(score::json::Error::kWrongType);
}

template <typename Output, typename Input, class, class, class, class, class, class>
score::Result<Output> score::json::TryLosslessCast(Input input)
{
    // Overload for the case Input is floating point.
    static_assert(!std::is_integral<Input>::value, "input must be floating point");

    if (IsFloatingPointValueContainedInOutputType<Output, Input>(input) == true)
    {
        return static_cast<Output>(input);
    }

    return score::MakeUnexpected(score::json::Error::kWrongType);
}

template <typename Output, typename Input, class, class, class, class, bool, bool, bool>
score::Result<Output> score::json::TryLosslessCast(Input input)
{
    // Overload for the case Output is bool.
    static_assert(std::is_same<Output, bool>::value, "Output must be bool");

    if (std::equal_to<Input>{}(input, static_cast<Input>(0)) == true)
    {
        return false;
    }
    else if (std::equal_to<Input>{}(input, static_cast<Input>(1)) == true)
    {
        return true;
    }
    else
    {
        // Needed for MISRA
    }

    return score::MakeUnexpected(score::json::Error::kWrongType);
}

namespace score
{
namespace json
{
// Output: int8_t
template score::Result<std::int8_t> TryLosslessCast(std::int8_t);
template score::Result<std::int8_t> TryLosslessCast(std::int16_t);
template score::Result<std::int8_t> TryLosslessCast(std::int32_t);
template score::Result<std::int8_t> TryLosslessCast(std::int64_t);
template score::Result<std::int8_t> TryLosslessCast(std::uint8_t);
template score::Result<std::int8_t> TryLosslessCast(std::uint16_t);
template score::Result<std::int8_t> TryLosslessCast(std::uint32_t);
template score::Result<std::int8_t> TryLosslessCast(std::uint64_t);
template score::Result<std::int8_t> TryLosslessCast(float);
template score::Result<std::int8_t> TryLosslessCast(double);

// Output: int16_t
template score::Result<std::int16_t> TryLosslessCast(std::int8_t);
template score::Result<std::int16_t> TryLosslessCast(std::int16_t);
template score::Result<std::int16_t> TryLosslessCast(std::int32_t);
template score::Result<std::int16_t> TryLosslessCast(std::int64_t);
template score::Result<std::int16_t> TryLosslessCast(std::uint8_t);
template score::Result<std::int16_t> TryLosslessCast(std::uint16_t);
template score::Result<std::int16_t> TryLosslessCast(std::uint32_t);
template score::Result<std::int16_t> TryLosslessCast(std::uint64_t);
template score::Result<std::int16_t> TryLosslessCast(float);
template score::Result<std::int16_t> TryLosslessCast(double);

// Output: int32_t
template score::Result<std::int32_t> TryLosslessCast(std::int8_t);
template score::Result<std::int32_t> TryLosslessCast(std::int16_t);
template score::Result<std::int32_t> TryLosslessCast(std::int32_t);
template score::Result<std::int32_t> TryLosslessCast(std::int64_t);
template score::Result<std::int32_t> TryLosslessCast(std::uint8_t);
template score::Result<std::int32_t> TryLosslessCast(std::uint16_t);
template score::Result<std::int32_t> TryLosslessCast(std::uint32_t);
template score::Result<std::int32_t> TryLosslessCast(std::uint64_t);
template score::Result<std::int32_t> TryLosslessCast(float);
template score::Result<std::int32_t> TryLosslessCast(double);

// Output: int64_t
template score::Result<std::int64_t> TryLosslessCast(std::int8_t);
template score::Result<std::int64_t> TryLosslessCast(std::int16_t);
template score::Result<std::int64_t> TryLosslessCast(std::int32_t);
template score::Result<std::int64_t> TryLosslessCast(std::int64_t);
template score::Result<std::int64_t> TryLosslessCast(std::uint8_t);
template score::Result<std::int64_t> TryLosslessCast(std::uint16_t);
template score::Result<std::int64_t> TryLosslessCast(std::uint32_t);
template score::Result<std::int64_t> TryLosslessCast(std::uint64_t);
template score::Result<std::int64_t> TryLosslessCast(float);
template score::Result<std::int64_t> TryLosslessCast(double);

// Output: uint8_t
template score::Result<std::uint8_t> TryLosslessCast(std::int8_t);
template score::Result<std::uint8_t> TryLosslessCast(std::int16_t);
template score::Result<std::uint8_t> TryLosslessCast(std::int32_t);
template score::Result<std::uint8_t> TryLosslessCast(std::int64_t);
template score::Result<std::uint8_t> TryLosslessCast(std::uint8_t);
template score::Result<std::uint8_t> TryLosslessCast(std::uint16_t);
template score::Result<std::uint8_t> TryLosslessCast(std::uint32_t);
template score::Result<std::uint8_t> TryLosslessCast(std::uint64_t);
template score::Result<std::uint8_t> TryLosslessCast(float);
template score::Result<std::uint8_t> TryLosslessCast(double);

// Output: uint16_t
template score::Result<std::uint16_t> TryLosslessCast(std::int8_t);
template score::Result<std::uint16_t> TryLosslessCast(std::int16_t);
template score::Result<std::uint16_t> TryLosslessCast(std::int32_t);
template score::Result<std::uint16_t> TryLosslessCast(std::int64_t);
template score::Result<std::uint16_t> TryLosslessCast(std::uint8_t);
template score::Result<std::uint16_t> TryLosslessCast(std::uint16_t);
template score::Result<std::uint16_t> TryLosslessCast(std::uint32_t);
template score::Result<std::uint16_t> TryLosslessCast(std::uint64_t);
template score::Result<std::uint16_t> TryLosslessCast(float);
template score::Result<std::uint16_t> TryLosslessCast(double);

// Output: uint32_t
template score::Result<std::uint32_t> TryLosslessCast(std::int8_t);
template score::Result<std::uint32_t> TryLosslessCast(std::int16_t);
template score::Result<std::uint32_t> TryLosslessCast(std::int32_t);
template score::Result<std::uint32_t> TryLosslessCast(std::int64_t);
template score::Result<std::uint32_t> TryLosslessCast(std::uint8_t);
template score::Result<std::uint32_t> TryLosslessCast(std::uint16_t);
template score::Result<std::uint32_t> TryLosslessCast(std::uint32_t);
template score::Result<std::uint32_t> TryLosslessCast(std::uint64_t);
template score::Result<std::uint32_t> TryLosslessCast(float);
template score::Result<std::uint32_t> TryLosslessCast(double);

// Output: uint64_t
template score::Result<std::uint64_t> TryLosslessCast(std::int8_t);
template score::Result<std::uint64_t> TryLosslessCast(std::int16_t);
template score::Result<std::uint64_t> TryLosslessCast(std::int32_t);
template score::Result<std::uint64_t> TryLosslessCast(std::int64_t);
template score::Result<std::uint64_t> TryLosslessCast(std::uint8_t);
template score::Result<std::uint64_t> TryLosslessCast(std::uint16_t);
template score::Result<std::uint64_t> TryLosslessCast(std::uint32_t);
template score::Result<std::uint64_t> TryLosslessCast(std::uint64_t);
template score::Result<std::uint64_t> TryLosslessCast(float);
template score::Result<std::uint64_t> TryLosslessCast(double);

// Output: float
template score::Result<float> TryLosslessCast(std::int8_t);
template score::Result<float> TryLosslessCast(std::int16_t);
template score::Result<float> TryLosslessCast(std::int32_t);
template score::Result<float> TryLosslessCast(std::int64_t);
template score::Result<float> TryLosslessCast(std::uint8_t);
template score::Result<float> TryLosslessCast(std::uint16_t);
template score::Result<float> TryLosslessCast(std::uint32_t);
template score::Result<float> TryLosslessCast(std::uint64_t);
template score::Result<float> TryLosslessCast(float);
template score::Result<float> TryLosslessCast(double);

// Output: double
template score::Result<double> TryLosslessCast(std::int8_t);
template score::Result<double> TryLosslessCast(std::int16_t);
template score::Result<double> TryLosslessCast(std::int32_t);
template score::Result<double> TryLosslessCast(std::int64_t);
template score::Result<double> TryLosslessCast(std::uint8_t);
template score::Result<double> TryLosslessCast(std::uint16_t);
template score::Result<double> TryLosslessCast(std::uint32_t);
template score::Result<double> TryLosslessCast(std::uint64_t);
template score::Result<double> TryLosslessCast(float);
template score::Result<double> TryLosslessCast(double);

// Output: bool
template score::Result<bool> TryLosslessCast(std::int8_t);
template score::Result<bool> TryLosslessCast(std::int16_t);
template score::Result<bool> TryLosslessCast(std::int32_t);
template score::Result<bool> TryLosslessCast(std::int64_t);
template score::Result<bool> TryLosslessCast(std::uint8_t);
template score::Result<bool> TryLosslessCast(std::uint16_t);
template score::Result<bool> TryLosslessCast(std::uint32_t);
template score::Result<bool> TryLosslessCast(std::uint64_t);
template score::Result<bool> TryLosslessCast(float);
template score::Result<bool> TryLosslessCast(double);

}  // namespace json
}  // namespace score

namespace compile_time_tests
{
// This symbol does not have external linkage
// coverity[autosar_cpp14_a3_3_1_violation: FALSE]
constexpr void TestIsInputTypeContainedInOutputType()
{
    static_assert(IsInputTypeContainedInOutputType<int8_t, int8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, int16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, int32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, uint8_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, uint16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, uint32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int8_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<int16_t, int8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, int16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, int32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, uint16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, uint32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int16_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<int32_t, int8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, int16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, int32_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, uint16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, uint32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int32_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<int64_t, int8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, int16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, int32_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, int64_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, uint16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, uint32_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<int64_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<uint8_t, int8_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, int16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, int32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, uint16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, uint32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint8_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<uint16_t, int8_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, int16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, int32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, uint16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, uint32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint16_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<uint32_t, int8_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, int16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, int32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, uint16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, uint32_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint32_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<uint64_t, int8_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, int16_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, int32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, uint16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, uint32_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, uint64_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, float>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<uint64_t, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<float, int8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<float, int16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<float, int32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<float, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<float, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<float, uint16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<float, uint32_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<float, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<float, float>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<float, double>() == false, "");

    static_assert(IsInputTypeContainedInOutputType<double, int8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<double, int16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<double, int32_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<double, int64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<double, uint8_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<double, uint16_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<double, uint32_t>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<double, uint64_t>() == false, "");
    static_assert(IsInputTypeContainedInOutputType<double, float>() == true, "");
    static_assert(IsInputTypeContainedInOutputType<double, double>() == true, "");
}

// This symbol does not have external linkage
// coverity[autosar_cpp14_a3_3_1_violation: FALSE]
constexpr void TestIsFloatingPointValueContainedInOutputType()
{
    // Input: float
    static_assert(IsFloatingPointValueContainedInOutputType<uint8_t, float>(255.0F) == true, "");
    static_assert(IsFloatingPointValueContainedInOutputType<uint16_t, float>(65535.0F) == true, "");
    static_assert(IsFloatingPointValueContainedInOutputType<uint32_t, float>(
                      1UL << static_cast<uint32_t>(std::numeric_limits<float>::digits)) == true,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<uint64_t, float>(
                      1UL << static_cast<uint32_t>(std::numeric_limits<float>::digits)) == true,
                  "");

    static_assert(IsFloatingPointValueContainedInOutputType<int8_t, float>(std::numeric_limits<int8_t>::min()) == true,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<int8_t, float>(std::numeric_limits<int8_t>::max()) == true,
                  "");
    static_assert(
        IsFloatingPointValueContainedInOutputType<int16_t, float>(std::numeric_limits<int16_t>::max()) == true, "");
    static_assert(
        IsFloatingPointValueContainedInOutputType<int16_t, float>(std::numeric_limits<int16_t>::min()) == true, "");
    static_assert(IsFloatingPointValueContainedInOutputType<int32_t, float>(
                      1UL << static_cast<uint32_t>(std::numeric_limits<float>::digits)) == true,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<int32_t, float>(-(
                      static_cast<int32_t>(1UL << static_cast<uint32_t>(std::numeric_limits<float>::digits)))) == true,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<int64_t, float>(
                      1UL << static_cast<uint32_t>(std::numeric_limits<float>::digits)) == true,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<int64_t, float>(-(
                      static_cast<int64_t>(1UL << static_cast<uint32_t>(std::numeric_limits<float>::digits)))) == true,
                  "");

    // Input: double
    static_assert(IsFloatingPointValueContainedInOutputType<float, double>(1.0) == true, "");
    static_assert(IsFloatingPointValueContainedInOutputType<float, double>(std::numeric_limits<double>::max()) == false,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<int8_t, double>(std::numeric_limits<int8_t>::min()) == true,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<int8_t, double>(std::numeric_limits<int8_t>::max()) == true,
                  "");
    static_assert(
        IsFloatingPointValueContainedInOutputType<int16_t, double>(std::numeric_limits<int16_t>::max()) == true, "");
    static_assert(
        IsFloatingPointValueContainedInOutputType<int16_t, double>(std::numeric_limits<int16_t>::min()) == true, "");
    static_assert(
        IsFloatingPointValueContainedInOutputType<int32_t, double>(std::numeric_limits<int32_t>::max()) == true, "");
    static_assert(
        IsFloatingPointValueContainedInOutputType<int32_t, double>(std::numeric_limits<int32_t>::min()) == true, "");
    static_assert(IsFloatingPointValueContainedInOutputType<int64_t, double>(
                      1UL << static_cast<uint32_t>(std::numeric_limits<double>::digits)) == true,
                  "");
    static_assert(IsFloatingPointValueContainedInOutputType<int64_t, double>(-(
                      static_cast<int64_t>(1UL << static_cast<uint32_t>(std::numeric_limits<double>::digits)))) == true,
                  "");
}
}  // namespace compile_time_tests
