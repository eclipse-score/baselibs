/********************************************************************************
 * Copyright (c) 2022 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2022 Contributors to the Eclipse Foundation
///

#include <score/type_traits.hpp>
#include <score/type_traits.hpp> // test include guard

#include <gtest/gtest.h>

void return_void();
int return_int();
int with_int(int);
int& with_ref_int(int&);
int* with_ptr_int(int*);
const int& with_const_ref_int(const int&);
const int* with_const_ptr_int(const int*);

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable, lambda)
{
    {
        const auto f = []() {};
        EXPECT_TRUE(score::cpp::is_invocable<decltype(f)>::value);
    }
    {
        const auto f = [](int) { return 23; };
        EXPECT_FALSE(score::cpp::is_invocable<decltype(f)>::value);
    }
    {
        const auto f = [](int) { return 23; };
        EXPECT_TRUE((score::cpp::is_invocable<decltype(f), int>::value));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable, function_object)
{
    {
        struct foo
        {
            void operator()();
        };
        EXPECT_TRUE(score::cpp::is_invocable<foo>::value);
    }
    {
        struct foo
        {
            int operator()(int);
        };
        EXPECT_FALSE(score::cpp::is_invocable<foo>::value);
    }
    {
        struct foo
        {
            int operator()(int);
        };
        EXPECT_TRUE((score::cpp::is_invocable<foo, int>::value));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable, function)
{
    EXPECT_TRUE(score::cpp::is_invocable<decltype(return_void)>::value);
    EXPECT_FALSE(score::cpp::is_invocable<decltype(with_int)>::value);
    EXPECT_TRUE((score::cpp::is_invocable<decltype(with_int), int>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable, member_function)
{
    struct foo
    {
    };

    EXPECT_TRUE((score::cpp::is_invocable<void (foo::*)(), foo*>::value));
    EXPECT_FALSE((score::cpp::is_invocable<void (foo::*)(int), foo*>::value));
    EXPECT_TRUE((score::cpp::is_invocable<void (foo::*)(int), foo*, int>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable, not_invocable)
{
    EXPECT_FALSE(score::cpp::is_invocable<int>::value);
    struct foo
    {
    };
    EXPECT_FALSE(score::cpp::is_invocable<foo>::value);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable_r, lambda)
{
    {
        const auto f = []() {};
        EXPECT_TRUE((score::cpp::is_invocable_r<void, decltype(f)>::value));
    }
    {
        const auto f = []() -> int { return 23; };
        EXPECT_TRUE((score::cpp::is_invocable_r<int, decltype(f)>::value));
    }
    {
        const auto f = [](int) -> int { return 23; };
        EXPECT_FALSE((score::cpp::is_invocable_r<int, decltype(f)>::value));
    }
    {
        const auto f = [](int) -> int { return 23; };
        EXPECT_TRUE((score::cpp::is_invocable_r<int, decltype(f), int>::value));
    }
    {
        const auto f = []() -> int { return 23; };
        EXPECT_TRUE((score::cpp::is_invocable_r<double, decltype(f)>::value));
    }
    {
        const auto f = []() { return 23; };
        EXPECT_FALSE((score::cpp::is_invocable_r<char*, decltype(f)>::value));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable_r, function_object)
{
    {
        struct foo
        {
            void operator()();
        };
        EXPECT_TRUE((score::cpp::is_invocable_r<void, foo>::value));
    }
    {
        struct foo
        {
            int operator()();
        };
        EXPECT_TRUE((score::cpp::is_invocable_r<int, foo>::value));
    }
    {
        struct foo
        {
            int operator()(int);
        };
        EXPECT_FALSE((score::cpp::is_invocable_r<int, foo>::value));
    }
    {
        struct foo
        {
            int operator()(int);
        };
        EXPECT_TRUE((score::cpp::is_invocable_r<int, foo, int>::value));
    }
    {
        struct foo
        {
            int operator()();
        };
        EXPECT_TRUE((score::cpp::is_invocable_r<double, foo>::value));
    }
    {
        struct foo
        {
            int operator()();
        };
        EXPECT_FALSE((score::cpp::is_invocable_r<char*, foo>::value));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable_r, function)
{
    EXPECT_TRUE((score::cpp::is_invocable_r<void, decltype(return_void)>::value));
    EXPECT_TRUE((score::cpp::is_invocable_r<int, decltype(return_int)>::value));
    EXPECT_FALSE((score::cpp::is_invocable_r<int, decltype(with_int)>::value));
    EXPECT_TRUE((score::cpp::is_invocable_r<int, decltype(with_int), int>::value));
    EXPECT_TRUE((score::cpp::is_invocable_r<double, decltype(return_int)>::value));
    EXPECT_FALSE((score::cpp::is_invocable_r<char*, decltype(return_int)>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable_r, member_function)
{
    struct foo
    {
    };

    EXPECT_TRUE((score::cpp::is_invocable_r<void, void (foo::*)(), foo*>::value));
    EXPECT_TRUE((score::cpp::is_invocable_r<int, int (foo::*)(), foo*>::value));
    EXPECT_FALSE((score::cpp::is_invocable_r<int, int (foo::*)(int), foo*>::value));
    EXPECT_TRUE((score::cpp::is_invocable_r<int, int (foo::*)(int), foo*, int>::value));
    EXPECT_TRUE((score::cpp::is_invocable_r<double, int (foo::*)(), foo*>::value));
    EXPECT_FALSE((score::cpp::is_invocable_r<char*, void (foo::*)(), foo*>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(is_invocable_r, not_invocable)
{
    EXPECT_FALSE((score::cpp::is_invocable_r<void, int>::value));
    struct foo
    {
    };
    EXPECT_FALSE((score::cpp::is_invocable_r<void, foo>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(invoke_result, lambda)
{
    {
        const auto f = []() {};
        EXPECT_TRUE((std::is_same<void, score::cpp::invoke_result_t<decltype(f)>>::value));
    }
    {
        const auto f = []() { return 23; };
        EXPECT_TRUE((std::is_same<int, score::cpp::invoke_result_t<decltype(f)>>::value));
    }
    {
        const auto f = [](int v) -> int { return v; };
        EXPECT_TRUE((std::is_same<int, score::cpp::invoke_result_t<decltype(f), int>>::value));
    }
    {
        const auto f = [](int& v) -> int& { return v; };
        EXPECT_TRUE((std::is_same<int&, score::cpp::invoke_result_t<decltype(f), int&>>::value));
    }
    {
        const auto f = [](int* v) -> int* { return v; };
        EXPECT_TRUE((std::is_same<int*, score::cpp::invoke_result_t<decltype(f), int*>>::value));
    }
    {
        const auto f = [](const int& v) -> const int& { return v; };
        EXPECT_TRUE((std::is_same<const int&, score::cpp::invoke_result_t<decltype(f), int&>>::value));
    }
    {
        const auto f = [](const int* v) -> const int* { return v; };
        EXPECT_TRUE((std::is_same<const int*, score::cpp::invoke_result_t<decltype(f), int*>>::value));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(invoke_result, function_object)
{
    {
        struct foo
        {
            void operator()();
        };
        EXPECT_TRUE((std::is_same<void, score::cpp::invoke_result_t<foo>>::value));
    }
    {
        struct foo
        {
            int operator()();
        };
        EXPECT_TRUE((std::is_same<int, score::cpp::invoke_result_t<foo>>::value));
    }
    {
        struct foo
        {
            int operator()(int);
        };
        EXPECT_TRUE((std::is_same<int, score::cpp::invoke_result_t<foo, int>>::value));
    }
    {
        struct foo
        {
            int& operator()(int&);
        };
        EXPECT_TRUE((std::is_same<int&, score::cpp::invoke_result_t<foo, int&>>::value));
    }
    {
        struct foo
        {
            int* operator()(int*);
        };
        EXPECT_TRUE((std::is_same<int*, score::cpp::invoke_result_t<foo, int*>>::value));
    }
    {
        struct foo
        {
            const int& operator()(const int&);
        };
        EXPECT_TRUE((std::is_same<const int&, score::cpp::invoke_result_t<foo, int&>>::value));
    }
    {
        struct foo
        {
            const int* operator()(const int*);
        };
        EXPECT_TRUE((std::is_same<const int*, score::cpp::invoke_result_t<foo, int*>>::value));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(invoke_result, function)
{
    EXPECT_TRUE((std::is_same<void, score::cpp::invoke_result_t<decltype(return_void)>>::value));
    EXPECT_TRUE((std::is_same<int, score::cpp::invoke_result_t<decltype(return_int)>>::value));
    EXPECT_TRUE((std::is_same<int, score::cpp::invoke_result_t<decltype(with_int), int>>::value));
    EXPECT_TRUE((std::is_same<int&, score::cpp::invoke_result_t<decltype(with_ref_int), int&>>::value));
    EXPECT_TRUE((std::is_same<int*, score::cpp::invoke_result_t<decltype(with_ptr_int), int*>>::value));
    EXPECT_TRUE((std::is_same<const int&, score::cpp::invoke_result_t<decltype(with_const_ref_int), const int&>>::value));
    EXPECT_TRUE((std::is_same<const int*, score::cpp::invoke_result_t<decltype(with_const_ptr_int), const int*>>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9758888
TEST(invoke_result, member_function)
{
    struct foo
    {
    };

    EXPECT_TRUE((std::is_same<void, score::cpp::invoke_result_t<void (foo::*)(), foo*>>::value));
    EXPECT_FALSE((std::is_same<int, score::cpp::invoke_result_t<int (foo::*)(int), foo*>>::value));
    EXPECT_TRUE((std::is_same<int&, score::cpp::invoke_result_t<int& (foo::*)(int), foo*, int>>::value));
    EXPECT_TRUE((std::is_same<int*, score::cpp::invoke_result_t<int* (foo::*)(int), foo*, int>>::value));
    EXPECT_TRUE((std::is_same<const int&, score::cpp::invoke_result_t<const int& (foo::*)(int), foo*, int>>::value));
    EXPECT_TRUE((std::is_same<const int*, score::cpp::invoke_result_t<const int* (foo::*)(int), foo*, int>>::value));
}

} // namespace
