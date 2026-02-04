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
#include <score/math.hpp>
#include <score/math.hpp> // test include guard
#include <score/math_constants.hpp>
#include <score/math_constants.hpp> // test include guard

#include <score/assert_support.hpp>

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

#include <gtest/gtest.h>

namespace
{

constexpr bool has_errno_support() { return (math_errhandling & MATH_ERRNO) != 0; }

using float_types = ::testing::Types<float, double>;
template <typename T>
class math_float_typed_test : public ::testing::Test
{
};
TYPED_TEST_SUITE(math_float_typed_test, float_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TYPED_TEST(math_float_typed_test, equals_bitexact)
{
    EXPECT_TRUE(score::cpp::equals_bitexact(TypeParam{-1.0}, TypeParam{-1.0}));
    EXPECT_TRUE(score::cpp::equals_bitexact(TypeParam{0.0}, TypeParam{0.0}));
    EXPECT_TRUE(score::cpp::equals_bitexact(TypeParam{1.0}, TypeParam{1.0}));

    const TypeParam positive_inf{std::numeric_limits<TypeParam>::infinity()};
    EXPECT_FALSE(score::cpp::equals_bitexact(TypeParam{0.0}, std::nextafter(TypeParam{0.0}, positive_inf)));
    EXPECT_FALSE(score::cpp::equals_bitexact(TypeParam{0.0}, std::nextafter(TypeParam{0.0}, -positive_inf)));
}

///
/// \test Tests float values that are actually almost equal
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TYPED_TEST(math_float_typed_test, equals_true)
{
    // Positive
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(0.0), static_cast<TypeParam>(0.0)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(42.1337), static_cast<TypeParam>(42.1337)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(0.00001337), static_cast<TypeParam>(0.00001337)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(1.0000000000000001), static_cast<TypeParam>(1.0000000000000002)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(42.000000000000001), static_cast<TypeParam>(42.000000000000002)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(1337.0000000000001), static_cast<TypeParam>(1337.0000000000002)));
    EXPECT_TRUE(score::cpp::equals(std::numeric_limits<TypeParam>::min(), std::numeric_limits<TypeParam>::min()));
    EXPECT_TRUE(score::cpp::equals(std::numeric_limits<TypeParam>::max(), std::numeric_limits<TypeParam>::max()));

    // Negative
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(-0.0), static_cast<TypeParam>(-0.0)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(-42.1337), static_cast<TypeParam>(-42.1337)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(-0.00001337), static_cast<TypeParam>(-0.00001337)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(-1.0000000000000001), static_cast<TypeParam>(-1.0000000000000002)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(-42.000000000000001), static_cast<TypeParam>(-42.000000000000002)));
    EXPECT_TRUE(score::cpp::equals(static_cast<TypeParam>(-1337.0000000000001), static_cast<TypeParam>(-1337.0000000000002)));
    EXPECT_TRUE(score::cpp::equals(-std::numeric_limits<TypeParam>::min(), -std::numeric_limits<TypeParam>::min()));
    EXPECT_TRUE(score::cpp::equals(std::numeric_limits<TypeParam>::lowest(), std::numeric_limits<TypeParam>::lowest()));
    EXPECT_TRUE(score::cpp::equals(-std::numeric_limits<TypeParam>::max(), -std::numeric_limits<TypeParam>::max()));
}

///
/// \test Tests float values that are NOT almost equal
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TYPED_TEST(math_float_typed_test, equals_false)
{
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(42.0), static_cast<TypeParam>(1337.0)));
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(1.000001), static_cast<TypeParam>(1.000002)));
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(42.00001), static_cast<TypeParam>(42.00002)));
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(1337.001), static_cast<TypeParam>(1337.002)));
    EXPECT_FALSE(score::cpp::equals(std::numeric_limits<TypeParam>::min(), std::numeric_limits<TypeParam>::max()));

    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(-42.0), static_cast<TypeParam>(-1337.0)));
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(-1.000001), static_cast<TypeParam>(-1.000002)));
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(-42.00001), static_cast<TypeParam>(-42.00002)));
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(-1337.001), static_cast<TypeParam>(-1337.002)));
    EXPECT_FALSE(score::cpp::equals(-std::numeric_limits<TypeParam>::min(), -std::numeric_limits<TypeParam>::max()));
}

///
/// \test Test NaN and 'infinity' float values that are not equal
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TYPED_TEST(math_float_typed_test, equals_with_inf_and_nan)
{
    const TypeParam inf{std::numeric_limits<TypeParam>::infinity()};
    const TypeParam nan{std::numeric_limits<TypeParam>::quiet_NaN()};

    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(0.0), inf));
    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(0.0), -inf));
    EXPECT_FALSE(score::cpp::equals(inf, static_cast<TypeParam>(0.0)));
    EXPECT_FALSE(score::cpp::equals(-inf, static_cast<TypeParam>(0.0)));
    EXPECT_FALSE(score::cpp::equals(-inf, inf));
    EXPECT_FALSE(score::cpp::equals(inf, -inf));
    EXPECT_TRUE(score::cpp::equals(-inf, -inf));
    EXPECT_TRUE(score::cpp::equals(inf, inf));

    EXPECT_FALSE(score::cpp::equals(static_cast<TypeParam>(0.0), nan));
    EXPECT_FALSE(score::cpp::equals(nan, static_cast<TypeParam>(0.0)));
    EXPECT_FALSE(score::cpp::equals(-nan, nan));
    EXPECT_FALSE(score::cpp::equals(nan, -nan));
    EXPECT_FALSE(score::cpp::equals(-nan, -nan));
    EXPECT_FALSE(score::cpp::equals(nan, nan));

    EXPECT_FALSE(score::cpp::equals(inf, nan));
    EXPECT_FALSE(score::cpp::equals(nan, inf));
}

///
/// \test Check correct wrapping to interval [0, x).
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, fmod_floored)
{
    // float
    const float float_tolerance = 0.000001F;
    EXPECT_NEAR(1.999F, score::cpp::fmod_floored(1.999F, 2.0F), float_tolerance);
    EXPECT_NEAR(1.999F, score::cpp::fmod_floored(3.999F, 2.0F), float_tolerance);
    EXPECT_FLOAT_EQ(0.0F, score::cpp::fmod_floored(2.0F, 2.0F));
    EXPECT_NEAR(0.001F, score::cpp::fmod_floored(-1.999F, 2.0F), float_tolerance);
    EXPECT_NEAR(0.9F, score::cpp::fmod_floored(3.0F, 2.1F), float_tolerance);
    EXPECT_NEAR(1.2F, score::cpp::fmod_floored(-3.0F, 2.1F), float_tolerance);
    EXPECT_NEAR(0.5F, score::cpp::fmod_floored(-1.5F, 2.0F), float_tolerance);
    EXPECT_NEAR(0.0F, score::cpp::fmod_floored(90.0F, 3.0F), float_tolerance);
    EXPECT_NEAR(2.0F, score::cpp::fmod_floored(92.0F, 3.0F), float_tolerance);
    EXPECT_NEAR(1.0F, score::cpp::fmod_floored(122.0F, 11.0F), float_tolerance);
    EXPECT_NEAR(0.0F, score::cpp::fmod_floored(-90.0F, 3.0F), float_tolerance);
    EXPECT_NEAR(1.0F, score::cpp::fmod_floored(-92.0F, 3.0F), float_tolerance);
    EXPECT_NEAR(10.0F, score::cpp::fmod_floored(-122.0F, 11.0F), float_tolerance);
    EXPECT_NEAR(0.1F, score::cpp::fmod_floored(1.1F, 1.0F), float_tolerance);
    EXPECT_NEAR(0.01F, score::cpp::fmod_floored(0.11F, 0.1F), float_tolerance);
    EXPECT_NEAR(-0.09F, score::cpp::fmod_floored(0.11F, -0.1F), float_tolerance);

    // double
    const double double_tolerance = 0.000000000000001;
    EXPECT_NEAR(1.999, score::cpp::fmod_floored(1.999, 2.0), double_tolerance);
    EXPECT_NEAR(1.999, score::cpp::fmod_floored(3.999, 2.0), double_tolerance);
    EXPECT_DOUBLE_EQ(0.0, score::cpp::fmod_floored(2.0, 2.0));
    EXPECT_NEAR(0.001, score::cpp::fmod_floored(-1.999, 2.0), double_tolerance);
    EXPECT_NEAR(0.9, score::cpp::fmod_floored(3.0, 2.1), double_tolerance);
    EXPECT_NEAR(1.2, score::cpp::fmod_floored(-3.0, 2.1), double_tolerance);
    EXPECT_NEAR(0.5, score::cpp::fmod_floored(-1.5, 2.0), double_tolerance);
    EXPECT_NEAR(0.0, score::cpp::fmod_floored(90.0, 3.0), double_tolerance);
    EXPECT_NEAR(2.0, score::cpp::fmod_floored(92.0, 3.0), double_tolerance);
    EXPECT_NEAR(1.0, score::cpp::fmod_floored(122.0, 11.0), double_tolerance);
    EXPECT_NEAR(0.0, score::cpp::fmod_floored(-90.0, 3.0), double_tolerance);
    EXPECT_NEAR(1.0, score::cpp::fmod_floored(-92.0, 3.0), double_tolerance);
    EXPECT_NEAR(10.0, score::cpp::fmod_floored(-122.0, 11.0), double_tolerance);
    EXPECT_NEAR(0.1, score::cpp::fmod_floored(1.1, 1.0), double_tolerance);
    EXPECT_NEAR(0.01, score::cpp::fmod_floored(0.11, 0.1), double_tolerance);
    EXPECT_NEAR(-0.09, score::cpp::fmod_floored(0.11, -0.1), double_tolerance);
}

///
/// \test Check correct wrapping to interval [0, x) for numeric limits.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, fmod_floored_numeric_limists_float)
{
    const float value = 23.0F;
    const float minus_inf = -std::numeric_limits<float>::infinity();
    const float plug_inf = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(std::isnan(score::cpp::fmod_floored(minus_inf, value)));
    EXPECT_TRUE(std::isnan(score::cpp::fmod_floored(plug_inf, value)));
    EXPECT_TRUE(std::isnan(score::cpp::fmod_floored(std::numeric_limits<float>::quiet_NaN(), value)));
    EXPECT_FLOAT_EQ(std::numeric_limits<float>::min(), score::cpp::fmod_floored(std::numeric_limits<float>::min(), value));
}
///
/// \test Check correct wrapping to interval [0, x) for numeric limits.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, fmod_floored_numeric_limists_double)
{
    const double value = 23.0;
    const double minus_inf = -std::numeric_limits<double>::infinity();
    const double plug_inf = std::numeric_limits<double>::infinity();
    EXPECT_TRUE(std::isnan(score::cpp::fmod_floored(minus_inf, value)));
    EXPECT_TRUE(std::isnan(score::cpp::fmod_floored(plug_inf, value)));
    EXPECT_TRUE(std::isnan(score::cpp::fmod_floored(std::numeric_limits<double>::quiet_NaN(), value)));
    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::min(), score::cpp::fmod_floored(std::numeric_limits<double>::min(), value));
}

///
/// \test Check correct wrapping to interval [0, x) at interval border.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, fmod_floored_interval_border_float)
{
    const float value = 23.0F;
    const float minus_inf = -std::numeric_limits<float>::infinity();
    const float plug_inf = std::numeric_limits<float>::infinity();
    const float value_less = std::nextafter(value, minus_inf);
    const float value_more = std::nextafter(value, plug_inf);

    // Positive dividend, positive divisor
    EXPECT_FLOAT_EQ(value_less, score::cpp::fmod_floored(value_less, value));
    EXPECT_FLOAT_EQ(0.0F, score::cpp::fmod_floored(value, value));
    EXPECT_FLOAT_EQ(value_more - value, score::cpp::fmod_floored(value_more, value));
    // Negative dividend, positive divisor
    EXPECT_FLOAT_EQ(value_more - value, score::cpp::fmod_floored(-value_less, value));
    EXPECT_FLOAT_EQ(0.0F, score::cpp::fmod_floored(-value, value));
    EXPECT_FLOAT_EQ(value_less, score::cpp::fmod_floored(-value_more, value));
    // Positive dividend, negative divisor
    EXPECT_FLOAT_EQ(value_less - value, score::cpp::fmod_floored(value_less, -value));
    EXPECT_FLOAT_EQ(0.0F, score::cpp::fmod_floored(value, -value));
    EXPECT_FLOAT_EQ(-value_less, score::cpp::fmod_floored(value_more, -value));
    // Negative dividend, negative divisor
    EXPECT_FLOAT_EQ(-value_less, score::cpp::fmod_floored(-value_less, -value));
    EXPECT_FLOAT_EQ(0.0F, score::cpp::fmod_floored(-value, -value));
    EXPECT_FLOAT_EQ(value - value_more, score::cpp::fmod_floored(-value_more, -value));
}

///
/// \test Check correct wrapping to interval [0, x) at interval border.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, fmod_floored_interval_border_double)
{
    const double value = 23.0F;
    const double minus_inf = -std::numeric_limits<double>::infinity();
    const double plug_inf = std::numeric_limits<double>::infinity();
    const double value_less = std::nextafter(value, minus_inf);
    const double value_more = std::nextafter(value, plug_inf);

    // Positive dividend, positive divisor
    EXPECT_DOUBLE_EQ(value_less, score::cpp::fmod_floored(value_less, value));
    EXPECT_DOUBLE_EQ(0.0, score::cpp::fmod_floored(value, value));
    EXPECT_DOUBLE_EQ(value_more - value, score::cpp::fmod_floored(value_more, value));
    // Negative dividend, positive divisor
    EXPECT_DOUBLE_EQ(value_more - value, score::cpp::fmod_floored(-value_less, value));
    EXPECT_DOUBLE_EQ(0.0, score::cpp::fmod_floored(-value, value));
    EXPECT_DOUBLE_EQ(value_less, score::cpp::fmod_floored(-value_more, value));
    // Positive dividend, negative divisor
    EXPECT_DOUBLE_EQ(value_less - value, score::cpp::fmod_floored(value_less, -value));
    EXPECT_DOUBLE_EQ(0.0, score::cpp::fmod_floored(value, -value));
    EXPECT_DOUBLE_EQ(-value_less, score::cpp::fmod_floored(value_more, -value));
    // Negative dividend, negative divisor
    EXPECT_DOUBLE_EQ(-value_less, score::cpp::fmod_floored(-value_less, -value));
    EXPECT_DOUBLE_EQ(0.0, score::cpp::fmod_floored(-value, -value));
    EXPECT_DOUBLE_EQ(value - value_more, score::cpp::fmod_floored(-value_more, -value));
}

///
/// \test Close to zero divisor is not allowed.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, fmod_floored_zero_divisor_float)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::ignore = std::isnan(score::cpp::fmod_floored(1.0F, 0.0F)));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::ignore = std::isnan(score::cpp::fmod_floored(1.0F, -0.0F)));
}

///
/// \test Test: Check mathematical constants.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17295346
TEST(math_test, constants)
{
    // float
    EXPECT_FLOAT_EQ(5.000000000000000000000000000000000000E-01F, score::cpp::math::constants::half<float>());
    EXPECT_FLOAT_EQ(3.333333333333333333333333333333333333E-01F, score::cpp::math::constants::third<float>());
    EXPECT_FLOAT_EQ(1.414213562373095048801688724209698078E+00F, score::cpp::math::constants::root_two<float>());
    EXPECT_FLOAT_EQ(1.732050807568877293527446341505872366E+00F, score::cpp::math::constants::root_three<float>());
    EXPECT_FLOAT_EQ(6.931471805599453094172321214581765680E-01F, score::cpp::math::constants::ln_two<float>());
    EXPECT_FLOAT_EQ(7.071067811865475244008443621048490392E-01F, score::cpp::math::constants::one_div_root_two<float>());

    EXPECT_FLOAT_EQ(3.141592653589793238462643383279502884E+00F, score::cpp::math::constants::pi<float>());
    EXPECT_FLOAT_EQ(6.283185307179586476925286766559005768E+00F, score::cpp::math::constants::two_pi<float>());
    EXPECT_FLOAT_EQ(1.570796326794896619231321691639751442E+00F, score::cpp::math::constants::half_pi<float>());
    EXPECT_FLOAT_EQ(0.785398163397448309615660845819875721E+00F, score::cpp::math::constants::quarter_pi<float>());

    EXPECT_FLOAT_EQ(score::cpp::math::constants::pi<float>(), 0.5F * score::cpp::math::constants::two_pi<float>());
    EXPECT_FLOAT_EQ(score::cpp::math::constants::pi<float>(), 2.0F * score::cpp::math::constants::half_pi<float>());
    EXPECT_FLOAT_EQ(score::cpp::math::constants::pi<float>(), 4.0F * score::cpp::math::constants::quarter_pi<float>());

    EXPECT_FLOAT_EQ(1.772453850905516027298167483341145182E+00F, score::cpp::math::constants::root_pi<float>());
    EXPECT_FLOAT_EQ(1.253314137315500251207882642405522626E+00F, score::cpp::math::constants::root_half_pi<float>());
    EXPECT_FLOAT_EQ(2.506628274631000502415765284811045253E+00F, score::cpp::math::constants::root_two_pi<float>());
    EXPECT_FLOAT_EQ(5.641895835477562869480794515607725858E-01F, score::cpp::math::constants::one_div_root_pi<float>());
    EXPECT_FLOAT_EQ(3.989422804014326779399460599343818684E-01F, score::cpp::math::constants::one_div_root_two_pi<float>());

    EXPECT_FLOAT_EQ(2.718281828459045235360287471352662497E+00F, score::cpp::math::constants::e<float>());

    // double
    EXPECT_DOUBLE_EQ(5.000000000000000000000000000000000000E-01, score::cpp::math::constants::half<double>());
    EXPECT_DOUBLE_EQ(3.333333333333333333333333333333333333E-01, score::cpp::math::constants::third<double>());
    EXPECT_DOUBLE_EQ(1.414213562373095048801688724209698078E+00, score::cpp::math::constants::root_two<double>());
    EXPECT_DOUBLE_EQ(1.732050807568877293527446341505872366E+00, score::cpp::math::constants::root_three<double>());
    EXPECT_DOUBLE_EQ(6.931471805599453094172321214581765680E-01, score::cpp::math::constants::ln_two<double>());
    EXPECT_DOUBLE_EQ(7.071067811865475244008443621048490392E-01, score::cpp::math::constants::one_div_root_two<double>());

    EXPECT_DOUBLE_EQ(3.141592653589793238462643383279502884E+00, score::cpp::math::constants::pi<double>());
    EXPECT_DOUBLE_EQ(6.283185307179586476925286766559005768E+00, score::cpp::math::constants::two_pi<double>());
    EXPECT_DOUBLE_EQ(1.570796326794896619231321691639751442E+00, score::cpp::math::constants::half_pi<double>());
    EXPECT_DOUBLE_EQ(0.785398163397448309615660845819875721E+00, score::cpp::math::constants::quarter_pi<double>());

    EXPECT_DOUBLE_EQ(score::cpp::math::constants::pi<double>(), 0.5 * score::cpp::math::constants::two_pi<double>());
    EXPECT_DOUBLE_EQ(score::cpp::math::constants::pi<double>(), 2.0 * score::cpp::math::constants::half_pi<double>());
    EXPECT_DOUBLE_EQ(score::cpp::math::constants::pi<double>(), 4.0 * score::cpp::math::constants::quarter_pi<double>());

    EXPECT_DOUBLE_EQ(1.772453850905516027298167483341145182E+00, score::cpp::math::constants::root_pi<double>());
    EXPECT_DOUBLE_EQ(1.253314137315500251207882642405522626E+00, score::cpp::math::constants::root_half_pi<double>());
    EXPECT_DOUBLE_EQ(2.506628274631000502415765284811045253E+00, score::cpp::math::constants::root_two_pi<double>());
    EXPECT_DOUBLE_EQ(5.641895835477562869480794515607725858E-01, score::cpp::math::constants::one_div_root_pi<double>());
    EXPECT_DOUBLE_EQ(3.989422804014326779399460599343818684E-01, score::cpp::math::constants::one_div_root_two_pi<double>());

    EXPECT_DOUBLE_EQ(2.718281828459045235360287471352662497E+00, score::cpp::math::constants::e<double>());
}

///
/// \test Test: Check mathematical constants.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, deg_to_rad)
{
    // float
    EXPECT_FLOAT_EQ(1.745329251994329576923690768488612713E-02F, score::cpp::math::deg_to_rad(1.0F));
    EXPECT_FLOAT_EQ(2.967059728390360280770274306430641612E-01F, score::cpp::math::deg_to_rad(17.0F));
    EXPECT_FLOAT_EQ(1.570796326794896619231321691639751442E+00F, score::cpp::math::deg_to_rad(90.0F));
    EXPECT_FLOAT_EQ(2.897246558310587097693326675691097103E+00F, score::cpp::math::deg_to_rad(166.0F));
    EXPECT_FLOAT_EQ(3.141592653589793238462643383279502884E+00F, score::cpp::math::deg_to_rad(180.0F));
    EXPECT_FLOAT_EQ(5.497787143782138167309625920739130045E+00F, score::cpp::math::deg_to_rad(315.0F));
    EXPECT_FLOAT_EQ(6.283185307179586476925286766559005768E+00F, score::cpp::math::deg_to_rad(360.0F));
    EXPECT_FLOAT_EQ(-1.570796326794896619231321691639751442E+00F, score::cpp::math::deg_to_rad(-90.0F));

    // double
    EXPECT_DOUBLE_EQ(1.745329251994329576923690768488612713E-02, score::cpp::math::deg_to_rad(1.0));
    EXPECT_DOUBLE_EQ(2.967059728390360280770274306430641612E-01, score::cpp::math::deg_to_rad(17.0));
    EXPECT_DOUBLE_EQ(1.570796326794896619231321691639751442E+00, score::cpp::math::deg_to_rad(90.0));
    EXPECT_DOUBLE_EQ(2.897246558310587097693326675691097103E+00, score::cpp::math::deg_to_rad(166.0));
    EXPECT_DOUBLE_EQ(3.141592653589793238462643383279502884E+00, score::cpp::math::deg_to_rad(180.0));
    EXPECT_DOUBLE_EQ(5.497787143782138167309625920739130045E+00, score::cpp::math::deg_to_rad(315.0));
    EXPECT_DOUBLE_EQ(6.283185307179586476925286766559005768E+00, score::cpp::math::deg_to_rad(360.0));
    EXPECT_DOUBLE_EQ(-1.570796326794896619231321691639751442E+00, score::cpp::math::deg_to_rad(-90.0));
}

///
/// \test Test: Check mathematical constants.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, rad_to_deg)
{
    // float
    EXPECT_FLOAT_EQ(5.729577951308232087679815481410517033E+01F, score::cpp::math::rad_to_deg(1.0F));
    EXPECT_FLOAT_EQ(1.145915590261646417535963096282103406E+01F, score::cpp::math::rad_to_deg(0.2F));
    EXPECT_FLOAT_EQ(8.594366926962348131519723222115775549E+01F, score::cpp::math::rad_to_deg(1.5F));
    EXPECT_FLOAT_EQ(1.661577605879387305427146489609049939E+02F, score::cpp::math::rad_to_deg(2.9F));
    EXPECT_FLOAT_EQ(1.776169164905551947180742799237260280E+02F, score::cpp::math::rad_to_deg(3.1F));
    EXPECT_FLOAT_EQ(3.093972093706445327347100359961679197E+02F, score::cpp::math::rad_to_deg(5.4F));
    EXPECT_FLOAT_EQ(3.552338329811103894361485598474520560E+02F, score::cpp::math::rad_to_deg(6.2F));
    EXPECT_FLOAT_EQ(-8.594366926962348131519723222115775545E+01F, score::cpp::math::rad_to_deg(-1.5F));

    // double
    EXPECT_DOUBLE_EQ(5.729577951308232087679815481410517033E+01, score::cpp::math::rad_to_deg(1.0));
    EXPECT_DOUBLE_EQ(1.145915590261646417535963096282103406E+01, score::cpp::math::rad_to_deg(0.2));
    EXPECT_DOUBLE_EQ(8.594366926962348131519723222115775549E+01, score::cpp::math::rad_to_deg(1.5));
    EXPECT_DOUBLE_EQ(1.661577605879387305427146489609049939E+02, score::cpp::math::rad_to_deg(2.9));
    EXPECT_DOUBLE_EQ(1.776169164905551947180742799237260280E+02, score::cpp::math::rad_to_deg(3.1));
    EXPECT_DOUBLE_EQ(3.093972093706445327347100359961679197E+02, score::cpp::math::rad_to_deg(5.4));
    EXPECT_DOUBLE_EQ(3.552338329811103894361485598474520560E+02, score::cpp::math::rad_to_deg(6.2));
    EXPECT_DOUBLE_EQ(-8.594366926962348131519723222115775545E+01, score::cpp::math::rad_to_deg(-1.5));
}

///
/// \test Tests that all possible 32 bit integer powers of two are accepted: 1, 2, 4, 8, etc.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, is_power_of_two_test_true)
{
    for (std::int32_t i = 0; i < 31; ++i)
    {
        EXPECT_TRUE(score::cpp::math::is_power_of_two(1 << i));
    }
}

///
/// \test Tests that 3, 5, 9, etc. are not a power of two.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, is_power_of_two_false)
{
    for (std::int32_t i = 1; i < 31; ++i)
    {
        EXPECT_FALSE(score::cpp::math::is_power_of_two((1 << i) + 1));
    }
}

///
/// \test Tests that 0 is not a power of two.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, is_power_of_two_zero)
{
    EXPECT_FALSE(score::cpp::math::is_power_of_two(-0));
    EXPECT_FALSE(score::cpp::math::is_power_of_two(+0));
}

///
/// \test Tests that negative number are not considered as power of two.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, is_power_of_two_negative_values)
{
    EXPECT_FALSE(score::cpp::math::is_power_of_two(-1));
    EXPECT_FALSE(score::cpp::math::is_power_of_two(-4));
    EXPECT_FALSE(score::cpp::math::is_power_of_two(-8));
    EXPECT_FALSE(score::cpp::math::is_power_of_two(-16));
    EXPECT_FALSE(score::cpp::math::is_power_of_two(-32));
    EXPECT_FALSE(score::cpp::math::is_power_of_two(std::numeric_limits<std::int32_t>::lowest()));
}

///
/// \test The output of wrap_to_pi shall be in the interval [-pi,pi).
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, wrap_to_pi)
{
    // float
    {
        const float pi = score::cpp::math::constants::pi<float>();
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_pi(pi), -pi);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_pi(4.0F * pi), 0.0F);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_pi(5.0F * pi), -pi);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_pi(-4.0F * pi), 0.0F);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_pi(-5.0F * pi), -pi);

        const float plus_inf = std::numeric_limits<float>::infinity();
        const float minus_inf = -plus_inf;
        const float pi_less = std::nextafter(std::nextafter(pi, minus_inf), minus_inf);
        const float pi_more = std::nextafter(pi, plus_inf);
        EXPECT_FLOAT_EQ(pi_less, score::cpp::math::wrap_to_pi(pi_less));
        EXPECT_FLOAT_EQ(-pi_less, score::cpp::math::wrap_to_pi(-pi_less));
        EXPECT_FLOAT_EQ(-pi, score::cpp::math::wrap_to_pi(pi));
        EXPECT_FLOAT_EQ(-pi, score::cpp::math::wrap_to_pi(-pi));
        EXPECT_FLOAT_EQ(-pi_less, score::cpp::math::wrap_to_pi(pi_more));
        EXPECT_FLOAT_EQ(pi, score::cpp::math::wrap_to_pi(-pi_more));
    }
    // double
    {
        const double pi = score::cpp::math::constants::pi<double>();
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_pi(pi), -pi);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_pi(4.0 * pi), 0.0);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_pi(5.0 * pi), -pi);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_pi(-4.0 * pi), 0.0);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_pi(-5.0 * pi), -pi);

        const double plus_inf = std::numeric_limits<double>::infinity();
        const double minus_inf = -plus_inf;
        const double pi_less = std::nextafter(std::nextafter(pi, minus_inf), minus_inf);
        const double pi_more = std::nextafter(pi, plus_inf);
        EXPECT_DOUBLE_EQ(pi_less, score::cpp::math::wrap_to_pi(pi_less));
        EXPECT_DOUBLE_EQ(-pi_less, score::cpp::math::wrap_to_pi(-pi_less));
        EXPECT_DOUBLE_EQ(-pi, score::cpp::math::wrap_to_pi(pi));
        EXPECT_DOUBLE_EQ(-pi, score::cpp::math::wrap_to_pi(-pi));
        EXPECT_DOUBLE_EQ(-pi_less, score::cpp::math::wrap_to_pi(pi_more));
        EXPECT_DOUBLE_EQ(pi, score::cpp::math::wrap_to_pi(-pi_more));
    }
}

///
/// \test The output of wrap_to_two_pi shall be in the interval [0,2*pi).
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, wrap_to_two_pi)
{
    // float
    {
        const float pi = score::cpp::math::constants::pi<float>();
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_two_pi(pi), pi);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_two_pi(4.0F * pi), 0.0F);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_two_pi(5.0F * pi), pi);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_two_pi(-4.0F * pi), 0.0F);
        EXPECT_FLOAT_EQ(score::cpp::math::wrap_to_two_pi(-5.0F * pi), pi);

        const float plus_inf = std::numeric_limits<float>::infinity();
        const float minus_inf = -plus_inf;
        const float two_pi = 2.0F * pi;
        const float two_pi_less = std::nextafter(two_pi, minus_inf);
        const float two_pi_more = std::nextafter(two_pi, plus_inf);
        EXPECT_FLOAT_EQ(two_pi_less, score::cpp::math::wrap_to_two_pi(two_pi_less));
        EXPECT_FLOAT_EQ(two_pi - two_pi_less, score::cpp::math::wrap_to_two_pi(-two_pi_less));
        EXPECT_FLOAT_EQ(0.0, score::cpp::math::wrap_to_two_pi(two_pi));
        EXPECT_FLOAT_EQ(0.0, score::cpp::math::wrap_to_two_pi(-two_pi));
        EXPECT_FLOAT_EQ(two_pi_more - two_pi, score::cpp::math::wrap_to_two_pi(two_pi_more));
        EXPECT_FLOAT_EQ(two_pi, score::cpp::math::wrap_to_two_pi(-two_pi_more));
    }
    // double
    {
        const double pi = score::cpp::math::constants::pi<double>();
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_two_pi(pi), pi);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_two_pi(4.0 * pi), 0.0);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_two_pi(5.0 * pi), pi);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_two_pi(-4.0 * pi), 0.0);
        EXPECT_DOUBLE_EQ(score::cpp::math::wrap_to_two_pi(-5.0 * pi), pi);

        const double plus_inf = std::numeric_limits<double>::infinity();
        const double minus_inf = -plus_inf;
        const double two_pi = 2.0 * pi;
        const double two_pi_less = std::nextafter(two_pi, minus_inf);
        const double two_pi_more = std::nextafter(two_pi, plus_inf);
        EXPECT_DOUBLE_EQ(two_pi_less, score::cpp::math::wrap_to_two_pi(two_pi_less));
        EXPECT_DOUBLE_EQ(two_pi - two_pi_less, score::cpp::math::wrap_to_two_pi(-two_pi_less));
        EXPECT_DOUBLE_EQ(0.0, score::cpp::math::wrap_to_two_pi(two_pi));
        EXPECT_DOUBLE_EQ(0.0, score::cpp::math::wrap_to_two_pi(-two_pi));
        EXPECT_DOUBLE_EQ(two_pi_more - two_pi, score::cpp::math::wrap_to_two_pi(two_pi_more));
        EXPECT_DOUBLE_EQ(two_pi, score::cpp::math::wrap_to_two_pi(-two_pi_more));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, signum_positive_case_close_to_zero)
{
    EXPECT_EQ(1.0, score::cpp::math::signum(1e-12));
    EXPECT_EQ(1.0F, score::cpp::math::signum(1e-8F));
    EXPECT_EQ(1, score::cpp::math::signum(1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, signum_zero_case)
{
    EXPECT_EQ(0.0, score::cpp::math::signum(0.0));
    EXPECT_EQ(0.0F, score::cpp::math::signum(0.0F));
    EXPECT_EQ(0, score::cpp::math::signum(0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, signum_negative_case_close_to_zero)
{
    EXPECT_EQ(-1.0, score::cpp::math::signum(-1e-12));
    EXPECT_EQ(-1.0F, score::cpp::math::signum(-1e-8F));
    EXPECT_EQ(-1, score::cpp::math::signum(-1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, square)
{
    EXPECT_EQ(25U, score::cpp::math::square(5U));
    EXPECT_EQ(36, score::cpp::math::square(6));
    EXPECT_FLOAT_EQ(6.25F, score::cpp::math::square(2.5F));
}

template <typename T>
void test_div_round_closest_unsigned()
{
    constexpr T t_max = std::numeric_limits<T>::max();
    constexpr T t_max_minus_one = t_max - T{1};
    constexpr T t_max_minus_one_half = t_max_minus_one / T{2};

    EXPECT_EQ(score::cpp::math::div_round_closest(T{5}, T{10}), T{1});
    EXPECT_EQ(score::cpp::math::div_round_closest(T{5}, T{11}), T{0});
    EXPECT_EQ(score::cpp::math::div_round_closest(T{5}, T{9}), T{1});

    EXPECT_EQ(score::cpp::math::div_round_closest(T{0}, t_max), T{0});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max, T{1}), t_max);
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max, t_max_minus_one_half), T{2});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max_minus_one, t_max), T{1});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max, t_max_minus_one), T{1});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max_minus_one_half, t_max_minus_one), T{1});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max_minus_one, t_max_minus_one_half), T{2});

    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(score::cpp::math::div_round_closest(t_max, T{0}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, div_round_closest_unsigned)
{
    test_div_round_closest_unsigned<std::uint8_t>();
    test_div_round_closest_unsigned<std::uint16_t>();
    test_div_round_closest_unsigned<std::uint32_t>();
    test_div_round_closest_unsigned<std::uint64_t>();
}

template <typename T>
void test_div_round_closest_signed()
{
    test_div_round_closest_unsigned<T>();

    constexpr T t_min = std::numeric_limits<T>::min();
    constexpr T t_min_half = std::numeric_limits<T>::min() / T{2};
    constexpr T t_min_half_plus_one = t_min_half + T{1};
    constexpr T t_max = std::numeric_limits<T>::max();

    EXPECT_EQ(score::cpp::math::div_round_closest(T{5}, T{-10}), 0);
    EXPECT_EQ(score::cpp::math::div_round_closest(T{-5}, T{10}), 0);
    EXPECT_EQ(score::cpp::math::div_round_closest(T{-5}, T{-10}), 1);

    EXPECT_EQ(score::cpp::math::div_round_closest(T{5}, T{-11}), 0);
    EXPECT_EQ(score::cpp::math::div_round_closest(T{-5}, T{11}), 0);
    EXPECT_EQ(score::cpp::math::div_round_closest(T{-5}, T{-11}), 0);

    EXPECT_EQ(score::cpp::math::div_round_closest(T{5}, T{-9}), -1);
    EXPECT_EQ(score::cpp::math::div_round_closest(T{-5}, T{9}), -1);
    EXPECT_EQ(score::cpp::math::div_round_closest(T{-5}, T{-9}), 1);

    EXPECT_EQ(score::cpp::math::div_round_closest(T{0}, t_min), T{0});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_min, T{1}), t_min);
    EXPECT_EQ(score::cpp::math::div_round_closest(t_min, T{2}), t_min_half);
    EXPECT_EQ(score::cpp::math::div_round_closest(t_min, t_min_half), T{2});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_min, T{-t_min_half}), T{-2});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_min, t_max), T{-1});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max, t_min), T{-1});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_min_half, t_max), T{-1});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max, t_min_half), T{-2});
    EXPECT_EQ(score::cpp::math::div_round_closest(t_max, T{-2}), t_min_half_plus_one);

    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(score::cpp::math::div_round_closest(t_min, T{-1}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, div_round_closest_signed)
{
    test_div_round_closest_signed<std::int8_t>();
    test_div_round_closest_signed<std::int16_t>();
    test_div_round_closest_signed<std::int32_t>();
    test_div_round_closest_signed<std::int64_t>();
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, acos)
{
    EXPECT_FLOAT_EQ(std::acos(0.0F), score::cpp::acos(0.0F));
    EXPECT_DOUBLE_EQ(std::acos(0.0), score::cpp::acos(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, asin)
{
    EXPECT_FLOAT_EQ(std::asin(0.0F), score::cpp::asin(0.0F));
    EXPECT_DOUBLE_EQ(std::asin(0.0), score::cpp::asin(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, atan2)
{
    EXPECT_FLOAT_EQ(std::atan2(0.0F, 0.0F), score::cpp::atan2(0.0F, 0.0F));
    EXPECT_DOUBLE_EQ(std::atan2(0.0, 0.0), score::cpp::atan2(0.0, 0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, atan)
{
    EXPECT_FLOAT_EQ(std::atan(0.0F), score::cpp::atan(0.0F));
    EXPECT_DOUBLE_EQ(std::atan(0.0), score::cpp::atan(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, cos)
{
    EXPECT_FLOAT_EQ(std::cos(0.0F), score::cpp::cos(0.0F));
    EXPECT_DOUBLE_EQ(std::cos(0.0), score::cpp::cos(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, sin)
{
    EXPECT_FLOAT_EQ(std::sin(0.0F), score::cpp::sin(0.0F));
    EXPECT_DOUBLE_EQ(std::sin(0.0), score::cpp::sin(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, tan)
{
    EXPECT_FLOAT_EQ(std::tan(0.0F), score::cpp::tan(0.0F));
    EXPECT_DOUBLE_EQ(std::tan(0.0), score::cpp::tan(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, acosh)
{
    EXPECT_FLOAT_EQ(std::acosh(1.0F), score::cpp::acosh(1.0F));
    EXPECT_DOUBLE_EQ(std::acosh(1.0), score::cpp::acosh(1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, asinh)
{
    EXPECT_FLOAT_EQ(std::asinh(0.0F), score::cpp::asinh(0.0F));
    EXPECT_DOUBLE_EQ(std::asinh(0.0), score::cpp::asinh(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, atanh)
{
    EXPECT_FLOAT_EQ(std::atanh(0.0F), score::cpp::atanh(0.0F));
    EXPECT_DOUBLE_EQ(std::atanh(0.0), score::cpp::atanh(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, cosh)
{
    EXPECT_FLOAT_EQ(std::cosh(0.0F), score::cpp::cosh(0.0F));
    EXPECT_DOUBLE_EQ(std::cosh(0.0), score::cpp::cosh(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, sinh)
{
    EXPECT_FLOAT_EQ(std::sinh(0.0F), score::cpp::sinh(0.0F));
    EXPECT_DOUBLE_EQ(std::sinh(0.0), score::cpp::sinh(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, tanh)
{
    EXPECT_FLOAT_EQ(std::tanh(0.0F), score::cpp::tanh(0.0F));
    EXPECT_DOUBLE_EQ(std::tanh(0.0), score::cpp::tanh(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, ilogb)
{
    EXPECT_EQ(std::ilogb(123.45F), score::cpp::ilogb(123.45F));
    EXPECT_EQ(std::ilogb(123.45), score::cpp::ilogb(123.45));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, log10)
{
    EXPECT_FLOAT_EQ(std::log10(10.0F), score::cpp::log10(10.0F));
    EXPECT_DOUBLE_EQ(std::log10(10.0), score::cpp::log10(10.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, log1p)
{
    EXPECT_FLOAT_EQ(std::log1p(-0.0F), score::cpp::log1p(-0.0F));
    EXPECT_DOUBLE_EQ(std::log1p(-0.0), score::cpp::log1p(-0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, log2)
{
    EXPECT_FLOAT_EQ(std::log2(1.0F), score::cpp::log2(1.0F));
    EXPECT_DOUBLE_EQ(std::log2(1.0), score::cpp::log2(1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, logb)
{
    EXPECT_FLOAT_EQ(std::logb(123.45F), score::cpp::logb(123.45F));
    EXPECT_DOUBLE_EQ(std::logb(123.45), score::cpp::logb(123.45));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, log)
{
    EXPECT_FLOAT_EQ(std::log(1.0F), score::cpp::log(1.0F));
    EXPECT_DOUBLE_EQ(std::log(1.0), score::cpp::log(1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmax)
{
    {
        EXPECT_FLOAT_EQ(std::fmax(-1.0F, -0.0F), score::cpp::fmax(-1.0F, -0.0F));
        EXPECT_FLOAT_EQ(std::fmax(-0.0F, -1.0F), score::cpp::fmax(-0.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::fmax(-1.0F, 1.0F), score::cpp::fmax(-1.0F, 1.0F));
        EXPECT_FLOAT_EQ(std::fmax(1.0F, -1.0F), score::cpp::fmax(1.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::fmax(1.0F, 0.0F), score::cpp::fmax(1.0F, 0.0F));
        EXPECT_FLOAT_EQ(std::fmax(0.0F, 1.0F), score::cpp::fmax(0.0F, 1.0F));
    }
    {
        EXPECT_DOUBLE_EQ(std::fmax(-1.0, -0.0), score::cpp::fmax(-1.0, -0.0));
        EXPECT_DOUBLE_EQ(std::fmax(-0.0, -1.0), score::cpp::fmax(-0.0, -1.0));
        EXPECT_DOUBLE_EQ(std::fmax(-1.0, 1.0), score::cpp::fmax(-1.0, 1.0));
        EXPECT_DOUBLE_EQ(std::fmax(1.0, -1.0), score::cpp::fmax(1.0, -1.0));
        EXPECT_DOUBLE_EQ(std::fmax(1.0, 0.0), score::cpp::fmax(1.0, 0.0));
        EXPECT_DOUBLE_EQ(std::fmax(0.0, 1.0), score::cpp::fmax(0.0, 1.0));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmax_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::fmax(-inf, -0.0F), score::cpp::fmax(-inf, -0.0F));
        EXPECT_FLOAT_EQ(std::fmax(-0.0F, -inf), score::cpp::fmax(-0.0F, -inf));
        EXPECT_FLOAT_EQ(std::fmax(inf, 0.0F), score::cpp::fmax(inf, 0.0F));
        EXPECT_FLOAT_EQ(std::fmax(0.0F, inf), score::cpp::fmax(0.0F, inf));
        EXPECT_FLOAT_EQ(std::fmax(inf, -inf), score::cpp::fmax(inf, -inf));
        EXPECT_FLOAT_EQ(std::fmax(-inf, inf), score::cpp::fmax(-inf, inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::fmax(-inf, -0.0), score::cpp::fmax(-inf, -0.0));
        EXPECT_DOUBLE_EQ(std::fmax(-0.0, -inf), score::cpp::fmax(-0.0, -inf));
        EXPECT_DOUBLE_EQ(std::fmax(inf, 0.0), score::cpp::fmax(inf, 0.0));
        EXPECT_DOUBLE_EQ(std::fmax(0.0, inf), score::cpp::fmax(0.0, inf));
        EXPECT_DOUBLE_EQ(std::fmax(inf, -inf), score::cpp::fmax(inf, -inf));
        EXPECT_DOUBLE_EQ(std::fmax(-inf, inf), score::cpp::fmax(-inf, inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmax_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::fmax(-nan, -0.0F), score::cpp::fmax(-nan, -0.0F));
        EXPECT_FLOAT_EQ(std::fmax(-0.0F, -nan), score::cpp::fmax(-0.0F, -nan));
        EXPECT_FLOAT_EQ(std::fmax(nan, 0.0F), score::cpp::fmax(nan, 0.0F));
        EXPECT_FLOAT_EQ(std::fmax(0.0F, nan), score::cpp::fmax(0.0F, nan));
        EXPECT_FLOAT_EQ(std::fmax(-nan, -inf), score::cpp::fmax(-nan, -inf));
        EXPECT_FLOAT_EQ(std::fmax(-inf, -nan), score::cpp::fmax(-inf, -nan));
        EXPECT_FLOAT_EQ(std::fmax(nan, inf), score::cpp::fmax(nan, inf));
        EXPECT_FLOAT_EQ(std::fmax(inf, nan), score::cpp::fmax(inf, nan));
        EXPECT_TRUE(std::isnan(std::fmax(-nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmax(nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmax(-nan, nan)));
        EXPECT_TRUE(std::isnan(std::fmax(nan, nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::fmax(-nan, -0.0), score::cpp::fmax(-nan, -0.0));
        EXPECT_DOUBLE_EQ(std::fmax(-0.0, -nan), score::cpp::fmax(-0.0, -nan));
        EXPECT_DOUBLE_EQ(std::fmax(nan, 0.0), score::cpp::fmax(nan, 0.0));
        EXPECT_DOUBLE_EQ(std::fmax(0.0, nan), score::cpp::fmax(0.0, nan));
        EXPECT_DOUBLE_EQ(std::fmax(-nan, -inf), score::cpp::fmax(-nan, -inf));
        EXPECT_DOUBLE_EQ(std::fmax(-inf, -nan), score::cpp::fmax(-inf, -nan));
        EXPECT_DOUBLE_EQ(std::fmax(nan, inf), score::cpp::fmax(nan, inf));
        EXPECT_DOUBLE_EQ(std::fmax(inf, nan), score::cpp::fmax(inf, nan));
        EXPECT_TRUE(std::isnan(std::fmax(-nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmax(nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmax(-nan, nan)));
        EXPECT_TRUE(std::isnan(std::fmax(nan, nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmax_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FLOAT_EQ(std::fmax(-denorm, -1.0F), score::cpp::fmax(-denorm, -1.0F));
        EXPECT_FLOAT_EQ(std::fmax(-1.0F, -denorm), score::cpp::fmax(-1.0F, -denorm));
        EXPECT_FLOAT_EQ(std::fmax(-denorm, -0.0F), score::cpp::fmax(-denorm, -0.0F));
        EXPECT_FLOAT_EQ(std::fmax(-0.0F, -denorm), score::cpp::fmax(-0.0F, -denorm));
        EXPECT_FLOAT_EQ(std::fmax(-denorm, denorm), score::cpp::fmax(-denorm, denorm));
        EXPECT_FLOAT_EQ(std::fmax(denorm, -denorm), score::cpp::fmax(denorm, -denorm));
        EXPECT_FLOAT_EQ(std::fmax(denorm, 0.0F), score::cpp::fmax(denorm, 0.0F));
        EXPECT_FLOAT_EQ(std::fmax(0.0F, denorm), score::cpp::fmax(0.0F, denorm));
        EXPECT_FLOAT_EQ(std::fmax(denorm, 1.0F), score::cpp::fmax(denorm, 1.0F));
        EXPECT_FLOAT_EQ(std::fmax(1.0F, denorm), score::cpp::fmax(1.0F, denorm));
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_DOUBLE_EQ(std::fmax(-denorm, -1.0), score::cpp::fmax(-denorm, -1.0));
        EXPECT_DOUBLE_EQ(std::fmax(-1.0, -denorm), score::cpp::fmax(-1.0, -denorm));
        EXPECT_DOUBLE_EQ(std::fmax(-denorm, -0.0), score::cpp::fmax(-denorm, -0.0));
        EXPECT_DOUBLE_EQ(std::fmax(-0.0, -denorm), score::cpp::fmax(-0.0, -denorm));
        EXPECT_DOUBLE_EQ(std::fmax(-denorm, denorm), score::cpp::fmax(-denorm, denorm));
        EXPECT_DOUBLE_EQ(std::fmax(denorm, -denorm), score::cpp::fmax(denorm, -denorm));
        EXPECT_DOUBLE_EQ(std::fmax(denorm, 0.0), score::cpp::fmax(denorm, 0.0));
        EXPECT_DOUBLE_EQ(std::fmax(0.0, denorm), score::cpp::fmax(0.0, denorm));
        EXPECT_DOUBLE_EQ(std::fmax(denorm, 1.0), score::cpp::fmax(denorm, 1.0));
        EXPECT_DOUBLE_EQ(std::fmax(1.0, denorm), score::cpp::fmax(1.0, denorm));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmin)
{
    {
        EXPECT_FLOAT_EQ(std::fmin(-1.0F, -0.0F), score::cpp::fmin(-1.0F, -0.0F));
        EXPECT_FLOAT_EQ(std::fmin(-0.0F, -1.0F), score::cpp::fmin(-0.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::fmin(-1.0F, 1.0F), score::cpp::fmin(-1.0F, 1.0F));
        EXPECT_FLOAT_EQ(std::fmin(1.0F, -1.0F), score::cpp::fmin(1.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::fmin(1.0F, 0.0F), score::cpp::fmin(1.0F, 0.0F));
        EXPECT_FLOAT_EQ(std::fmin(0.0F, 1.0F), score::cpp::fmin(0.0F, 1.0F));
    }
    {
        EXPECT_DOUBLE_EQ(std::fmin(-1.0, -0.0), score::cpp::fmin(-1.0, -0.0));
        EXPECT_DOUBLE_EQ(std::fmin(-0.0, -1.0), score::cpp::fmin(-0.0, -1.0));
        EXPECT_DOUBLE_EQ(std::fmin(-1.0, 1.0), score::cpp::fmin(-1.0, 1.0));
        EXPECT_DOUBLE_EQ(std::fmin(1.0, -1.0), score::cpp::fmin(1.0, -1.0));
        EXPECT_DOUBLE_EQ(std::fmin(1.0, 0.0), score::cpp::fmin(1.0, 0.0));
        EXPECT_DOUBLE_EQ(std::fmin(0.0, 1.0), score::cpp::fmin(0.0, 1.0));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmin_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::fmin(-inf, -0.0F), score::cpp::fmin(-inf, -0.0F));
        EXPECT_FLOAT_EQ(std::fmin(-0.0F, -inf), score::cpp::fmin(-0.0F, -inf));
        EXPECT_FLOAT_EQ(std::fmin(inf, 0.0F), score::cpp::fmin(inf, 0.0F));
        EXPECT_FLOAT_EQ(std::fmin(0.0F, inf), score::cpp::fmin(0.0F, inf));
        EXPECT_FLOAT_EQ(std::fmin(inf, -inf), score::cpp::fmin(inf, -inf));
        EXPECT_FLOAT_EQ(std::fmin(-inf, inf), score::cpp::fmin(-inf, inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::fmin(-inf, -0.0), score::cpp::fmin(-inf, -0.0));
        EXPECT_DOUBLE_EQ(std::fmin(-0.0, -inf), score::cpp::fmin(-0.0, -inf));
        EXPECT_DOUBLE_EQ(std::fmin(inf, 0.0), score::cpp::fmin(inf, 0.0));
        EXPECT_DOUBLE_EQ(std::fmin(0.0, inf), score::cpp::fmin(0.0, inf));
        EXPECT_DOUBLE_EQ(std::fmin(inf, -inf), score::cpp::fmin(inf, -inf));
        EXPECT_DOUBLE_EQ(std::fmin(-inf, inf), score::cpp::fmin(-inf, inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmin_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::fmin(-nan, -0.0F), score::cpp::fmin(-nan, -0.0F));
        EXPECT_FLOAT_EQ(std::fmin(-0.0F, -nan), score::cpp::fmin(-0.0F, -nan));
        EXPECT_FLOAT_EQ(std::fmin(nan, 0.0F), score::cpp::fmin(nan, 0.0F));
        EXPECT_FLOAT_EQ(std::fmin(0.0F, nan), score::cpp::fmin(0.0F, nan));
        EXPECT_FLOAT_EQ(std::fmin(-nan, -inf), score::cpp::fmin(-nan, -inf));
        EXPECT_FLOAT_EQ(std::fmin(-inf, -nan), score::cpp::fmin(-inf, -nan));
        EXPECT_FLOAT_EQ(std::fmin(nan, inf), score::cpp::fmin(nan, inf));
        EXPECT_FLOAT_EQ(std::fmin(inf, nan), score::cpp::fmin(inf, nan));
        EXPECT_TRUE(std::isnan(std::fmin(-nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmin(nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmin(-nan, nan)));
        EXPECT_TRUE(std::isnan(std::fmin(nan, nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::fmin(-nan, -0.0), score::cpp::fmin(-nan, -0.0));
        EXPECT_DOUBLE_EQ(std::fmin(-0.0, -nan), score::cpp::fmin(-0.0, -nan));
        EXPECT_DOUBLE_EQ(std::fmin(nan, 0.0), score::cpp::fmin(nan, 0.0));
        EXPECT_DOUBLE_EQ(std::fmin(0.0, nan), score::cpp::fmin(0.0, nan));
        EXPECT_DOUBLE_EQ(std::fmin(-nan, -inf), score::cpp::fmin(-nan, -inf));
        EXPECT_DOUBLE_EQ(std::fmin(-inf, -nan), score::cpp::fmin(-inf, -nan));
        EXPECT_DOUBLE_EQ(std::fmin(nan, inf), score::cpp::fmin(nan, inf));
        EXPECT_DOUBLE_EQ(std::fmin(inf, nan), score::cpp::fmin(inf, nan));
        EXPECT_TRUE(std::isnan(std::fmin(-nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmin(nan, -nan)));
        EXPECT_TRUE(std::isnan(std::fmin(-nan, nan)));
        EXPECT_TRUE(std::isnan(std::fmin(nan, nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmin_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FLOAT_EQ(std::fmin(-denorm, -1.0F), score::cpp::fmin(-denorm, -1.0F));
        EXPECT_FLOAT_EQ(std::fmin(-1.0F, -denorm), score::cpp::fmin(-1.0F, -denorm));
        EXPECT_FLOAT_EQ(std::fmin(-denorm, -0.0F), score::cpp::fmin(-denorm, -0.0F));
        EXPECT_FLOAT_EQ(std::fmin(-0.0F, -denorm), score::cpp::fmin(-0.0F, -denorm));
        EXPECT_FLOAT_EQ(std::fmin(-denorm, denorm), score::cpp::fmin(-denorm, denorm));
        EXPECT_FLOAT_EQ(std::fmin(denorm, -denorm), score::cpp::fmin(denorm, -denorm));
        EXPECT_FLOAT_EQ(std::fmin(denorm, 0.0F), score::cpp::fmin(denorm, 0.0F));
        EXPECT_FLOAT_EQ(std::fmin(0.0F, denorm), score::cpp::fmin(0.0F, denorm));
        EXPECT_FLOAT_EQ(std::fmin(denorm, 1.0F), score::cpp::fmin(denorm, 1.0F));
        EXPECT_FLOAT_EQ(std::fmin(1.0F, denorm), score::cpp::fmin(1.0F, denorm));
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_DOUBLE_EQ(std::fmin(-denorm, -1.0), score::cpp::fmin(-denorm, -1.0));
        EXPECT_DOUBLE_EQ(std::fmin(-1.0, -denorm), score::cpp::fmin(-1.0, -denorm));
        EXPECT_DOUBLE_EQ(std::fmin(-denorm, -0.0), score::cpp::fmin(-denorm, -0.0));
        EXPECT_DOUBLE_EQ(std::fmin(-0.0, -denorm), score::cpp::fmin(-0.0, -denorm));
        EXPECT_DOUBLE_EQ(std::fmin(-denorm, denorm), score::cpp::fmin(-denorm, denorm));
        EXPECT_DOUBLE_EQ(std::fmin(denorm, -denorm), score::cpp::fmin(denorm, -denorm));
        EXPECT_DOUBLE_EQ(std::fmin(denorm, 0.0), score::cpp::fmin(denorm, 0.0));
        EXPECT_DOUBLE_EQ(std::fmin(0.0, denorm), score::cpp::fmin(0.0, denorm));
        EXPECT_DOUBLE_EQ(std::fmin(denorm, 1.0), score::cpp::fmin(denorm, 1.0));
        EXPECT_DOUBLE_EQ(std::fmin(1.0, denorm), score::cpp::fmin(1.0, denorm));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, ceil)
{
    {
        EXPECT_FLOAT_EQ(std::ceil(-0.5F), score::cpp::ceil(-0.5F));
        EXPECT_FLOAT_EQ(std::ceil(-0.0F), score::cpp::ceil(-0.0F));
        EXPECT_FLOAT_EQ(std::ceil(0.0F), score::cpp::ceil(0.0F));
        EXPECT_FLOAT_EQ(std::ceil(0.5F), score::cpp::ceil(0.5F));
    }
    {
        EXPECT_FLOAT_EQ(std::ceil(-0.5F), ::score_future_cpp_ceilf(-0.5F));
        EXPECT_FLOAT_EQ(std::ceil(-0.0F), ::score_future_cpp_ceilf(-0.0F));
        EXPECT_FLOAT_EQ(std::ceil(0.0F), ::score_future_cpp_ceilf(0.0F));
        EXPECT_FLOAT_EQ(std::ceil(0.5F), ::score_future_cpp_ceilf(0.5F));
    }
    {
        EXPECT_DOUBLE_EQ(std::ceil(-0.5), score::cpp::ceil(-0.5));
        EXPECT_DOUBLE_EQ(std::ceil(-0.0), score::cpp::ceil(-0.0));
        EXPECT_DOUBLE_EQ(std::ceil(0.0), score::cpp::ceil(0.0));
        EXPECT_DOUBLE_EQ(std::ceil(0.5), score::cpp::ceil(0.5));
    }
    {
        EXPECT_DOUBLE_EQ(std::ceil(-0.5), ::score_future_cpp_ceil(-0.5));
        EXPECT_DOUBLE_EQ(std::ceil(-0.0), ::score_future_cpp_ceil(-0.0));
        EXPECT_DOUBLE_EQ(std::ceil(0.0), ::score_future_cpp_ceil(0.0));
        EXPECT_DOUBLE_EQ(std::ceil(0.5), ::score_future_cpp_ceil(0.5));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, ceil_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::ceil(-inf), score::cpp::ceil(-inf));
        EXPECT_FLOAT_EQ(std::ceil(inf), score::cpp::ceil(inf));
        EXPECT_FLOAT_EQ(std::ceil(-inf), ::score_future_cpp_ceilf(-inf));
        EXPECT_FLOAT_EQ(std::ceil(inf), ::score_future_cpp_ceilf(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::ceil(-inf), score::cpp::ceil(-inf));
        EXPECT_DOUBLE_EQ(std::ceil(inf), score::cpp::ceil(inf));
        EXPECT_DOUBLE_EQ(std::ceil(-inf), ::score_future_cpp_ceil(-inf));
        EXPECT_DOUBLE_EQ(std::ceil(inf), ::score_future_cpp_ceil(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, ceil_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::ceil(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::ceil(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_ceilf(nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::ceil(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::ceil(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_ceil(nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, floor)
{
    {
        EXPECT_FLOAT_EQ(std::floor(-0.5F), score::cpp::floor(-0.5F));
        EXPECT_FLOAT_EQ(std::floor(-0.0F), score::cpp::floor(-0.0F));
        EXPECT_FLOAT_EQ(std::floor(0.0F), score::cpp::floor(0.0F));
        EXPECT_FLOAT_EQ(std::floor(0.5F), score::cpp::floor(0.5F));
    }
    {
        EXPECT_DOUBLE_EQ(std::floor(-0.5), score::cpp::floor(-0.5));
        EXPECT_DOUBLE_EQ(std::floor(-0.0), score::cpp::floor(-0.0));
        EXPECT_DOUBLE_EQ(std::floor(0.0), score::cpp::floor(0.0));
        EXPECT_DOUBLE_EQ(std::floor(0.5), score::cpp::floor(0.5));
    }
    {
        EXPECT_FLOAT_EQ(std::floor(-0.5F), ::score_future_cpp_floorf(-0.5F));
        EXPECT_FLOAT_EQ(std::floor(-0.0F), ::score_future_cpp_floorf(-0.0F));
        EXPECT_FLOAT_EQ(std::floor(0.0F), ::score_future_cpp_floorf(0.0F));
        EXPECT_FLOAT_EQ(std::floor(0.5F), ::score_future_cpp_floorf(0.5F));
    }
    {
        EXPECT_DOUBLE_EQ(std::floor(-0.5), ::score_future_cpp_floor(-0.5));
        EXPECT_DOUBLE_EQ(std::floor(-0.0), ::score_future_cpp_floor(-0.0));
        EXPECT_DOUBLE_EQ(std::floor(0.0), ::score_future_cpp_floor(0.0));
        EXPECT_DOUBLE_EQ(std::floor(0.5), ::score_future_cpp_floor(0.5));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, floor_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::floor(-inf), score::cpp::floor(-inf));
        EXPECT_FLOAT_EQ(std::floor(inf), score::cpp::floor(inf));
        EXPECT_FLOAT_EQ(std::floor(-inf), ::score_future_cpp_floorf(-inf));
        EXPECT_FLOAT_EQ(std::floor(inf), ::score_future_cpp_floorf(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::floor(-inf), score::cpp::floor(-inf));
        EXPECT_DOUBLE_EQ(std::floor(inf), score::cpp::floor(inf));
        EXPECT_DOUBLE_EQ(std::floor(-inf), ::score_future_cpp_floor(-inf));
        EXPECT_DOUBLE_EQ(std::floor(inf), ::score_future_cpp_floor(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, floor_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::floor(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::floor(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_floorf(nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::floor(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::floor(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_floor(nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, lrint)
{
    EXPECT_EQ(std::lrint(2.1F), score::cpp::lrint(2.1F));
    EXPECT_EQ(std::lrint(2.1), score::cpp::lrint(2.1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, lround)
{
    EXPECT_EQ(std::lround(0.4F), score::cpp::lround(0.4F));
    EXPECT_EQ(std::lround(0.4), score::cpp::lround(0.4));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, nearbyint)
{
    EXPECT_FLOAT_EQ(std::nearbyint(1.5F), score::cpp::nearbyint(1.5F));
    EXPECT_DOUBLE_EQ(std::nearbyint(1.5), score::cpp::nearbyint(1.5));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, nextafter)
{
    EXPECT_FLOAT_EQ(std::nextafter(0.0F, 1.0F), score::cpp::nextafter(0.0F, 1.0F));
    EXPECT_DOUBLE_EQ(std::nextafter(0.0, 1.0), score::cpp::nextafter(0.0, 1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, nexttoward)
{
    EXPECT_FLOAT_EQ(std::nexttoward(0.0F, 1.0F), score::cpp::nexttoward(0.0F, 1.0F));
    EXPECT_DOUBLE_EQ(std::nexttoward(0.0, 1.0), score::cpp::nexttoward(0.0, 1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, rint)
{
    {
        EXPECT_FLOAT_EQ(std::rint(-0.9F), score::cpp::rint(-0.9F));
        EXPECT_FLOAT_EQ(std::rint(-0.5F), score::cpp::rint(-0.5F));
        EXPECT_FLOAT_EQ(std::rint(-0.0F), score::cpp::rint(-0.0F));
        EXPECT_FLOAT_EQ(std::rint(0.0F), score::cpp::rint(0.0F));
        EXPECT_FLOAT_EQ(std::rint(0.5F), score::cpp::rint(0.5F));
        EXPECT_FLOAT_EQ(std::rint(0.9F), score::cpp::rint(0.9F));
    }
    {
        EXPECT_DOUBLE_EQ(std::rint(-0.9), score::cpp::rint(-0.9));
        EXPECT_DOUBLE_EQ(std::rint(-0.5), score::cpp::rint(-0.5));
        EXPECT_DOUBLE_EQ(std::rint(-0.0), score::cpp::rint(-0.0));
        EXPECT_DOUBLE_EQ(std::rint(0.0), score::cpp::rint(0.0));
        EXPECT_DOUBLE_EQ(std::rint(0.5), score::cpp::rint(0.5));
        EXPECT_DOUBLE_EQ(std::rint(0.9), score::cpp::rint(0.9));
    }
    {
        EXPECT_FLOAT_EQ(std::rint(-0.9F), ::score_future_cpp_rintf(-0.9F));
        EXPECT_FLOAT_EQ(std::rint(-0.5F), ::score_future_cpp_rintf(-0.5F));
        EXPECT_FLOAT_EQ(std::rint(-0.0F), ::score_future_cpp_rintf(-0.0F));
        EXPECT_FLOAT_EQ(std::rint(0.0F), ::score_future_cpp_rintf(0.0F));
        EXPECT_FLOAT_EQ(std::rint(0.5F), ::score_future_cpp_rintf(0.5F));
        EXPECT_FLOAT_EQ(std::rint(0.9F), ::score_future_cpp_rintf(0.9F));
    }
    {
        EXPECT_DOUBLE_EQ(std::rint(-0.9), ::score_future_cpp_rint(-0.9));
        EXPECT_DOUBLE_EQ(std::rint(-0.5), ::score_future_cpp_rint(-0.5));
        EXPECT_DOUBLE_EQ(std::rint(-0.0), ::score_future_cpp_rint(-0.0));
        EXPECT_DOUBLE_EQ(std::rint(0.0), ::score_future_cpp_rint(0.0));
        EXPECT_DOUBLE_EQ(std::rint(0.5), ::score_future_cpp_rint(0.5));
        EXPECT_DOUBLE_EQ(std::rint(0.9), ::score_future_cpp_rint(0.9));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, rint_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::rint(-inf), score::cpp::rint(-inf));
        EXPECT_FLOAT_EQ(std::rint(inf), score::cpp::rint(inf));
        EXPECT_FLOAT_EQ(std::rint(-inf), ::score_future_cpp_rintf(-inf));
        EXPECT_FLOAT_EQ(std::rint(inf), ::score_future_cpp_rintf(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::rint(-inf), score::cpp::rint(-inf));
        EXPECT_DOUBLE_EQ(std::rint(inf), score::cpp::rint(inf));
        EXPECT_DOUBLE_EQ(std::rint(-inf), ::score_future_cpp_rint(-inf));
        EXPECT_DOUBLE_EQ(std::rint(inf), ::score_future_cpp_rint(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, rint_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::rint(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::rint(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_rintf(nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::rint(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::rint(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_rint(nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, round)
{
    {
        EXPECT_FLOAT_EQ(std::round(-2.5F), score::cpp::round(-2.5F));
        EXPECT_FLOAT_EQ(std::round(-1.5F), score::cpp::round(-1.5F));
        EXPECT_FLOAT_EQ(std::round(-0.5F), score::cpp::round(-0.5F));
        EXPECT_FLOAT_EQ(std::round(-0.0F), score::cpp::round(-0.0F));
        EXPECT_FLOAT_EQ(std::round(0.0F), score::cpp::round(0.0F));
        EXPECT_FLOAT_EQ(std::round(0.5F), score::cpp::round(0.5F));
        EXPECT_FLOAT_EQ(std::round(1.5F), score::cpp::round(1.5F));
        EXPECT_FLOAT_EQ(std::round(2.5F), score::cpp::round(2.5F));
    }
    {
        EXPECT_DOUBLE_EQ(std::round(-2.5), score::cpp::round(-2.5));
        EXPECT_DOUBLE_EQ(std::round(-1.5), score::cpp::round(-1.5));
        EXPECT_DOUBLE_EQ(std::round(-0.5), score::cpp::round(-0.5));
        EXPECT_DOUBLE_EQ(std::round(-0.0), score::cpp::round(-0.0));
        EXPECT_DOUBLE_EQ(std::round(0.0), score::cpp::round(0.0));
        EXPECT_DOUBLE_EQ(std::round(0.5), score::cpp::round(0.5));
        EXPECT_DOUBLE_EQ(std::round(1.5), score::cpp::round(1.5));
        EXPECT_DOUBLE_EQ(std::round(2.5), score::cpp::round(2.5));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, round_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::round(-inf), score::cpp::round(-inf));
        EXPECT_FLOAT_EQ(std::round(inf), score::cpp::round(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::round(-inf), score::cpp::round(-inf));
        EXPECT_DOUBLE_EQ(std::round(inf), score::cpp::round(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, round_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::round(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::round(nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::round(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::round(nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, trunc)
{
    {
        EXPECT_FLOAT_EQ(std::trunc(-0.5F), score::cpp::trunc(-0.5F));
        EXPECT_FLOAT_EQ(std::trunc(-0.0F), score::cpp::trunc(-0.0F));
        EXPECT_FLOAT_EQ(std::trunc(0.0F), score::cpp::trunc(0.0F));
        EXPECT_FLOAT_EQ(std::trunc(0.5F), score::cpp::trunc(0.5F));
    }
    {
        EXPECT_DOUBLE_EQ(std::trunc(-0.5), score::cpp::trunc(-0.5));
        EXPECT_DOUBLE_EQ(std::trunc(-0.0), score::cpp::trunc(-0.0));
        EXPECT_DOUBLE_EQ(std::trunc(0.0), score::cpp::trunc(0.0));
        EXPECT_DOUBLE_EQ(std::trunc(0.5), score::cpp::trunc(0.5));
    }

    {
        EXPECT_FLOAT_EQ(std::trunc(-0.5F), ::score_future_cpp_truncf(-0.5F));
        EXPECT_FLOAT_EQ(std::trunc(-0.0F), ::score_future_cpp_truncf(-0.0F));
        EXPECT_FLOAT_EQ(std::trunc(0.0F), ::score_future_cpp_truncf(0.0F));
        EXPECT_FLOAT_EQ(std::trunc(0.5F), ::score_future_cpp_truncf(0.5F));
    }
    {
        EXPECT_DOUBLE_EQ(std::trunc(-0.5), ::score_future_cpp_trunc(-0.5));
        EXPECT_DOUBLE_EQ(std::trunc(-0.0), ::score_future_cpp_trunc(-0.0));
        EXPECT_DOUBLE_EQ(std::trunc(0.0), ::score_future_cpp_trunc(0.0));
        EXPECT_DOUBLE_EQ(std::trunc(0.5), ::score_future_cpp_trunc(0.5));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, trunc_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::trunc(-inf), score::cpp::trunc(-inf));
        EXPECT_FLOAT_EQ(std::trunc(inf), score::cpp::trunc(inf));
        EXPECT_FLOAT_EQ(std::trunc(-inf), ::score_future_cpp_truncf(-inf));
        EXPECT_FLOAT_EQ(std::trunc(inf), ::score_future_cpp_truncf(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::trunc(-inf), score::cpp::trunc(-inf));
        EXPECT_DOUBLE_EQ(std::trunc(inf), score::cpp::trunc(inf));
        EXPECT_DOUBLE_EQ(std::trunc(-inf), ::score_future_cpp_trunc(-inf));
        EXPECT_DOUBLE_EQ(std::trunc(inf), ::score_future_cpp_trunc(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, trunc_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::trunc(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::trunc(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_truncf(nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::trunc(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::trunc(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_trunc(nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, erfc)
{
    EXPECT_FLOAT_EQ(std::erfc(0.0F), score::cpp::erfc(0.0F));
    EXPECT_DOUBLE_EQ(std::erfc(0.0), score::cpp::erfc(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, erf)
{
    EXPECT_FLOAT_EQ(std::erf(0.0F), score::cpp::erf(0.0F));
    EXPECT_DOUBLE_EQ(std::erf(0.0), score::cpp::erf(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, cbrt)
{
    EXPECT_FLOAT_EQ(std::cbrt(2.0F), score::cpp::cbrt(2.0F));
    EXPECT_DOUBLE_EQ(std::cbrt(2.0), score::cpp::cbrt(2.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, exp2)
{
    EXPECT_FLOAT_EQ(std::exp2(-0.0F), score::cpp::exp2(-0.0F));
    EXPECT_DOUBLE_EQ(std::exp2(-0.0), score::cpp::exp2(-0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, expm1)
{
    EXPECT_FLOAT_EQ(std::expm1(-0.0F), score::cpp::expm1(-0.0F));
    EXPECT_DOUBLE_EQ(std::expm1(-0.0), score::cpp::expm1(-0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, exp)
{
    EXPECT_FLOAT_EQ(std::exp(-0.0F), score::cpp::exp(-0.0F));
    EXPECT_DOUBLE_EQ(std::exp(-0.0), score::cpp::exp(-0.0));
    EXPECT_DOUBLE_EQ(std::exp(-0), score::cpp::exp(-0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, pow)
{
    EXPECT_FLOAT_EQ(std::pow(0.0F, 1.0F), score::cpp::pow(0.0F, 1.0F));
    EXPECT_DOUBLE_EQ(std::pow(0.0, 1.0), score::cpp::pow(0.0, 1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, int_pow)
{
    for (const double v : {std::numeric_limits<double>::min(),
                           std::numeric_limits<double>::max(),
                           std::numeric_limits<double>::lowest(),
                           std::numeric_limits<double>::infinity(),
                           -std::numeric_limits<double>::infinity(),
                           -0.0,
                           1.0,
                           -2.0})
    {
        EXPECT_DOUBLE_EQ(std::pow(v, 1.0), score::cpp::int_pow<1>(v));
        EXPECT_DOUBLE_EQ(std::pow(v, 2.0), score::cpp::int_pow<2>(v));
        EXPECT_DOUBLE_EQ(std::pow(v, 3.0), score::cpp::int_pow<3>(v));
        EXPECT_DOUBLE_EQ(std::pow(v, 6.0), score::cpp::int_pow<6>(v));
    }

    EXPECT_TRUE(std::isnan(score::cpp::int_pow<3>(std::numeric_limits<double>::quiet_NaN())));

    for (const float v : {std::numeric_limits<float>::min(),
                          std::numeric_limits<float>::max(),
                          std::numeric_limits<float>::lowest(),
                          std::numeric_limits<float>::infinity(),
                          -std::numeric_limits<float>::infinity(),
                          -0.0F,
                          1.0F,
                          -2.0F})
    {
        EXPECT_FLOAT_EQ(std::pow(v, 1.0F), score::cpp::int_pow<1>(v));
        EXPECT_FLOAT_EQ(std::pow(v, 2.0F), score::cpp::int_pow<2>(v));
        EXPECT_FLOAT_EQ(std::pow(v, 3.0F), score::cpp::int_pow<3>(v));
        EXPECT_FLOAT_EQ(std::pow(v, 6.0F), score::cpp::int_pow<6>(v));
    }
    EXPECT_TRUE(std::isnan(score::cpp::int_pow<3>(std::numeric_limits<float>::quiet_NaN())));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, sqrt)
{
    {
        EXPECT_FLOAT_EQ(std::sqrt(-0.0F), score::cpp::sqrt(-0.0F));
        EXPECT_FLOAT_EQ(std::sqrt(0.0F), score::cpp::sqrt(0.0F));
        EXPECT_FLOAT_EQ(std::sqrt(2.0F), score::cpp::sqrt(2.0F));
    }
    {
        EXPECT_DOUBLE_EQ(std::sqrt(-0.0), score::cpp::sqrt(-0.0));
        EXPECT_DOUBLE_EQ(std::sqrt(0.0), score::cpp::sqrt(0.0));
        EXPECT_DOUBLE_EQ(std::sqrt(2.0), score::cpp::sqrt(2.0));
    }

    {
        EXPECT_DOUBLE_EQ(std::sqrt(-0), score::cpp::sqrt(-0));
        EXPECT_DOUBLE_EQ(std::sqrt(0), score::cpp::sqrt(0));
        EXPECT_DOUBLE_EQ(std::sqrt(2), score::cpp::sqrt(2));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, sqrt_negative)
{
    {
        for (const float v : {-std::numeric_limits<float>::infinity(), -2.0F})
        {
            errno = 0;
            EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

            EXPECT_TRUE(std::isnan(std::sqrt(v)));

            EXPECT_EQ(std::fetestexcept(FE_INVALID), FE_INVALID);
            if (has_errno_support())
            {
                EXPECT_EQ(errno, EDOM);
            }
        }
        for (const float v : {-std::numeric_limits<float>::infinity(), -2.0F})
        {
            errno = 0;
            EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

            EXPECT_TRUE(std::isnan(score::cpp::sqrt(v)));

            EXPECT_EQ(std::fetestexcept(FE_INVALID), FE_INVALID);
            if (has_errno_support())
            {
                EXPECT_EQ(errno, EDOM);
            }
        }
    }
    {
        for (const double v : {-std::numeric_limits<double>::infinity(), -2.0})
        {
            errno = 0;
            EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

            EXPECT_TRUE(std::isnan(std::sqrt(v)));

            EXPECT_EQ(std::fetestexcept(FE_INVALID), FE_INVALID);
            if (has_errno_support())
            {
                EXPECT_EQ(errno, EDOM);
            }
        }
        for (const double v : {-std::numeric_limits<double>::infinity(), -2.0})
        {
            errno = 0;
            EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

            EXPECT_TRUE(std::isnan(score::cpp::sqrt(v)));

            EXPECT_EQ(std::fetestexcept(FE_INVALID), FE_INVALID);
            if (has_errno_support())
            {
                EXPECT_EQ(errno, EDOM);
            }
        }
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, sqrt_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::sqrt(inf), score::cpp::sqrt(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::sqrt(inf), score::cpp::sqrt(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, sqrt_nan)
{
    EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::sqrt(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::sqrt(nan)));
        EXPECT_TRUE(std::isnan(std::sqrt(-nan)));
        EXPECT_TRUE(std::isnan(score::cpp::sqrt(-nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::sqrt(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::sqrt(nan)));
        EXPECT_TRUE(std::isnan(std::sqrt(-nan)));
        EXPECT_TRUE(std::isnan(score::cpp::sqrt(-nan)));
    }

    EXPECT_EQ(std::fetestexcept(FE_ALL_EXCEPT), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, lgamma)
{
    EXPECT_FLOAT_EQ(std::lgamma(1.0F), score::cpp::lgamma(1.0F));
    EXPECT_DOUBLE_EQ(std::lgamma(1.0), score::cpp::lgamma(1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, tgamma)
{
    EXPECT_FLOAT_EQ(std::tgamma(1.0F), score::cpp::tgamma(1.0F));
    EXPECT_DOUBLE_EQ(std::tgamma(1.0), score::cpp::tgamma(1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fmod)
{
    EXPECT_FLOAT_EQ(std::fmod(0.0F, 1.0F), score::cpp::fmod(0.0F, 1.0F));
    EXPECT_DOUBLE_EQ(std::fmod(0.0, 1.0), score::cpp::fmod(0.0, 1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, modf)
{
    {
        float std_result{};
        float score_future_cpp_result{};
        EXPECT_FLOAT_EQ(std::modf(-0.0F, &std_result), score::cpp::modf(-0.0F, &score_future_cpp_result));
        EXPECT_FLOAT_EQ(std_result, score_future_cpp_result);
    }
    {
        double std_result{};
        double score_future_cpp_result{};
        EXPECT_DOUBLE_EQ(std::modf(-0.0, &std_result), score::cpp::modf(-0.0, &score_future_cpp_result));
        EXPECT_DOUBLE_EQ(std_result, score_future_cpp_result);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, remainder)
{
    EXPECT_FLOAT_EQ(std::remainder(0.0F, 1.0F), score::cpp::remainder(0.0F, 1.0F));
    EXPECT_DOUBLE_EQ(std::remainder(0.0, 1.0), score::cpp::remainder(0.0, 1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, remquo)
{
    {
        int std_result{};
        int score_future_cpp_result{};
        EXPECT_FLOAT_EQ(std::remquo(0.1F, 0.2F, &std_result), score::cpp::remquo(0.1F, 0.2F, &score_future_cpp_result));
        EXPECT_EQ(std_result, score_future_cpp_result);
    }
    {
        int std_result{};
        int score_future_cpp_result{};
        EXPECT_DOUBLE_EQ(std::remquo(0.1, 0.2, &std_result), score::cpp::remquo(0.1, 0.2, &score_future_cpp_result));
        EXPECT_EQ(std_result, score_future_cpp_result);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, copysign)
{
    {
        EXPECT_FLOAT_EQ(std::copysign(-0.0F, -1.0F), score::cpp::copysign(-0.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, -1.0F), score::cpp::copysign(0.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::copysign(-0.0F, 1.0F), score::cpp::copysign(-0.0F, 1.0F));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, 1.0F), score::cpp::copysign(0.0F, 1.0F));
    }
    {
        EXPECT_DOUBLE_EQ(std::copysign(-0.0, -1.0), score::cpp::copysign(-0.0, -1.0));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, -1.0), score::cpp::copysign(0.0, -1.0));
        EXPECT_DOUBLE_EQ(std::copysign(-0.0, 1.0), score::cpp::copysign(-0.0, 1.0));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, 1.0), score::cpp::copysign(0.0, 1.0));
    }
    {
        EXPECT_FLOAT_EQ(std::copysign(-0.0F, -1.0F), ::score_future_cpp_copysignf(-0.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, -1.0F), ::score_future_cpp_copysignf(0.0F, -1.0F));
        EXPECT_FLOAT_EQ(std::copysign(-0.0F, 1.0F), ::score_future_cpp_copysignf(-0.0F, 1.0F));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, 1.0F), ::score_future_cpp_copysignf(0.0F, 1.0F));
    }
    {
        EXPECT_DOUBLE_EQ(std::copysign(-0.0, -1.0), ::score_future_cpp_copysign(-0.0, -1.0));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, -1.0), ::score_future_cpp_copysign(0.0, -1.0));
        EXPECT_DOUBLE_EQ(std::copysign(-0.0, 1.0), ::score_future_cpp_copysign(-0.0, 1.0));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, 1.0), ::score_future_cpp_copysign(0.0, 1.0));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, copysign_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::copysign(0.0F, -inf), score::cpp::copysign(0.0F, -inf));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, inf), score::cpp::copysign(0.0F, inf));
        EXPECT_FLOAT_EQ(std::copysign(inf, -0.0F), score::cpp::copysign(inf, -0.0F));
        EXPECT_FLOAT_EQ(std::copysign(inf, 0.0F), score::cpp::copysign(inf, 0.0F));
        EXPECT_FLOAT_EQ(std::copysign(inf, -inf), score::cpp::copysign(inf, -inf));
        EXPECT_FLOAT_EQ(std::copysign(inf, inf), score::cpp::copysign(inf, inf));

        EXPECT_FLOAT_EQ(std::copysign(0.0F, -inf), ::score_future_cpp_copysignf(0.0F, -inf));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, inf), ::score_future_cpp_copysignf(0.0F, inf));
        EXPECT_FLOAT_EQ(std::copysign(inf, -0.0F), ::score_future_cpp_copysignf(inf, -0.0F));
        EXPECT_FLOAT_EQ(std::copysign(inf, 0.0F), ::score_future_cpp_copysignf(inf, 0.0F));
        EXPECT_FLOAT_EQ(std::copysign(inf, -inf), ::score_future_cpp_copysignf(inf, -inf));
        EXPECT_FLOAT_EQ(std::copysign(inf, inf), ::score_future_cpp_copysignf(inf, inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::copysign(0.0, -inf), score::cpp::copysign(0.0, -inf));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, inf), score::cpp::copysign(0.0, inf));
        EXPECT_DOUBLE_EQ(std::copysign(inf, -0.0), score::cpp::copysign(inf, -0.0));
        EXPECT_DOUBLE_EQ(std::copysign(inf, 0.0), score::cpp::copysign(inf, 0.0));
        EXPECT_DOUBLE_EQ(std::copysign(inf, -inf), score::cpp::copysign(inf, -inf));
        EXPECT_DOUBLE_EQ(std::copysign(inf, inf), score::cpp::copysign(inf, inf));

        EXPECT_DOUBLE_EQ(std::copysign(0.0, -inf), ::score_future_cpp_copysign(0.0, -inf));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, inf), ::score_future_cpp_copysign(0.0, inf));
        EXPECT_DOUBLE_EQ(std::copysign(inf, -0.0), ::score_future_cpp_copysign(inf, -0.0));
        EXPECT_DOUBLE_EQ(std::copysign(inf, 0.0), ::score_future_cpp_copysign(inf, 0.0));
        EXPECT_DOUBLE_EQ(std::copysign(inf, -inf), ::score_future_cpp_copysign(inf, -inf));
        EXPECT_DOUBLE_EQ(std::copysign(inf, inf), ::score_future_cpp_copysign(inf, inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, copysign_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_FLOAT_EQ(std::copysign(0.0F, -nan), score::cpp::copysign(0.0F, -nan));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, nan), score::cpp::copysign(0.0F, nan));
        EXPECT_TRUE(std::isnan(std::copysign(nan, -0.0F)));
        EXPECT_TRUE(std::isnan(std::copysign(nan, -nan)));
        EXPECT_TRUE(std::isnan(std::copysign(nan, 0.0F)));
        EXPECT_TRUE(std::isnan(std::copysign(nan, nan)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, -0.0F)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, -nan)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, 0.0F)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(std::copysign(-nan, 0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(-nan), score::cpp::bit_cast<std::uint32_t>(std::copysign(-nan, -0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(std::copysign(nan, 0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(-nan), score::cpp::bit_cast<std::uint32_t>(std::copysign(nan, -0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(score::cpp::copysign(-nan, 0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(-nan), score::cpp::bit_cast<std::uint32_t>(score::cpp::copysign(-nan, -0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(score::cpp::copysign(nan, 0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(-nan), score::cpp::bit_cast<std::uint32_t>(score::cpp::copysign(nan, -0.0F)));

        EXPECT_FLOAT_EQ(std::copysign(0.0F, -nan), ::score_future_cpp_copysignf(0.0F, -nan));
        EXPECT_FLOAT_EQ(std::copysign(0.0F, nan), ::score_future_cpp_copysignf(0.0F, nan));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysignf(nan, -0.0F)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysignf(nan, -nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysignf(nan, 0.0F)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysignf(nan, nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(::score_future_cpp_copysignf(-nan, 0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(-nan), score::cpp::bit_cast<std::uint32_t>(::score_future_cpp_copysignf(-nan, -0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(::score_future_cpp_copysignf(nan, 0.0F)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(-nan), score::cpp::bit_cast<std::uint32_t>(::score_future_cpp_copysignf(nan, -0.0F)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_DOUBLE_EQ(std::copysign(0.0, -nan), score::cpp::copysign(0.0, -nan));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, nan), score::cpp::copysign(0.0, nan));
        EXPECT_TRUE(std::isnan(std::copysign(nan, -0.0)));
        EXPECT_TRUE(std::isnan(std::copysign(nan, -nan)));
        EXPECT_TRUE(std::isnan(std::copysign(nan, 0.0)));
        EXPECT_TRUE(std::isnan(std::copysign(nan, nan)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, -0.0)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, -nan)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, 0.0)));
        EXPECT_TRUE(std::isnan(score::cpp::copysign(nan, nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(std::copysign(-nan, 0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(-nan), score::cpp::bit_cast<std::uint64_t>(std::copysign(-nan, -0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(std::copysign(nan, 0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(-nan), score::cpp::bit_cast<std::uint64_t>(std::copysign(nan, -0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(score::cpp::copysign(-nan, 0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(-nan), score::cpp::bit_cast<std::uint64_t>(score::cpp::copysign(-nan, -0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(score::cpp::copysign(nan, 0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(-nan), score::cpp::bit_cast<std::uint64_t>(score::cpp::copysign(nan, -0.0)));

        EXPECT_DOUBLE_EQ(std::copysign(0.0, -nan), ::score_future_cpp_copysign(0.0, -nan));
        EXPECT_DOUBLE_EQ(std::copysign(0.0, nan), ::score_future_cpp_copysign(0.0, nan));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysign(nan, -0.0)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysign(nan, -nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysign(nan, 0.0)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_copysign(nan, nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(::score_future_cpp_copysign(-nan, 0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(-nan), score::cpp::bit_cast<std::uint64_t>(::score_future_cpp_copysign(-nan, -0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(::score_future_cpp_copysign(nan, 0.0)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(-nan), score::cpp::bit_cast<std::uint64_t>(::score_future_cpp_copysign(nan, -0.0)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, abs_integral)
{
    EXPECT_EQ(std::abs(-2), score::cpp::abs(-2));
    EXPECT_EQ(std::abs(-2L), score::cpp::abs(-2L));
    EXPECT_EQ(std::abs(-2LL), score::cpp::abs(-2LL));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, abs_floating_point)
{
    {
        EXPECT_FLOAT_EQ(std::abs(-0.01F), score::cpp::abs(-0.01F));
        EXPECT_FLOAT_EQ(std::abs(-0.0F), score::cpp::abs(-0.0F));
        EXPECT_FLOAT_EQ(std::abs(0.0F), score::cpp::abs(0.0F));
        EXPECT_FLOAT_EQ(std::abs(0.01F), score::cpp::abs(-0.01F));

        const float low{std::numeric_limits<float>::lowest()};
        EXPECT_FLOAT_EQ(std::abs(low), score::cpp::abs(low));
        const float high{std::numeric_limits<float>::max()};
        EXPECT_FLOAT_EQ(std::abs(high), score::cpp::abs(high));
    }
    {
        EXPECT_DOUBLE_EQ(std::abs(-0.01), score::cpp::abs(-0.01));
        EXPECT_DOUBLE_EQ(std::abs(-0.0), score::cpp::abs(-0.0));
        EXPECT_DOUBLE_EQ(std::abs(0.0), score::cpp::abs(0.0));
        EXPECT_DOUBLE_EQ(std::abs(0.01), score::cpp::abs(-0.01));

        const double low{std::numeric_limits<double>::lowest()};
        EXPECT_DOUBLE_EQ(std::abs(low), score::cpp::abs(low));
        const double high{std::numeric_limits<double>::max()};
        EXPECT_DOUBLE_EQ(std::abs(high), score::cpp::abs(high));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, abs_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FLOAT_EQ(std::abs(-denorm), score::cpp::abs(-denorm));
        EXPECT_FLOAT_EQ(std::abs(denorm), score::cpp::abs(denorm));
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_DOUBLE_EQ(std::abs(-denorm), score::cpp::abs(-denorm));
        EXPECT_DOUBLE_EQ(std::abs(denorm), score::cpp::abs(denorm));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, abs_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::abs(-inf), score::cpp::abs(-inf));
        EXPECT_FLOAT_EQ(std::abs(inf), score::cpp::abs(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::abs(-inf), score::cpp::abs(-inf));
        EXPECT_DOUBLE_EQ(std::abs(inf), score::cpp::abs(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, abs_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::abs(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::abs(nan)));
        EXPECT_TRUE(std::isnan(std::abs(-nan)));
        EXPECT_TRUE(std::isnan(score::cpp::abs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(std::abs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(score::cpp::abs(-nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::abs(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::abs(nan)));
        EXPECT_TRUE(std::isnan(std::abs(-nan)));
        EXPECT_TRUE(std::isnan(score::cpp::abs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(std::abs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(score::cpp::abs(-nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fabs)
{
    {
        EXPECT_FLOAT_EQ(std::fabs(-0.01F), score::cpp::fabs(-0.01F));
        EXPECT_FLOAT_EQ(std::fabs(-0.0F), score::cpp::fabs(-0.0F));
        EXPECT_FLOAT_EQ(std::fabs(0.0F), score::cpp::fabs(0.0F));
        EXPECT_FLOAT_EQ(std::fabs(0.01F), score::cpp::fabs(-0.01F));

        EXPECT_FLOAT_EQ(std::fabs(-0.01F), ::score_future_cpp_fabsf(-0.01F));
        EXPECT_FLOAT_EQ(std::fabs(-0.0F), ::score_future_cpp_fabsf(-0.0F));
        EXPECT_FLOAT_EQ(std::fabs(0.0F), ::score_future_cpp_fabsf(0.0F));
        EXPECT_FLOAT_EQ(std::fabs(0.01F), ::score_future_cpp_fabsf(-0.01F));

        const float low{std::numeric_limits<float>::lowest()};
        EXPECT_FLOAT_EQ(std::fabs(low), score::cpp::fabs(low));
        EXPECT_FLOAT_EQ(std::fabs(low), ::score_future_cpp_fabsf(low));
        const float high{std::numeric_limits<float>::max()};
        EXPECT_FLOAT_EQ(std::fabs(high), score::cpp::fabs(high));
        EXPECT_FLOAT_EQ(std::fabs(high), ::score_future_cpp_fabsf(high));
    }
    {
        EXPECT_DOUBLE_EQ(std::fabs(-0.01), score::cpp::fabs(-0.01));
        EXPECT_DOUBLE_EQ(std::fabs(-0.0), score::cpp::fabs(-0.0));
        EXPECT_DOUBLE_EQ(std::fabs(0.0), score::cpp::fabs(0.0));
        EXPECT_DOUBLE_EQ(std::fabs(0.01), score::cpp::fabs(-0.01));

        EXPECT_DOUBLE_EQ(std::fabs(-0.01), ::score_future_cpp_fabs(-0.01));
        EXPECT_DOUBLE_EQ(std::fabs(-0.0), ::score_future_cpp_fabs(-0.0));
        EXPECT_DOUBLE_EQ(std::fabs(0.0), ::score_future_cpp_fabs(0.0));
        EXPECT_DOUBLE_EQ(std::fabs(0.01), ::score_future_cpp_fabs(-0.01));

        const double low{std::numeric_limits<double>::lowest()};
        EXPECT_DOUBLE_EQ(std::fabs(low), score::cpp::fabs(low));
        EXPECT_DOUBLE_EQ(std::fabs(low), ::score_future_cpp_fabs(low));
        const double high{std::numeric_limits<double>::max()};
        EXPECT_DOUBLE_EQ(std::fabs(high), score::cpp::fabs(high));
        EXPECT_DOUBLE_EQ(std::fabs(high), score_future_cpp_fabs(high));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fabs_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FLOAT_EQ(std::fabs(-denorm), score::cpp::fabs(-denorm));
        EXPECT_FLOAT_EQ(std::fabs(denorm), score::cpp::fabs(denorm));
        EXPECT_FLOAT_EQ(std::fabs(-denorm), ::score_future_cpp_fabsf(-denorm));
        EXPECT_FLOAT_EQ(std::fabs(denorm), ::score_future_cpp_fabsf(denorm));
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_DOUBLE_EQ(std::fabs(-denorm), score::cpp::fabs(-denorm));
        EXPECT_DOUBLE_EQ(std::fabs(denorm), score::cpp::fabs(denorm));
        EXPECT_DOUBLE_EQ(std::fabs(-denorm), ::score_future_cpp_fabs(-denorm));
        EXPECT_DOUBLE_EQ(std::fabs(denorm), ::score_future_cpp_fabs(denorm));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fabs_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FLOAT_EQ(std::fabs(-inf), score::cpp::fabs(-inf));
        EXPECT_FLOAT_EQ(std::fabs(inf), score::cpp::fabs(inf));
        EXPECT_FLOAT_EQ(std::fabs(-inf), ::score_future_cpp_fabsf(-inf));
        EXPECT_FLOAT_EQ(std::fabs(inf), ::score_future_cpp_fabsf(inf));
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_DOUBLE_EQ(std::fabs(-inf), score::cpp::fabs(-inf));
        EXPECT_DOUBLE_EQ(std::fabs(inf), score::cpp::fabs(inf));
        EXPECT_DOUBLE_EQ(std::fabs(-inf), ::score_future_cpp_fabs(-inf));
        EXPECT_DOUBLE_EQ(std::fabs(inf), ::score_future_cpp_fabs(inf));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fabs_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::fabs(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::fabs(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_fabsf(nan)));
        EXPECT_TRUE(std::isnan(std::fabs(-nan)));
        EXPECT_TRUE(std::isnan(score::cpp::fabs(-nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_fabsf(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(std::fabs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(score::cpp::fabs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint32_t>(nan), score::cpp::bit_cast<std::uint32_t>(::score_future_cpp_fabsf(-nan)));
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(std::isnan(std::fabs(nan)));
        EXPECT_TRUE(std::isnan(score::cpp::fabs(nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_fabs(nan)));
        EXPECT_TRUE(std::isnan(std::fabs(-nan)));
        EXPECT_TRUE(std::isnan(score::cpp::fabs(-nan)));
        EXPECT_TRUE(std::isnan(::score_future_cpp_fabs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(std::fabs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(score::cpp::fabs(-nan)));
        EXPECT_EQ(score::cpp::bit_cast<std::uint64_t>(nan), score::cpp::bit_cast<std::uint64_t>(::score_future_cpp_fabs(-nan)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fdim)
{
    EXPECT_FLOAT_EQ(std::fdim(0.0F, 0.0F), score::cpp::fdim(0.0F, 0.0F));
    EXPECT_DOUBLE_EQ(std::fdim(0.0, 0.0), score::cpp::fdim(0.0, 0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fma)
{
    EXPECT_FLOAT_EQ(std::fma(0.0F, 0.0F, 0.0F), score::cpp::fma(0.0F, 0.0F, 0.0F));
    EXPECT_DOUBLE_EQ(std::fma(0.0, 0.0, 0.0), score::cpp::fma(0.0, 0.0, 0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, frexp)
{
    {
        int std_result{};
        int score_future_cpp_result{};
        EXPECT_FLOAT_EQ(std::frexp(1.1F, &std_result), score::cpp::frexp(1.1F, &score_future_cpp_result));
        EXPECT_EQ(std_result, score_future_cpp_result);
    }
    {
        int std_result{};
        int score_future_cpp_result{};
        EXPECT_DOUBLE_EQ(std::frexp(1.1, &std_result), score::cpp::frexp(1.1, &score_future_cpp_result));
        EXPECT_EQ(std_result, score_future_cpp_result);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, hypot)
{
    EXPECT_FLOAT_EQ(std::hypot(0.4F, 0.5F), score::cpp::hypot(0.4F, 0.5F));
    EXPECT_DOUBLE_EQ(std::hypot(0.4, 0.5), score::cpp::hypot(0.4, 0.5));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, ldexp)
{
    EXPECT_FLOAT_EQ(std::ldexp(-0.0F, 10), score::cpp::ldexp(-0.0F, 10));
    EXPECT_DOUBLE_EQ(std::ldexp(-0.0, 10), score::cpp::ldexp(-0.0, 10));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, nan)
{
    EXPECT_EQ(std::isnan(std::nanf("1")), std::isnan(score::cpp::nanf("1")));
    EXPECT_EQ(std::isnan(std::nan("1")), std::isnan(score::cpp::nan("1")));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, scalbln)
{
    EXPECT_FLOAT_EQ(std::scalbln(-0.0F, 10), score::cpp::scalbln(-0.0F, 10));
    EXPECT_DOUBLE_EQ(std::scalbln(-0.0, 10), score::cpp::scalbln(-0.0, 10));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, scalbn)
{
    EXPECT_FLOAT_EQ(std::scalbn(-0.0F, 10), score::cpp::scalbn(-0.0F, 10));
    EXPECT_DOUBLE_EQ(std::scalbn(-0.0, 10), score::cpp::scalbn(-0.0, 10));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, fpclassify)
{
    EXPECT_EQ(std::fpclassify(0.0F), score::cpp::fpclassify(0.0F));
    EXPECT_EQ(std::fpclassify(0.0), score::cpp::fpclassify(0.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite)
{
    EXPECT_EQ(std::isfinite(0.0F), score::cpp::isfinite(0.0F));
    EXPECT_EQ(std::isfinite(-0.0F), score::cpp::isfinite(-0.0F));
    EXPECT_EQ(std::isfinite(0.0F), ::score_future_cpp_isfinitef(0.0F) != 0);
    EXPECT_EQ(std::isfinite(-0.0F), ::score_future_cpp_isfinitef(-0.0F) != 0);

    EXPECT_EQ(std::isfinite(0.0), score::cpp::isfinite(0.0));
    EXPECT_EQ(std::isfinite(-0.0), score::cpp::isfinite(-0.0));
    EXPECT_EQ(std::isfinite(0.0), ::score_future_cpp_isfinite(0.0) != 0);
    EXPECT_EQ(std::isfinite(-0.0), ::score_future_cpp_isfinite(-0.0) != 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite_integral)
{
    EXPECT_EQ(score::cpp::isfinite(static_cast<double>(0)), score::cpp::isfinite(0));
    EXPECT_EQ(score::cpp::isfinite(static_cast<double>(0L)), score::cpp::isfinite(0L));
    EXPECT_EQ(score::cpp::isfinite(static_cast<double>(0LL)), score::cpp::isfinite(0LL));
    EXPECT_EQ(score::cpp::isfinite(static_cast<double>(0U)), score::cpp::isfinite(0U));
    EXPECT_EQ(score::cpp::isfinite(static_cast<double>(0UL)), score::cpp::isfinite(0UL));
    EXPECT_EQ(score::cpp::isfinite(static_cast<double>(0ULL)), score::cpp::isfinite(0ULL));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::isfinite(nan));
        EXPECT_FALSE(std::isfinite(nan));
        EXPECT_EQ(::score_future_cpp_isfinitef(nan), 0);
        EXPECT_FALSE(score::cpp::isfinite(-nan));
        EXPECT_FALSE(std::isfinite(-nan));
        EXPECT_EQ(::score_future_cpp_isfinitef(-nan), 0);
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::isfinite(nan));
        EXPECT_FALSE(std::isfinite(nan));
        EXPECT_EQ(::score_future_cpp_isfinite(nan), 0);
        EXPECT_FALSE(score::cpp::isfinite(-nan));
        EXPECT_FALSE(std::isfinite(-nan));
        EXPECT_EQ(::score_future_cpp_isfinite(-nan), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite_signaling_nan)
{
#if !defined(__SSE4_2__) && !defined(__ARM_NEON)
    GTEST_SKIP() << "not IEE754";
#endif

    EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

    {
        const float nan{std::numeric_limits<float>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::isfinite(nan));
        EXPECT_FALSE(score::cpp::isfinite(-nan));
    }
    {
        const double nan{std::numeric_limits<double>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::isfinite(nan));
        EXPECT_FALSE(score::cpp::isfinite(-nan));
    }

    EXPECT_EQ(std::fetestexcept(FE_ALL_EXCEPT), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite_infinity)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FALSE(score::cpp::isfinite(inf));
        EXPECT_FALSE(std::isfinite(inf));
        EXPECT_EQ(::score_future_cpp_isfinitef(inf), 0);
        EXPECT_FALSE(score::cpp::isfinite(-inf));
        EXPECT_FALSE(std::isfinite(-inf));
        EXPECT_EQ(::score_future_cpp_isfinitef(-inf), 0);
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_FALSE(score::cpp::isfinite(inf));
        EXPECT_FALSE(std::isfinite(inf));
        EXPECT_EQ(::score_future_cpp_isfinite(inf), 0);
        EXPECT_FALSE(score::cpp::isfinite(-inf));
        EXPECT_FALSE(std::isfinite(-inf));
        EXPECT_EQ(::score_future_cpp_isfinite(-inf), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_TRUE(score::cpp::isfinite(denorm));
        EXPECT_TRUE(std::isfinite(denorm));
        EXPECT_NE(::score_future_cpp_isfinitef(denorm), 0);
        EXPECT_TRUE(score::cpp::isfinite(-denorm));
        EXPECT_TRUE(std::isfinite(-denorm));
        EXPECT_NE(::score_future_cpp_isfinitef(-denorm), 0);
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_TRUE(score::cpp::isfinite(denorm));
        EXPECT_TRUE(std::isfinite(denorm));
        EXPECT_NE(::score_future_cpp_isfinite(denorm), 0);
        EXPECT_TRUE(score::cpp::isfinite(-denorm));
        EXPECT_TRUE(std::isfinite(-denorm));
        EXPECT_NE(::score_future_cpp_isfinite(-denorm), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite_min)
{
    {
        const float min{std::numeric_limits<float>::denorm_min()};
        EXPECT_TRUE(score::cpp::isfinite(min));
        EXPECT_TRUE(std::isfinite(min));
        EXPECT_NE(::score_future_cpp_isfinitef(min), 0);
        EXPECT_TRUE(score::cpp::isfinite(-min));
        EXPECT_TRUE(std::isfinite(-min));
        EXPECT_NE(::score_future_cpp_isfinitef(-min), 0);
    }
    {
        const double min{std::numeric_limits<double>::denorm_min()};
        EXPECT_TRUE(score::cpp::isfinite(min));
        EXPECT_TRUE(std::isfinite(min));
        EXPECT_NE(::score_future_cpp_isfinite(min), 0);
        EXPECT_TRUE(score::cpp::isfinite(-min));
        EXPECT_TRUE(std::isfinite(-min));
        EXPECT_NE(::score_future_cpp_isfinite(-min), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isfinite_max)
{
    {
        const float max{std::numeric_limits<float>::max()};
        const float lowest{std::numeric_limits<float>::lowest()};
        EXPECT_TRUE(score::cpp::isfinite(max));
        EXPECT_TRUE(std::isfinite(max));
        EXPECT_NE(::score_future_cpp_isfinitef(max), 0);
        EXPECT_TRUE(score::cpp::isfinite(lowest));
        EXPECT_TRUE(std::isfinite(lowest));
        EXPECT_NE(::score_future_cpp_isfinite(lowest), 0);
    }
    {
        const double max{std::numeric_limits<double>::max()};
        const double lowest{std::numeric_limits<double>::lowest()};
        EXPECT_TRUE(score::cpp::isfinite(max));
        EXPECT_TRUE(std::isfinite(max));
        EXPECT_NE(::score_future_cpp_isfinite(max), 0);
        EXPECT_TRUE(score::cpp::isfinite(lowest));
        EXPECT_TRUE(std::isfinite(lowest));
        EXPECT_NE(::score_future_cpp_isfinite(lowest), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isinf)
{
    EXPECT_EQ(std::isinf(0.0F), score::cpp::isinf(0.0F));
    EXPECT_EQ(std::isinf(0.0F), ::score_future_cpp_isinff(0.0F) != 0);
    EXPECT_EQ(std::isinf(-0.0F), score::cpp::isinf(-0.0F));
    EXPECT_EQ(std::isinf(-0.0F), ::score_future_cpp_isinff(-0.0F) != 0);
    EXPECT_EQ(std::isinf(0.0), score::cpp::isinf(0.0));
    EXPECT_EQ(std::isinf(0.0), ::score_future_cpp_isinf(0.0) != 0);
    EXPECT_EQ(std::isinf(-0.0), score::cpp::isinf(-0.0));
    EXPECT_EQ(std::isinf(-0.0), ::score_future_cpp_isinf(-0.0) != 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isinf_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::isinf(nan));
        EXPECT_FALSE(std::isinf(nan));
        EXPECT_EQ(::score_future_cpp_isinff(nan), 0);
        EXPECT_FALSE(score::cpp::isinf(-nan));
        EXPECT_FALSE(std::isinf(-nan));
        EXPECT_EQ(::score_future_cpp_isinff(-nan), 0);
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::isinf(nan));
        EXPECT_FALSE(std::isinf(nan));
        EXPECT_EQ(::score_future_cpp_isinf(nan), 0);
        EXPECT_FALSE(score::cpp::isinf(-nan));
        EXPECT_FALSE(std::isinf(-nan));
        EXPECT_EQ(::score_future_cpp_isinf(-nan), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isinf_signaling_nan)
{
#if !defined(__SSE4_2__) && !defined(__ARM_NEON)
    GTEST_SKIP() << "not IEE754";
#endif

    EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

    {
        const float nan{std::numeric_limits<float>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::isinf(nan));
        EXPECT_FALSE(score::cpp::isinf(-nan));
    }
    {
        const double nan{std::numeric_limits<double>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::isinf(nan));
        EXPECT_FALSE(score::cpp::isinf(-nan));
    }

    EXPECT_EQ(std::fetestexcept(FE_ALL_EXCEPT), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isinf_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_TRUE(score::cpp::isinf(inf));
        EXPECT_TRUE(std::isinf(inf));
        EXPECT_NE(::score_future_cpp_isinff(inf), 0);
        EXPECT_TRUE(score::cpp::isinf(-inf));
        EXPECT_TRUE(std::isinf(-inf));
        EXPECT_NE(::score_future_cpp_isinff(-inf), 0);
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_TRUE(score::cpp::isinf(inf));
        EXPECT_TRUE(std::isinf(inf));
        EXPECT_NE(::score_future_cpp_isinf(inf), 0);
        EXPECT_TRUE(score::cpp::isinf(-inf));
        EXPECT_TRUE(std::isinf(-inf));
        EXPECT_NE(::score_future_cpp_isinf(-inf), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isinf_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FALSE(score::cpp::isinf(denorm));
        EXPECT_FALSE(std::isinf(denorm));
        EXPECT_EQ(::score_future_cpp_isinff(denorm), 0);
        EXPECT_FALSE(score::cpp::isinf(-denorm));
        EXPECT_FALSE(std::isinf(-denorm));
        EXPECT_EQ(::score_future_cpp_isinff(-denorm), 0);
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_FALSE(score::cpp::isinf(denorm));
        EXPECT_FALSE(std::isinf(denorm));
        EXPECT_EQ(::score_future_cpp_isinf(denorm), 0);
        EXPECT_FALSE(score::cpp::isinf(-denorm));
        EXPECT_FALSE(std::isinf(-denorm));
        EXPECT_EQ(::score_future_cpp_isinf(-denorm), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isinf_max)
{
    {
        const float lowest{std::numeric_limits<float>::lowest()};
        const float max{std::numeric_limits<float>::max()};
        EXPECT_FALSE(score::cpp::isinf(lowest));
        EXPECT_FALSE(std::isinf(lowest));
        EXPECT_EQ(::score_future_cpp_isinff(lowest), 0);
        EXPECT_FALSE(score::cpp::isinf(max));
        EXPECT_FALSE(std::isinf(max));
        EXPECT_EQ(::score_future_cpp_isinff(max), 0);
    }
    {
        const double lowest{std::numeric_limits<double>::lowest()};
        const double max{std::numeric_limits<double>::max()};
        EXPECT_FALSE(score::cpp::isinf(lowest));
        EXPECT_FALSE(std::isinf(lowest));
        EXPECT_EQ(::score_future_cpp_isinf(lowest), 0);
        EXPECT_FALSE(score::cpp::isinf(max));
        EXPECT_FALSE(std::isinf(max));
        EXPECT_EQ(::score_future_cpp_isinf(max), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isinf_min)
{
    {
        const float min{std::numeric_limits<float>::min()};
        EXPECT_FALSE(score::cpp::isinf(min));
        EXPECT_FALSE(std::isinf(min));
        EXPECT_EQ(::score_future_cpp_isinff(min), 0);
        EXPECT_FALSE(score::cpp::isinf(-min));
        EXPECT_FALSE(std::isinf(-min));
        EXPECT_EQ(::score_future_cpp_isinff(-min), 0);
    }
    {
        const double min{std::numeric_limits<double>::min()};
        EXPECT_FALSE(score::cpp::isinf(min));
        EXPECT_FALSE(std::isinf(min));
        EXPECT_EQ(::score_future_cpp_isinf(min), 0);
        EXPECT_FALSE(score::cpp::isinf(-min));
        EXPECT_FALSE(std::isinf(-min));
        EXPECT_EQ(::score_future_cpp_isinf(-min), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnan)
{
    EXPECT_EQ(std::isnan(0.0F), score::cpp::isnan(0.0F));
    EXPECT_EQ(std::isnan(-0.0F), score::cpp::isnan(-0.0F));
    EXPECT_NE(std::isnan(-0.0F), ::score_future_cpp_isnanf(-0.0F) == 0);
    EXPECT_NE(std::isnan(0.0F), ::score_future_cpp_isnanf(0.0F) == 0);
    EXPECT_EQ(std::isnan(0.0), score::cpp::isnan(0.0));
    EXPECT_EQ(std::isnan(-0.0), score::cpp::isnan(-0.0));
    EXPECT_NE(std::isnan(0.0), ::score_future_cpp_isnan(0.0) == 0);
    EXPECT_NE(std::isnan(-0.0), ::score_future_cpp_isnan(-0.0) == 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnan_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_TRUE(score::cpp::isnan(nan));
        EXPECT_TRUE(std::isnan(nan));
        EXPECT_NE(::score_future_cpp_isnanf(nan), 0);
        EXPECT_TRUE(score::cpp::isnan(-nan));
        EXPECT_TRUE(std::isnan(-nan));
        EXPECT_NE(::score_future_cpp_isnanf(-nan), 0);
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_TRUE(score::cpp::isnan(nan));
        EXPECT_TRUE(std::isnan(nan));
        EXPECT_NE(::score_future_cpp_isnan(nan), 0);
        EXPECT_TRUE(score::cpp::isnan(-nan));
        EXPECT_TRUE(std::isnan(-nan));
        EXPECT_NE(::score_future_cpp_isnan(-nan), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnan_signaling_nan)
{
#if !defined(__SSE4_2__) && !defined(__ARM_NEON)
    GTEST_SKIP() << "not IEE754";
#endif

    EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

    {
        const float nan{std::numeric_limits<float>::signaling_NaN()};
        EXPECT_TRUE(score::cpp::isnan(nan));
        EXPECT_TRUE(score::cpp::isnan(-nan));
    }
    {
        const double nan{std::numeric_limits<double>::signaling_NaN()};
        EXPECT_TRUE(score::cpp::isnan(nan));
        EXPECT_TRUE(score::cpp::isnan(-nan));
    }

    EXPECT_EQ(std::fetestexcept(FE_ALL_EXCEPT), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnan_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FALSE(score::cpp::isnan(inf));
        EXPECT_FALSE(std::isnan(inf));
        EXPECT_EQ(::score_future_cpp_isnanf(inf), 0);
        EXPECT_FALSE(score::cpp::isnan(-inf));
        EXPECT_FALSE(std::isnan(-inf));
        EXPECT_EQ(::score_future_cpp_isnanf(-inf), 0);
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_FALSE(score::cpp::isnan(inf));
        EXPECT_FALSE(std::isnan(inf));
        EXPECT_EQ(::score_future_cpp_isnan(inf), 0);
        EXPECT_FALSE(score::cpp::isnan(-inf));
        EXPECT_FALSE(std::isnan(-inf));
        EXPECT_EQ(::score_future_cpp_isnan(-inf), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnan_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FALSE(score::cpp::isnan(denorm));
        EXPECT_FALSE(std::isnan(denorm));
        EXPECT_EQ(::score_future_cpp_isnanf(denorm), 0);
        EXPECT_FALSE(score::cpp::isnan(-denorm));
        EXPECT_FALSE(std::isnan(-denorm));
        EXPECT_EQ(::score_future_cpp_isnanf(-denorm), 0);
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_FALSE(score::cpp::isnan(denorm));
        EXPECT_FALSE(std::isnan(denorm));
        EXPECT_EQ(::score_future_cpp_isnan(denorm), 0);
        EXPECT_FALSE(score::cpp::isnan(-denorm));
        EXPECT_FALSE(std::isnan(-denorm));
        EXPECT_EQ(::score_future_cpp_isnan(-denorm), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnan_max)
{
    {
        const float lowest{std::numeric_limits<float>::lowest()};
        const float max{std::numeric_limits<float>::max()};
        EXPECT_FALSE(score::cpp::isnan(lowest));
        EXPECT_FALSE(std::isnan(lowest));
        EXPECT_EQ(::score_future_cpp_isnanf(lowest), 0);
        EXPECT_FALSE(score::cpp::isnan(max));
        EXPECT_FALSE(std::isnan(max));
        EXPECT_EQ(::score_future_cpp_isnanf(max), 0);
    }
    {
        const double lowest{std::numeric_limits<double>::lowest()};
        const double max{std::numeric_limits<double>::max()};
        EXPECT_FALSE(score::cpp::isnan(lowest));
        EXPECT_FALSE(std::isnan(lowest));
        EXPECT_EQ(::score_future_cpp_isnan(lowest), 0);
        EXPECT_FALSE(score::cpp::isnan(max));
        EXPECT_FALSE(std::isnan(max));
        EXPECT_EQ(::score_future_cpp_isnan(max), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnan_min)
{
    {
        const float min{std::numeric_limits<float>::min()};
        EXPECT_FALSE(score::cpp::isnan(min));
        EXPECT_FALSE(std::isnan(min));
        EXPECT_EQ(::score_future_cpp_isnanf(min), 0);
        EXPECT_FALSE(score::cpp::isnan(-min));
        EXPECT_FALSE(std::isnan(-min));
        EXPECT_EQ(::score_future_cpp_isnanf(-min), 0);
    }
    {
        const double min{std::numeric_limits<double>::min()};
        EXPECT_FALSE(score::cpp::isnan(min));
        EXPECT_FALSE(std::isnan(min));
        EXPECT_EQ(::score_future_cpp_isnan(min), 0);
        EXPECT_FALSE(score::cpp::isnan(-min));
        EXPECT_FALSE(std::isnan(-min));
        EXPECT_EQ(::score_future_cpp_isnan(-min), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnormal)
{
    EXPECT_EQ(std::isnormal(0.0F), score::cpp::isnormal(0.0F));
    EXPECT_EQ(std::isnormal(-0.0F), score::cpp::isnormal(-0.0F));
    EXPECT_EQ(std::isnormal(0.0F), ::score_future_cpp_isnormalf(0.0F) != 0);
    EXPECT_EQ(std::isnormal(-0.0F), ::score_future_cpp_isnormalf(-0.0F) != 0);

    EXPECT_EQ(std::isnormal(0.0), score::cpp::isnormal(0.0));
    EXPECT_EQ(std::isnormal(-0.0), score::cpp::isnormal(-0.0));
    EXPECT_EQ(std::isnormal(0.0), ::score_future_cpp_isnormal(0.0) != 0);
    EXPECT_EQ(std::isnormal(-0.0), ::score_future_cpp_isnormal(-0.0) != 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnormal_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::isnormal(nan));
        EXPECT_FALSE(std::isnormal(nan));
        EXPECT_EQ(::score_future_cpp_isnormalf(nan), 0);
        EXPECT_FALSE(score::cpp::isnormal(-nan));
        EXPECT_EQ(::score_future_cpp_isnormalf(-nan), 0);
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::isnormal(nan));
        EXPECT_FALSE(std::isnormal(nan));
        EXPECT_EQ(::score_future_cpp_isnormal(nan), 0);
        EXPECT_FALSE(score::cpp::isnormal(-nan));
        EXPECT_FALSE(std::isnormal(-nan));
        EXPECT_EQ(::score_future_cpp_isnormal(-nan), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnormal_signaling_nan)
{
#if !defined(__SSE4_2__) && !defined(__ARM_NEON)
    GTEST_SKIP() << "not IEE754";
#endif

    EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

    {
        const float nan{std::numeric_limits<float>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::isnormal(nan));
        EXPECT_FALSE(score::cpp::isnormal(-nan));
    }
    {
        const double nan{std::numeric_limits<double>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::isnormal(nan));
        EXPECT_FALSE(score::cpp::isnormal(-nan));
    }

    EXPECT_EQ(std::fetestexcept(FE_ALL_EXCEPT), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnormal_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FALSE(score::cpp::isnormal(inf));
        EXPECT_FALSE(std::isnormal(inf));
        EXPECT_EQ(::score_future_cpp_isnormalf(inf), 0);
        EXPECT_FALSE(score::cpp::isnormal(-inf));
        EXPECT_FALSE(std::isnormal(-inf));
        EXPECT_EQ(::score_future_cpp_isnormalf(-inf), 0);
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_FALSE(score::cpp::isnormal(inf));
        EXPECT_FALSE(std::isnormal(inf));
        EXPECT_EQ(::score_future_cpp_isnormal(inf), 0);
        EXPECT_FALSE(score::cpp::isnormal(-inf));
        EXPECT_FALSE(std::isnormal(-inf));
        EXPECT_EQ(::score_future_cpp_isnormal(-inf), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnormal_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FALSE(score::cpp::isnormal(denorm));
        EXPECT_FALSE(std::isnormal(denorm));
        EXPECT_EQ(::score_future_cpp_isnormalf(denorm), 0);
        EXPECT_FALSE(score::cpp::isnormal(-denorm));
        EXPECT_FALSE(std::isnormal(-denorm));
        EXPECT_EQ(::score_future_cpp_isnormalf(-denorm), 0);
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_FALSE(score::cpp::isnormal(denorm));
        EXPECT_FALSE(std::isnormal(denorm));
        EXPECT_EQ(::score_future_cpp_isnormal(denorm), 0);
        EXPECT_FALSE(score::cpp::isnormal(-denorm));
        EXPECT_FALSE(std::isnormal(-denorm));
        EXPECT_EQ(::score_future_cpp_isnormal(-denorm), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnormal_max)
{
    {
        const float lowest{std::numeric_limits<float>::lowest()};
        const float max{std::numeric_limits<float>::max()};
        EXPECT_TRUE(score::cpp::isnormal(lowest));
        EXPECT_TRUE(std::isnormal(lowest));
        EXPECT_NE(::score_future_cpp_isnormalf(lowest), 0);
        EXPECT_TRUE(score::cpp::isnormal(max));
        EXPECT_TRUE(std::isnormal(max));
        EXPECT_NE(::score_future_cpp_isnormalf(max), 0);
    }
    {
        const double lowest{std::numeric_limits<double>::lowest()};
        const double max{std::numeric_limits<double>::max()};
        EXPECT_TRUE(score::cpp::isnormal(lowest));
        EXPECT_TRUE(std::isnormal(lowest));
        EXPECT_NE(::score_future_cpp_isnormal(lowest), 0);
        EXPECT_TRUE(score::cpp::isnormal(max));
        EXPECT_TRUE(std::isnormal(max));
        EXPECT_NE(::score_future_cpp_isnormal(max), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isnormal_min)
{
    {
        const float min{std::numeric_limits<float>::min()};
        EXPECT_TRUE(score::cpp::isnormal(-min));
        EXPECT_TRUE(std::isnormal(-min));
        EXPECT_NE(::score_future_cpp_isnormalf(-min), 0);
        EXPECT_TRUE(score::cpp::isnormal(min));
        EXPECT_TRUE(std::isnormal(min));
        EXPECT_NE(::score_future_cpp_isnormalf(min), 0);
    }
    {
        const double min{std::numeric_limits<double>::min()};
        EXPECT_TRUE(score::cpp::isnormal(-min));
        EXPECT_TRUE(std::isnormal(-min));
        EXPECT_NE(::score_future_cpp_isnormal(-min), 0);
        EXPECT_TRUE(score::cpp::isnormal(min));
        EXPECT_TRUE(std::isnormal(min));
        EXPECT_NE(::score_future_cpp_isnormal(min), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit)
{
    EXPECT_EQ(std::signbit(0.0F), score::cpp::signbit(0.0F));
    EXPECT_EQ(std::signbit(-0.0F), score::cpp::signbit(-0.0F));
    EXPECT_NE(std::signbit(0.0F), ::score_future_cpp_signbitf(0.0F) == 0);
    EXPECT_EQ(std::signbit(-0.0F), ::score_future_cpp_signbitf(-0.0F) != 0);

    EXPECT_EQ(std::signbit(0.0), score::cpp::signbit(0.0));
    EXPECT_EQ(std::signbit(-0.0), score::cpp::signbit(-0.0));
    EXPECT_NE(std::signbit(0.0), ::score_future_cpp_signbit(0.0) == 0);
    EXPECT_EQ(std::signbit(-0.0), ::score_future_cpp_signbit(-0.0) != 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit_integral)
{
    EXPECT_EQ(std::signbit(0), score::cpp::signbit(0));
    EXPECT_EQ(std::signbit(-1), score::cpp::signbit(-1));
    EXPECT_EQ(std::signbit(0L), score::cpp::signbit(0L));
    EXPECT_EQ(std::signbit(-1L), score::cpp::signbit(-1L));
    EXPECT_EQ(std::signbit(0LL), score::cpp::signbit(0LL));
    EXPECT_EQ(std::signbit(-1LL), score::cpp::signbit(-1LL));
    EXPECT_EQ(std::signbit(0U), score::cpp::signbit(0U));
    EXPECT_EQ(std::signbit(0UL), score::cpp::signbit(0UL));
    EXPECT_EQ(std::signbit(0ULL), score::cpp::signbit(0ULL));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit_nan)
{
    {
        const float nan{std::numeric_limits<float>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::signbit(nan));
        EXPECT_FALSE(std::signbit(nan));
        EXPECT_EQ(::score_future_cpp_signbitf(nan), 0);
        EXPECT_TRUE(score::cpp::signbit(-nan));
        EXPECT_TRUE(std::signbit(-nan));
        EXPECT_NE(::score_future_cpp_signbitf(-nan), 0);
    }
    {
        const double nan{std::numeric_limits<double>::quiet_NaN()};
        EXPECT_FALSE(score::cpp::signbit(nan));
        EXPECT_FALSE(std::signbit(nan));
        EXPECT_EQ(::score_future_cpp_signbit(nan), 0);
        EXPECT_TRUE(score::cpp::signbit(-nan));
        EXPECT_TRUE(std::signbit(-nan));
        EXPECT_NE(::score_future_cpp_signbit(-nan), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit_signaling_nan)
{
#if !defined(__SSE4_2__) && !defined(__ARM_NEON)
    GTEST_SKIP() << "not IEE754";
#endif

    EXPECT_EQ(std::feclearexcept(FE_ALL_EXCEPT), 0);

    {
        const float nan{std::numeric_limits<float>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::signbit(nan));
        EXPECT_TRUE(score::cpp::signbit(-nan));
    }
    {
        const double nan{std::numeric_limits<double>::signaling_NaN()};
        EXPECT_FALSE(score::cpp::signbit(nan));
        EXPECT_TRUE(score::cpp::signbit(-nan));
    }

    EXPECT_EQ(std::fetestexcept(FE_ALL_EXCEPT), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit_inf)
{
    {
        const float inf{std::numeric_limits<float>::infinity()};
        EXPECT_FALSE(score::cpp::signbit(inf));
        EXPECT_FALSE(std::signbit(inf));
        EXPECT_EQ(::score_future_cpp_signbitf(inf), 0);
        EXPECT_TRUE(score::cpp::signbit(-inf));
        EXPECT_TRUE(std::signbit(-inf));
        EXPECT_NE(::score_future_cpp_signbitf(-inf), 0);
    }
    {
        const double inf{std::numeric_limits<double>::infinity()};
        EXPECT_FALSE(score::cpp::signbit(inf));
        EXPECT_FALSE(std::signbit(inf));
        EXPECT_EQ(::score_future_cpp_signbit(inf), 0);
        EXPECT_TRUE(score::cpp::signbit(-inf));
        EXPECT_TRUE(std::signbit(-inf));
        EXPECT_NE(::score_future_cpp_signbit(-inf), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit_denorm)
{
    {
        const float denorm{std::numeric_limits<float>::denorm_min()};
        EXPECT_FALSE(score::cpp::signbit(denorm));
        EXPECT_FALSE(std::signbit(denorm));
        EXPECT_EQ(::score_future_cpp_signbitf(denorm), 0);
        EXPECT_TRUE(score::cpp::signbit(-denorm));
        EXPECT_TRUE(std::signbit(-denorm));
        EXPECT_NE(::score_future_cpp_signbitf(-denorm), 0);
    }
    {
        const double denorm{std::numeric_limits<double>::denorm_min()};
        EXPECT_FALSE(score::cpp::signbit(denorm));
        EXPECT_FALSE(std::signbit(denorm));
        EXPECT_EQ(::score_future_cpp_signbit(denorm), 0);
        EXPECT_TRUE(score::cpp::signbit(-denorm));
        EXPECT_TRUE(std::signbit(-denorm));
        EXPECT_NE(::score_future_cpp_signbit(-denorm), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit_max)
{
    {
        const float lowest{std::numeric_limits<float>::lowest()};
        const float max{std::numeric_limits<float>::max()};
        EXPECT_FALSE(score::cpp::signbit(max));
        EXPECT_FALSE(std::signbit(max));
        EXPECT_EQ(::score_future_cpp_signbitf(max), 0);
        EXPECT_TRUE(score::cpp::signbit(lowest));
        EXPECT_TRUE(std::signbit(lowest));
        EXPECT_NE(::score_future_cpp_signbitf(lowest), 0);
    }
    {
        const double lowest{std::numeric_limits<double>::lowest()};
        const double max{std::numeric_limits<double>::max()};
        EXPECT_FALSE(score::cpp::signbit(max));
        EXPECT_FALSE(std::signbit(max));
        EXPECT_EQ(::score_future_cpp_signbit(max), 0);
        EXPECT_TRUE(score::cpp::signbit(lowest));
        EXPECT_TRUE(std::signbit(lowest));
        EXPECT_NE(::score_future_cpp_signbit(lowest), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, signbit_min)
{
    {
        const float min{std::numeric_limits<float>::min()};
        EXPECT_FALSE(score::cpp::signbit(min));
        EXPECT_FALSE(std::signbit(min));
        EXPECT_EQ(::score_future_cpp_signbitf(min), 0);
        EXPECT_TRUE(score::cpp::signbit(-min));
        EXPECT_TRUE(std::signbit(-min));
        EXPECT_NE(::score_future_cpp_signbitf(-min), 0);
    }
    {
        const double min{std::numeric_limits<double>::min()};
        EXPECT_FALSE(score::cpp::signbit(min));
        EXPECT_FALSE(std::signbit(min));
        EXPECT_EQ(::score_future_cpp_signbit(min), 0);
        EXPECT_TRUE(score::cpp::signbit(-min));
        EXPECT_TRUE(std::signbit(-min));
        EXPECT_NE(::score_future_cpp_signbit(-min), 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isgreater)
{
    EXPECT_EQ(std::isgreater(0.0F, 0), score::cpp::isgreater(0.0F, 0));
    EXPECT_EQ(std::isgreater(0.0, 0), score::cpp::isgreater(0.0, 0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isgreaterequal)
{
    EXPECT_EQ(std::isgreaterequal(0.0F, 0), score::cpp::isgreaterequal(0.0F, 0));
    EXPECT_EQ(std::isgreaterequal(0.0, 0), score::cpp::isgreaterequal(0.0, 0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isless)
{
    EXPECT_EQ(std::isless(0.0F, 0), score::cpp::isless(0.0F, 0));
    EXPECT_EQ(std::isless(0.0, 0), score::cpp::isless(0.0, 0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, islessequal)
{
    EXPECT_EQ(std::islessequal(0.0F, 0), score::cpp::islessequal(0.0F, 0));
    EXPECT_EQ(std::islessequal(0.0, 0), score::cpp::islessequal(0.0, 0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, islessgreater)
{
    EXPECT_EQ(std::islessgreater(0.0F, 0), score::cpp::islessgreater(0.0F, 0));
    EXPECT_EQ(std::islessgreater(0.0, 0), score::cpp::islessgreater(0.0, 0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9079509
TEST(math_test, isunordered)
{
    EXPECT_EQ(std::isunordered(0.0F, 0), score::cpp::isunordered(0.0F, 0));
    EXPECT_EQ(std::isunordered(0.0, 0), score::cpp::isunordered(0.0, 0));
}

// Helper function for three-way-comparison
template <typename T>
constexpr std::int32_t compare_three_way(const T a, const T b)
{
    if (a < b)
    {
        return -1;
    }
    if (a > b)
    {
        return 1;
    }
    return 0;
}

///
/// \test Check requirements for lerp with float values.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_float_interpolation)
{
    EXPECT_FLOAT_EQ(0.1F, score::cpp::lerp(0.0F, 1.0F, 0.1F));
    EXPECT_FLOAT_EQ(0.5F, score::cpp::lerp(0.0F, 1.0F, 0.5F));
    EXPECT_FLOAT_EQ(0.9F, score::cpp::lerp(0.0F, 1.0F, 0.9F));
    EXPECT_FLOAT_EQ(12.0F, score::cpp::lerp(10.0F, 15.0F, 0.4F));
    EXPECT_FLOAT_EQ(71.5F, score::cpp::lerp(5.0F, 100.0F, 0.7F));
    EXPECT_FLOAT_EQ(0.0F, score::cpp::lerp(-1.0F, 1.0F, 0.5F));
    EXPECT_FLOAT_EQ(-0.8F, score::cpp::lerp(-1.0F, 1.0F, 0.1F));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_float_extrapolation)
{
    EXPECT_FLOAT_EQ(2.0F, score::cpp::lerp(0.0F, 1.0F, 2.0F));
    EXPECT_FLOAT_EQ(-1.0F, score::cpp::lerp(0.0F, 1.0F, -1.0F));
    EXPECT_FLOAT_EQ(-1.0F, score::cpp::lerp(0.0F, 2.0F, -0.5F));
    EXPECT_FLOAT_EQ(0.95F, score::cpp::lerp(1.0F, 1.5F, -0.1F));
    EXPECT_FLOAT_EQ(1.6F, score::cpp::lerp(1.0F, 1.5F, 1.2F));
    EXPECT_FLOAT_EQ(2.0F, score::cpp::lerp(-1.0F, 1.0F, 1.5F));
    EXPECT_FLOAT_EQ(-3.2F, score::cpp::lerp(-1.0F, 1.0F, -1.1F));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_float_exactness)
{
    const float a{1.0F};
    const float b{2.0F};
    EXPECT_FLOAT_EQ(a, score::cpp::lerp(a, b, 0.0F));
    EXPECT_FLOAT_EQ(b, score::cpp::lerp(a, b, 1.0F));
    EXPECT_FLOAT_EQ(b, score::cpp::lerp(b, a, 0.0F));
    EXPECT_FLOAT_EQ(a, score::cpp::lerp(b, a, 1.0F));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_float_monotonicity)
{
    const float a{1.0F};
    const float b{2.0F};
    const float t1{0.1F};
    const float t2{0.2F};
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(a, b, t2), score::cpp::lerp(a, b, t1)) * compare_three_way(t2, t1) *
                 compare_three_way(b, a)) >= 0);
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(b, a, t2), score::cpp::lerp(b, a, t1)) * compare_three_way(t2, t1) *
                 compare_three_way(a, b)) >= 0);
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(a, b, t1), score::cpp::lerp(a, b, t2)) * compare_three_way(t1, t2) *
                 compare_three_way(b, a)) >= 0);
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(b, a, t1), score::cpp::lerp(b, a, t2)) * compare_three_way(t1, t2) *
                 compare_three_way(a, b)) >= 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_float_consistency)
{
    const float a{1.0F};
    const float b{2.0F};
    const float t1{0.1F};
    const float t2{0.2F};
    EXPECT_FLOAT_EQ(a, score::cpp::lerp(a, a, t1));
    EXPECT_FLOAT_EQ(b, score::cpp::lerp(b, b, t2));
}

///
/// \test Check requirements for lerp with double values.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_double_interpolation)
{
    EXPECT_DOUBLE_EQ(0.1, score::cpp::lerp(0.0, 1.0, 0.1));
    EXPECT_DOUBLE_EQ(0.5, score::cpp::lerp(0.0, 1.0, 0.5));
    EXPECT_DOUBLE_EQ(0.9, score::cpp::lerp(0.0, 1.0, 0.9));
    EXPECT_DOUBLE_EQ(12.0, score::cpp::lerp(10.0, 15.0, 0.4));
    EXPECT_DOUBLE_EQ(71.5, score::cpp::lerp(5.0, 100.0, 0.7));
    EXPECT_DOUBLE_EQ(0.0, score::cpp::lerp(-1.0, 1.0, 0.5));
    EXPECT_DOUBLE_EQ(-0.8, score::cpp::lerp(-1.0, 1.0, 0.1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_double_extrapolation)
{
    EXPECT_DOUBLE_EQ(2.0, score::cpp::lerp(0.0, 1.0, 2.0));
    EXPECT_DOUBLE_EQ(-1.0, score::cpp::lerp(0.0, 1.0, -1.0));
    EXPECT_DOUBLE_EQ(-1.0, score::cpp::lerp(0.0, 2.0, -0.5));
    EXPECT_DOUBLE_EQ(0.95, score::cpp::lerp(1.0, 1.5, -0.1));
    EXPECT_DOUBLE_EQ(1.6, score::cpp::lerp(1.0, 1.5, 1.2));
    EXPECT_DOUBLE_EQ(2.0, score::cpp::lerp(-1.0, 1.0, 1.5));
    EXPECT_DOUBLE_EQ(-3.2, score::cpp::lerp(-1.0, 1.0, -1.1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_double_exactness)
{
    const double a{1.0};
    const double b{2.0};
    EXPECT_DOUBLE_EQ(a, score::cpp::lerp(a, b, 0.0));
    EXPECT_DOUBLE_EQ(b, score::cpp::lerp(a, b, 1.0));
    EXPECT_DOUBLE_EQ(b, score::cpp::lerp(b, a, 0.0));
    EXPECT_DOUBLE_EQ(a, score::cpp::lerp(b, a, 1.0));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_double_monotonicity)
{
    const double a{1.0};
    const double b{2.0};
    const double t1{0.1};
    const double t2{0.2};
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(a, b, t2), score::cpp::lerp(a, b, t1)) * compare_three_way(t2, t1) *
                 compare_three_way(b, a)) >= 0);
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(b, a, t2), score::cpp::lerp(b, a, t1)) * compare_three_way(t2, t1) *
                 compare_three_way(a, b)) >= 0);
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(a, b, t1), score::cpp::lerp(a, b, t2)) * compare_three_way(t1, t2) *
                 compare_three_way(b, a)) >= 0);
    EXPECT_TRUE((compare_three_way(score::cpp::lerp(b, a, t1), score::cpp::lerp(b, a, t2)) * compare_three_way(t1, t2) *
                 compare_three_way(a, b)) >= 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18288545
TEST(math_test, lerp_double_consistency)
{
    const double a{1.0};
    const double b{2.0};
    const double t1{0.1};
    const double t2{0.2};
    EXPECT_DOUBLE_EQ(a, score::cpp::lerp(a, a, t1));
    EXPECT_DOUBLE_EQ(b, score::cpp::lerp(b, b, t2));
}

} // namespace
