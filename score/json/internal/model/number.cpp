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

#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>

#include "score/overload.hpp"
#include <score/assert.hpp>
#include <cmath>

namespace score
{
namespace json
{
namespace
{

static_assert(std::numeric_limits<float>::is_iec559 && std::numeric_limits<double>::is_iec559, "IEEE-754 required");

// Every integer with a magnitude up to this bound is exactly representable in Floating.
template <typename Floating>
constexpr std::uint64_t kMaxExactInteger{std::uint64_t{1} << std::numeric_limits<Floating>::digits};

template <typename Floating>
constexpr bool IsInfinite(const Floating value) noexcept
{
    return (value > std::numeric_limits<Floating>::max()) || (value < std::numeric_limits<Floating>::lowest());
}

// Integer narrowing is rejected by type, except unsigned to signed, which is checked by value.
template <typename Out, typename In>
constexpr bool IsLosslessCast(const In value) noexcept
{
    if constexpr (std::is_same_v<Out, In>)
    {
        return true;
    }
    else if constexpr (std::is_same_v<Out, bool>)
    {
        return std::equal_to<In>{}(value, In{0}) || std::equal_to<In>{}(value, In{1});
    }
    else if constexpr (std::is_integral_v<In> && std::is_integral_v<Out>)
    {
        if constexpr (std::is_signed_v<In> == std::is_signed_v<Out>)
        {
            return std::numeric_limits<Out>::max() > std::numeric_limits<In>::max();
        }
        else if constexpr (std::is_unsigned_v<In>)
        {
            return static_cast<std::uint64_t>(value) <= static_cast<std::uint64_t>(std::numeric_limits<Out>::max());
        }
        else
        {
            return false;
        }
    }
    else if constexpr (std::is_integral_v<In>)
    {
        constexpr auto kLimit = kMaxExactInteger<Out>;
        return (static_cast<std::int64_t>(std::numeric_limits<In>::min()) >= -static_cast<std::int64_t>(kLimit)) &&
               (static_cast<std::uint64_t>(std::numeric_limits<In>::max()) <= kLimit);
    }
    else if constexpr (std::is_integral_v<Out>)
    {
        constexpr auto kMin = static_cast<In>(std::max(-static_cast<std::int64_t>(kMaxExactInteger<In>),
                                                       static_cast<std::int64_t>(std::numeric_limits<Out>::min())));
        constexpr auto kMax = static_cast<In>(
            std::min(kMaxExactInteger<In>, static_cast<std::uint64_t>(std::numeric_limits<Out>::max())));
        // The range check makes the cast below defined and also rejects NaN.
        return (value >= kMin) && (value <= kMax) &&
               std::equal_to<In>{}(static_cast<In>(static_cast<Out>(value)), value);
    }
    else
    {
        // Precision loss and underflow are accepted, overflow to infinity is not.
        return IsInfinite(value) || !IsInfinite(static_cast<Out>(value));
    }
}

template <typename Out, typename In>
score::Result<Out> TryLosslessCast(const In value)
{
    if (IsLosslessCast<Out>(value))
    {
        return static_cast<Out>(value);
    }
    return score::MakeUnexpected(Error::kWrongType);
}

}  // namespace

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
