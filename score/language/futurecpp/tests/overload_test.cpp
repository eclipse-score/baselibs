/********************************************************************************
 * Copyright (c) 2018 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2018 Contributors to the Eclipse Foundation
///

#include <score/overload.hpp>
#include <score/overload.hpp> // test include guard

#include <score/variant.hpp>

#include <tuple>

#include <gtest/gtest.h>

namespace
{

/// \test The correct overloaded function shall be called for the returned object of overload(). The side effect caused
/// by the invocation of the returned object shall be visible in the captured variable.
/// \testmethods TM_REQUIREMENT
/// \requirement CB-#18260831
TEST(overload_test, void_return_type)
{
    int i{0};
    const auto visitor =
        score::cpp::overload([&i](int) -> void { i = 23; }, [&i](double) -> void { i = 42; }, [&i](long) -> void { i = 73; });

    visitor(int{0});
    EXPECT_EQ(23, i);

    visitor(double{0});
    EXPECT_EQ(42, i);

    visitor(long{0});
    EXPECT_EQ(73, i);
}

/// \test The correct overloaded function shall be called for the returned object of overload(). The invocation of the
/// returned object shall be visible in the returned value.
/// \testmethods TM_REQUIREMENT
/// \requirement CB-#18260831
TEST(overload_test, non_void_return_type)
{
    const auto visitor =
        score::cpp::overload([](int) -> int { return 23; }, [](double) -> int { return 42; }, [](long) -> int { return 73; });

    EXPECT_EQ(23, visitor(int{0}));
    EXPECT_EQ(42, visitor(double{0}));
    EXPECT_EQ(73, visitor(long{0}));
}

/// \test The returned object of overload() shall behave as normal function calls .
/// \testmethods TM_REQUIREMENT
/// \requirement CB-#18260831
TEST(overload_test, implicit_conversion)
{
    const auto visitor = score::cpp::overload([](int) -> int { return 23; });

    EXPECT_EQ(23, visitor(short{0}));
    EXPECT_EQ(23, visitor(int{0}));
    EXPECT_EQ(23, visitor(long{0}));
}

/// \test The overloaded functions shall preserve the internal state when invoked. The internal state shall not interfer
/// with the other overloaded functions.
/// \testmethods TM_REQUIREMENT
/// \requirement CB-#18260831
TEST(overload_test, stateful)
{
    auto visitor = score::cpp::overload(
        [](int) -> int {
            static int i{0};
            return ++i;
        },
        [](double) -> int {
            static int i{0};
            return ++i;
        },
        [](long) -> int {
            static int i{0};
            return ++i;
        });

    EXPECT_EQ(1, visitor(int{0}));
    EXPECT_EQ(1, visitor(double{0}));
    EXPECT_EQ(1, visitor(long{0}));
    EXPECT_EQ(2, visitor(int{0}));
    EXPECT_EQ(3, visitor(int{0}));
    EXPECT_EQ(2, visitor(double{0}));
    EXPECT_EQ(3, visitor(double{0}));
    EXPECT_EQ(2, visitor(long{0}));
    EXPECT_EQ(3, visitor(long{0}));
}

/// \test The functor passed into overload() shall be perfectly forwarded.
/// \testmethods TM_REQUIREMENT
/// \requirement CB-#18260831
TEST(overload_test, perfect_forward)
{
    struct functor
    {
        functor() : copies{0}, moves{0} {}
        functor(const functor& other) : copies{other.copies + 1}, moves{other.moves} {}
        functor(functor&& other) : copies{other.copies}, moves{other.moves + 1} {}

        std::tuple<int, int> operator()() const { return std::make_tuple(copies, moves); }
        int copies;
        int moves;
    };

    functor f;

    EXPECT_EQ(std::make_tuple(0, 0), f());
    const auto visitor1 = score::cpp::overload(f);
    EXPECT_EQ(std::make_tuple(1, 0), visitor1());
    const auto visitor2 = score::cpp::overload(std::move(f));
    EXPECT_EQ(std::make_tuple(0, 1), visitor2());
}

/// \test The returned object of overload() shall be usuable with variant visitation. The side effect caused by invoking
/// the returned object call shall be visible in the captured variable.
/// \testmethods TM_REQUIREMENT
/// \requirement CB-#18260831
TEST(overload_test, variant_void_visitation)
{
    int i{0};
    score::cpp::variant<int, long> var;

    var = int{1};
    score::cpp::visit(score::cpp::overload([&i](long) -> void { i = 23; }), var);
    EXPECT_EQ(23, i);

    var = long{23};
    const auto visitor = score::cpp::overload([&i](int) -> void { i = 42; }, [&i](long) -> void { i = 73; });
    score::cpp::visit(visitor, var);
    EXPECT_EQ(73, i);
}

/// \test The returned object of overload() shall be usuable with variant visitation. Invoking the returned object shall
/// call the correct overloaded function and shall be visible in the returned value.
/// \testmethods TM_REQUIREMENT
/// \requirement CB-#18260831
TEST(overload_test, const_variant_non_void_visitation)
{
    const auto visitor = score::cpp::overload([](int) -> int { return 23; }, [](double) -> int { return 42; });

    const score::cpp::variant<int, double> var1{1};
    const int result1{score::cpp::visit(visitor, var1)};
    EXPECT_EQ(23, result1);

    const score::cpp::variant<int, double> var2{23.0};
    const int result2{score::cpp::visit(visitor, var2)};
    EXPECT_EQ(42, result2);
}

} // namespace
