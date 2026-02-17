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

///
/// \file
/// \copyright Copyright (c) 2025 Contributors to the Eclipse Foundation
///

#include <score/private/utility/in_range.hpp>
#include <score/private/utility/in_range.hpp> // test include guard

#include <gtest/gtest.h>

namespace
{

template <typename T>
class integer_types_test_fixture : public ::testing::Test
{
};

using integer_types = ::testing::Types<unsigned char,
                                       unsigned short int,
                                       unsigned int,
                                       unsigned long int,
                                       unsigned long long int,
                                       signed char,
                                       signed short int,
                                       signed int,
                                       signed long int,
                                       signed long long int>;
TYPED_TEST_SUITE(integer_types_test_fixture, integer_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55625676
TYPED_TEST(integer_types_test_fixture, in_range_equal_types)
{
    EXPECT_TRUE(score::cpp::in_range<TypeParam>(std::numeric_limits<TypeParam>::max()));
    EXPECT_TRUE(score::cpp::in_range<TypeParam>(std::numeric_limits<TypeParam>::lowest()));
}

template <typename T>
struct first_type_within_second_type_test_fixture;

template <typename First, typename Second>
struct first_type_within_second_type_test_fixture<::testing::Types<First, Second>> : public ::testing::Test
{
    using FirstType = First;
    using SecondType = Second;
};

using first_type_within_second_type_types = ::testing::Types<::testing::Types<unsigned char, unsigned short int>,
                                                             ::testing::Types<unsigned short int, unsigned long int>,
                                                             ::testing::Types<unsigned int, unsigned long long int>,
                                                             ::testing::Types<signed char, signed short int>,
                                                             ::testing::Types<signed short int, signed long int>,
                                                             ::testing::Types<signed int, signed long long int>,
                                                             ::testing::Types<unsigned char, signed short int>,
                                                             ::testing::Types<unsigned short int, signed long int>,
                                                             ::testing::Types<unsigned int, signed long long int>>;

TYPED_TEST_SUITE(first_type_within_second_type_test_fixture, first_type_within_second_type_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55625676
TYPED_TEST(first_type_within_second_type_test_fixture, in_range_first_type_within_second_type)
{
    using SmallerType = typename TestFixture::FirstType;
    using LargerType = typename TestFixture::SecondType;

    EXPECT_FALSE(score::cpp::in_range<SmallerType>(std::numeric_limits<LargerType>::max()));
    EXPECT_TRUE(score::cpp::in_range<SmallerType>(SmallerType{0}));
    EXPECT_TRUE(score::cpp::in_range<LargerType>(std::numeric_limits<SmallerType>::max()));
    EXPECT_TRUE(score::cpp::in_range<LargerType>(std::numeric_limits<SmallerType>::lowest()));
}

template <typename T>
struct partial_overlap_types_test_fixture;

template <typename First, typename Second>
struct partial_overlap_types_test_fixture<::testing::Types<First, Second>> : public ::testing::Test
{
    using FirstType = First;
    using SecondType = Second;
};

using partial_overlap_types = ::testing::Types<::testing::Types<signed char, unsigned char>,
                                               ::testing::Types<signed short int, unsigned short int>,
                                               ::testing::Types<signed int, unsigned int>,
                                               ::testing::Types<signed long int, unsigned long int>,
                                               ::testing::Types<signed long long int, unsigned long long int>,
                                               ::testing::Types<signed char, unsigned int>>;

TYPED_TEST_SUITE(partial_overlap_types_test_fixture, partial_overlap_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55625676
TYPED_TEST(partial_overlap_types_test_fixture, in_range_first_type_within_second_type)
{
    using LowerType = typename TestFixture::FirstType;
    using HigherType = typename TestFixture::SecondType;

    EXPECT_FALSE(score::cpp::in_range<LowerType>(std::numeric_limits<HigherType>::max()));
    EXPECT_TRUE(score::cpp::in_range<LowerType>(HigherType{5}));
    EXPECT_FALSE(score::cpp::in_range<HigherType>(std::numeric_limits<LowerType>::lowest()));
    EXPECT_TRUE(score::cpp::in_range<HigherType>(LowerType{7}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55625676
TEST(test_is_in_range_supported_type, test_supported_and_unsupported_types)
{
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<unsigned char>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<unsigned short int>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<unsigned int>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<unsigned long int>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<unsigned long long int>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<signed char>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<signed short int>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<signed int>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<signed long int>());
    EXPECT_TRUE(score::cpp::detail::is_in_range_supported_type<signed long long int>());

    EXPECT_FALSE(score::cpp::detail::is_in_range_supported_type<float>());
    EXPECT_FALSE(score::cpp::detail::is_in_range_supported_type<double>());
    EXPECT_FALSE(score::cpp::detail::is_in_range_supported_type<char>());
    EXPECT_FALSE(score::cpp::detail::is_in_range_supported_type<bool>());
}

} // namespace
