/********************************************************************************
 * Copyright (c) 2023 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2023 Contributors to the Eclipse Foundation
///

#include <score/move_only_function.hpp>
#include <score/move_only_function.hpp> // test include guard

#include <functional>

#include <gtest/gtest.h>

namespace
{
int free_function() { return 42; }

struct move_only
{
    move_only() {}

    move_only(move_only const&) = delete;
    move_only& operator=(move_only const&) = delete;

    move_only(move_only&&) = default;
    move_only& operator=(move_only&&) = default;

    int operator()() { return 42; }
};

struct count_copies
{
    mutable int count;
    count_copies() : count(0) {}

    count_copies(count_copies const& rhs)
    {
        ++rhs.count;
        count = rhs.count;
    }

    count_copies& operator=(count_copies const& rhs)
    {
        ++rhs.count;
        count = rhs.count;
        return *this;
    }

    count_copies(count_copies&&) = default;
    count_copies& operator=(count_copies&&) = default;
};

auto return_0 = []() { return 0; };
auto return_42 = []() { return 42; };

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, given_default_constructed_expect_available_default_size_match)
{
    EXPECT_EQ(score::cpp::move_only_function<int()>::capacity_t::value, score::cpp::detail::default_capacity());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, default_construction)
{
    score::cpp::move_only_function<int()> f;
    EXPECT_TRUE(f.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_nullptr)
{
    score::cpp::move_only_function<int()> f{nullptr};
    EXPECT_TRUE(f.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_null_function_pointer)
{
    int (*p)() = nullptr;
    score::cpp::move_only_function<int()> f{p};
    EXPECT_TRUE(f.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_null_member_pointer)
{
    int (move_only::*p)() = nullptr;
    score::cpp::move_only_function<int(move_only*)> f{p};
    EXPECT_TRUE(f.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_empty_move_only_function_of_different_type)
{
    score::cpp::move_only_function<int()> f{return_42};
    score::cpp::move_only_function<int(), 64U> g{std::move(f)};
    EXPECT_EQ(g(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_move_only_function_of_different_type)
{
    score::cpp::move_only_function<int()> f{};
    score::cpp::move_only_function<int(), 64U> g{std::move(f)};
    EXPECT_TRUE(g.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_free_function)
{
    score::cpp::move_only_function<int()> f(free_function);
    EXPECT_EQ(f(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_move_only_functor)
{
    move_only mo;
    score::cpp::move_only_function<int()> f(std::move(mo));
    EXPECT_EQ(f(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_lambda)
{
    score::cpp::move_only_function<int()> f(return_42);
    EXPECT_EQ(f(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, construction_from_pointer_to_member_function)
{
    int (move_only::*p)() = &move_only::operator();
    move_only mo;
    score::cpp::move_only_function<int(move_only*)> f(p);
    EXPECT_EQ(f(&mo), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, movable_arguments)
{
    int expected = 0;
    auto func = [&expected](count_copies c) { ASSERT_EQ(c.count, expected); };
    func(count_copies());

    expected = 1;
    {
        count_copies c;
        func(c);
    }

    score::cpp::move_only_function<void(count_copies)> stdfunc(func);
    expected = 0;
    {
        count_copies c;
        stdfunc(std::move(c));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_construction)
{
    score::cpp::move_only_function<int()> func1(return_42);
    score::cpp::move_only_function<int()> func2(std::move(func1));
    EXPECT_EQ(func2(), 42);
    EXPECT_NO_THROW(score::cpp::move_only_function<int()> func3(score::cpp::move_only_function<int()>{}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_construction_from_empty_move_only_function)
{
    score::cpp::move_only_function<int()> func1{};
    ASSERT_TRUE(func1.empty());
    score::cpp::move_only_function<int()> func2{std::move(func1)};
    EXPECT_TRUE(func2.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_assignment)
{
    score::cpp::move_only_function<int()> func1(return_42);
    score::cpp::move_only_function<int()> func2(return_0);
    ASSERT_EQ(func2(), 0);
    func2 = std::move(func1);
    EXPECT_EQ(func2(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_assignment_from_empty_move_only_function)
{
    score::cpp::move_only_function<int()> func(return_42);
    ASSERT_FALSE(func.empty());
    func = score::cpp::move_only_function<int()>{};
    EXPECT_TRUE(func.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_assignment_from_empty_move_only_function_of_different_type)
{
    score::cpp::move_only_function<int(), 64U> func(static_cast<int (*)()>(return_42));
    ASSERT_FALSE(func.empty());
    func = score::cpp::move_only_function<int()>{};
    EXPECT_TRUE(func.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_assignment_from_move_only_function_of_different_type)
{
    score::cpp::move_only_function<int(), 64U> func(static_cast<int (*)()>(return_0));
    ASSERT_EQ(func(), 0);
    func = score::cpp::move_only_function<int()>{static_cast<int (*)()>(return_42)};
    ASSERT_EQ(func(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_assignment_from_null_function_pointer)
{
    score::cpp::move_only_function<int()> func(static_cast<int (*)()>(return_42));
    ASSERT_FALSE(func.empty());
    func = nullptr;
    EXPECT_TRUE(func.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_assignment_from_nullptr)
{
    score::cpp::move_only_function<int()> func(static_cast<int (*)()>(return_42));
    ASSERT_FALSE(func.empty());
    func = static_cast<int (*)()>(nullptr);
    EXPECT_TRUE(func.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, empty)
{
    score::cpp::move_only_function<int()> func0;

    score::cpp::move_only_function<int()> func1(return_42);
    score::cpp::move_only_function<int()> func2(return_0);
    ASSERT_FALSE(func1.empty());
    func2 = std::move(func1);
    EXPECT_TRUE(func1.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, noexcept_test)
{
    score::cpp::move_only_function<int()> func(return_42);
    static_assert(std::is_nothrow_move_constructible<decltype(func)>::value,
                  "Noexcept on move constructor is missing.");
}

struct non_trivial_destructor
{
    explicit non_trivial_destructor(std::size_t* b) : destruction_counter{b} {}
    ~non_trivial_destructor() { (*destruction_counter)++; }

    non_trivial_destructor(move_only const&) = delete;
    non_trivial_destructor& operator=(non_trivial_destructor const&) = delete;

    non_trivial_destructor(non_trivial_destructor&&) = default;
    non_trivial_destructor& operator=(non_trivial_destructor&&) = default;

    int operator()() const { return 42; }
    std::size_t* destruction_counter;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_assign_calls_destructor)
{
    std::size_t destruction_counter{0};
    non_trivial_destructor bar{&destruction_counter};
    score::cpp::move_only_function<int()> func(std::move(bar));

    func = score::cpp::move_only_function<int()>{return_42};

    EXPECT_EQ(destruction_counter, 1);
    EXPECT_EQ(func(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, move_to_non_empty_object_destructs_previous_object)
{
    std::size_t destruction_counter{0};
    non_trivial_destructor bar{&destruction_counter};

    score::cpp::move_only_function<int()> func(std::move(bar));
    score::cpp::move_only_function<int()> func1{std::move(func)};
    func1 = score::cpp::move_only_function<int()>{return_42};

    EXPECT_EQ(destruction_counter, 2);
    EXPECT_EQ(func1(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, destruction_of_move_only_function_destructs_callable)
{
    std::size_t destruction_counter{0};
    non_trivial_destructor bar{&destruction_counter};

    score::cpp::move_only_function<int()>(std::move(bar));

    EXPECT_EQ(destruction_counter, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, call_and_return_ref)
{
    const int answer{42};
    score::cpp::move_only_function<const int&(const int&)> f([](const int& a) -> const int& { return a; });
    EXPECT_EQ(f(answer), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, capture_and_return_ref)
{
    const int answer{42};
    score::cpp::move_only_function<const int&()> f([&answer]() -> const int& { return answer; });
    EXPECT_EQ(f(), 42);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, equality)
{
    score::cpp::move_only_function<int()> func{return_42};
    score::cpp::move_only_function<int()> empty;
    EXPECT_FALSE(func == nullptr);
    EXPECT_TRUE(empty == nullptr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, inequality)
{
    score::cpp::move_only_function<int()> func{return_42};
    score::cpp::move_only_function<int()> empty;
    EXPECT_TRUE(func != nullptr);
    EXPECT_FALSE(empty != nullptr);
}

int test_move_only_function_overload_set(const score::cpp::move_only_function<void(int)>) { return 0; }
int test_move_only_function_overload_set(const score::cpp::move_only_function<void(char*)>) { return 1; }

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, can_only_be_constructed_from_callables_with_compatible_type)
{
    // If the move_only_function(Callable&&) constructor weren't sufficiently constrained, then the following calls
    // would be ambiguous.
    EXPECT_EQ(test_move_only_function_overload_set([](int) {}), 0);
    EXPECT_EQ(test_move_only_function_overload_set([](char*) {}), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17505799
TEST(move_only_function, does_not_accept_ref_qualified_callables)
{
    struct foo_without_ref_qualification
    {
        void operator()();
    };
    struct foo_with_lvalue_ref_qualification
    {
        void operator()() &;
    };
    struct foo_with_rvalue_ref_qualification
    {
        void operator()() &&;
    };
    struct foo_with_lvalue_and_rvalue_ref_qualification
    {
        void operator()() &;
        void operator()() &&;
    };
    static_assert(std::is_constructible<score::cpp::move_only_function<void()>, foo_without_ref_qualification>::value, "");
    static_assert(!std::is_constructible<score::cpp::move_only_function<void()>, foo_with_lvalue_ref_qualification>::value,
                  "");
    static_assert(!std::is_constructible<score::cpp::move_only_function<void()>, foo_with_rvalue_ref_qualification>::value,
                  "");
    static_assert(
        std::is_constructible<score::cpp::move_only_function<void()>, foo_with_lvalue_and_rvalue_ref_qualification>::value,
        "");
}

} // namespace
