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
#include "score/string_manipulation/split_string_view.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::string_view_literals::operator""sv;

namespace score::string_manipulation
{

namespace
{

using StringSequence = std::vector<std::string_view>;

const auto kSeperator = '|';

std::string Join(const StringSequence& string_sequence)
{
    std::string result{};
    for (std::string_view substring : string_sequence)
    {
        result.append(std::string{substring.begin(), substring.size()});
        result.push_back(kSeperator);
    }

    if (!result.empty())
    {
        // Remove trailing seperator
        result.pop_back();
    }

    return result;
}

StringSequence GetSplitSequence(std::string input)
{
    StringSequence seq{};
    LazySplitStringView splitter(input, kSeperator);
    for (const auto& segment : splitter)
    {
        seq.push_back(segment);
    }
    return seq;
}

void ExpectEqualSequences(const StringSequence& lhs, const StringSequence& rhs)
{
    EXPECT_TRUE(std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

TEST(StringSplitterTests, EmptyStringShallReturnEmptyRange)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that splitting an empty string produces an empty range.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    LazySplitStringView splitter{"", kSeperator};
    EXPECT_TRUE(splitter.begin() == splitter.end());
}

TEST(StringSplitterTests, NoSeperatorShallReturnOneItem)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that a string without delimiters is returned as one substring view.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringSequence seq{"Hello World"sv};
    ExpectEqualSequences(GetSplitSequence(Join(seq)), seq);
}

TEST(StringSplitterTests, OneSeperatorShallReturnTwoItems)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that one delimiter separates the input into two substring views.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    StringSequence seq{"Hello World"sv, "Foobar"sv};

    ExpectEqualSequences(GetSplitSequence(Join(seq)), seq);
}

TEST(StringSplitterTests, SeperatorAtBeginShallReturnEmptyString)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that a leading delimiter produces an empty first substring.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    StringSequence seq{""sv, "Hello World"sv, "Foobar"sv};

    ExpectEqualSequences(GetSplitSequence(Join(seq)), seq);
}

TEST(StringSplitterTests, SeperatorAtEndShallBeDiscarded)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that a trailing delimiter does not create an additional substring.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    StringSequence seq{"Hello World"sv, "Foobar"sv};

    ExpectEqualSequences(GetSplitSequence(Join(seq) + kSeperator), seq);
}

TEST(StringSplitterTests, SeperatorOnlyStringShallReturnEmptySubstring)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that an input consisting only of the delimiter yields one empty view.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    ExpectEqualSequences(GetSplitSequence(std::string{"|"}), StringSequence{""sv});
}

TEST(StringSplitterTests, TwoSeperatorsShallReturnTwoEmptySubstring)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that two consecutive delimiters yield two empty substring views.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    ExpectEqualSequences(GetSplitSequence(std::string{"||"}), StringSequence{""sv, ""sv});
}

TEST(StringSplitterTests, MultipleSeperatorsInRowShallReturnEmptySubstring)
{
    RecordProperty("PartiallyVerifies", "comp_req__string_manipulation__lazy_split");
    RecordProperty("Description", "Check that repeated delimiters preserve the empty substring between them.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    ExpectEqualSequences(GetSplitSequence(std::string{"Foo||Bar"}), StringSequence{"Foo"sv, ""sv, "Bar"sv});
}

}  // namespace

}  // namespace score::string_manipulation
