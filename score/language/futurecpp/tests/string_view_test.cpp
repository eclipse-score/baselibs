///
/// @file
/// @copyright Copyright (C) 2020, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include <score/string_view.hpp>
#include <score/string_view.hpp> // test include guard

#include <score/assert_support.hpp>
#include <score/string.hpp>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenDefaultConstructed_ExpectEmpty)
{
    const score::cpp::string_view v{};
    EXPECT_EQ(nullptr, v.data());
    EXPECT_EQ(0, v.size());
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.begin(), v.end());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenNullptr_ExpectEmpty)
{
    const score::cpp::string_view v{nullptr};
    EXPECT_EQ(nullptr, v.data());
    EXPECT_EQ(0, v.size());
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.begin(), v.end());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenCStringWithoutLength_ExpectView)
{
    const char* s{"foo"};
    const score::cpp::string_view v{s};
    EXPECT_EQ(s, v.data());
    EXPECT_EQ(3, v.size());
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(3, std::distance(v.begin(), v.end()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenCStringWithLength_ExpectView)
{
    const char* s{"foobar"};
    const score::cpp::string_view v{s, 3};
    EXPECT_EQ(s, v.data());
    EXPECT_EQ(3, v.size());
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(3, std::distance(v.begin(), v.end()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenString_ExpectView)
{
    const std::string s{"foo"};
    const score::cpp::string_view v{s};
    EXPECT_EQ(s.data(), v.data());
    EXPECT_EQ(s.size(), v.size());
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(3, std::distance(v.begin(), v.end()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenString_ExpectAccessIndividualCharactersWithAt)
{
    const char* s{"abc"};
    const score::cpp::string_view v{s};
    EXPECT_EQ('a', score::cpp::at(v, 0));
    EXPECT_EQ('b', score::cpp::at(v, 1));
    EXPECT_EQ('c', score::cpp::at(v, 2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenString_ExpectAccessIndividualCharactersWithIterator)
{
    const char* s{"abc"};
    const score::cpp::string_view v{s};

    auto it = v.begin();
    EXPECT_EQ('a', *it++);
    EXPECT_EQ('b', *it++);
    EXPECT_EQ('c', *it++);
    EXPECT_EQ(it, v.end());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenAccessOutOfBound_ExpectContractViolation)
{
    const char* s{"abc"};
    const score::cpp::string_view v{s};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::at(v, 2));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::at(v, 3));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenString_ExpectFrontReturnsFirstChar)
{
    const char* a{"abc"};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::string_view{a}.front());
    EXPECT_EQ('a', score::cpp::string_view{a}.front());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenEmptyString_ExpectFrontPreconditionTriggers)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::string_view{""}.front());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenString_ExpectBackReturnsLastChar)
{
    const char* a{"abc"};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::string_view{a}.back());
    EXPECT_EQ('c', score::cpp::string_view{a}.back());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenEmptyString_ExpectBackPreconditionTriggers)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::string_view{""}.back());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenTwoStrings_ExpectEqual)
{
    {
        const char* a{"a"};
        const char* b{"a"};
        EXPECT_EQ(0, score::cpp::string_view{a}.compare(score::cpp::string_view{b}));
        EXPECT_TRUE(score::cpp::string_view{a} == score::cpp::string_view{b});
        EXPECT_TRUE(score::cpp::string_view{a} <= score::cpp::string_view{b});
        EXPECT_TRUE(score::cpp::string_view{a} >= score::cpp::string_view{b});
    }
    {
        const char* a{""};
        const char* b{""};
        EXPECT_EQ(0, score::cpp::string_view{a}.compare(score::cpp::string_view{b}));
        EXPECT_TRUE(score::cpp::string_view{a} == score::cpp::string_view{b});
        EXPECT_TRUE(score::cpp::string_view{a} <= score::cpp::string_view{b});
        EXPECT_TRUE(score::cpp::string_view{a} >= score::cpp::string_view{b});
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenTwoStrings_ExpectNotEqual)
{
    const char* a{"a"};
    const char* b{"b"};
    EXPECT_NE(0, score::cpp::string_view{a}.compare(score::cpp::string_view{b}));
    EXPECT_TRUE(score::cpp::string_view{a} != score::cpp::string_view{b});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenTwoStrings_ExpectLessThanBecauseOfSize)
{
    const char* a{"ab"};
    const char* b{"abc"};
    EXPECT_LE(score::cpp::string_view{a}.compare(score::cpp::string_view{b}), 0);
    EXPECT_TRUE(score::cpp::string_view{a} < score::cpp::string_view{b});
    EXPECT_TRUE(score::cpp::string_view{a} <= score::cpp::string_view{b});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenTwoStrings_ExpectLessThanBecauseOfLexicographicalComparision)
{
    const char* a{"abc"};
    const char* b{"abd"};
    EXPECT_LE(score::cpp::string_view{a}.compare(score::cpp::string_view{b}), 0);
    EXPECT_TRUE(score::cpp::string_view{a} < score::cpp::string_view{b});
    EXPECT_TRUE(score::cpp::string_view{a} <= score::cpp::string_view{b});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenTwoStrings_ExpectGreaterThanBecauseOfSize)
{
    const char* a{"abcd"};
    const char* b{"abc"};
    EXPECT_GE(score::cpp::string_view{a}.compare(score::cpp::string_view{b}), 0);
    EXPECT_TRUE(score::cpp::string_view{a} > score::cpp::string_view{b});
    EXPECT_TRUE(score::cpp::string_view{a} >= score::cpp::string_view{b});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenTwoStrings_ExpectGreaterThanBecauseOfLexicographicalComparision)
{
    const char* a{"abcf"};
    const char* b{"abcd"};
    EXPECT_GE(score::cpp::string_view{a}.compare(score::cpp::string_view{b}), 0);
    EXPECT_TRUE(score::cpp::string_view{a} > score::cpp::string_view{b});
    EXPECT_TRUE(score::cpp::string_view{a} >= score::cpp::string_view{b});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringAndChar_ExpectFindChar)
{
    const char* a{"abc"};
    EXPECT_EQ(score::cpp::string_view{a}.find('a'), 0);
    EXPECT_EQ(score::cpp::string_view{a}.find('b'), 1);
    EXPECT_EQ(score::cpp::string_view{a}.find('c'), 2);
    EXPECT_EQ(score::cpp::string_view{a}.find('d'), score::cpp::string_view::npos);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringAndString_ExpectFindString)
{
    const char* a{"abc"};
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{""}), 0);
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{"a"}), 0);
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{"ab"}), 0);
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{"abc"}), 0);
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{"bc"}), 1);
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{"c"}), 2);
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{"d"}), score::cpp::string_view::npos);
    EXPECT_EQ(score::cpp::string_view{a}.find(score::cpp::string_view{"abcde"}), score::cpp::string_view::npos);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringView_ExpectContainsStringView)
{
    const char* a{"abc"};
    EXPECT_TRUE(score::cpp::string_view{a}.contains(score::cpp::string_view{""}));
    EXPECT_TRUE(score::cpp::string_view{a}.contains(score::cpp::string_view{"a"}));
    EXPECT_TRUE(score::cpp::string_view{a}.contains(score::cpp::string_view{"ab"}));
    EXPECT_TRUE(score::cpp::string_view{a}.contains(score::cpp::string_view{"abc"}));
    EXPECT_TRUE(score::cpp::string_view{a}.contains(score::cpp::string_view{"bc"}));
    EXPECT_TRUE(score::cpp::string_view{a}.contains(score::cpp::string_view{"c"}));
    EXPECT_FALSE(score::cpp::string_view{a}.contains(score::cpp::string_view{"d"}));
    EXPECT_FALSE(score::cpp::string_view{a}.contains(score::cpp::string_view{"abcde"}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenCString_ExpectContainsCString)
{
    const char* a{"abc"};
    EXPECT_TRUE(score::cpp::string_view{a}.contains("a"));
    EXPECT_TRUE(score::cpp::string_view{a}.contains("abc"));
    EXPECT_FALSE(score::cpp::string_view{a}.contains("d"));
    EXPECT_FALSE(score::cpp::string_view{a}.contains("abcd"));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenChar_ExpectContainsChar)
{
    const char* a{"abc"};
    EXPECT_TRUE(score::cpp::string_view{a}.contains('a'));
    EXPECT_TRUE(score::cpp::string_view{a}.contains('b'));
    EXPECT_TRUE(score::cpp::string_view{a}.contains('c'));
    EXPECT_FALSE(score::cpp::string_view{a}.contains('d'));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringAndChar_ExpectStartsWithChar)
{
    const char* a{"ab"};
    EXPECT_TRUE(score::cpp::string_view{a}.starts_with('a'));
    EXPECT_FALSE(score::cpp::string_view{a}.starts_with('b'));
    EXPECT_FALSE(score::cpp::string_view{a}.starts_with('c'));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringAndString_ExpectStartsWithString)
{
    const char* a{"abc"};
    EXPECT_TRUE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{""}));
    EXPECT_TRUE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{"a"}));
    EXPECT_TRUE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{"ab"}));
    EXPECT_TRUE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{"abc"}));
    EXPECT_FALSE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{"bc"}));
    EXPECT_FALSE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{"c"}));
    EXPECT_FALSE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{"d"}));
    EXPECT_FALSE(score::cpp::string_view{a}.starts_with(score::cpp::string_view{"abcd"}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenEmptyString_ExpectStartsWithNothingFound)
{
    const char* a{""};
    EXPECT_FALSE(score::cpp::string_view{a}.starts_with(' '));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringAndChar_ExpectEndsWithChar)
{
    const char* a{"bc"};
    EXPECT_FALSE(score::cpp::string_view{a}.ends_with('a'));
    EXPECT_FALSE(score::cpp::string_view{a}.ends_with('b'));
    EXPECT_TRUE(score::cpp::string_view{a}.ends_with('c'));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringAndString_ExpectEndsWithString)
{
    const char* a{"abc"};
    EXPECT_TRUE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{""}));
    EXPECT_FALSE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{"a"}));
    EXPECT_FALSE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{"ab"}));
    EXPECT_TRUE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{"abc"}));
    EXPECT_TRUE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{"bc"}));
    EXPECT_TRUE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{"c"}));
    EXPECT_FALSE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{"d"}));
    EXPECT_FALSE(score::cpp::string_view{a}.ends_with(score::cpp::string_view{"abcd"}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenEmptyString_ExpectEndsWithNothingFound)
{
    const char* a{""};
    EXPECT_FALSE(score::cpp::string_view{a}.ends_with(' '));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenString_ExpectRemovePrefix)
{
    const char* a{"abc"};
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_prefix(0U));
        EXPECT_EQ(score::cpp::string_view{"abc"}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_prefix(1U));
        EXPECT_EQ(score::cpp::string_view{"bc"}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_prefix(2U));
        EXPECT_EQ(score::cpp::string_view{"c"}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_prefix(3U));
        EXPECT_EQ(score::cpp::string_view{""}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(b.remove_prefix(4U));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenString_RemoveSuffix)
{
    const char* a{"abc"};
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_suffix(0U));
        EXPECT_EQ(score::cpp::string_view{"abc"}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_suffix(1U));
        EXPECT_EQ(score::cpp::string_view{"ab"}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_suffix(2U));
        EXPECT_EQ(score::cpp::string_view{"a"}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(b.remove_suffix(3U));
        EXPECT_EQ(score::cpp::string_view{""}, b);
    }
    {
        score::cpp::string_view b{a};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(b.remove_suffix(4U));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringLiteral_ExpectCorrectView)
{
    using score::cpp::literals::string_view_literals::operator""_sv;

    EXPECT_EQ("abc\0\0def"_sv.size(), 8U);
    EXPECT_EQ("abc\0\0def"_sv, score::cpp::string_view("abc\0\0def", 8U));

    EXPECT_EQ(score::cpp::string_view("abc\0\0def").size(), 3U);
    EXPECT_EQ(score::cpp::string_view("abc\0\0def"), score::cpp::string_view("abc"));

    // check for evaluation in constexpr-context
    static_assert("abc\0\0def"_sv.size() == 8U, "failure");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringViewHash_ExpectEqualToStringHash)
{
    {
        score::cpp::pmr::string s{};
        EXPECT_EQ(std::hash<score::cpp::pmr::string>()(s), std::hash<score::cpp::string_view>()(score::cpp::string_view(s)));
    }
    {
        score::cpp::pmr::string s{"abcd"};
        EXPECT_EQ(std::hash<score::cpp::pmr::string>()(s), std::hash<score::cpp::string_view>()(score::cpp::string_view(s)));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, GivenStringView_ExpectConversionToString)
{
    EXPECT_EQ(score::cpp::string_view{"test"}.to_string(), score::cpp::pmr::string{"test"});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9372297
TEST(StringViewTest, NonStandard_SimplifyTransitionToStdStringView)
{
    static_assert(std::is_convertible_v<std::string_view, score::cpp::string_view>);
    static_assert(std::is_convertible_v<score::cpp::string_view, std::string_view>);
}

} // namespace
