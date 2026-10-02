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

#ifndef SCORE_LIB_JSON_INTERNAL_MODEL_NUMBER_H
#define SCORE_LIB_JSON_INTERNAL_MODEL_NUMBER_H

#include "score/json/internal/model/error.h"
#include "score/result/result.h"

#include <variant>

namespace score
{
namespace json
{

using ArithmeticType = std::variant<std::uint8_t,
                                    std::uint16_t,
                                    std::uint32_t,
                                    std::uint64_t,
                                    std::int8_t,
                                    std::int16_t,
                                    std::int32_t,
                                    std::int64_t,
                                    float,
                                    double>;

/// \brief Represents a JSON number
///
/// \details enables the conversion from a JSON number string to a C++ arithmetic type.
/// Supported arithmetic types:
/// - Unsigned integers: uint64_t, uint32_t, uint16_t, uint8_t
/// - Signed integers: int64_t, int32_t, int16_t, int8_t
/// - Floating point: float, double
/// - Bool
/// Note: The number is parsed accordingly to the requested type on every call to As(). In the future, a fast path by
/// caching the result in the private state could be introduced if necessary.
// Rationale: False positive. Number is declared only once.
// coverity[autosar_cpp14_m3_2_3_violation]
class Number
{
  public:
    explicit Number(const ArithmeticType& value);

    template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value, bool>>
    score::Result<T> As() const noexcept;

    friend bool operator==(const Number& lhs, const Number& rhs) noexcept;

  private:
    ArithmeticType value_;
};

// Explicit instantiation declaration for TryLosslessCast() to meet MISRA requirements.
extern template score::Result<std::uint64_t> score::json::Number::As() const noexcept;
extern template score::Result<std::uint32_t> score::json::Number::As() const noexcept;
extern template score::Result<std::uint16_t> score::json::Number::As() const noexcept;
extern template score::Result<std::uint8_t> score::json::Number::As() const noexcept;
extern template score::Result<std::int64_t> score::json::Number::As() const noexcept;
extern template score::Result<std::int32_t> score::json::Number::As() const noexcept;
extern template score::Result<std::int16_t> score::json::Number::As() const noexcept;
extern template score::Result<std::int8_t> score::json::Number::As() const noexcept;
extern template score::Result<float> score::json::Number::As() const noexcept;
extern template score::Result<double> score::json::Number::As() const noexcept;
extern template score::Result<bool> score::json::Number::As() const noexcept;

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_MODEL_NUMBER_H
