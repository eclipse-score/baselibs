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

#include <score/private/numeric/saturate_cast.hpp>
#include <score/private/numeric/saturate_cast.hpp> // test include guard

#include <score/type_traits.hpp>

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
/// @requirement CB-#55626118
TYPED_TEST(integer_types_test_fixture, saturate_cast_equal_types)
{
    EXPECT_EQ(score::cpp::saturate_cast<TypeParam>(TypeParam{19}), TypeParam{19});
    EXPECT_EQ(score::cpp::saturate_cast<TypeParam>(std::numeric_limits<TypeParam>::lowest()),
              std::numeric_limits<TypeParam>::lowest());
    EXPECT_EQ(score::cpp::saturate_cast<TypeParam>(std::numeric_limits<TypeParam>::max()),
              std::numeric_limits<TypeParam>::max());
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
/// @requirement CB-#55626118
TYPED_TEST(first_type_within_second_type_test_fixture, saturate_cast_first_type_within_second_type)
{
    using SmallerType = typename TestFixture::FirstType;
    using LargerType = typename TestFixture::SecondType;

    EXPECT_EQ(score::cpp::saturate_cast<SmallerType>(std::numeric_limits<LargerType>::max()),
              std::numeric_limits<SmallerType>::max());
    EXPECT_EQ(score::cpp::saturate_cast<SmallerType>(std::numeric_limits<LargerType>::lowest()),
              std::numeric_limits<SmallerType>::lowest());

    EXPECT_EQ(score::cpp::saturate_cast<LargerType>(std::numeric_limits<SmallerType>::max()),
              std::numeric_limits<SmallerType>::max());
    EXPECT_EQ(score::cpp::saturate_cast<LargerType>(std::numeric_limits<SmallerType>::lowest()),
              std::numeric_limits<SmallerType>::lowest());
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
/// @requirement CB-#55626118
TYPED_TEST(partial_overlap_types_test_fixture, saturate_cast_partial_overlap)
{
    using LowerType = typename TestFixture::FirstType;
    using HigherType = typename TestFixture::SecondType;

    EXPECT_EQ(score::cpp::saturate_cast<LowerType>(std::numeric_limits<HigherType>::max()),
              std::numeric_limits<LowerType>::max());
    EXPECT_EQ(score::cpp::saturate_cast<LowerType>(HigherType{5}), LowerType{5});
    EXPECT_EQ(score::cpp::saturate_cast<HigherType>(std::numeric_limits<LowerType>::lowest()),
              std::numeric_limits<HigherType>::lowest());
    EXPECT_EQ(score::cpp::saturate_cast<HigherType>(LowerType{7}), HigherType{7});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55626118
TEST(test_is_saturate_cast_supported_type, test_supported_and_unsupported_types)
{
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<unsigned char>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<unsigned short int>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<unsigned int>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<unsigned long int>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<unsigned long long int>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<signed char>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<signed short int>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<signed int>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<signed long int>());
    EXPECT_TRUE(score::cpp::detail::is_saturate_cast_supported_type<signed long long int>());

    EXPECT_FALSE(score::cpp::detail::is_saturate_cast_supported_type<float>());
    EXPECT_FALSE(score::cpp::detail::is_saturate_cast_supported_type<double>());
    EXPECT_FALSE(score::cpp::detail::is_saturate_cast_supported_type<char>());
    EXPECT_FALSE(score::cpp::detail::is_saturate_cast_supported_type<bool>());
}

template <typename T, typename U, typename Enabled = score::cpp::void_t<>>
struct can_saturate_cast : std::false_type
{
};

template <typename T, typename U>
struct can_saturate_cast<T, U, score::cpp::void_t<decltype(&score::cpp::saturate_cast<T, U>)>> : std::true_type
{
};

struct test_struct
{
};

/// \brief Here it is tested that score::cpp::saturate_cast is SFINAE friendly, i.e. that it allows substitution failure
/// without hard error
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#55626118
TEST(saturate_cast_test, test_sfinae_friendliyness)
{
    EXPECT_TRUE((can_saturate_cast<unsigned short int, unsigned long int>::value));
    EXPECT_TRUE((can_saturate_cast<unsigned int, unsigned long long int>::value));
    EXPECT_TRUE((can_saturate_cast<signed int, signed long int>::value));

    EXPECT_FALSE((can_saturate_cast<signed int, bool>::value));
    EXPECT_FALSE((can_saturate_cast<char, signed int>::value));
    EXPECT_FALSE((can_saturate_cast<test_struct, signed int>::value));
}

} // namespace
