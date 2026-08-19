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
#ifndef SCORE_LIB_RESULT_EXPECTED_EXTENSIONS_H
#define SCORE_LIB_RESULT_EXPECTED_EXTENSIONS_H

#include "score/result/details/expected/expected.h"

#include <score/expected.hpp>
#include <score/optional.hpp>

#include <optional>
#include <type_traits>

namespace score::details
{

template <typename T, typename E>
auto to_score_future_cpp_expected(const expected<T, E>& expected) -> score::cpp::expected<T, E>
{
    if (expected.has_value())
    {
        return expected.value();
    }
    else
    {
        return score::cpp::unexpected{expected.error()};
    }
}

template <typename T, typename E>
auto to_score_future_cpp_expected(expected<T, E>&& expected) -> score::cpp::expected<T, E>
{
    if (expected.has_value())
    {
        return std::move(expected).value();
    }
    else
    {
        return score::cpp::unexpected{std::move(expected).error()};
    }
}

template <typename T, typename E>
auto to_score_expected(const score::cpp::expected<T, E>& expected) -> score::details::expected<T, E>
{
    if (expected.has_value())
    {
        return expected.value();
    }
    else
    {
        return unexpected{expected.error()};
    }
}

template <typename T, typename E>
auto to_score_expected(score::cpp::expected<T, E>&& expected) -> score::details::expected<T, E>
{
    if (expected.has_value())
    {
        return std::move(expected).value();
    }
    else
    {
        return unexpected{std::move(expected).error()};
    }
}

template <typename T, typename E, typename F, typename = std::enable_if_t<std::is_invocable_v<F, const E&>>>
auto expected_value_to_score_future_cpp_optional_or_else(const expected<T, E>& expected, F&& error_handling)
    -> score::cpp::optional<T>
{
    if (expected.has_value())
    {
        return expected.value();
    }
    else
    {
        std::invoke(std::forward<F>(error_handling), expected.error());
        return score::cpp::nullopt;
    }
}

template <typename T, typename E, typename F, typename = std::enable_if_t<std::is_invocable_v<F, E&&>>>
auto expected_value_to_score_future_cpp_optional_or_else(expected<T, E>&& expected, F&& error_handling)
    -> score::cpp::optional<T>
{
    if (expected.has_value())
    {
        return std::move(expected).value();
    }
    else
    {
        std::invoke(std::forward<F>(error_handling), std::move(expected).error());
        return score::cpp::nullopt;
    }
}

template <typename T, typename E, typename F, typename = std::enable_if_t<std::is_invocable_v<F, const E&>>>
auto expected_value_to_optional_or_else(const expected<T, E>& expected, F&& error_handling) -> std::optional<T>
{
    if (expected.has_value())
    {
        return expected.value();
    }
    else
    {
        std::invoke(std::forward<F>(error_handling), expected.error());
        return std::nullopt;
    }
}

template <typename T, typename E, typename F, typename = std::enable_if_t<std::is_invocable_v<F, E&&>>>
auto expected_value_to_optional_or_else(expected<T, E>&& expected, F&& error_handling) -> std::optional<T>
{
    if (expected.has_value())
    {
        return std::move(expected).value();
    }
    else
    {
        std::invoke(std::forward<F>(error_handling), std::move(expected).error());
        return std::nullopt;
    }
}

}  // namespace score::details

#endif  // SCORE_LIB_RESULT_EXPECTED_EXTENSIONS_H
