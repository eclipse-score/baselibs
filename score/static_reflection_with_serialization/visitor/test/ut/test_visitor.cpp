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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <type_traits>

struct test_visitor_t
{
};

struct test_visitable_t
{
    bool visited = false;
    bool convertedToVisitableType = false;
    operator ::score::common::visitor::visitable_type()
    {
        convertedToVisitableType = true;
        return ::score::common::visitor::visitable_type{};
    }
};

auto visit_as(test_visitor_t, test_visitable_t& t)
{
    t.visited = true;
    return 123;
}

struct test_nonvisitable_t
{
    bool visited = false;
    bool convertedToVisitableType = false;
    operator ::score::common::visitor::visitable_type()
    {
        convertedToVisitableType = true;
        return ::score::common::visitor::visitable_type{};
    }
};

TEST(visitor, visitable_and_nonvisitable)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the visitability and the non-visitability.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    test_visitable_t v1;
    test_nonvisitable_t nv1;
    EXPECT_EQ(::score::common::visitor::visit(test_visitor_t{}, v1), 123);
    ::score::common::visitor::visit(test_visitor_t{}, nv1);
    EXPECT_EQ(v1.visited, true);
    EXPECT_EQ(v1.convertedToVisitableType, false);
    EXPECT_EQ(nv1.visited, false);
    EXPECT_EQ(nv1.convertedToVisitableType, true);

    using namespace ::score::common::visitor;
    test_visitable_t v2;
    test_nonvisitable_t nv2;
    EXPECT_EQ(visit(test_visitor_t{}, v2), 123);
    visit(test_visitor_t{}, nv2);
    EXPECT_EQ(v2.visited, true);
    EXPECT_EQ(v2.convertedToVisitableType, false);
    EXPECT_EQ(nv2.visited, false);
    EXPECT_EQ(nv2.convertedToVisitableType, true);
}

template <typename T>
struct test_wrapper_t
{
};

namespace ns1
{

struct test_visitor_t
{
};
struct test_visitable_t
{
};

int visit_as(test_visitor_t, test_visitable_t)
{
    return 11;
}

}  // namespace ns1

namespace ns2
{

struct test_visitor_t
{
};
struct test_visitable_t
{
};

int visit_as(ns1::test_visitor_t, test_visitable_t)
{
    return 12;
}

int visit_as(test_visitor_t, ns1::test_visitable_t)
{
    return 21;
}

int visit_as(ns2::test_visitor_t, test_wrapper_t<ns2::test_visitable_t>)
{
    return 22;
}

}  // namespace ns2

int visit_as(ns2::test_visitor_t, test_visitable_t)
{
    return 20;
}

TEST(visitor, namespaces)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check visitability equality from different namespaces.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using namespace ::score::common::visitor;
    EXPECT_EQ(visit(ns1::test_visitor_t{}, ns1::test_visitable_t{}), 11);
    EXPECT_EQ(visit(ns1::test_visitor_t{}, ns2::test_visitable_t{}), 12);
    EXPECT_EQ(visit(ns2::test_visitor_t{}, ns1::test_visitable_t{}), 21);
    EXPECT_EQ(visit(ns2::test_visitor_t{}, test_wrapper_t<ns2::test_visitable_t>{}), 22);
    EXPECT_EQ(visit(ns2::test_visitor_t{}, test_visitable_t{}), 20);
}

template <typename T, typename Enable = void>
struct is_scalar : std::false_type
{
};

template <typename T>
struct is_scalar<T, std::enable_if_t<std::is_integral<T>::value>> : std::true_type
{
};

template <typename T>
struct is_scalar<T, std::enable_if_t<std::is_floating_point<T>::value>> : std::true_type
{
};

template <typename T, std::enable_if_t<is_scalar<T>::value, int> = 0>
auto visit_as(const test_visitor_t&, const T& t)
{
    return t;
}

auto visit_as(const test_visitor_t&, int t)
{
    return t / 10 * 13 + t % 10;
}

TEST(visitor, overloads)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the int and float overloads for 'visit' API.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using namespace ::score::common::visitor;
    EXPECT_EQ(visit(test_visitor_t{}, 42.0), 42.0);
    EXPECT_EQ(visit(test_visitor_t{}, 42), 6 * 9);
}

struct test_visitor_derived_t : test_visitor_t
{
};

struct test_int_convertible_t
{
    int value;
    test_int_convertible_t(int v) : value(v) {}
    operator int()
    {
        return value;
    }
};

TEST(visitor, conversions)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check the convertible argument passing to 'visit' API.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    using namespace ::score::common::visitor;
    EXPECT_EQ(visit(test_visitor_derived_t{}, test_int_convertible_t{42}), 6 * 9);
}
