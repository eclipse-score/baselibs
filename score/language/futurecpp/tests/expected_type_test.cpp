/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/expected.hpp>
#include <score/expected.hpp> // test include guard

#include <stdexcept>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>
namespace
{
template <bool CTorThrows, bool CopyTorThrows, bool MoveTorThrows, bool DTorThrows>
struct test_type
{
    test_type() noexcept(!CTorThrows){};
    test_type(const test_type&) noexcept(!CopyTorThrows){};
    test_type(test_type&&) noexcept(!MoveTorThrows){};
    ~test_type() noexcept(!DTorThrows){};
};

constexpr bool t = true;
constexpr bool f = false;
/// the splitting is needed as googletest framework has a max number of types of 50!
using test_value_types_1 = std::tuple<test_type<f, f, f, f>, test_type<f, f, t, f>, test_type<f, t, f, f>>;
using test_value_types_2 = std::tuple<test_type<f, t, t, f>, test_type<f, t, f, t>, test_type<f, f, f, t>>;
using test_value_types_3 = std::tuple<test_type<f, f, t, t>, test_type<f, t, t, t>>;
// We separate test entries that can throw in the default contructor as we don't want to use them as ErrorType in tests.
using test_value_types_4 = std::tuple<test_type<t, t, f, f>, test_type<t, f, f, f>, test_type<t, f, t, f>>;
using test_value_types_5 = std::tuple<test_type<t, f, f, t>, test_type<t, t, t, f>, test_type<t, t, f, t>>;
using test_value_types_6 = std::tuple<test_type<t, t, t, t>, test_type<t, f, t, t>>;

using test_error_types = decltype(std::tuple_cat(
    std::declval<test_value_types_1>(), std::declval<test_value_types_2>(), std::declval<test_value_types_3>()));

template <typename, typename>
struct type_pair
{
};

template <typename, typename>
class pairwise_testing_types;

template <typename... V, typename... E>
class pairwise_testing_types<std::tuple<V...>, std::tuple<E...>>
{
    template <size_t Idx>
    using make_combination = std::tuple<type_pair<std::tuple_element_t<Idx, std::tuple<V...>>, E>...>;

    template <typename... Ts>
    [[noreturn]] static constexpr auto as_test_type(std::tuple<Ts...>) -> ::testing::Types<Ts...>
    {
        std::terminate();
    }

    template <size_t... VIdxs>
    [[noreturn]] static constexpr auto make_test_types(std::index_sequence<VIdxs...>)
        -> decltype(as_test_type(std::tuple_cat(std::declval<make_combination<VIdxs>>()...)))
    {
        std::terminate();
    }

public:
    using type = decltype(make_test_types(std::index_sequence_for<V...>()));
};

using all_test_types_1 = typename pairwise_testing_types<test_value_types_1, test_error_types>::type;
using all_test_types_2 = typename pairwise_testing_types<test_value_types_2, test_error_types>::type;
using all_test_types_3 = typename pairwise_testing_types<test_value_types_3, test_error_types>::type;
using all_test_types_4 = typename pairwise_testing_types<test_value_types_4, test_error_types>::type;
using all_test_types_5 = typename pairwise_testing_types<test_value_types_5, test_error_types>::type;
using all_test_types_6 = typename pairwise_testing_types<test_value_types_6, test_error_types>::type;

template <typename>
class test_expected_type_fixture;

template <typename ValueType, typename ErrorType>
class test_expected_type_fixture<type_pair<ValueType, ErrorType>> : public ::testing::Test
{
public:
    using value_type = ValueType;
    using error_type = ErrorType;
    using excepted_type = score::cpp::expected<value_type, error_type>;
};

TYPED_TEST_SUITE_P(test_expected_type_fixture);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TYPED_TEST_P(test_expected_type_fixture, check_ctor)
{
    bool actual = std::is_nothrow_constructible<typename TestFixture::excepted_type>::value;
    bool expected = std::is_nothrow_constructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_constructible<typename TestFixture::error_type>::value;
    EXPECT_EQ(actual, expected);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TYPED_TEST_P(test_expected_type_fixture, check_dtor)
{
    bool actual = std::is_nothrow_destructible<typename TestFixture::excepted_type>::value;
    bool expected = std::is_nothrow_destructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_destructible<typename TestFixture::error_type>::value;
    EXPECT_EQ(actual, expected);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TYPED_TEST_P(test_expected_type_fixture, check_copy_tor)
{
    bool actual = std::is_nothrow_copy_constructible<typename TestFixture::excepted_type>::value;
    bool expected = std::is_nothrow_copy_constructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_copy_constructible<typename TestFixture::error_type>::value;
    EXPECT_EQ(actual, expected);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TYPED_TEST_P(test_expected_type_fixture, check_move_tor)
{
    bool actual = std::is_nothrow_move_constructible<typename TestFixture::excepted_type>::value;
    bool expected = std::is_nothrow_move_constructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_move_constructible<typename TestFixture::error_type>::value;
    EXPECT_EQ(actual, expected);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TYPED_TEST_P(test_expected_type_fixture, check_copy_assignment)
{
    bool actual = std::is_nothrow_copy_assignable<typename TestFixture::excepted_type>::value;
    bool expected = std::is_nothrow_copy_constructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_copy_constructible<typename TestFixture::error_type>::value &&
                    std::is_nothrow_destructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_destructible<typename TestFixture::error_type>::value;
    EXPECT_EQ(actual, expected);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16631224
TYPED_TEST_P(test_expected_type_fixture, check_move_assignment)
{
    bool actual = std::is_nothrow_move_assignable<typename TestFixture::excepted_type>::value;
    bool expected = std::is_nothrow_move_constructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_move_constructible<typename TestFixture::error_type>::value &&
                    std::is_nothrow_destructible<typename TestFixture::value_type>::value &&
                    std::is_nothrow_destructible<typename TestFixture::error_type>::value;
    EXPECT_EQ(actual, expected);
}

REGISTER_TYPED_TEST_SUITE_P(test_expected_type_fixture,
                            check_ctor,
                            check_dtor,
                            check_copy_tor,
                            check_move_tor,
                            check_copy_assignment,
                            check_move_assignment);
INSTANTIATE_TYPED_TEST_SUITE_P(part1, test_expected_type_fixture, all_test_types_1, /*unused*/);
INSTANTIATE_TYPED_TEST_SUITE_P(part2, test_expected_type_fixture, all_test_types_2, /*unused*/);
INSTANTIATE_TYPED_TEST_SUITE_P(part3, test_expected_type_fixture, all_test_types_3, /*unused*/);
INSTANTIATE_TYPED_TEST_SUITE_P(part4, test_expected_type_fixture, all_test_types_4, /*unused*/);
INSTANTIATE_TYPED_TEST_SUITE_P(part5, test_expected_type_fixture, all_test_types_5, /*unused*/);
INSTANTIATE_TYPED_TEST_SUITE_P(part6, test_expected_type_fixture, all_test_types_6, /*unused*/);
} // namespace
