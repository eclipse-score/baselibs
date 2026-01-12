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
#include "score/result/details/expected/expected.h"

#include "score/result/details/expected/test_types.h"

#include <gtest/gtest.h>

#include <type_traits>

namespace score::details
{
namespace
{

TEST(ExpectedTest, HasValueTypeTypeTrait)
{
    // When wrapping the type as value_type with an expected
    using unit = expected<ValueType, ErrorType>;

    // Expect the expected to have a type trait value_type matching above type
    static_assert(std::is_same_v<unit::value_type, ValueType>);
}

TEST(ExpectedTest, HasErrorTypeTypeTrait)
{
    // When wrapping the type as error_type with an expected
    using unit = expected<ValueType, ErrorType>;

    // Expect the expected to have a type trait error_type matching above type
    static_assert(std::is_same_v<unit::error_type, ErrorType>);
}

TEST(ExpectedTest, HasUnexpectedTypeTypeTrait)
{
    // When wrapping the type as error_type with an expected
    using unit = expected<ValueType, ErrorType>;

    // Expect the expected to have a type trait unexpected_type matching above type wrapped by unexpected
    static_assert(std::is_same_v<unit::unexpected_type, unexpected<ErrorType>>);
}

TEST(ExpectedTest, HasRebindTypeTrait)
{
    struct OtherValueType
    {
    };
    // When wrapping the type as error_type with an expected
    using unit = expected<ValueType, ErrorType>;
    using rebound = unit::rebind<OtherValueType>;

    // Expect the expected to have a type trait rebind that preserves the error_type while exchanging the value_type
    static_assert(std::is_same_v<rebound::value_type, OtherValueType>);
    static_assert(std::is_same_v<rebound::error_type, ErrorType>);
}

TEST(ExpectedVoidTest, HasValueTypeTypeTrait)
{
    // When wrapping the void type with an expected
    using unit = expected<void, ErrorType>;

    // Expect the expected to have a type trait value_type equal to void
    static_assert(std::is_same_v<unit::value_type, void>);
}

TEST(ExpectedVoidTest, HasErrorTypeTypeTrait)
{
    // When wrapping the type as error_type with an expected
    using unit = expected<void, ErrorType>;

    // Expect the expected to have a type trait error_type matching above type
    static_assert(std::is_same_v<unit::error_type, ErrorType>);
}

TEST(ExpectedVoidTest, HasUnexpectedTypeTypeTrait)
{
    // When wrapping the type as error_type with an expected
    using unit = expected<void, ErrorType>;

    // Expect the expected to have a type trait unexpected_type matching above type wrapped by unexpected
    static_assert(std::is_same_v<unit::unexpected_type, unexpected<ErrorType>>);
}

TEST(ExpectedVoidTest, HasRebindTypeTrait)
{
    // When wrapping the type as error_type with an expected
    using unit = expected<void, ErrorType>;
    using rebound = unit::rebind<ValueType>;

    // Expect the expected to have a type trait rebind that preserves the error_type while exchanging the value_type
    static_assert(std::is_same_v<rebound::value_type, ValueType>);
    static_assert(std::is_same_v<rebound::error_type, ErrorType>);
}

}  // namespace
}  // namespace score::details
