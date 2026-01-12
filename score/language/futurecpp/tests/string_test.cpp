///
/// \file
/// \copyright Copyright (C) 2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/string.hpp>
#include <score/string.hpp> // test include guard

#include <score/string_view.hpp>
#include <score/unordered_map.hpp>

#include <cstddef>
#include <functional>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST(hash_test, use_hash_function_for_pmr_strings)
{
    const auto hash_function = [](const score::cpp::pmr::string& data) { return score::cpp::hash_bytes(data.c_str(), data.size()); };
    score::cpp::pmr::string my_key{"my_pmr_string_key", score::cpp::pmr::new_delete_resource()};
    score::cpp::pmr::unordered_map<score::cpp::pmr::string, bool, std::function<std::size_t(const score::cpp::pmr::string&)>> test_map{
        {{my_key, true}}, 1, hash_function, score::cpp::pmr::new_delete_resource()};
    EXPECT_TRUE(test_map[my_key]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST(score_future_cpp_pmr_string, use_std_hash_overload)
{
    score::cpp::pmr::string my_key{"my_pmr_string_key", score::cpp::pmr::new_delete_resource()};
    score::cpp::pmr::unordered_map<score::cpp::pmr::string, bool> test_map{{{my_key, true}}, 1, score::cpp::pmr::new_delete_resource()};
    EXPECT_TRUE(test_map[my_key]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST(score_future_cpp_pmr_u16string, use_std_hash_overload)
{
    score::cpp::pmr::u16string my_key(u"my_pmr_u16string_key", score::cpp::pmr::new_delete_resource());
    score::cpp::pmr::unordered_map<score::cpp::pmr::u16string, bool> test_map{{{my_key, true}}, 1, score::cpp::pmr::new_delete_resource()};
    EXPECT_TRUE(test_map[my_key]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST(score_future_cpp_pmr_u32string, use_std_hash_overload)
{
    score::cpp::pmr::u32string my_key(U"my_pmr_u32string_key", score::cpp::pmr::new_delete_resource());
    score::cpp::pmr::unordered_map<score::cpp::pmr::u32string, bool> test_map{{{my_key, true}}, 1, score::cpp::pmr::new_delete_resource()};
    EXPECT_TRUE(test_map[my_key]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST(score_future_cpp_pmr_wstring, use_std_hash_overload)
{
    score::cpp::pmr::wstring my_key{L"my_pmr_string_key", score::cpp::pmr::new_delete_resource()};
    score::cpp::pmr::unordered_map<score::cpp::pmr::wstring, bool> test_map{{{my_key, true}}, 1, score::cpp::pmr::new_delete_resource()};
    EXPECT_TRUE(test_map[my_key]);
}

class int_to_string_fixture : public testing::TestWithParam<std::int32_t>
{
};

INSTANTIATE_TEST_SUITE_P(IntToStringCases,
                         int_to_string_fixture,
                         ::testing::Values(0,
                                           1,
                                           2,
                                           10,
                                           100,
                                           -1,
                                           std::numeric_limits<std::int32_t>::lowest(),
                                           std::numeric_limits<std::int32_t>::max(),
                                           999'999'999));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST_P(int_to_string_fixture, CorrectConversionTest)
{
    auto result = to_string(GetParam(), score::cpp::pmr::new_delete_resource());
    std::string expected{std::to_string(GetParam())};
    EXPECT_STREQ(result.c_str(), expected.c_str());
}

class int64_to_string_fixture : public testing::TestWithParam<std::int64_t>
{
};

INSTANTIATE_TEST_SUITE_P(Int64ToStringCases,
                         int64_to_string_fixture,
                         ::testing::Values(0,
                                           1,
                                           2,
                                           10,
                                           100,
                                           -1,
                                           std::numeric_limits<std::int64_t>::lowest(),
                                           std::numeric_limits<std::int64_t>::max(),
                                           999'999'999'999'999'999));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST_P(int64_to_string_fixture, CorrectConversionTest)
{
    auto result = to_string(GetParam(), score::cpp::pmr::new_delete_resource());
    std::string expected{std::to_string(GetParam())};
    EXPECT_STREQ(result.c_str(), expected.c_str());
}

class uint64_to_string_fixture : public testing::TestWithParam<std::uint64_t>
{
};

INSTANTIATE_TEST_SUITE_P(UInt64ToStringCases,
                         uint64_to_string_fixture,
                         ::testing::Values(0U,
                                           1U,
                                           2U,
                                           10U,
                                           100U,
                                           std::numeric_limits<std::uint64_t>::lowest(),
                                           std::numeric_limits<std::uint64_t>::max(),
                                           999'999'999'999'999'999));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST_P(uint64_to_string_fixture, CorrectConversionTest)
{
    auto result = to_string(GetParam(), score::cpp::pmr::new_delete_resource());
    std::string expected{std::to_string(GetParam())};
    EXPECT_STREQ(result.c_str(), expected.c_str());
}

class uint32_to_string_fixture : public testing::TestWithParam<std::uint32_t>
{
};

INSTANTIATE_TEST_SUITE_P(UInt32ToStringCases,
                         uint32_to_string_fixture,
                         ::testing::Values(0U,
                                           1U,
                                           2U,
                                           10U,
                                           100U,
                                           std::numeric_limits<std::uint32_t>::lowest(),
                                           std::numeric_limits<std::uint32_t>::max(),
                                           999'999'999));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST_P(uint32_to_string_fixture, CorrectConversionTest)
{
    auto result = to_string(GetParam(), score::cpp::pmr::new_delete_resource());
    std::string expected{std::to_string(GetParam())};
    EXPECT_STREQ(result.c_str(), expected.c_str());
}

class double_to_string_fixture : public testing::TestWithParam<double>
{
};

INSTANTIATE_TEST_SUITE_P(doubleToStringCases,
                         double_to_string_fixture,
                         ::testing::Values(-0.0,
                                           0.0,
                                           1.2,
                                           -1000.99,
                                           std::numeric_limits<double>::min(),
                                           -std::numeric_limits<double>::min(),
                                           std::numeric_limits<double>::quiet_NaN(),
                                           -std::numeric_limits<double>::quiet_NaN(),
                                           std::numeric_limits<double>::lowest(),
                                           std::numeric_limits<double>::infinity(),
                                           -std::numeric_limits<double>::infinity(),
                                           std::numeric_limits<double>::denorm_min(),
                                           -std::numeric_limits<double>::denorm_min(),
                                           std::numeric_limits<double>::max()));

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18679980
TEST_P(double_to_string_fixture, CorrectConversionTest)
{
    auto result = score::cpp::pmr::to_string(GetParam(), score::cpp::pmr::new_delete_resource());
    std::string expected{std::to_string(GetParam())};
    EXPECT_EQ(score::cpp::string_view{result}, score::cpp::string_view{expected});
}
} // namespace
