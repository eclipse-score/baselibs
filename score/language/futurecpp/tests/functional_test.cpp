/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/functional.hpp>
#include <score/functional.hpp> // test include guard

#include <functional>
#include <type_traits>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16079319
TEST(functional_test, identity)
{
    EXPECT_EQ(1, score::cpp::identity{}(1));

    int a{23};
    static_assert(std::is_same<decltype(score::cpp::identity{}(a)), int&>::value, "Mismatch.");
    EXPECT_EQ(23, score::cpp::identity{}(a));

    const int b{42};
    static_assert(std::is_same<decltype(score::cpp::identity{}(b)), const int&>::value, "Mismatch.");
    EXPECT_EQ(42, score::cpp::identity{}(b));
}

struct may_throw
{
    may_throw() {}
    may_throw(const may_throw&) {}
    may_throw(may_throw&&) {}
    may_throw& operator=(const may_throw&) { return *this; }
    may_throw& operator=(may_throw&&) { return *this; }

    void operator()() const& {}
    void operator()() && {}
};

struct never_throws
{
    never_throws() noexcept {}
    never_throws(const never_throws&) noexcept {}
    never_throws(never_throws&&) noexcept {}
    never_throws& operator=(const never_throws&) noexcept { return *this; }
    never_throws& operator=(never_throws&&) noexcept { return *this; }

    void operator()() const& noexcept {}
    void operator()() && noexcept {}
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, call_operator_noexcept)
{
    using wrapper_never_throws = decltype(score::cpp::bind_front(std::declval<never_throws>()));
    using wrapper_may_throw = decltype(score::cpp::bind_front(std::declval<may_throw>()));

    static_assert(noexcept(std::declval<wrapper_never_throws&>()()));
    static_assert(!noexcept(std::declval<wrapper_may_throw&>()()));

    static_assert(noexcept(std::declval<const wrapper_never_throws&>()()));
    static_assert(!noexcept(std::declval<const wrapper_may_throw&>()()));

    static_assert(noexcept(std::declval<wrapper_never_throws&&>()()));
    static_assert(!noexcept(std::declval<wrapper_may_throw&&>()()));

    static_assert(noexcept(std::declval<const wrapper_never_throws&&>()()));
    static_assert(!noexcept(std::declval<const wrapper_may_throw&&>()()));
}

struct move_copy_counter
{
    static inline int move_count{0};
    static inline int copy_count{0};

    move_copy_counter() = default;
    move_copy_counter(move_copy_counter&) { ++copy_count; }
    move_copy_counter(const move_copy_counter&) { ++copy_count; }
    move_copy_counter(move_copy_counter&&) { ++move_count; }
    move_copy_counter(const move_copy_counter&&) { ++move_count; }

    void operator()() const {}

    static void reset()
    {
        move_count = 0;
        copy_count = 0;
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, constructor_function_value_categories_and_constness)
{
    {
        move_copy_counter f{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(f);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(std::move(f));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
    {
        const move_copy_counter f{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(f);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(std::move(f));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, constructor_bound_args_value_categories_and_constness)
{
    const auto fn = [](move_copy_counter) {};

    {
        move_copy_counter arg{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(fn, arg);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(fn, std::move(arg));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
    {
        const move_copy_counter arg{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(fn, arg);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_front(fn, std::move(arg));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
}

struct type_a
{
    int data;
};

struct type_b
{
    int data;
};

struct type_c
{
    int data;
};

struct type_d
{
    int data;
};

struct functor_with_overload_on_ref_qualifier
{
    type_a operator()() & { return type_a{1}; }
    type_b operator()() const& { return type_b{2}; }
    type_c operator()() && { return type_c{3}; }
    type_d operator()() const&& { return type_d{4}; }
};

struct functor_with_overload_on_signature
{
    type_a operator()(int&) const { return type_a{1}; }
    type_b operator()(const int&) const { return type_b{2}; }
    type_c operator()(int&&) const { return type_c{3}; }
    type_d operator()(const int&&) const { return type_d{4}; }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, callable_ref_qualifiers)
{
    auto f = score::cpp::bind_front(functor_with_overload_on_ref_qualifier{});
    const auto const_f = score::cpp::bind_front(functor_with_overload_on_ref_qualifier{});

    const type_a a{f()};
    const type_b b{const_f()};
    const type_c c{std::move(f)()};
    const type_d d{std::move(const_f)()};

    EXPECT_EQ(1, a.data);
    EXPECT_EQ(2, b.data);
    EXPECT_EQ(3, c.data);
    EXPECT_EQ(4, d.data);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, bound_args_value_categories_and_constness)
{
    auto f = score::cpp::bind_front(functor_with_overload_on_signature{}, 0);
    const auto const_f = score::cpp::bind_front(functor_with_overload_on_signature{}, 0);

    const type_a a{f()};
    const type_b b{const_f()};
    const type_c c{std::move(f)()};
    const type_d d{std::move(const_f)()};

    EXPECT_EQ(1, a.data);
    EXPECT_EQ(2, b.data);
    EXPECT_EQ(3, c.data);
    EXPECT_EQ(4, d.data);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, call_args_value_categories_and_constness)
{
    const auto f = score::cpp::bind_front(functor_with_overload_on_signature{});

    int x{};
    const int y{};

    const type_a a{f(x)};
    const type_b b{f(y)};
    const type_c c{f(std::move(x))};
    const type_d d{f(std::move(y))};

    EXPECT_EQ(1, a.data);
    EXPECT_EQ(2, b.data);
    EXPECT_EQ(3, c.data);
    EXPECT_EQ(4, d.data);
}

int fn(type_a a, type_b b, type_c c) { return a.data + b.data + c.data; }

struct foo
{
    int operator()(type_a a, type_b b, type_c c) const { return fn(a, b, c); }
    int bar(type_a a, type_b b, type_c c) const { return fn(a, b, c); }
    int baz{42};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, lambda)
{
    const auto f = [](type_a a, type_b b, type_c c) { return fn(a, b, c); };
    const auto g = score::cpp::bind_front(f, type_a{1}, type_b{2});
    EXPECT_EQ(6, g(type_c{3}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, function_object)
{
    const foo f{};
    const auto g = score::cpp::bind_front(f, type_a{1}, type_b{2});
    EXPECT_EQ(6, g(type_c{3}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, member_function)
{
    const foo f{};
    const auto g = score::cpp::bind_front(&foo::bar, f, type_a{1}, type_b{2});
    EXPECT_EQ(6, g(type_c{3}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, member_variable)
{
    const foo f{};
    const auto g = score::cpp::bind_front(&foo::baz, f);
    EXPECT_EQ(42, g());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, free_function)
{
    const auto f = &fn;
    const auto g = score::cpp::bind_front(f, type_a{1}, type_b{2});
    EXPECT_EQ(6, g(type_c{3}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, mutable_arg)
{
    int a{1};
    int b{2};
    const auto f = [](int& lhs, int& rhs) { std::swap(lhs, rhs); };
    score::cpp::bind_front(f, std::reference_wrapper<int>(a))(b);
    EXPECT_EQ(2, a);
    EXPECT_EQ(1, b);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, mutable_callable)
{
    auto f = [x = 0](int n) mutable {
        const int res{x};
        x += n;
        return res;
    };
    const auto g = score::cpp::bind_front(std::reference_wrapper<decltype(f)>(f), 42);
    EXPECT_EQ(0, g());
    EXPECT_EQ(42, g());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62587763
TEST(bind_front, fully_applied)
{
    const auto f = [](int x) { return x; };
    const auto g = score::cpp::bind_front(f, 42);
    EXPECT_EQ(42, g());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, call_operator_noexcept)
{
    using wrapper_never_throws = decltype(score::cpp::bind_back(std::declval<never_throws>()));
    using wrapper_may_throw = decltype(score::cpp::bind_back(std::declval<may_throw>()));

    static_assert(noexcept(std::declval<wrapper_never_throws&>()()));
    static_assert(!noexcept(std::declval<wrapper_may_throw&>()()));

    static_assert(noexcept(std::declval<const wrapper_never_throws&>()()));
    static_assert(!noexcept(std::declval<const wrapper_may_throw&>()()));

    static_assert(noexcept(std::declval<wrapper_never_throws&&>()()));
    static_assert(!noexcept(std::declval<wrapper_may_throw&&>()()));

    static_assert(noexcept(std::declval<const wrapper_never_throws&&>()()));
    static_assert(!noexcept(std::declval<const wrapper_may_throw&&>()()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, constructor_function_value_categories_and_constness)
{
    {
        move_copy_counter f{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(f);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(std::move(f));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
    {
        const move_copy_counter f{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(f);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(std::move(f));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, constructor_bound_args_value_categories_and_constness)
{
    const auto fn = [](move_copy_counter) {};

    {
        move_copy_counter arg{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(fn, arg);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(fn, std::move(arg));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
    {
        const move_copy_counter arg{};
        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(fn, arg);

        EXPECT_EQ(0, move_copy_counter::move_count);
        EXPECT_EQ(1, move_copy_counter::copy_count);

        move_copy_counter::reset();

        std::ignore = score::cpp::bind_back(fn, std::move(arg));

        EXPECT_EQ(1, move_copy_counter::move_count);
        EXPECT_EQ(0, move_copy_counter::copy_count);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, callable_ref_qualifiers)
{
    auto f = score::cpp::bind_back(functor_with_overload_on_ref_qualifier{});
    const auto const_f = score::cpp::bind_back(functor_with_overload_on_ref_qualifier{});

    const type_a a{f()};
    const type_b b{const_f()};
    const type_c c{std::move(f)()};
    const type_d d{std::move(const_f)()};

    EXPECT_EQ(1, a.data);
    EXPECT_EQ(2, b.data);
    EXPECT_EQ(3, c.data);
    EXPECT_EQ(4, d.data);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, bound_args_value_categories_and_constness)
{
    auto f = score::cpp::bind_back(functor_with_overload_on_signature{}, 0);
    const auto const_f = score::cpp::bind_back(functor_with_overload_on_signature{}, 0);

    const type_a a{f()};
    const type_b b{const_f()};
    const type_c c{std::move(f)()};
    const type_d d{std::move(const_f)()};

    EXPECT_EQ(1, a.data);
    EXPECT_EQ(2, b.data);
    EXPECT_EQ(3, c.data);
    EXPECT_EQ(4, d.data);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, call_args_value_categories_and_constness)
{
    const auto f = score::cpp::bind_back(functor_with_overload_on_signature{});

    int x{};
    const int y{};

    const type_a a{f(x)};
    const type_b b{f(y)};
    const type_c c{f(std::move(x))};
    const type_d d{f(std::move(y))};

    EXPECT_EQ(1, a.data);
    EXPECT_EQ(2, b.data);
    EXPECT_EQ(3, c.data);
    EXPECT_EQ(4, d.data);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, lambda)
{
    const auto f = [](type_a a, type_b b, type_c c) { return fn(a, b, c); };
    const auto g = score::cpp::bind_back(f, type_b{2}, type_c{3});
    EXPECT_EQ(6, g(type_a{1}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, function_object)
{
    const foo f{};
    const auto g = score::cpp::bind_back(f, type_b{2}, type_c{3});
    EXPECT_EQ(6, g(type_a{1}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, member_function)
{
    const foo f{};
    const auto g = score::cpp::bind_back(&foo::bar, type_b{2}, type_c{3});
    EXPECT_EQ(6, g(f, type_a{1}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, member_variable)
{
    const foo f{};
    const auto g = score::cpp::bind_back(&foo::baz, f);
    EXPECT_EQ(42, g());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, free_function)
{
    const auto f = &fn;
    const auto g = score::cpp::bind_back(f, type_b{2}, type_c{3});
    EXPECT_EQ(6, g(type_a{1}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, mutable_arg)
{
    int a{1};
    int b{2};
    const auto f = [](int& lhs, int& rhs) { std::swap(lhs, rhs); };
    score::cpp::bind_back(f, std::reference_wrapper<int>(a))(b);
    EXPECT_EQ(2, a);
    EXPECT_EQ(1, b);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, mutable_callable)
{
    auto f = [x = 0](int n) mutable {
        const int res{x};
        x += n;
        return res;
    };
    const auto g = score::cpp::bind_back(std::reference_wrapper<decltype(f)>(f), 42);
    EXPECT_EQ(0, g());
    EXPECT_EQ(42, g());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#62590909
TEST(bind_back, fully_applied)
{
    const auto f = [](int x) { return x; };
    const auto g = score::cpp::bind_back(f, 42);
    EXPECT_EQ(42, g());
}

} // namespace
