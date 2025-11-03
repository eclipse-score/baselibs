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
#include "visitor/visit.h"
#include "visitor/visit_as_struct.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstring>
#include <string>
#include <type_traits>

namespace test
{

struct S1
{
    int f1;
};

struct S2
{
    int f1, f2;
};

struct S3
{
    int f1, f2, f3;
};

struct S4
{
    int f1, f2, f3, f4;
};

struct S5
{
    int f1, f2, f3, f4, f5;
};

struct S6
{
    int f1, f2, f3, f4, f5, f6;
};

struct S7
{
    int f1, f2, f3, f4, f5, f6, f7;
};

struct S8
{
    int f1, f2, f3, f4, f5, f6, f7, f8;
};

struct S9
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9;
};

struct S10
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10;
};

struct S11
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11;
};

struct S12
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12;
};

struct S13
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13;
};

struct S14
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14;
};

struct S15
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15;
};

struct S16
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16;
};

struct S17
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17;
};

struct S18
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18;
};

struct S19
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19;
};

struct S20
{
    int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20;
};

STRUCT_VISITABLE(S1, f1)
STRUCT_VISITABLE(S2, f1, f2)
STRUCT_VISITABLE(S3, f1, f2, f3)
STRUCT_VISITABLE(S4, f1, f2, f3, f4)
STRUCT_VISITABLE(S5, f1, f2, f3, f4, f5)
STRUCT_VISITABLE(S6, f1, f2, f3, f4, f5, f6)
STRUCT_VISITABLE(S7, f1, f2, f3, f4, f5, f6, f7)
STRUCT_VISITABLE(S8, f1, f2, f3, f4, f5, f6, f7, f8)
STRUCT_VISITABLE(S9, f1, f2, f3, f4, f5, f6, f7, f8, f9)
STRUCT_VISITABLE(S10, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10)
STRUCT_VISITABLE(S11, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11)
STRUCT_VISITABLE(S12, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12)
STRUCT_VISITABLE(S13, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13)
STRUCT_VISITABLE(S14, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14)
STRUCT_VISITABLE(S15, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15)
STRUCT_VISITABLE(S16, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16)
STRUCT_VISITABLE(S17, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17)
STRUCT_VISITABLE(S18, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18)
STRUCT_VISITABLE(S19, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19)
STRUCT_VISITABLE(S20, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20)

}  // namespace test

struct test_visitor_t
{
};

template <typename S, typename... Args>
std::size_t visit_as_struct(test_visitor_t, S&&, Args&&... args)
{
    return sizeof...(args);
}

template <typename S>
bool check_visitable(const char* name, std::size_t fields)
{
    using visitable = ::score::common::visitor::struct_visitable<S>;
    if (visitable::name() != name || visitable::fields != fields)
    {
        return false;
    }
    for (std::size_t i = 0; i != fields; ++i)
    {
        if (std::string("f") + std::to_string(i + 1) != visitable::field_name(i))
        {
            return false;
        }
    }
    return visitable::visit(test_visitor_t{}, S{}) == fields;
}

TEST(struct_visitor, struct_visitable)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check visitability of different structures.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_TRUE(check_visitable<test::S1>("test::S1", 1));
    EXPECT_TRUE(check_visitable<test::S2>("test::S2", 2));
    EXPECT_TRUE(check_visitable<test::S3>("test::S3", 3));
    EXPECT_TRUE(check_visitable<test::S4>("test::S4", 4));
    EXPECT_TRUE(check_visitable<test::S5>("test::S5", 5));
    EXPECT_TRUE(check_visitable<test::S6>("test::S6", 6));
    EXPECT_TRUE(check_visitable<test::S7>("test::S7", 7));
    EXPECT_TRUE(check_visitable<test::S8>("test::S8", 8));
    EXPECT_TRUE(check_visitable<test::S9>("test::S9", 9));
    EXPECT_TRUE(check_visitable<test::S10>("test::S10", 10));
    EXPECT_TRUE(check_visitable<test::S11>("test::S11", 11));
    EXPECT_TRUE(check_visitable<test::S12>("test::S12", 12));
    EXPECT_TRUE(check_visitable<test::S13>("test::S13", 13));
    EXPECT_TRUE(check_visitable<test::S14>("test::S14", 14));
    EXPECT_TRUE(check_visitable<test::S15>("test::S15", 15));
    EXPECT_TRUE(check_visitable<test::S16>("test::S16", 16));
    EXPECT_TRUE(check_visitable<test::S17>("test::S17", 17));
    EXPECT_TRUE(check_visitable<test::S18>("test::S18", 18));
    EXPECT_TRUE(check_visitable<test::S19>("test::S19", 19));
    EXPECT_TRUE(check_visitable<test::S20>("test::S20", 20));
}

TEST(struct_visitor, visit_as)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check visitability of different structures fields.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S1{}), 1);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S2{}), 2);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S3{}), 3);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S4{}), 4);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S5{}), 5);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S6{}), 6);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S7{}), 7);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S8{}), 8);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S9{}), 9);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S10{}), 10);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S11{}), 11);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S12{}), 12);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S13{}), 13);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S14{}), 14);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S15{}), 15);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S16{}), 16);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S17{}), 17);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S18{}), 18);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S19{}), 19);
    EXPECT_EQ(visit_as(test_visitor_t{}, test::S20{}), 20);

    using namespace ::score::common::visitor;
    EXPECT_EQ(visit(test_visitor_t{}, test::S1{}), 1);
    EXPECT_EQ(visit(test_visitor_t{}, test::S2{}), 2);
    EXPECT_EQ(visit(test_visitor_t{}, test::S3{}), 3);
    EXPECT_EQ(visit(test_visitor_t{}, test::S4{}), 4);
    EXPECT_EQ(visit(test_visitor_t{}, test::S5{}), 5);
    EXPECT_EQ(visit(test_visitor_t{}, test::S6{}), 6);
    EXPECT_EQ(visit(test_visitor_t{}, test::S7{}), 7);
    EXPECT_EQ(visit(test_visitor_t{}, test::S8{}), 8);
    EXPECT_EQ(visit(test_visitor_t{}, test::S9{}), 9);
    EXPECT_EQ(visit(test_visitor_t{}, test::S10{}), 10);
    EXPECT_EQ(visit(test_visitor_t{}, test::S11{}), 11);
    EXPECT_EQ(visit(test_visitor_t{}, test::S12{}), 12);
    EXPECT_EQ(visit(test_visitor_t{}, test::S13{}), 13);
    EXPECT_EQ(visit(test_visitor_t{}, test::S14{}), 14);
    EXPECT_EQ(visit(test_visitor_t{}, test::S15{}), 15);
    EXPECT_EQ(visit(test_visitor_t{}, test::S16{}), 16);
    EXPECT_EQ(visit(test_visitor_t{}, test::S17{}), 17);
    EXPECT_EQ(visit(test_visitor_t{}, test::S18{}), 18);
    EXPECT_EQ(visit(test_visitor_t{}, test::S19{}), 19);
    EXPECT_EQ(visit(test_visitor_t{}, test::S20{}), 20);
}

struct S1
{
    int x1;
};

STRUCT_VISITABLE(S1, x1)

struct test_visitor2_t
{
};

template <typename S, typename... Args>
auto visit_as_struct(test_visitor2_t, S&&, Args&&...)
{
    using visitable = ::score::common::visitor::struct_visitable<S>;
    return visitable::name();
}

TEST(struct_visitor, namespaces)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check visitability fields.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using namespace ::score::common::visitor;
    EXPECT_EQ(struct_visitable<S1>::field_name(0), "x1");
    EXPECT_EQ(struct_visitable<test::S1>::field_name(0), "f1");

    EXPECT_EQ(visit(test_visitor2_t{}, S1{}), "S1");
    EXPECT_EQ(visit(test_visitor2_t{}, test::S1{}), "test::S1");
}

template <std::size_t MaxSize>
struct TemplatedStruct
{
    std::array<std::int32_t, MaxSize> arr;
};

using TemplateStructDefaultSize = TemplatedStruct<297>;

STRUCT_VISITABLE(TemplateStructDefaultSize, arr)

TEST(struct_visitor, TemplatedStructShallNotContainTrailingWhitespace)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the templated structs shall not contain trailing whaitspace."
                   "serialization/deserialization.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using namespace ::score::common::visitor;

    // GCC inserts a trailing whitespace for templated structs in __PRETTY__FUNCTION__.
    // The visitor shall strip the trailing whitespace.
    EXPECT_EQ(visit(test_visitor2_t{}, TemplateStructDefaultSize{}), "TemplatedStruct<297>");
}

TEST(struct_visitor_utils, TupleToArrayTest)
{
    const std::array<std::string, 4> reference = {"1", "2", "3", "4"};
    const auto result =
        ::score::common::visitor::detail::tuple_to_array(::score::common::visitor::detail::pack_values("1", "2", "3", "4"));
    EXPECT_EQ(result.size(), reference.size());
    EXPECT_TRUE(std::equal(result.begin(), result.end(), reference.begin(), [](const char* s1, const std::string s2) {
        return std::strcmp(s1, s2.c_str()) == 0;
    }));
}
