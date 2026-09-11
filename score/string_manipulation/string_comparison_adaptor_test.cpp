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
#include "score/string_manipulation/string_comparison_adaptor.h"

#include <score/assert.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <type_traits>

namespace score::string_manipulation
{
namespace
{

using ::testing::Eq;
using ::testing::IsTrue;
using ::testing::Ne;
using ::testing::StrEq;

/// Set of specialised template functions which allows creating a std::string, std::string_view, or
/// the underlying string type in a generic way, solely based on the template type. We use specialized template
/// functions instead of overloading, since the signatures only differ in the return type.
template <typename T>
T CreateUnderlyingString(std::string_view /* string_view */)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(
        false, "There is no default implementation. Only specialized functions should be called.");
    return T{};
}

template <>
std::string CreateUnderlyingString(const std::string_view string_view)
{
    return std::string(string_view);
}

template <>
std::string_view CreateUnderlyingString(const std::string_view string_view)
{
    return string_view;
}

template <typename T>
class StringComparisonAdaptorFixture : public ::testing::Test
{
  public:
};

// Gtest will run all tests in the StringComparisonAdaptorFixture once for every type, t, in MyTypes, such that
// TypeParam == t for each run.
using MyTypes = ::testing::Types<std::string, std::string_view>;
TYPED_TEST_SUITE(StringComparisonAdaptorFixture, MyTypes, );

TEST(StringComparisonAdaptorHelpersFixture, CreateUnderlyingStringReturnCorrectValues)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that the test helper creates equivalent string and string-view content.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    EXPECT_THAT(CreateUnderlyingString<std::string>("test_string"), "test_string");
    EXPECT_THAT(CreateUnderlyingString<std::string_view>("test_string").data(), StrEq("test_string"));
}

TYPED_TEST(StringComparisonAdaptorFixture, CanBeConvertedImplicitly)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    this->RecordProperty("Description",
                         "Check that each supported string representation converts implicitly to the adaptor.");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    EXPECT_THAT((std::is_convertible<TypeParam, StringComparisonAdaptor>::value), IsTrue());
}

TYPED_TEST(StringComparisonAdaptorFixture, CanBeCopyConstructed)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    this->RecordProperty(
        "Description",
        "Check that constructing an adaptor from a supported string representation preserves its content.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    const auto underlying_string = CreateUnderlyingString<TypeParam>("b");
    StringComparisonAdaptor adaptor{underlying_string};
    EXPECT_THAT(adaptor, Eq(underlying_string));
}

TYPED_TEST(StringComparisonAdaptorFixture, CanBeCopyAssigned)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    this->RecordProperty("Description",
                         "Check that assigning a supported string representation updates the adaptor content.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor{"a"};
    const auto underlying_string = CreateUnderlyingString<TypeParam>("b");
    adaptor = underlying_string;
    EXPECT_THAT(adaptor, Eq(underlying_string));
}

TYPED_TEST(StringComparisonAdaptorFixture, GetStringViewReturnsValidStringView)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    this->RecordProperty("Description",
                         "Check that the adaptor exposes its stored content as the expected string view.");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    const auto underlying_string = CreateUnderlyingString<TypeParam>("a");
    StringComparisonAdaptor adaptor{underlying_string};

    EXPECT_THAT(adaptor.GetAsStringView(), Eq(std::string_view{"a"}));
}

TYPED_TEST(StringComparisonAdaptorFixture, ComparisonReturnsTrueForSameContent)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    this->RecordProperty("Description", "Check that an adaptor compares equal to a string with identical content.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    const auto underlying_string = CreateUnderlyingString<TypeParam>("a");
    StringComparisonAdaptor adaptor{underlying_string};
    EXPECT_THAT(adaptor, Eq("a"));
}

TYPED_TEST(StringComparisonAdaptorFixture, ComparisonReturnsFalseForDifferentContent)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    this->RecordProperty("Description", "Check that adaptors with different content compare unequal.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    const auto underlying_string_1 = CreateUnderlyingString<TypeParam>("a");
    StringComparisonAdaptor adaptor{underlying_string_1};

    const auto underlying_string_2 = CreateUnderlyingString<TypeParam>("b");
    EXPECT_THAT(adaptor, Ne(underlying_string_2));
}

TYPED_TEST(StringComparisonAdaptorFixture, HashIsSameForTwoEqualAdaptors)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    this->RecordProperty(
        "Description",
        "Check that equal adaptor content produces equal hash values for both supported representations.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    const auto underlying_string_1 = CreateUnderlyingString<TypeParam>("a");
    StringComparisonAdaptor adaptor1{underlying_string_1};

    const auto underlying_string_2 = CreateUnderlyingString<TypeParam>("a");
    StringComparisonAdaptor adaptor2{underlying_string_2};
    EXPECT_THAT(std::hash<StringComparisonAdaptor>{}(adaptor1), Eq(std::hash<StringComparisonAdaptor>{}(adaptor2)));
}

TEST(StringComparisonAdaptor, CanBeCopyConstructedWithAdaptor)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description",
                   "Check that copying an adaptor preserves its content and keeps the instances independent.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor1{"a"};
    StringComparisonAdaptor adaptor2{adaptor1};

    EXPECT_THAT(adaptor2, Eq(StringComparisonAdaptor{"a"}));

    adaptor2 = "b";

    EXPECT_THAT(adaptor1, Eq(StringComparisonAdaptor{"a"}));
    EXPECT_THAT(adaptor2, Eq(StringComparisonAdaptor{"b"}));
}

TEST(StringComparisonAdaptor, CanBeMoveConstructedWithAdaptor)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that moving an adaptor transfers its string-like content to the destination.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor1{"a"};
    StringComparisonAdaptor adaptor2{std::move(adaptor1)};

    EXPECT_THAT(adaptor2, Eq(StringComparisonAdaptor{"a"}));
}

TEST(StringComparisonAdaptorWithString, CanBeMoveConstructed)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that moving a string into the adaptor preserves the original content.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str{"b"};
    StringComparisonAdaptor adaptor{std::move(str)};
    EXPECT_THAT(adaptor, Eq(std::string{"b"}));
}

TEST(StringComparisonAdaptorWithString, CanBeMoveAssigned)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that move assignment from a string replaces the adaptor content.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor{"a"};
    std::string str{"b"};
    adaptor = std::move(str);
    EXPECT_THAT(adaptor, Eq(std::string{"b"}));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, ComparisonWorksBetweenStringAndStringView)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description",
                   "Check that equal string and string-view content compares equal through the adaptor.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str{"a"};
    StringComparisonAdaptor adaptor{str};

    std::string_view str_view{str};
    EXPECT_THAT(adaptor, Eq(str_view));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, ComparisonWorksBetweenStringAndCString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that adaptor content compares equal with an equivalent C string.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str{"a"};
    StringComparisonAdaptor adaptor{str};

    EXPECT_THAT(adaptor, Eq("a"));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, ComparisonWorksBetweenStringViewAndString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description",
                   "Check that equal string-view and string content compares equal through the adaptor.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str{"a"};
    std::string_view str_view{str};
    StringComparisonAdaptor adaptor{str_view};

    EXPECT_THAT(adaptor, Eq(str));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, ComparisonWorksBetweenStringViewAndCString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that string-view adaptor content compares equal with an equivalent C string.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str{"a"};
    std::string_view str_view{str};
    StringComparisonAdaptor adaptor{str_view};

    EXPECT_THAT(adaptor, Eq("a"));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, ComparisonWorksBetweenCStringAndString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that C-string adaptor content compares equal with an equivalent string.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor{"a"};

    std::string str{"a"};
    EXPECT_THAT(adaptor, Eq(str));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, ComparisonWorksBetweenCStringAndStringView)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that C-string adaptor content compares equal with an equivalent string view.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor{"a"};

    std::string str{"a"};
    std::string_view str_view{str};
    EXPECT_THAT(adaptor, Eq(str_view));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, HashIsSameForEqualStringAndStringView)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that equal string and string-view content produces the same adaptor hash.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str1{"a"};
    StringComparisonAdaptor adaptor1{str1};

    std::string str2{"a"};
    std::string_view str_view{str2};
    StringComparisonAdaptor adaptor2{str_view};
    EXPECT_THAT(std::hash<StringComparisonAdaptor>{}(adaptor1), Eq(std::hash<StringComparisonAdaptor>{}(adaptor2)));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, HashIsSameForEqualStringAndCString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that equal string and C-string content produces the same adaptor hash.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str1{"a"};
    StringComparisonAdaptor adaptor1{str1};

    StringComparisonAdaptor adaptor2{"a"};
    EXPECT_THAT(std::hash<StringComparisonAdaptor>{}(adaptor1), Eq(std::hash<StringComparisonAdaptor>{}(adaptor2)));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, HashIsSameForEqualStringViewAndString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that equal string-view and string content produces the same adaptor hash.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str1{"a"};
    std::string_view str_view1{str1};
    StringComparisonAdaptor adaptor1{str_view1};

    std::string str2{"a"};
    StringComparisonAdaptor adaptor2{str2};
    EXPECT_THAT(std::hash<StringComparisonAdaptor>{}(adaptor1), Eq(std::hash<StringComparisonAdaptor>{}(adaptor2)));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, HashIsSameForEqualStringViewAndCString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that equal string-view and C-string content produces the same adaptor hash.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    std::string str1{"a"};
    std::string_view str_view1{str1};
    StringComparisonAdaptor adaptor1{str_view1};

    StringComparisonAdaptor adaptor2{"a"};
    EXPECT_THAT(std::hash<StringComparisonAdaptor>{}(adaptor1), Eq(std::hash<StringComparisonAdaptor>{}(adaptor2)));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, HashIsSameForCStringViewAndString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that equal C-string and string content produces the same adaptor hash.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor1{"a"};

    std::string str{"a"};
    StringComparisonAdaptor str_adaptor{str};
    EXPECT_THAT(std::hash<StringComparisonAdaptor>{}(adaptor1), Eq(std::hash<StringComparisonAdaptor>{}(str_adaptor)));
}

TEST(StringComparisonAdaptorWithDifferentContentTypes, HashIsSameForEqualCStringAndStringView)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that equal C-string and string-view content produces the same adaptor hash.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor1{"a"};

    std::string string{"a"};
    std::string_view string_view{string};
    StringComparisonAdaptor string_view_adaptor{string_view};
    EXPECT_THAT(std::hash<StringComparisonAdaptor>{}(adaptor1),
                Eq(std::hash<StringComparisonAdaptor>{}(string_view_adaptor)));
}

TEST(StringComparisonAdaptorLessComparison, LessThan)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__string_like_comparison_hashing");
    RecordProperty("Description", "Check that the adaptor orders string content lexicographically.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringComparisonAdaptor adaptor1{"ab"};
    StringComparisonAdaptor adaptor2{"ac"};
    EXPECT_EQ(adaptor1 < adaptor2, true);
}

}  // namespace
}  // namespace score::string_manipulation
