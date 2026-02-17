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

#include <score/private/utility/int_cmp.hpp>
#include <score/private/utility/int_cmp.hpp> // test include guard

#include <gtest/gtest.h>

#include <limits>

namespace
{

template <typename T>
struct unsigned_and_signed_types_test_fixture;

template <typename First, typename Second>
struct unsigned_and_signed_types_test_fixture<::testing::Types<First, Second>> : public ::testing::Test
{
    using FirstType = First;
    using SecondType = Second;
};

using unsigned_and_signed_types = ::testing::Types<::testing::Types<unsigned char, signed long long int>,
                                                   ::testing::Types<unsigned int, signed long int>,
                                                   ::testing::Types<unsigned long int, signed int>,
                                                   ::testing::Types<unsigned long long int, signed char>>;
TYPED_TEST_SUITE(unsigned_and_signed_types_test_fixture, unsigned_and_signed_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55625676
TYPED_TEST(unsigned_and_signed_types_test_fixture, compare_unsigned_and_signed)
{

    using UnsignedType = typename TestFixture::FirstType;
    using SignedType = typename TestFixture::SecondType;

    EXPECT_TRUE(score::cpp::cmp_equal(UnsignedType{5}, SignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_equal(SignedType{5}, UnsignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_equal(SignedType{3}, UnsignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_equal(UnsignedType{3}, SignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_equal(UnsignedType{8}, SignedType{-5}));
    EXPECT_FALSE(score::cpp::cmp_equal(SignedType{-27}, UnsignedType{55}));

    EXPECT_FALSE(score::cpp::cmp_not_equal(UnsignedType{5}, SignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_not_equal(SignedType{5}, UnsignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_not_equal(SignedType{3}, UnsignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_not_equal(UnsignedType{3}, SignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_not_equal(UnsignedType{8}, SignedType{-5}));
    EXPECT_TRUE(score::cpp::cmp_not_equal(SignedType{-27}, UnsignedType{55}));

    EXPECT_FALSE(score::cpp::cmp_less(UnsignedType{5}, SignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_less(SignedType{5}, UnsignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_less(SignedType{3}, UnsignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_less(UnsignedType{3}, SignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_less(UnsignedType{8}, SignedType{-5}));
    EXPECT_TRUE(score::cpp::cmp_less(SignedType{-27}, UnsignedType{55}));

    EXPECT_FALSE(score::cpp::cmp_greater(UnsignedType{5}, SignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_greater(SignedType{5}, UnsignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_greater(SignedType{3}, UnsignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_greater(UnsignedType{3}, SignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_greater(UnsignedType{8}, SignedType{-5}));
    EXPECT_FALSE(score::cpp::cmp_greater(SignedType{-27}, UnsignedType{55}));

    EXPECT_TRUE(score::cpp::cmp_less_equal(UnsignedType{5}, SignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_less_equal(SignedType{5}, UnsignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_less_equal(SignedType{3}, UnsignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_less_equal(UnsignedType{3}, SignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_less_equal(UnsignedType{8}, SignedType{-5}));
    EXPECT_TRUE(score::cpp::cmp_less_equal(SignedType{-27}, UnsignedType{55}));

    EXPECT_TRUE(score::cpp::cmp_greater_equal(UnsignedType{5}, SignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_greater_equal(SignedType{5}, UnsignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_greater_equal(SignedType{3}, UnsignedType{5}));
    EXPECT_FALSE(score::cpp::cmp_greater_equal(UnsignedType{3}, SignedType{5}));
    EXPECT_TRUE(score::cpp::cmp_greater_equal(UnsignedType{8}, SignedType{-5}));
    EXPECT_FALSE(score::cpp::cmp_greater_equal(SignedType{-27}, UnsignedType{55}));
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
TYPED_TEST(first_type_within_second_type_test_fixture, compare_first_type_within_second_type)
{
    using SmallerType = typename TestFixture::FirstType;
    using LargerType = typename TestFixture::SecondType;

    EXPECT_FALSE(score::cpp::cmp_equal(std::numeric_limits<LargerType>::max(), std::numeric_limits<SmallerType>::max()));
    EXPECT_FALSE(score::cpp::cmp_equal(std::numeric_limits<SmallerType>::max(), std::numeric_limits<LargerType>::max()));

    EXPECT_TRUE(score::cpp::cmp_not_equal(std::numeric_limits<LargerType>::max(), std::numeric_limits<SmallerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_not_equal(std::numeric_limits<SmallerType>::max(), std::numeric_limits<LargerType>::max()));

    EXPECT_FALSE(score::cpp::cmp_less(std::numeric_limits<LargerType>::max(), std::numeric_limits<SmallerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_less(std::numeric_limits<SmallerType>::max(), std::numeric_limits<LargerType>::max()));

    EXPECT_TRUE(score::cpp::cmp_greater(std::numeric_limits<LargerType>::max(), std::numeric_limits<SmallerType>::max()));
    EXPECT_FALSE(score::cpp::cmp_greater(std::numeric_limits<SmallerType>::max(), std::numeric_limits<LargerType>::max()));

    EXPECT_FALSE(score::cpp::cmp_less_equal(std::numeric_limits<LargerType>::max(), std::numeric_limits<SmallerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_less_equal(std::numeric_limits<SmallerType>::max(), std::numeric_limits<LargerType>::max()));

    EXPECT_TRUE(
        score::cpp::cmp_greater_equal(std::numeric_limits<LargerType>::max(), std::numeric_limits<SmallerType>::max()));
    EXPECT_FALSE(
        score::cpp::cmp_greater_equal(std::numeric_limits<SmallerType>::max(), std::numeric_limits<LargerType>::max()));
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
TYPED_TEST(partial_overlap_types_test_fixture, compare_partial_overlap_types)
{
    using LowerType = typename TestFixture::FirstType;
    using HigherType = typename TestFixture::SecondType;

    EXPECT_FALSE(score::cpp::cmp_equal(std::numeric_limits<HigherType>::max(), std::numeric_limits<LowerType>::max()));
    EXPECT_FALSE(score::cpp::cmp_equal(std::numeric_limits<LowerType>::max(), std::numeric_limits<HigherType>::max()));
    EXPECT_FALSE(score::cpp::cmp_equal(std::numeric_limits<HigherType>::lowest(), std::numeric_limits<LowerType>::lowest()));
    EXPECT_FALSE(score::cpp::cmp_equal(std::numeric_limits<LowerType>::lowest(), std::numeric_limits<HigherType>::lowest()));

    EXPECT_TRUE(score::cpp::cmp_not_equal(std::numeric_limits<HigherType>::max(), std::numeric_limits<LowerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_not_equal(std::numeric_limits<LowerType>::max(), std::numeric_limits<HigherType>::max()));
    EXPECT_TRUE(score::cpp::cmp_not_equal(std::numeric_limits<HigherType>::lowest(), std::numeric_limits<LowerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_not_equal(std::numeric_limits<LowerType>::lowest(), std::numeric_limits<HigherType>::max()));

    EXPECT_FALSE(score::cpp::cmp_less(std::numeric_limits<HigherType>::max(), std::numeric_limits<LowerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_less(std::numeric_limits<LowerType>::max(), std::numeric_limits<HigherType>::max()));
    EXPECT_TRUE(score::cpp::cmp_less(HigherType{0}, std::numeric_limits<LowerType>::max()));
    EXPECT_FALSE(score::cpp::cmp_less(std::numeric_limits<LowerType>::max(), HigherType{0}));
    EXPECT_FALSE(score::cpp::cmp_less(std::numeric_limits<HigherType>::lowest(), std::numeric_limits<LowerType>::lowest()));
    EXPECT_TRUE(score::cpp::cmp_less(std::numeric_limits<LowerType>::lowest(), std::numeric_limits<HigherType>::lowest()));

    EXPECT_TRUE(score::cpp::cmp_greater(std::numeric_limits<HigherType>::max(), std::numeric_limits<LowerType>::max()));
    EXPECT_FALSE(score::cpp::cmp_greater(std::numeric_limits<LowerType>::max(), std::numeric_limits<HigherType>::max()));
    EXPECT_FALSE(score::cpp::cmp_greater(HigherType{0}, std::numeric_limits<LowerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_greater(std::numeric_limits<LowerType>::max(), HigherType{0}));
    EXPECT_TRUE(score::cpp::cmp_greater(std::numeric_limits<HigherType>::lowest(), std::numeric_limits<LowerType>::lowest()));
    EXPECT_FALSE(score::cpp::cmp_greater(std::numeric_limits<LowerType>::lowest(), std::numeric_limits<HigherType>::lowest()));

    EXPECT_FALSE(score::cpp::cmp_less_equal(std::numeric_limits<HigherType>::max(), std::numeric_limits<LowerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_less_equal(std::numeric_limits<LowerType>::max(), std::numeric_limits<HigherType>::max()));
    EXPECT_TRUE(score::cpp::cmp_less_equal(HigherType{0}, std::numeric_limits<LowerType>::max()));
    EXPECT_FALSE(score::cpp::cmp_less_equal(std::numeric_limits<LowerType>::max(), HigherType{0}));
    EXPECT_FALSE(
        score::cpp::cmp_less_equal(std::numeric_limits<HigherType>::lowest(), std::numeric_limits<LowerType>::lowest()));
    EXPECT_TRUE(
        score::cpp::cmp_less_equal(std::numeric_limits<LowerType>::lowest(), std::numeric_limits<HigherType>::lowest()));

    EXPECT_TRUE(score::cpp::cmp_greater_equal(std::numeric_limits<HigherType>::max(), std::numeric_limits<LowerType>::max()));
    EXPECT_FALSE(score::cpp::cmp_greater_equal(std::numeric_limits<LowerType>::max(), std::numeric_limits<HigherType>::max()));
    EXPECT_FALSE(score::cpp::cmp_greater_equal(HigherType{0}, std::numeric_limits<LowerType>::max()));
    EXPECT_TRUE(score::cpp::cmp_greater_equal(std::numeric_limits<LowerType>::max(), HigherType{0}));
    EXPECT_TRUE(
        score::cpp::cmp_greater_equal(std::numeric_limits<HigherType>::lowest(), std::numeric_limits<LowerType>::lowest()));
    EXPECT_FALSE(
        score::cpp::cmp_greater_equal(std::numeric_limits<LowerType>::lowest(), std::numeric_limits<HigherType>::lowest()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55625676
TEST(test_is_int_cmp_supported_type, test_supported_and_unsupported_types)
{
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<unsigned char>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<unsigned short int>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<unsigned int>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<unsigned long int>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<unsigned long long int>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<signed char>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<signed short int>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<signed int>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<signed long int>());
    EXPECT_TRUE(score::cpp::detail::is_int_cmp_supported_type<signed long long int>());

    EXPECT_FALSE(score::cpp::detail::is_int_cmp_supported_type<float>());
    EXPECT_FALSE(score::cpp::detail::is_int_cmp_supported_type<double>());
    EXPECT_FALSE(score::cpp::detail::is_int_cmp_supported_type<char>());
    EXPECT_FALSE(score::cpp::detail::is_int_cmp_supported_type<bool>());
}

} // namespace
