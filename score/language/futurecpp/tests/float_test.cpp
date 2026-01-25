/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///

#include <score/float.hpp>
#include <score/float.hpp> // test include guard

#include <score/inplace_vector.hpp>

#include <cmath>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

namespace
{

/// \brief Set up typed tests for equals() function for iterables of float and double.
typedef ::testing::
    Types<std::vector<float>, std::vector<double>, score::cpp::inplace_vector<float, 12U>, score::cpp::inplace_vector<double, 12U>>
        float_iterable_types;
template <typename T>
class float_iterable_typed_test : public ::testing::Test
{
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18222553
TYPED_TEST_SUITE(float_iterable_typed_test, float_iterable_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18222553
TYPED_TEST(float_iterable_typed_test, equals_iterable_different_size)
{
    using IterableType = TypeParam;
    using ValueType = typename TypeParam::value_type;

    IterableType test_vec_lhs{static_cast<ValueType>(30.0)};
    IterableType test_vec_rhs{static_cast<ValueType>(30.0), static_cast<ValueType>(30.0)};

    EXPECT_FALSE(score::cpp::equals(test_vec_lhs, test_vec_rhs));
    EXPECT_FALSE(score::cpp::equals(
        std::begin(test_vec_lhs), std::end(test_vec_lhs), std::begin(test_vec_rhs), std::end(test_vec_rhs)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18222553
TYPED_TEST(float_iterable_typed_test, equals_iterable_true)
{
    using IterableType = TypeParam;
    using ValueType = typename TypeParam::value_type;

    IterableType test_vec_lhs{static_cast<ValueType>(30.0),
                              static_cast<ValueType>(1.0000000000000001),
                              -static_cast<ValueType>(1337.0000000000001),
                              std::numeric_limits<ValueType>::min()};
    IterableType test_vec_rhs{static_cast<ValueType>(30.0),
                              static_cast<ValueType>(1.0000000000000002),
                              -static_cast<ValueType>(1337.0000000000002),
                              std::numeric_limits<ValueType>::min()};

    EXPECT_TRUE(score::cpp::equals(test_vec_lhs, test_vec_rhs));
    EXPECT_TRUE(score::cpp::equals(
        std::begin(test_vec_lhs), std::end(test_vec_lhs), std::begin(test_vec_rhs), std::end(test_vec_rhs)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18222553
TYPED_TEST(float_iterable_typed_test, equals_iterable_false)
{
    using IterableType = TypeParam;
    using ValueType = typename TypeParam::value_type;

    IterableType test_vec_lhs{
        static_cast<ValueType>(30.0), static_cast<ValueType>(1337.001), -static_cast<ValueType>(1337.0000000000001)};
    IterableType test_vec_rhs{
        static_cast<ValueType>(30.0), static_cast<ValueType>(1337.002), -static_cast<ValueType>(1337.0000000000002)};

    EXPECT_FALSE(score::cpp::equals(test_vec_lhs, test_vec_rhs));
    EXPECT_FALSE(score::cpp::equals(
        std::begin(test_vec_lhs), std::end(test_vec_lhs), std::begin(test_vec_rhs), std::end(test_vec_rhs)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18222553
TYPED_TEST(float_iterable_typed_test, equals_iterable_max_rel_diff)
{
    using IterableType = TypeParam;
    using ValueType = typename TypeParam::value_type;

    const ValueType zero_eps = static_cast<ValueType>(0.0);
    const ValueType eps = std::numeric_limits<ValueType>::epsilon();

    IterableType test_vec_lhs{static_cast<ValueType>(1.0), static_cast<ValueType>(42.0), static_cast<ValueType>(2.0)};
    IterableType test_vec_rhs{static_cast<ValueType>(1.0),
                              std::nextafter(static_cast<ValueType>(42.0), static_cast<ValueType>(43.0)),
                              static_cast<ValueType>(2.0)};

    EXPECT_FALSE(score::cpp::equals(test_vec_lhs, test_vec_rhs, zero_eps));
    EXPECT_FALSE(score::cpp::equals(
        std::begin(test_vec_lhs), std::end(test_vec_lhs), std::begin(test_vec_rhs), std::end(test_vec_rhs), zero_eps));

    EXPECT_TRUE(score::cpp::equals(test_vec_lhs, test_vec_rhs, eps));
    EXPECT_TRUE(score::cpp::equals(
        std::begin(test_vec_lhs), std::end(test_vec_lhs), std::begin(test_vec_rhs), std::end(test_vec_rhs), eps));
}

///
/// \test Tests correct behavior of the max_rel_diff parameter of score::cpp::equals
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18222553
///
TYPED_TEST(float_iterable_typed_test, equals_max_rel_diff)
{
    using ValueType = typename TypeParam::value_type;
    TypeParam test_vector;
    test_vector.push_back(static_cast<ValueType>(-1.0));
    test_vector.push_back(static_cast<ValueType>(1.0));
    test_vector.push_back(static_cast<ValueType>(-42.0));
    test_vector.push_back(static_cast<ValueType>(42.0));
    test_vector.push_back(static_cast<ValueType>(-1337.0));
    test_vector.push_back(static_cast<ValueType>(1337.0));
    test_vector.push_back(static_cast<ValueType>(-42.1337));
    test_vector.push_back(static_cast<ValueType>(42.1337));
    test_vector.push_back(static_cast<ValueType>(-1337.42));
    test_vector.push_back(static_cast<ValueType>(1337.42));
    test_vector.push_back(static_cast<ValueType>(-1.0e38));
    test_vector.push_back(static_cast<ValueType>(1.0e38));

    for (auto it = test_vector.cbegin(); it != test_vector.cend(); ++it)
    {
        const ValueType value = *it;
        const ValueType two = static_cast<ValueType>(2.0);
        const ValueType value_1ulp = std::nextafter(value, two * value);
        const ValueType value_2ulp = std::nextafter(value_1ulp, two * value);
        const ValueType value_3ulp = std::nextafter(value_2ulp, two * value);
        const ValueType three = static_cast<ValueType>(3.0);
        const ValueType four = static_cast<ValueType>(4.0);
        const ValueType zero_eps = static_cast<ValueType>(0.0);
        const ValueType eps = std::numeric_limits<ValueType>::epsilon();
        const ValueType two_eps = two * eps;
        const ValueType three_eps = three * eps;
        const ValueType four_eps = four * eps;

        EXPECT_FALSE(score::cpp::equals(value, value_1ulp, zero_eps)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_1ulp, eps)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_1ulp, two_eps)) << "Value: " << value;

        EXPECT_FALSE(score::cpp::equals(value, value_2ulp, eps)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_2ulp, two_eps)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_2ulp, three_eps)) << "Value: " << value;

        EXPECT_FALSE(score::cpp::equals(value, value_3ulp, two_eps)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_3ulp, three_eps)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_3ulp, four_eps)) << "Value: " << value;
    }
}

///
/// \test Tests values near float minimum, that should always result in equality.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18222553
///
TYPED_TEST(float_iterable_typed_test, equals_near_float_min)
{
    using ValueType = typename TypeParam::value_type;

    const ValueType plus_inf = std::numeric_limits<ValueType>::infinity();
    const ValueType minus_inf = -plus_inf;
    TypeParam test_vector;
    test_vector.push_back(-std::numeric_limits<ValueType>::min());
    test_vector.push_back(std::numeric_limits<ValueType>::min());
    test_vector.push_back(std::nextafter(-std::numeric_limits<ValueType>::min(), minus_inf));
    test_vector.push_back(std::nextafter(std::numeric_limits<ValueType>::min(), plus_inf));
    test_vector.push_back(std::nextafter(std::nextafter(-std::numeric_limits<ValueType>::min(), minus_inf), minus_inf));
    test_vector.push_back(std::nextafter(std::nextafter(std::numeric_limits<ValueType>::min(), plus_inf), plus_inf));

    for (auto it = test_vector.cbegin(); it != test_vector.cend(); ++it)
    {
        const ValueType two = static_cast<ValueType>(2.0);
        const ValueType value = *it;
        const ValueType value_1ulp = std::nextafter(value, two * value);
        const ValueType value_2ulp = std::nextafter(value_1ulp, two * value);
        const ValueType value_3ulp = std::nextafter(value_2ulp, two * value);

        EXPECT_TRUE(score::cpp::equals(value, value_1ulp)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_2ulp)) << "Value: " << value;
        EXPECT_TRUE(score::cpp::equals(value, value_3ulp)) << "Value: " << value;
    }
}

} // namespace
