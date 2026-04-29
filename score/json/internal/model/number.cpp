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

#include "number.h"

#include "score/json/internal/model/lossless_cast.h"

#include <cstdint>
#include <limits>

#include "score/overload.hpp"
#include <score/assert.hpp>
#include <cmath>

namespace score
{
namespace json
{

class VariantComparator
{
  public:
    explicit VariantComparator(const Number& lhs) : lhs_(lhs) {}
    template <typename T, std::enable_if_t<(!std::is_same_v<T, float>) && (!std::is_same_v<T, double>), bool> = true>
    bool operator()(const T& rhs) const noexcept
    {
        // LCOV_EXCL_BR_START Failed branch is unreachable since types are compared before
        if (const auto result = lhs_.As<T>(); result.has_value())
        // LCOV_EXCL_BR_STOP
        {
            return rhs == *result;
        }
        return false;  // LCOV_EXCL_LINE Unreachable since types are compared before
    }

    template <typename T, std::enable_if_t<(std::is_same_v<T, float> || std::is_same_v<T, double>), bool> = true>
    bool operator()(const T& rhs) const noexcept
    {
        // LCOV_EXCL_BR_START Failed branch is unreachable since types are compared before
        if (const auto result = lhs_.As<T>(); result.has_value())
        // LCOV_EXCL_BR_STOP
        {
            return std::abs(rhs - *result) < std::numeric_limits<T>::epsilon();
        }
        return false;  // LCOV_EXCL_LINE Unreachable since types are compared before
    }

  private:
    Number lhs_;
};

Number::Number(const ArithmeticType& value) : value_{value} {}

// std::visit throws std::bad_variant_access if as-variant(value_i).valueless_by_exception() is true
// this case is already handled inside an assertion
// coverity[autosar_cpp14_a15_5_3_violation]
bool operator==(const Number& lhs, const Number& rhs) noexcept
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!lhs.value_.valueless_by_exception());
    if (lhs.value_.index() != rhs.value_.index())
    {
        return false;  // The variants hold different types
    }
    return std::visit(VariantComparator{rhs}, lhs.value_);
}

template <typename T, class>
// std::visit throws std::bad_variant_access if as-variant(value_i).valueless_by_exception() is true
// this case is already handled inside an assertion
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<T> Number::As() const noexcept
{
    //  we are suppressing clang format here that results in coverity violation:
    //  "AUTOSAR C++14 A7-1-7" rule finding. This rule states: "Each expression
    //  statement and identifier declaration shall be placed on a separate line."
    // clang-format off

    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!value_.valueless_by_exception());
    return std::visit(
        score::cpp::overload(
            [](const std::uint8_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const std::uint16_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const std::uint32_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const std::uint64_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const std::int8_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const std::int16_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const std::int32_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const std::int64_t v) {
                return TryLosslessCast<T>(v);
            },
            [](const float v) {
                return TryLosslessCast<T>(v);
            },
            [](const double v) {
                return TryLosslessCast<T>(v);
            }
            ),
        value_
    );
    // clang-format on
}

template score::Result<std::uint64_t> Number::As() const noexcept;
template score::Result<std::uint32_t> Number::As() const noexcept;
template score::Result<std::uint16_t> Number::As() const noexcept;
template score::Result<std::uint8_t> Number::As() const noexcept;

template score::Result<std::int64_t> Number::As() const noexcept;
template score::Result<std::int32_t> Number::As() const noexcept;
template score::Result<std::int16_t> Number::As() const noexcept;
template score::Result<std::int8_t> Number::As() const noexcept;

template score::Result<float> Number::As() const noexcept;
template score::Result<double> Number::As() const noexcept;

template score::Result<bool> Number::As() const noexcept;

}  // namespace json
}  // namespace score
