/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

#include <score/charconv.hpp>
#include <score/charconv.hpp> // test include guard

#include <score/string_view.hpp>

#include <array>
#include <tuple>

#include <gtest/gtest.h>

namespace
{

template <typename T>
class to_chars_base16 : public testing::TestWithParam<std::tuple<T, score::cpp::string_view>>
{
protected:
    void check(const T value, score::cpp::string_view expected_str) const
    {
        std::array<char, 32> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), value, 16)};
        ASSERT_EQ(result.ec, std::errc{});
        const score::cpp::string_view result_str{str.data(),
                                          static_cast<score::cpp::string_view::size_type>(result.ptr - str.data())};

        EXPECT_EQ(result_str, expected_str);
    }
};

class to_chars_base16_int8_fixture : public to_chars_base16<std::int8_t>
{
};

INSTANTIATE_TEST_SUITE_P(Int8ToHexStringCases,
                         to_chars_base16_int8_fixture,
                         ::testing::Values(std::make_tuple(-128U, "-80"),
                                           std::make_tuple(-127, "-7f"),
                                           std::make_tuple(-15, "-f"),
                                           std::make_tuple(-1, "-1"),
                                           std::make_tuple(0, "0"),
                                           std::make_tuple(1, "1"),
                                           std::make_tuple(15, "f"),
                                           std::make_tuple(16, "10"),
                                           std::make_tuple(63, "3f"),
                                           std::make_tuple(64, "40"),
                                           std::make_tuple(65, "41"),
                                           std::make_tuple(126, "7e"),
                                           std::make_tuple(127, "7f")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_int8_fixture, CorrectConversionTest) { check(std::get<0>(GetParam()), std::get<1>(GetParam())); }

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_int8, InsufficientBufferSize)
{
    {
        std::array<char, 3> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int8_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 2> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int8_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
    {
        std::array<char, 2> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int8_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 1> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int8_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

class to_chars_base16_uint8_fixture : public to_chars_base16<std::uint8_t>
{
};

INSTANTIATE_TEST_SUITE_P(UInt8ToHexStringCases,
                         to_chars_base16_uint8_fixture,
                         ::testing::Values(std::make_tuple(0U, "0"),
                                           std::make_tuple(1U, "1"),
                                           std::make_tuple(2U, "2"),
                                           std::make_tuple(15U, "f"),
                                           std::make_tuple(16U, "10"),
                                           std::make_tuple(63U, "3f"),
                                           std::make_tuple(64U, "40"),
                                           std::make_tuple(65U, "41"),
                                           std::make_tuple(126U, "7e"),
                                           std::make_tuple(127U, "7f"),
                                           std::make_tuple(128U, "80"),
                                           std::make_tuple(255U, "ff")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_uint8_fixture, CorrectConversionTest)
{
    check(std::get<0>(GetParam()), std::get<1>(GetParam()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_uint8, InsufficientBufferSize)
{
    {
        std::array<char, 2> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint8_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 1> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint8_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

class to_chars_base16_int16_fixture : public to_chars_base16<std::int16_t>
{
};

INSTANTIATE_TEST_SUITE_P(Int16ToHexStringCases,
                         to_chars_base16_int16_fixture,
                         ::testing::Values(std::make_tuple(-32768U, "-8000"),
                                           std::make_tuple(-4095, "-fff"),
                                           std::make_tuple(-255, "-ff"),
                                           std::make_tuple(-15, "-f"),
                                           std::make_tuple(-1, "-1"),
                                           std::make_tuple(0, "0"),
                                           std::make_tuple(1, "1"),
                                           std::make_tuple(15, "f"),
                                           std::make_tuple(16, "10"),
                                           std::make_tuple(63, "3f"),
                                           std::make_tuple(64, "40"),
                                           std::make_tuple(65, "41"),
                                           std::make_tuple(126, "7e"),
                                           std::make_tuple(127, "7f"),
                                           std::make_tuple(128, "80"),
                                           std::make_tuple(255, "ff"),
                                           std::make_tuple(256, "100"),
                                           std::make_tuple(4095, "fff"),
                                           std::make_tuple(4096, "1000"),
                                           std::make_tuple(32767, "7fff")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_int16_fixture, CorrectConversionTest)
{
    check(std::get<0>(GetParam()), std::get<1>(GetParam()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_int16, InsufficientBufferSize)
{
    {
        std::array<char, 5> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int16_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 4> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int16_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
    {
        std::array<char, 4> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int16_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 3> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int16_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

class to_chars_base16_uint16_fixture : public to_chars_base16<std::uint16_t>
{
};

INSTANTIATE_TEST_SUITE_P(UInt16ToHexStringCases,
                         to_chars_base16_uint16_fixture,
                         ::testing::Values(std::make_tuple(0U, "0"),
                                           std::make_tuple(1U, "1"),
                                           std::make_tuple(15U, "f"),
                                           std::make_tuple(16U, "10"),
                                           std::make_tuple(63U, "3f"),
                                           std::make_tuple(64U, "40"),
                                           std::make_tuple(65U, "41"),
                                           std::make_tuple(126U, "7e"),
                                           std::make_tuple(127U, "7f"),
                                           std::make_tuple(128U, "80"),
                                           std::make_tuple(255U, "ff"),
                                           std::make_tuple(256U, "100"),
                                           std::make_tuple(4095U, "fff"),
                                           std::make_tuple(4096U, "1000"),
                                           std::make_tuple(65535U, "ffff")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_uint16_fixture, CorrectConversionTest)
{
    check(std::get<0>(GetParam()), std::get<1>(GetParam()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_uint16, InsufficientBufferSize)
{
    {
        std::array<char, 4> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint16_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 3> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint16_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

class to_chars_base16_int32_fixture : public to_chars_base16<std::int32_t>
{
};

INSTANTIATE_TEST_SUITE_P(Int32ToHexStringCases,
                         to_chars_base16_int32_fixture,
                         ::testing::Values(std::make_tuple(-2147483648U, "-80000000"),
                                           std::make_tuple(-251658240, "-f000000"),
                                           std::make_tuple(-16777215, "-ffffff"),
                                           std::make_tuple(-1048575, "-fffff"),
                                           std::make_tuple(-65535, "-ffff"),
                                           std::make_tuple(-4095, "-fff"),
                                           std::make_tuple(-255, "-ff"),
                                           std::make_tuple(-15, "-f"),
                                           std::make_tuple(-1, "-1"),
                                           std::make_tuple(0, "0"),
                                           std::make_tuple(1, "1"),
                                           std::make_tuple(15, "f"),
                                           std::make_tuple(16, "10"),
                                           std::make_tuple(63, "3f"),
                                           std::make_tuple(64, "40"),
                                           std::make_tuple(65, "41"),
                                           std::make_tuple(126, "7e"),
                                           std::make_tuple(127, "7f"),
                                           std::make_tuple(128, "80"),
                                           std::make_tuple(255, "ff"),
                                           std::make_tuple(256, "100"),
                                           std::make_tuple(4095, "fff"),
                                           std::make_tuple(4096, "1000"),
                                           std::make_tuple(65535, "ffff"),
                                           std::make_tuple(1048575, "fffff"),
                                           std::make_tuple(16777215, "ffffff"),
                                           std::make_tuple(268435455, "fffffff"),
                                           std::make_tuple(2147483647, "7fffffff")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_int32_fixture, CorrectConversionTest)
{
    check(std::get<0>(GetParam()), std::get<1>(GetParam()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_int32, InsufficientBufferSize)
{
    {
        std::array<char, 9> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int32_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 8> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int32_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
    {
        std::array<char, 8> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int32_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 7> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int32_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

class to_chars_base16_uint32_fixture : public to_chars_base16<std::uint32_t>
{
};

INSTANTIATE_TEST_SUITE_P(UInt32ToHexStringCases,
                         to_chars_base16_uint32_fixture,
                         ::testing::Values(std::make_tuple(0U, "0"),
                                           std::make_tuple(1U, "1"),
                                           std::make_tuple(15U, "f"),
                                           std::make_tuple(16U, "10"),
                                           std::make_tuple(63U, "3f"),
                                           std::make_tuple(64U, "40"),
                                           std::make_tuple(65U, "41"),
                                           std::make_tuple(126U, "7e"),
                                           std::make_tuple(127U, "7f"),
                                           std::make_tuple(128U, "80"),
                                           std::make_tuple(255U, "ff"),
                                           std::make_tuple(256U, "100"),
                                           std::make_tuple(4095U, "fff"),
                                           std::make_tuple(4096U, "1000"),
                                           std::make_tuple(65535U, "ffff"),
                                           std::make_tuple(1048575U, "fffff"),
                                           std::make_tuple(16777215U, "ffffff"),
                                           std::make_tuple(268435455U, "fffffff"),
                                           std::make_tuple(4294967295U, "ffffffff")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_uint32_fixture, CorrectConversionTest)
{
    check(std::get<0>(GetParam()), std::get<1>(GetParam()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_uint32, InsufficientBufferSize)
{
    {
        std::array<char, 8> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint32_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 7> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint32_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

class to_chars_base16_int64_fixture : public to_chars_base16<std::int64_t>
{
};

INSTANTIATE_TEST_SUITE_P(Int64ToHexStringCases,
                         to_chars_base16_int64_fixture,
                         ::testing::Values(std::make_tuple(-9223372036854775808U, "-8000000000000000"),
                                           std::make_tuple(-1152921504606846975, "-fffffffffffffff"),
                                           std::make_tuple(-72057594037927935, "-ffffffffffffff"),
                                           std::make_tuple(-4503599627370495, "-fffffffffffff"),
                                           std::make_tuple(-281474976710655, "-ffffffffffff"),
                                           std::make_tuple(-17592186044415, "-fffffffffff"),
                                           std::make_tuple(-1099511627775, "-ffffffffff"),
                                           std::make_tuple(-68719476735, "-fffffffff"),
                                           std::make_tuple(-4294967295, "-ffffffff"),
                                           std::make_tuple(-268435455, "-fffffff"),
                                           std::make_tuple(-16777215, "-ffffff"),
                                           std::make_tuple(-1048575, "-fffff"),
                                           std::make_tuple(-65535, "-ffff"),
                                           std::make_tuple(-4095, "-fff"),
                                           std::make_tuple(-255, "-ff"),
                                           std::make_tuple(-15, "-f"),
                                           std::make_tuple(-1, "-1"),
                                           std::make_tuple(0, "0"),
                                           std::make_tuple(1, "1"),
                                           std::make_tuple(15, "f"),
                                           std::make_tuple(16, "10"),
                                           std::make_tuple(63, "3f"),
                                           std::make_tuple(64, "40"),
                                           std::make_tuple(65, "41"),
                                           std::make_tuple(126, "7e"),
                                           std::make_tuple(127, "7f"),
                                           std::make_tuple(128, "80"),
                                           std::make_tuple(255, "ff"),
                                           std::make_tuple(256, "100"),
                                           std::make_tuple(4095, "fff"),
                                           std::make_tuple(4096, "1000"),
                                           std::make_tuple(65535, "ffff"),
                                           std::make_tuple(1048575, "fffff"),
                                           std::make_tuple(16777215, "ffffff"),
                                           std::make_tuple(268435455, "fffffff"),
                                           std::make_tuple(4294967295, "ffffffff"),
                                           std::make_tuple(68719476735, "fffffffff"),
                                           std::make_tuple(1099511627775, "ffffffffff"),
                                           std::make_tuple(17592186044415, "fffffffffff"),
                                           std::make_tuple(281474976710655, "ffffffffffff"),
                                           std::make_tuple(4503599627370495, "fffffffffffff"),
                                           std::make_tuple(72057594037927935, "ffffffffffffff"),
                                           std::make_tuple(1152921504606846975, "fffffffffffffff"),
                                           std::make_tuple(9223372036854775807LL, "7fffffffffffffff")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_int64_fixture, CorrectConversionTest)
{
    check(std::get<0>(GetParam()), std::get<1>(GetParam()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_int64, InsufficientBufferSize)
{
    {
        std::array<char, 17> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int64_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 16> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int64_t{-1}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
    {
        std::array<char, 16> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int64_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 15> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::int64_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

class to_chars_base16_uint64_fixture : public to_chars_base16<std::uint64_t>
{
};

INSTANTIATE_TEST_SUITE_P(UInt64ToHexStringCases,
                         to_chars_base16_uint64_fixture,
                         ::testing::Values(std::make_tuple(0U, "0"),
                                           std::make_tuple(1U, "1"),
                                           std::make_tuple(15U, "f"),
                                           std::make_tuple(16U, "10"),
                                           std::make_tuple(63U, "3f"),
                                           std::make_tuple(64U, "40"),
                                           std::make_tuple(65U, "41"),
                                           std::make_tuple(126U, "7e"),
                                           std::make_tuple(127U, "7f"),
                                           std::make_tuple(128U, "80"),
                                           std::make_tuple(255U, "ff"),
                                           std::make_tuple(256U, "100"),
                                           std::make_tuple(4095U, "fff"),
                                           std::make_tuple(4096U, "1000"),
                                           std::make_tuple(65535U, "ffff"),
                                           std::make_tuple(1048575U, "fffff"),
                                           std::make_tuple(16777215U, "ffffff"),
                                           std::make_tuple(268435455U, "fffffff"),
                                           std::make_tuple(4294967295U, "ffffffff"),
                                           std::make_tuple(68719476735U, "fffffffff"),
                                           std::make_tuple(1099511627775U, "ffffffffff"),
                                           std::make_tuple(17592186044415U, "fffffffffff"),
                                           std::make_tuple(281474976710655U, "ffffffffffff"),
                                           std::make_tuple(4503599627370495U, "fffffffffffff"),
                                           std::make_tuple(72057594037927935U, "ffffffffffffff"),
                                           std::make_tuple(1152921504606846975U, "fffffffffffffff"),
                                           std::make_tuple(18446744073709551615U, "ffffffffffffffff")));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST_P(to_chars_base16_uint64_fixture, CorrectConversionTest)
{
    check(std::get<0>(GetParam()), std::get<1>(GetParam()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990752
TEST(to_chars_base16_uint64, InsufficientBufferSize)
{
    {
        std::array<char, 16> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint64_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc{});
    }
    {
        std::array<char, 15> str;
        const score::cpp::to_chars_result result{score::cpp::to_chars(str.begin(), str.end(), std::uint64_t{}, 16)};
        EXPECT_EQ(result.ec, std::errc::value_too_large);
    }
}

} // namespace
