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
#include "static_reflection_with_serialization/visitor/visit_ostream.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <sstream>

template <typename T>
std::string test_to_string(T&& t)
{
    using namespace ::score::common::visitor;
    std::ostringstream out;
    ostream_visitor v{out};
    visit(v, t);
    return out.str();
}

namespace test
{

struct S2
{
    int f1, f2;
};

STRUCT_VISITABLE(S2, f1, f2)

}  // namespace test

/// @req{VISIT-OSTREAM}
TEST(ostream_visitor, basic)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test the basic types.");
    RecordProperty("TestType", "Interface test");

    EXPECT_EQ(test_to_string('A'), "A");
    EXPECT_EQ(test_to_string(5), "5");
    EXPECT_EQ(test_to_string(0.5), "0.5");
    std::string str("qwerty");
    EXPECT_EQ(test_to_string(str), "qwerty");
    std::string& strref = str;
    EXPECT_EQ(test_to_string(strref), "qwerty");
    int carr[3] = {1, 2, 3};
    EXPECT_EQ(test_to_string(carr), "[1,2,3]");
    std::array<int, 3> arr = {11, 12, 13};
    EXPECT_EQ(test_to_string(arr), "[11,12,13]");
    std::vector<int> vec = {21, 22, 23};
    EXPECT_EQ(test_to_string(vec), "[21;22;23]");
    EXPECT_EQ(test_to_string(std::make_pair(5, 0.5)), "(5,0.5)");
    EXPECT_EQ(test_to_string(std::make_tuple(2, 0.25, 'q')), "(2;0.25;q)");
    EXPECT_EQ(test_to_string(std::make_tuple(2, 0.25, 'q')), "(2;0.25;q)");
    EXPECT_EQ(test_to_string(test::S2{21, 22}), "struct test::S2{f1=21;f2=22;}");
}

struct S3
{
    int f1, f2, f3;
};

struct SS2S3
{
    test::S2 s2;
    S3 s3;
};

STRUCT_VISITABLE(S3, f1, f2, f3)
STRUCT_VISITABLE(SS2S3, s2, s3)

/// @req{VISIT-OSTREAM-COMPOUND}
TEST(ostream_visitor, compound)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test the compound types.");
    RecordProperty("TestType", "Interface test");

    using namespace std;

    int carr[2][3] = {{1, 2, 3}, {4, 5, 6}};
    EXPECT_EQ(test_to_string(carr), "[[1,2,3],[4,5,6]]");

    array<array<int, 3>, 2> arr = {array<int, 3>{11, 12, 13}, array<int, 3>{14, 15, 16}};
    EXPECT_EQ(test_to_string(arr), "[[11,12,13],[14,15,16]]");

    vector<vector<int>> vec = {{21, 22, 23}, {24, 25, 26}};
    EXPECT_EQ(test_to_string(vec), "[[21;22;23];[24;25;26]]");

    EXPECT_EQ(test_to_string(make_pair(make_pair(5, 0.5), 'A')), "((5,0.5),A)");

    EXPECT_EQ(test_to_string(make_tuple(2, make_tuple(0.25, 'q'))), "(2;(0.25;q))");

    SS2S3 s = {{21, 22}, {31, 32, 33}};
    EXPECT_EQ(test_to_string(s),
              "struct SS2S3{"
              "s2=struct test::S2{f1=21;f2=22;};"
              "s3=struct S3{f1=31;f2=32;f3=33;};"
              "}");
}
