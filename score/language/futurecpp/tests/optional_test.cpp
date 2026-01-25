/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///

#include <score/optional.hpp>
#include <score/optional.hpp> // test include guard

#include <score/assert_support.hpp>
#include <score/expected.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace
{

class non_default_ctor
{
public:
    explicit non_default_ctor(int first, int second) : first_(first), second_(second) { ctor_counter++; }
    ~non_default_ctor() { dtor_counter++; }
    non_default_ctor(const non_default_ctor&) { ctor_counter++; }
    non_default_ctor(non_default_ctor&&) { ctor_counter++; }
    non_default_ctor& operator=(const non_default_ctor&) = default;
    non_default_ctor& operator=(non_default_ctor&&) = default;

    static std::uint32_t count() { return ctor_counter - dtor_counter; }
    static std::uint32_t ctor_count() { return ctor_counter; }
    static std::uint32_t dtor_count() { return dtor_counter; }

    static void reset()
    {
        ctor_counter = 0;
        dtor_counter = 0;
    }

    std::int32_t first() const { return first_; }
    std::int32_t second() const { return second_; }

private:
    static std::uint32_t ctor_counter;
    static std::uint32_t dtor_counter;

    std::int32_t first_;
    std::int32_t second_;
};
std::uint32_t non_default_ctor::ctor_counter = 0;
std::uint32_t non_default_ctor::dtor_counter = 0;

class copy_detector
{
    unsigned member_;

public:
    copy_detector() { member_ = 0; }

    copy_detector(const copy_detector& x) { member_ = x.member() + 1; }

    copy_detector& operator=(const copy_detector& x)
    {
        member_ = x.member() + 1;
        return *this;
    }

    const std::uint32_t& member() const { return member_; }
};

class move_only
{
    static std::uint32_t ctor_counter;
    static std::uint32_t dtor_counter;

public:
    move_only() { ++ctor_counter; }

    move_only(const move_only&) = delete;
    move_only(move_only&&) { ++ctor_counter; };
    ~move_only() { ++dtor_counter; };

    move_only& operator=(const move_only& x) = delete;
    move_only& operator=(move_only&&) { return *this; };

    static std::uint32_t count() { return ctor_counter - dtor_counter; }
    static std::uint32_t ctor_count() { return ctor_counter; }
    static std::uint32_t dtor_count() { return dtor_counter; }

    static void reset() { ctor_counter = dtor_counter = 0; }
};
std::uint32_t move_only::ctor_counter = 0;
std::uint32_t move_only::dtor_counter = 0;

class is_const_member
{
public:
    static std::uint32_t const_called;
    static std::uint32_t non_const_called;

    is_const_member() {}

    void member() { non_const_called++; }

    void member() const { const_called++; }
};
std::uint32_t is_const_member::const_called = 0;
std::uint32_t is_const_member::non_const_called = 0;

struct without_noexcept
{
    without_noexcept() = default;
    without_noexcept(const without_noexcept&) = default;
    without_noexcept& operator=(const without_noexcept&) = default;
    without_noexcept(without_noexcept&&) {}
    without_noexcept& operator=(without_noexcept&&) { return *this; }
    ~without_noexcept() = default;
};

struct noexcept_move_ctor
{
    noexcept_move_ctor() = default;
    noexcept_move_ctor(const noexcept_move_ctor&) = default;
    noexcept_move_ctor& operator=(const noexcept_move_ctor&) = default;
    noexcept_move_ctor(noexcept_move_ctor&&) noexcept {}
    noexcept_move_ctor& operator=(noexcept_move_ctor&&) { return *this; }
    ~noexcept_move_ctor() = default;
};

struct noexcept_move_assign
{
    noexcept_move_assign() = default;
    noexcept_move_assign(const noexcept_move_assign&) = default;
    noexcept_move_assign& operator=(const noexcept_move_assign&) = default;
    noexcept_move_assign(noexcept_move_assign&&) {}
    noexcept_move_assign& operator=(noexcept_move_assign&&) noexcept { return *this; }
    ~noexcept_move_assign() = default;
};

struct noexcept_move_assign_and_ctor
{
    noexcept_move_assign_and_ctor() = default;
    noexcept_move_assign_and_ctor(const noexcept_move_assign_and_ctor&) = default;
    noexcept_move_assign_and_ctor& operator=(const noexcept_move_assign_and_ctor&) = default;
    noexcept_move_assign_and_ctor(noexcept_move_assign_and_ctor&&) noexcept {}
    noexcept_move_assign_and_ctor& operator=(noexcept_move_assign_and_ctor&&) noexcept { return *this; }
    ~noexcept_move_assign_and_ctor() = default;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, basic_functionality)
{
    score::cpp::optional<int> fixture;

    EXPECT_FALSE(fixture.has_value());
    EXPECT_FALSE(fixture);

    const int value = -5;

    fixture = value;

    EXPECT_TRUE(fixture.has_value());
    EXPECT_TRUE(fixture);

    EXPECT_EQ(value, fixture.value());
    EXPECT_EQ(value, *fixture);

    fixture.reset();

    EXPECT_FALSE(fixture.has_value());
    EXPECT_FALSE(fixture);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, braced_default_initialization)
{
    score::cpp::optional<int> fixture{};

    EXPECT_FALSE(fixture.has_value());
    EXPECT_FALSE(fixture);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, optional_copy_list_initialization)
{
    score::cpp::optional<int> sut = {};

    EXPECT_FALSE(sut.has_value());
    EXPECT_FALSE(sut);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, optional_direct_list_initialization)
{
    score::cpp::optional<int> sut{};

    EXPECT_FALSE(sut.has_value());
    EXPECT_FALSE(sut);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, non_default_ctor_optional_direct_list_initialization)
{
    score::cpp::optional<non_default_ctor> sut{};

    EXPECT_FALSE(sut.has_value());
    EXPECT_FALSE(sut);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, optional_assignment_from_copy_list)
{
    score::cpp::optional<int> sut{score::cpp::nullopt};

    sut = {};

    EXPECT_FALSE(sut.has_value());
    EXPECT_FALSE(sut);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, optional_self_assignment)
{
    score::cpp::optional<int> sut{1};

    auto& sut_ref = sut; // using an additional reference to silence warning for intentional self-assign

    sut = sut_ref;

    ASSERT_TRUE(sut.has_value());
    EXPECT_EQ(sut.value(), 1);

    sut = std::move(sut_ref);

    ASSERT_TRUE(sut.has_value());
    EXPECT_EQ(sut.value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, optional_copy_asignment_from_blank)
{
    score::cpp::optional<int> sut{1};
    score::cpp::optional<int> sut_2{};
    EXPECT_FALSE(sut_2.has_value());
    sut = sut_2;
    EXPECT_FALSE(sut.has_value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, non_default_ctor_optional_assignment_from_copy_list)
{
    score::cpp::optional<non_default_ctor> sut{score::cpp::nullopt};

    sut = {};

    EXPECT_FALSE(sut.has_value());
    EXPECT_FALSE(sut);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, create_from_expected_with_lvalue)
{
    const char uval{'a'};
    const int eval{42};
    const score::cpp::expected<int, char> val_unexpected{score::cpp::make_unexpected(uval)};
    const score::cpp::expected<int, char> val_expected{eval};

    EXPECT_FALSE(val_unexpected.has_value());
    EXPECT_EQ(uval, val_unexpected.error());
    EXPECT_TRUE(val_expected.has_value());
    EXPECT_EQ(eval, val_expected.value());

    {
        // ctor unexpected
        score::cpp::optional<int> val_optional{val_unexpected};
        EXPECT_FALSE(val_optional.has_value());
    }

    {
        // ctor expected
        score::cpp::optional<int> val_optional{val_expected};
        EXPECT_TRUE(val_optional.has_value());
        EXPECT_EQ(eval, val_optional.value());
    }

    {
        // assignment unexpected
        score::cpp::optional<int> val_optional{eval};
        EXPECT_TRUE(val_optional.has_value());
        val_optional = val_unexpected;
        EXPECT_FALSE(val_optional.has_value());
    }

    {
        // assignment expected
        score::cpp::optional<int> val_optional{};
        EXPECT_FALSE(val_optional.has_value());
        val_optional = val_expected;
        EXPECT_TRUE(val_optional.has_value());
        EXPECT_EQ(eval, val_optional.value());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, create_from_expected_with_rvalue)
{
    const char uval{'a'};
    const int eval{42};

    {
        // ctor unexpected
        score::cpp::optional<int> val_optional{score::cpp::expected<int, char>{score::cpp::make_unexpected(uval)}};
        EXPECT_FALSE(val_optional.has_value());
    }

    {
        // ctor expected
        score::cpp::optional<int> val_optional{score::cpp::expected<int, char>{eval}};
        EXPECT_TRUE(val_optional.has_value());
        EXPECT_EQ(eval, val_optional.value());
    }

    {
        // assignment unexpected
        score::cpp::optional<int> val_optional{eval};
        EXPECT_TRUE(val_optional.has_value());
        val_optional = score::cpp::expected<int, char>{score::cpp::make_unexpected(uval)};
        EXPECT_FALSE(val_optional.has_value());
    }

    {
        // assignment expected
        score::cpp::optional<int> val_optional{};
        EXPECT_FALSE(val_optional.has_value());
        val_optional = score::cpp::expected<int, char>{eval};
        EXPECT_TRUE(val_optional.has_value());
        EXPECT_EQ(eval, val_optional.value());
    }
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, copy_assignment_deletion)
{
    EXPECT_EQ(0, non_default_ctor::count());

    {
        score::cpp::optional<non_default_ctor> fixture;

        EXPECT_EQ(0, non_default_ctor::count());

        non_default_ctor arg(1, 2);

        fixture = arg;

        EXPECT_EQ(2, non_default_ctor::count());

        // copy
        score::cpp::optional<non_default_ctor> fixture2(fixture);

        EXPECT_EQ(3, non_default_ctor::count());

        score::cpp::optional<non_default_ctor> fixture3;
        EXPECT_EQ(3, non_default_ctor::count());

        // assign to empty
        fixture3 = fixture2;
        EXPECT_EQ(4, non_default_ctor::count());

        // copy empty
        score::cpp::optional<non_default_ctor> fixture4;
        score::cpp::optional<non_default_ctor> fixture5(fixture4);
        EXPECT_EQ(4, non_default_ctor::count());

        // invalidate empty
        fixture5.reset();
        EXPECT_EQ(4, non_default_ctor::count());

        // invalidate
        fixture.reset();
        EXPECT_FALSE(fixture.has_value());
        EXPECT_EQ(3, non_default_ctor::count());

        // assign to non-empty
        fixture = arg;
        EXPECT_EQ(4, non_default_ctor::count());
        EXPECT_TRUE(fixture2.has_value());
        EXPECT_TRUE(fixture.has_value());
        EXPECT_EQ(5, non_default_ctor::ctor_count());
        EXPECT_EQ(1, non_default_ctor::dtor_count());
        fixture = fixture2;
        EXPECT_EQ(4, non_default_ctor::count());
        EXPECT_EQ(6, non_default_ctor::ctor_count());
        EXPECT_EQ(2, non_default_ctor::dtor_count());

        // delete value because of move-assignment of empty optional
        fixture = score::cpp::optional<non_default_ctor>();
        EXPECT_FALSE(fixture.has_value());
        EXPECT_EQ(3, non_default_ctor::count());

        // construct from value
        score::cpp::optional<non_default_ctor> fixture6(arg);
        EXPECT_TRUE(fixture6.has_value());
        EXPECT_EQ(4, non_default_ctor::count());

        // assignment from value
        EXPECT_TRUE(fixture6.has_value());
        EXPECT_EQ(4, non_default_ctor::count());
        EXPECT_EQ(7, non_default_ctor::ctor_count());
        EXPECT_EQ(3, non_default_ctor::dtor_count());
        // we expect that arg will be copy-assigned to the contained value inside fixture6.
        fixture6 = arg;
        EXPECT_EQ(4, non_default_ctor::count());
        EXPECT_EQ(7, non_default_ctor::ctor_count());
        EXPECT_EQ(3, non_default_ctor::dtor_count());

        // delete value because of copy-assignment of empty optional
        ASSERT_TRUE(fixture6.has_value());
        score::cpp::optional<non_default_ctor> empty;
        fixture6 = empty;
        EXPECT_FALSE(fixture6.has_value());
        EXPECT_EQ(3, non_default_ctor::count());
        EXPECT_EQ(7, non_default_ctor::ctor_count());
        EXPECT_EQ(4, non_default_ctor::dtor_count());
    }

    EXPECT_EQ(0, non_default_ctor::count());
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, move_construct)
{
    move_only::reset();
    {
        move_only value{};
        score::cpp::optional<move_only> sut_first{std::move(value)};
        EXPECT_EQ(move_only::ctor_count(), 2);
        EXPECT_TRUE(sut_first.has_value());

        score::cpp::optional<move_only> sut_second{std::move(sut_first)};
        EXPECT_TRUE(sut_first.has_value());
        EXPECT_TRUE(sut_second.has_value());

        score::cpp::optional<move_only> sut_third{score::cpp::optional<move_only>{}};
        EXPECT_EQ(move_only::ctor_count(), 3);
        EXPECT_EQ(move_only::dtor_count(), 0);
    }
    EXPECT_EQ(move_only::ctor_count(), 3);
    EXPECT_EQ(move_only::dtor_count(), 3);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, move_construct_noexcept)
{
    static_assert(std::is_nothrow_move_constructible<score::cpp::optional<noexcept_move_ctor>>::value, "failed");
    static_assert(!std::is_nothrow_move_constructible<score::cpp::optional<without_noexcept>>::value, "failed");
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, move_assign)
{
    move_only::reset();
    {
        move_only value{};
        score::cpp::optional<move_only> sut_first{std::move(value)};
        score::cpp::optional<move_only> sut_second{};
        EXPECT_TRUE(sut_first.has_value());
        EXPECT_FALSE(sut_second.has_value());
        EXPECT_EQ(move_only::ctor_count(), 2);

        sut_second = std::move(sut_first);
        EXPECT_TRUE(sut_first.has_value());
        EXPECT_TRUE(sut_second.has_value());

        EXPECT_EQ(move_only::ctor_count(), 3);
        EXPECT_EQ(move_only::dtor_count(), 0);
    }
    EXPECT_EQ(move_only::ctor_count(), 3);
    EXPECT_EQ(move_only::dtor_count(), 3);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, move_assign_noexcept)
{
    static_assert(std::is_nothrow_move_assignable<score::cpp::optional<noexcept_move_assign_and_ctor>>::value, "failed");
    static_assert(!std::is_nothrow_move_assignable<score::cpp::optional<without_noexcept>>::value, "failed");
    static_assert(!std::is_nothrow_move_assignable<score::cpp::optional<noexcept_move_ctor>>::value, "failed");
    static_assert(!std::is_nothrow_move_assignable<score::cpp::optional<noexcept_move_assign>>::value, "failed");
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, deep_copy)
{
    score::cpp::optional<int> fixture;
    const int value1 = -5;
    fixture = value1;

    const int value2 = -6;

    score::cpp::optional<int> copy(fixture);
    EXPECT_EQ(copy.value(), fixture.value());

    fixture = value2;
    EXPECT_EQ(value1, copy.value());
    EXPECT_EQ(value2, fixture.value());

    score::cpp::optional<int> assign;
    assign = copy;
    EXPECT_EQ(copy.value(), assign.value());

    const int value3 = -9;
    copy = value3;
    EXPECT_EQ(value3, copy.value());
    EXPECT_EQ(value1, assign.value());

    const score::cpp::optional<int> const_copy(copy);
    EXPECT_EQ(copy.value(), const_copy.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, manipulate)
{
    score::cpp::optional<int> fixture;
    fixture = 42;

    EXPECT_EQ(42, fixture.value());

    fixture.value() = 23;

    EXPECT_EQ(23, fixture.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, value_or_test_constness)
{
    score::cpp::optional<is_const_member> fixture;

    EXPECT_EQ(0, is_const_member::const_called);
    EXPECT_EQ(0, is_const_member::non_const_called);

    fixture.value_or(is_const_member()).member();
    EXPECT_EQ(1, is_const_member::const_called);
    EXPECT_EQ(0, is_const_member::non_const_called);

    fixture = is_const_member();
    fixture.value_or(is_const_member()).member();
    EXPECT_EQ(2, is_const_member::const_called);
    EXPECT_EQ(0, is_const_member::non_const_called);

    is_const_member helper = fixture.value_or(is_const_member());
    helper.member();
    EXPECT_EQ(2, is_const_member::const_called);
    EXPECT_EQ(1, is_const_member::non_const_called);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, value_or_test_copy)
{
    copy_detector helper;
    EXPECT_EQ(0, helper.member());
    score::cpp::optional<copy_detector> fixture;
    fixture = helper;                       // one copy here
    EXPECT_EQ(1, fixture.value().member()); // no copy here, passed by reference

    EXPECT_EQ(2, fixture.value_or(helper).member()); // copy in return -> object in fixture -> increase 1 => 2

    fixture.reset();
    EXPECT_EQ(1, fixture.value_or(helper).member()); // copy in return -> object in argument -> increase 0 => 1
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, value_or_test_basics)
{
    score::cpp::optional<int> fixture;
    EXPECT_EQ(5, fixture.value_or(5));

    fixture = 1;
    EXPECT_EQ(1, fixture.value_or(5));

    fixture.reset();
    EXPECT_EQ(5, fixture.value_or(5));

    fixture = 2;
    EXPECT_EQ(2, fixture.value_or(5));
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

struct functor_and_then_with_overload_on_signature
{
    score::cpp::optional<type_a> operator()(int&) const { return score::cpp::optional<type_a>{type_a{1}}; }
    score::cpp::optional<type_b> operator()(const int&) const { return score::cpp::optional<type_b>{type_b{2}}; }
    score::cpp::optional<type_c> operator()(int&&) const { return score::cpp::optional<type_c>{type_c{3}}; }
    score::cpp::optional<type_d> operator()(const int&&) const { return score::cpp::optional<type_d>{type_d{4}}; }
};

struct functor_and_then_with_overload_on_ref_qualifier
{
    score::cpp::optional<type_a> operator()(int) & { return score::cpp::optional<type_a>{type_a{1}}; }
    score::cpp::optional<type_b> operator()(int) const& { return score::cpp::optional<type_b>{type_b{2}}; }
    score::cpp::optional<type_c> operator()(int) && { return score::cpp::optional<type_c>{type_c{3}}; }
    score::cpp::optional<type_d> operator()(int) const&& { return score::cpp::optional<type_d>{type_d{4}}; }
};

struct functor_and_then_with_state
{
    score::cpp::optional<int> operator()(int) { return score::cpp::optional<int>{++data}; }
    int data;
};

score::cpp::optional<int> free_and_then(int) { return score::cpp::optional<int>{1}; }

struct functor_transform_with_overload_on_signature
{
    type_a operator()(int&) const { return type_a{1}; }
    type_b operator()(const int&) const { return type_b{2}; }
    type_c operator()(int&&) const { return type_c{3}; }
    type_d operator()(const int&&) const { return type_d{4}; }
};

struct functor_transform_with_overload_on_ref_qualifier
{
    type_a operator()(int) & { return type_a{1}; }
    type_b operator()(int) const& { return type_b{2}; }
    type_c operator()(int) && { return type_c{3}; }
    type_d operator()(int) const&& { return type_d{4}; }
};

struct functor_transform_with_state
{
    int operator()(int) { return ++data; }
    int data;
};

int free_transform(int) { return 1; }

struct functor_or_else_with_overload_on_ref_qualifier
{
    score::cpp::optional<int> operator()() & { return score::cpp::optional<int>{1}; }
    score::cpp::optional<int> operator()() const& { return score::cpp::optional<int>{2}; }
    score::cpp::optional<int> operator()() && { return score::cpp::optional<int>{3}; }
    score::cpp::optional<int> operator()() const&& { return score::cpp::optional<int>{4}; }
};

struct functor_or_else_with_state
{
    score::cpp::optional<int> operator()() { return score::cpp::optional<int>{++data}; }
    int data;
};

score::cpp::optional<int> free_or_else() { return score::cpp::optional<int>{1}; }

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, and_then_empty_optional)
{
    const auto fn = [](int) -> score::cpp::optional<int> { return score::cpp::optional<int>{1}; };

    score::cpp::optional<int> fixture{};

    EXPECT_FALSE(fixture.and_then(fn).has_value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, and_then_modify_optional)
{
    const auto fn = [](int& i) -> score::cpp::optional<int> { return score::cpp::optional<int>{++i}; };

    score::cpp::optional<int> fixture{0};

    const score::cpp::optional<int> result{fixture.and_then(fn)};

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    ASSERT_TRUE(fixture.has_value());
    EXPECT_EQ(fixture.value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, and_then_modify_callable)
{
    functor_and_then_with_state ftor{0};

    score::cpp::optional<int> fixture{0};

    const score::cpp::optional<int> result{fixture.and_then(ftor)};

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    EXPECT_EQ(ftor.data, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, and_then_invocation_free_function)
{
    score::cpp::optional<int> fixture{0};

    ASSERT_TRUE(fixture.and_then(free_and_then).has_value());
    EXPECT_EQ(fixture.and_then(free_and_then).value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, and_then_optional_value_categories_and_constness)
{
    score::cpp::optional<int> fixture{0};
    const score::cpp::optional<int> const_fixture{0};

    score::cpp::optional<type_a> a{fixture.and_then(functor_and_then_with_overload_on_signature{})};
    score::cpp::optional<type_b> b{const_fixture.and_then(functor_and_then_with_overload_on_signature{})};
    score::cpp::optional<type_c> c{std::move(fixture).and_then(functor_and_then_with_overload_on_signature{})};
    score::cpp::optional<type_d> d{std::move(const_fixture).and_then(functor_and_then_with_overload_on_signature{})};

    ASSERT_TRUE(a.has_value());
    EXPECT_EQ(a.value().data, 1);

    ASSERT_TRUE(b.has_value());
    EXPECT_EQ(b.value().data, 2);

    ASSERT_TRUE(c.has_value());
    EXPECT_EQ(c.value().data, 3);

    ASSERT_TRUE(d.has_value());
    EXPECT_EQ(d.value().data, 4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, and_then_callable_ref_qualifiers)
{
    score::cpp::optional<int> fixture{0};

    functor_and_then_with_overload_on_ref_qualifier ftor{};
    const functor_and_then_with_overload_on_ref_qualifier const_ftor{};

    score::cpp::optional<type_a> a{fixture.and_then(ftor)};
    score::cpp::optional<type_b> b{fixture.and_then(const_ftor)};
    score::cpp::optional<type_c> c{fixture.and_then(std::move(ftor))};
    score::cpp::optional<type_d> d{fixture.and_then(std::move(const_ftor))};

    ASSERT_TRUE(a.has_value());
    EXPECT_EQ(a.value().data, 1);

    ASSERT_TRUE(b.has_value());
    EXPECT_EQ(b.value().data, 2);

    ASSERT_TRUE(c.has_value());
    EXPECT_EQ(c.value().data, 3);

    ASSERT_TRUE(d.has_value());
    EXPECT_EQ(d.value().data, 4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, and_then_callable_return_types)
{
    score::cpp::optional<int> fixture{0};

    score::cpp::optional<int> captured{3};
    score::cpp::optional<int> other_captured{4};

    const auto fn1 = [](int) -> score::cpp::optional<int> { return score::cpp::optional<int>{1}; };
    const auto fn2 = [](int) -> const score::cpp::optional<int> { return score::cpp::optional<int>{2}; };
    const auto fn3 = [&captured](int) -> score::cpp::optional<int>& { return captured; };
    const auto fn4 = [&other_captured](int) -> const score::cpp::optional<int>& { return other_captured; };

    score::cpp::optional<int> result1{fixture.and_then(fn1)};
    score::cpp::optional<int> result2{fixture.and_then(fn2)};
    score::cpp::optional<int> result3{fixture.and_then(fn3)};
    score::cpp::optional<int> result4{fixture.and_then(fn4)};

    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 1);

    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), 2);

    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(result3.value(), 3);

    ASSERT_TRUE(result4.has_value());
    EXPECT_EQ(result4.value(), 4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, transform_empty_optional)
{
    const auto fn = [](int) -> int { return 1; };

    score::cpp::optional<int> fixture{};

    EXPECT_FALSE(fixture.transform(fn).has_value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, transform_modify_optional)
{
    const auto fn = [](int& i) -> int { return ++i; };

    score::cpp::optional<int> fixture{0};

    const score::cpp::optional<int> result{fixture.transform(fn)};

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    ASSERT_TRUE(fixture.has_value());
    EXPECT_EQ(fixture.value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, transform_modify_callable)
{
    functor_transform_with_state ftor{0};

    score::cpp::optional<int> fixture{0};

    const score::cpp::optional<int> result{fixture.transform(ftor)};

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    EXPECT_EQ(ftor.data, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, transform_invocation_free_function)
{
    score::cpp::optional<int> fixture{0};

    EXPECT_TRUE(fixture.transform(free_transform).has_value());
    EXPECT_EQ(fixture.transform(free_transform).value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, transform_optional_value_categories_and_constness)
{
    score::cpp::optional<int> fixture{0};
    const score::cpp::optional<int> const_fixture{0};

    score::cpp::optional<type_a> a{fixture.transform(functor_transform_with_overload_on_signature{})};
    score::cpp::optional<type_b> b{const_fixture.transform(functor_transform_with_overload_on_signature{})};
    score::cpp::optional<type_c> c{std::move(fixture).transform(functor_transform_with_overload_on_signature{})};
    score::cpp::optional<type_d> d{std::move(const_fixture).transform(functor_transform_with_overload_on_signature{})};

    ASSERT_TRUE(a.has_value());
    EXPECT_EQ(a.value().data, 1);

    ASSERT_TRUE(b.has_value());
    EXPECT_EQ(b.value().data, 2);

    ASSERT_TRUE(c.has_value());
    EXPECT_EQ(c.value().data, 3);

    ASSERT_TRUE(d.has_value());
    EXPECT_EQ(d.value().data, 4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, transform_callable_ref_qualifiers)
{
    score::cpp::optional<int> fixture{0};

    functor_transform_with_overload_on_ref_qualifier ftor{};
    const functor_transform_with_overload_on_ref_qualifier const_ftor{};

    score::cpp::optional<type_a> a{fixture.transform(ftor)};
    score::cpp::optional<type_b> b{fixture.transform(const_ftor)};
    score::cpp::optional<type_c> c{fixture.transform(std::move(ftor))};
    score::cpp::optional<type_d> d{fixture.transform(std::move(const_ftor))};

    ASSERT_TRUE(a.has_value());
    EXPECT_EQ(a.value().data, 1);

    ASSERT_TRUE(b.has_value());
    EXPECT_EQ(b.value().data, 2);

    ASSERT_TRUE(c.has_value());
    EXPECT_EQ(c.value().data, 3);

    ASSERT_TRUE(d.has_value());
    EXPECT_EQ(d.value().data, 4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, transform_callable_return_types)
{
    score::cpp::optional<int> fixture{0};

    const auto fn1 = [](int) -> int { return 1; };
    const auto fn2 = [](int) -> const std::vector<int> { return std::vector<int>{}; };

    score::cpp::optional<int> result1{fixture.transform(fn1)};
    score::cpp::optional<std::vector<int>> result2{fixture.transform(fn2)};

    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 1);

    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), std::vector<int>{});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_filled_optional)
{
    const auto fn = []() -> score::cpp::optional<int> { return score::cpp::optional<int>{0}; };

    score::cpp::optional<int> fixture{1};

    ASSERT_TRUE(fixture.or_else(fn).has_value());
    EXPECT_EQ(fixture.or_else(fn).value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_modify_callable)
{
    functor_or_else_with_state ftor{0};

    score::cpp::optional<int> fixture{};

    const score::cpp::optional<int> result{fixture.or_else(ftor)};

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    EXPECT_EQ(ftor.data, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_invocation_free_function)
{
    score::cpp::optional<int> fixture{};

    ASSERT_TRUE(fixture.or_else(free_or_else).has_value());
    EXPECT_EQ(fixture.or_else(free_or_else).value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_empty_optional_value_categories)
{
    score::cpp::optional<int> fixture{};

    score::cpp::optional<int> result1{fixture.or_else(free_or_else)};
    score::cpp::optional<int> result2{std::move(fixture).or_else(free_or_else)};

    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 1);

    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_filled_optional_copy_semantics)
{
    const auto fn = []() -> score::cpp::optional<copy_detector> { return score::cpp::optional<copy_detector>{}; };

    copy_detector value{};
    EXPECT_EQ(value.member(), 0);

    const score::cpp::optional<copy_detector> fixture{value};
    EXPECT_EQ(fixture.value().member(), 1);

    score::cpp::optional<copy_detector> result{fixture.or_else(fn)};
    EXPECT_EQ(fixture.value().member(), 1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().member(), 2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_filled_optional_move_semantics)
{
    const auto fn = []() -> score::cpp::optional<move_only> { return score::cpp::optional<move_only>{}; };

    move_only value{};
    score::cpp::optional<move_only> fixture{std::move(value)};

    move_only::reset();
    EXPECT_EQ(move_only::ctor_count(), 0);
    EXPECT_EQ(move_only::dtor_count(), 0);

    score::cpp::optional<move_only> result{std::move(fixture).or_else(fn)};
    EXPECT_EQ(move_only::ctor_count(), 1);
    EXPECT_EQ(move_only::dtor_count(), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_callable_ref_qualifiers)
{
    score::cpp::optional<int> fixture{};

    functor_or_else_with_overload_on_ref_qualifier ftor{};
    const functor_or_else_with_overload_on_ref_qualifier const_ftor{};

    score::cpp::optional<int> result1{fixture.or_else(ftor)};
    score::cpp::optional<int> result2{fixture.or_else(const_ftor)};
    score::cpp::optional<int> result3{fixture.or_else(std::move(ftor))};
    score::cpp::optional<int> result4{fixture.or_else(std::move(const_ftor))};

    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 1);

    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), 2);

    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(result3.value(), 3);

    ASSERT_TRUE(result4.has_value());
    EXPECT_EQ(result4.value(), 4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22372964
TEST(optional, or_else_callable_return_types)
{
    score::cpp::optional<int> fixture{};

    score::cpp::optional<int> captured{3};
    score::cpp::optional<int> other_captured{4};

    const auto fn1 = []() -> score::cpp::optional<int> { return score::cpp::optional<int>{1}; };
    const auto fn2 = []() -> const score::cpp::optional<int> { return score::cpp::optional<int>{2}; };
    const auto fn3 = [&captured]() -> score::cpp::optional<int>& { return captured; };
    const auto fn4 = [&other_captured]() -> const score::cpp::optional<int>& { return other_captured; };

    score::cpp::optional<int> result1{fixture.or_else(fn1)};
    score::cpp::optional<int> result2{fixture.or_else(fn2)};
    score::cpp::optional<int> result3{fixture.or_else(fn3)};
    score::cpp::optional<int> result4{fixture.or_else(fn4)};

    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 1);

    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), 2);

    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(result3.value(), 3);

    ASSERT_TRUE(result4.has_value());
    EXPECT_EQ(result4.value(), 4);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, set_via_assign)
{
    score::cpp::optional<int> fixture;
    EXPECT_FALSE(fixture.has_value());
    fixture = 42;
    EXPECT_TRUE(fixture.has_value());

    EXPECT_EQ(42, fixture.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, access)
{
    struct x
    {
        int x;
    };

    score::cpp::optional<x> fixture;
    EXPECT_FALSE(fixture);

    const x tmp = {42};
    fixture = tmp;
    EXPECT_TRUE(fixture);
    EXPECT_EQ(42, fixture->x);
    EXPECT_EQ(42, (*fixture).x);
    EXPECT_EQ(42, fixture.value().x);

    (*fixture).x = 23;
    EXPECT_TRUE(fixture);
    EXPECT_EQ(23, fixture->x);
    EXPECT_EQ(23, (*fixture).x);
    EXPECT_EQ(23, fixture.value().x);

    fixture->x = 42;
    EXPECT_TRUE(fixture);
    EXPECT_EQ(42, fixture->x);
    EXPECT_EQ(42, (*fixture).x);
    EXPECT_EQ(42, fixture.value().x);

    fixture.value().x = 23;
    EXPECT_TRUE(fixture);
    EXPECT_EQ(23, fixture->x);
    EXPECT_EQ(23, (*fixture).x);
    EXPECT_EQ(23, fixture.value().x);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_access)
{
    struct x
    {
        int x;
    };

    const x tmp = {42};
    const score::cpp::optional<x> fixture(tmp);
    EXPECT_TRUE(fixture);

    EXPECT_EQ(42, fixture->x);
    EXPECT_EQ(42, (*fixture).x);
    EXPECT_EQ(42, fixture.value().x);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, value_shall_trigger_precondition_when_empty_optional)
{
    {
        const score::cpp::optional<int> fixture{};
        EXPECT_FALSE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(fixture.value());
    }
    {
        const score::cpp::optional<int> fixture{42};
        EXPECT_TRUE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(fixture.value());
    }
    {
        score::cpp::optional<int> fixture{};
        EXPECT_FALSE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(fixture.value());
    }
    {
        score::cpp::optional<int> fixture{42};
        EXPECT_TRUE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(fixture.value());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, dereferecing_shall_trigger_precondition_when_empty_optional)
{
    {
        const score::cpp::optional<int> fixture{};
        EXPECT_FALSE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(*fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(fixture.operator->());
    }
    {
        const score::cpp::optional<int> fixture{42};
        EXPECT_TRUE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(*fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(fixture.operator->());
    }
    {
        score::cpp::optional<int> fixture{};
        EXPECT_FALSE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(*fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(fixture.operator->());
    }
    {
        score::cpp::optional<int> fixture{42};
        EXPECT_TRUE(fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(*fixture);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(fixture.operator->());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, emplace)
{
    score::cpp::optional<copy_detector> fixture;
    EXPECT_FALSE(fixture);

    fixture.emplace();
    EXPECT_TRUE(fixture);
    EXPECT_EQ(0, fixture.value().member());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, emplace_not_leak)
{
    non_default_ctor::reset();
    EXPECT_EQ(0, non_default_ctor::ctor_count());
    EXPECT_EQ(0, non_default_ctor::dtor_count());
    score::cpp::optional<non_default_ctor> fixture{non_default_ctor{1, 2}};
    EXPECT_TRUE(fixture.has_value());

    EXPECT_EQ(2, non_default_ctor::ctor_count());
    EXPECT_EQ(1, non_default_ctor::dtor_count());

    fixture.emplace(2, 3);
    EXPECT_TRUE(fixture.has_value());

    EXPECT_EQ(3, non_default_ctor::ctor_count());
    EXPECT_EQ(2, non_default_ctor::dtor_count());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, emplace_with_varargs)
{
    score::cpp::optional<non_default_ctor> fixture;
    EXPECT_FALSE(fixture);

    non_default_ctor& value = fixture.emplace(1, 2);
    EXPECT_TRUE(fixture);
    EXPECT_EQ(1, value.first());
    EXPECT_EQ(2, value.second());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, reset)
{
    score::cpp::optional<int> fixture;
    EXPECT_FALSE(fixture);

    fixture = 42;
    EXPECT_TRUE(fixture);

    fixture.reset();
    EXPECT_FALSE(fixture);
}

class forward_counter
{
    std::int32_t copy_construction_count_;
    std::int32_t move_construction_count_;
    std::int32_t copy_assignment_count_;
    std::int32_t move_assignment_count_;

public:
    forward_counter()
        : copy_construction_count_(0), move_construction_count_(0), copy_assignment_count_(0), move_assignment_count_(0)
    {
    }
    forward_counter(const forward_counter& other)
        : copy_construction_count_(other.copy_construction_count_ + 1)
        , move_construction_count_(other.move_construction_count_)
        , copy_assignment_count_(other.copy_assignment_count_)
        , move_assignment_count_(other.move_assignment_count_)
    {
    }
    forward_counter(forward_counter&& other)
        : copy_construction_count_(other.copy_construction_count_)
        , move_construction_count_(other.move_construction_count_ + 1)
        , copy_assignment_count_(other.copy_assignment_count_)
        , move_assignment_count_(other.move_assignment_count_)
    {
    }
    forward_counter& operator=(const forward_counter&)
    {
        ++copy_assignment_count_;
        return *this;
    }
    forward_counter& operator=(forward_counter&&)
    {
        ++move_assignment_count_;
        return *this;
    }

    std::int32_t copy_constructor_calls() const { return copy_construction_count_; }
    std::int32_t move_constructor_calls() const { return move_construction_count_; }
    std::int32_t copy_assignment_calls() const { return copy_assignment_count_; }
    std::int32_t move_assignment_calls() const { return move_assignment_count_; }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, perfect_forward_converting_constructor)
{
    forward_counter value;

    score::cpp::optional<forward_counter> fixture_1(std::move(value));
    EXPECT_EQ(1, fixture_1->move_constructor_calls());
    EXPECT_EQ(0, fixture_1->copy_constructor_calls());
    EXPECT_EQ(0, fixture_1->move_assignment_calls());
    EXPECT_EQ(0, fixture_1->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_2(value);
    EXPECT_EQ(1, fixture_2->copy_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_assignment_calls());
    EXPECT_EQ(0, fixture_2->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_3(forward_counter{});
    EXPECT_EQ(1, fixture_3->move_constructor_calls());
    EXPECT_EQ(0, fixture_3->copy_constructor_calls());
    EXPECT_EQ(0, fixture_3->move_assignment_calls());
    EXPECT_EQ(0, fixture_3->copy_assignment_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, perfect_forward_converting_assignment_to_empty_optional)
{
    forward_counter value;

    score::cpp::optional<forward_counter> fixture_1;
    fixture_1 = std::move(value);
    EXPECT_EQ(1, fixture_1->move_constructor_calls());
    EXPECT_EQ(0, fixture_1->copy_constructor_calls());
    EXPECT_EQ(0, fixture_1->move_assignment_calls());
    EXPECT_EQ(0, fixture_1->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_2;
    fixture_2 = value;
    EXPECT_EQ(1, fixture_2->copy_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_assignment_calls());
    EXPECT_EQ(0, fixture_2->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_3;
    fixture_3 = forward_counter{};
    EXPECT_EQ(1, fixture_3->move_constructor_calls());
    EXPECT_EQ(0, fixture_3->copy_constructor_calls());
    EXPECT_EQ(0, fixture_3->move_assignment_calls());
    EXPECT_EQ(0, fixture_3->copy_assignment_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, perfect_forward_converting_assignment_to_nonempty_optional)
{
    forward_counter value;

    score::cpp::optional<forward_counter> fixture_1{score::cpp::in_place};
    fixture_1 = std::move(value);
    EXPECT_EQ(1, fixture_1->move_assignment_calls());
    EXPECT_EQ(0, fixture_1->move_constructor_calls());
    EXPECT_EQ(0, fixture_1->copy_constructor_calls());
    EXPECT_EQ(0, fixture_1->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_2{score::cpp::in_place};
    fixture_2 = value;
    EXPECT_EQ(1, fixture_2->copy_assignment_calls());
    EXPECT_EQ(0, fixture_2->copy_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_assignment_calls());

    score::cpp::optional<forward_counter> fixture_3{score::cpp::in_place};
    fixture_3 = forward_counter{};
    EXPECT_EQ(1, fixture_3->move_assignment_calls());
    EXPECT_EQ(0, fixture_3->move_constructor_calls());
    EXPECT_EQ(0, fixture_3->copy_constructor_calls());
    EXPECT_EQ(0, fixture_3->copy_assignment_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, perfect_forward_assignment_to_empty_optional)
{
    score::cpp::optional<forward_counter> value{score::cpp::in_place};

    score::cpp::optional<forward_counter> fixture_1;
    fixture_1 = std::move(value);
    EXPECT_EQ(1, fixture_1->move_constructor_calls());
    EXPECT_EQ(0, fixture_1->copy_constructor_calls());
    EXPECT_EQ(0, fixture_1->move_assignment_calls());
    EXPECT_EQ(0, fixture_1->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_2;
    fixture_2 = value;
    EXPECT_EQ(1, fixture_2->copy_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_assignment_calls());
    EXPECT_EQ(0, fixture_2->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_3;
    fixture_3 = score::cpp::optional<forward_counter>{score::cpp::in_place};
    EXPECT_EQ(1, fixture_3->move_constructor_calls());
    EXPECT_EQ(0, fixture_3->copy_constructor_calls());
    EXPECT_EQ(0, fixture_3->move_assignment_calls());
    EXPECT_EQ(0, fixture_3->copy_assignment_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, perfect_forward_assignment_to_nonempty_optional)
{
    score::cpp::optional<forward_counter> value{score::cpp::in_place};

    score::cpp::optional<forward_counter> fixture_1{score::cpp::in_place};
    fixture_1 = std::move(value);
    EXPECT_EQ(0, fixture_1->move_assignment_calls());
    EXPECT_EQ(1, fixture_1->move_constructor_calls());
    EXPECT_EQ(0, fixture_1->copy_constructor_calls());
    EXPECT_EQ(0, fixture_1->copy_assignment_calls());

    score::cpp::optional<forward_counter> fixture_2{score::cpp::in_place};
    fixture_2 = value;
    EXPECT_EQ(0, fixture_2->copy_assignment_calls());
    EXPECT_EQ(0, fixture_2->move_assignment_calls());
    EXPECT_EQ(1, fixture_2->copy_constructor_calls());
    EXPECT_EQ(0, fixture_2->move_constructor_calls());

    score::cpp::optional<forward_counter> fixture_3{score::cpp::in_place};
    fixture_3 = score::cpp::optional<forward_counter>{score::cpp::in_place};
    EXPECT_EQ(0, fixture_3->move_assignment_calls());
    EXPECT_EQ(1, fixture_3->move_constructor_calls());
    EXPECT_EQ(0, fixture_3->copy_constructor_calls());
    EXPECT_EQ(0, fixture_3->copy_assignment_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, construct_from_nullopt)
{
    score::cpp::optional<int> sut{score::cpp::nullopt};

    EXPECT_FALSE(sut.has_value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, assign_from_nullopt)
{
    score::cpp::optional<int> sut{42};

    sut = score::cpp::nullopt;

    EXPECT_FALSE(sut.has_value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, in_place_construct)
{
    score::cpp::optional<int> sut1{score::cpp::in_place};
    EXPECT_TRUE(sut1.has_value());
    EXPECT_EQ(*sut1, 0);

    score::cpp::optional<int> sut2{score::cpp::in_place, 42};
    EXPECT_TRUE(sut2.has_value());
    EXPECT_EQ(*sut2, 42);

    score::cpp::optional<forward_counter> sut3{score::cpp::in_place};
    EXPECT_TRUE(sut3.has_value());
    EXPECT_EQ(0, sut3->move_assignment_calls());
    EXPECT_EQ(0, sut3->move_constructor_calls());
    EXPECT_EQ(0, sut3->copy_constructor_calls());
    EXPECT_EQ(0, sut3->copy_assignment_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_both_empty)
{
    score::cpp::optional<int32_t> fixture_lhs;
    score::cpp::optional<int32_t> fixture_rhs;

    EXPECT_TRUE(fixture_lhs == fixture_rhs);
    EXPECT_FALSE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_empty_and_rhs_filled)
{
    score::cpp::optional<int32_t> fixture_lhs;
    score::cpp::optional<int32_t> fixture_rhs{5};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_TRUE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_FALSE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_filled_and_rhs_empty)
{
    score::cpp::optional<int32_t> fixture_lhs{4};
    score::cpp::optional<int32_t> fixture_rhs;

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_FALSE(fixture_lhs <= fixture_rhs);
    EXPECT_TRUE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_less_than_rhs)
{
    score::cpp::optional<int32_t> fixture_lhs{4};
    score::cpp::optional<int32_t> fixture_rhs{5};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_TRUE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_FALSE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_greater_than_rhs)
{
    score::cpp::optional<int32_t> fixture_lhs{9};
    score::cpp::optional<int32_t> fixture_rhs{5};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_FALSE(fixture_lhs <= fixture_rhs);
    EXPECT_TRUE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_both_filled_and_equals)
{
    score::cpp::optional<int32_t> fixture_lhs{7};
    score::cpp::optional<int32_t> fixture_rhs{7};

    EXPECT_TRUE(fixture_lhs == fixture_rhs);
    EXPECT_FALSE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_nullopt_and_rhs_empty)
{
    score::cpp::optional<int32_t> fixture_rhs{};

    EXPECT_TRUE(score::cpp::nullopt == fixture_rhs);
    EXPECT_FALSE(score::cpp::nullopt != fixture_rhs);
    EXPECT_FALSE(score::cpp::nullopt < fixture_rhs);
    EXPECT_TRUE(score::cpp::nullopt <= fixture_rhs);
    EXPECT_FALSE(score::cpp::nullopt > fixture_rhs);
    EXPECT_TRUE(score::cpp::nullopt >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_nullopt_and_rhs_filled)
{
    score::cpp::optional<int32_t> fixture_rhs{7};

    EXPECT_FALSE(score::cpp::nullopt == fixture_rhs);
    EXPECT_TRUE(score::cpp::nullopt != fixture_rhs);
    EXPECT_TRUE(score::cpp::nullopt < fixture_rhs);
    EXPECT_TRUE(score::cpp::nullopt <= fixture_rhs);
    EXPECT_FALSE(score::cpp::nullopt > fixture_rhs);
    EXPECT_FALSE(score::cpp::nullopt >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_empty_and_rhs_nullopt)
{
    score::cpp::optional<int32_t> fixture_lhs{};

    EXPECT_TRUE(fixture_lhs == score::cpp::nullopt);
    EXPECT_FALSE(fixture_lhs != score::cpp::nullopt);
    EXPECT_FALSE(fixture_lhs < score::cpp::nullopt);
    EXPECT_TRUE(fixture_lhs <= score::cpp::nullopt);
    EXPECT_FALSE(fixture_lhs > score::cpp::nullopt);
    EXPECT_TRUE(fixture_lhs >= score::cpp::nullopt);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_filled_and_rhs_nullopt)
{
    score::cpp::optional<int32_t> fixture_lhs{7};

    EXPECT_FALSE(fixture_lhs == score::cpp::nullopt);
    EXPECT_TRUE(fixture_lhs != score::cpp::nullopt);
    EXPECT_FALSE(fixture_lhs < score::cpp::nullopt);
    EXPECT_FALSE(fixture_lhs <= score::cpp::nullopt);
    EXPECT_TRUE(fixture_lhs > score::cpp::nullopt);
    EXPECT_TRUE(fixture_lhs >= score::cpp::nullopt);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_empty_and_rhs_value)
{
    score::cpp::optional<int32_t> fixture_lhs{};
    int32_t fixture_rhs{7};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_TRUE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_FALSE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_filled_and_less_than_rhs_value)
{
    score::cpp::optional<int32_t> fixture_lhs{5};
    int32_t fixture_rhs{7};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_TRUE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_FALSE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_filled_and_greater_than_rhs_value)
{
    score::cpp::optional<int32_t> fixture_lhs{9};
    int32_t fixture_rhs{7};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_FALSE(fixture_lhs <= fixture_rhs);
    EXPECT_TRUE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_filled_and_equal_to_rhs_value)
{
    score::cpp::optional<int32_t> fixture_lhs{7};
    int32_t fixture_rhs{7};

    EXPECT_TRUE(fixture_lhs == fixture_rhs);
    EXPECT_FALSE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_value_and_rhs_empty)
{
    int32_t fixture_lhs{7};
    score::cpp::optional<int32_t> fixture_rhs{};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_FALSE(fixture_lhs <= fixture_rhs);
    EXPECT_TRUE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_value_less_than_filled_rhs)
{
    int32_t fixture_lhs{5};
    score::cpp::optional<int32_t> fixture_rhs{7};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_TRUE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_FALSE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_value_greater_than_filled_rhs)
{
    int32_t fixture_lhs{9};
    score::cpp::optional<int32_t> fixture_rhs{7};

    EXPECT_FALSE(fixture_lhs == fixture_rhs);
    EXPECT_TRUE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_FALSE(fixture_lhs <= fixture_rhs);
    EXPECT_TRUE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, comparison_operators_when_lhs_value_equal_to_filled_rhs)
{
    int32_t fixture_lhs{7};
    score::cpp::optional<int32_t> fixture_rhs{7};

    EXPECT_TRUE(fixture_lhs == fixture_rhs);
    EXPECT_FALSE(fixture_lhs != fixture_rhs);
    EXPECT_FALSE(fixture_lhs < fixture_rhs);
    EXPECT_TRUE(fixture_lhs <= fixture_rhs);
    EXPECT_FALSE(fixture_lhs > fixture_rhs);
    EXPECT_TRUE(fixture_lhs >= fixture_rhs);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, constructor_is_sufficiently_constrained)
{
    struct overload_set
    {
        static int foo(score::cpp::optional<int>) { return 0; }
        static int foo(score::cpp::optional<const char*>) { return 1; }
    };

    EXPECT_EQ(overload_set::foo(1), 0);
    EXPECT_EQ(overload_set::foo("foo"), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_braced_default_initialization)
{
    score::cpp::optional<const int> fixture{};
    EXPECT_FALSE(fixture);
    EXPECT_FALSE(fixture.has_value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_initialization_with_value)
{
    score::cpp::optional<const int> fixture{23};
    EXPECT_EQ(23, fixture.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_initialization_with_in_place)
{
    score::cpp::optional<const int> fixture{score::cpp::in_place, 23};
    EXPECT_EQ(23, fixture.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_copy_construct)
{
    score::cpp::optional<const int> fixture{23};
    score::cpp::optional<const int> copy{fixture};
    EXPECT_EQ(23, copy.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_move_construct)
{
    score::cpp::optional<const int> fixture{23};
    score::cpp::optional<const int> move{std::move(fixture)};
    EXPECT_EQ(23, move.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_emplace)
{
    score::cpp::optional<const int> fixture{};
    fixture.emplace(23);
    EXPECT_EQ(23, fixture.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(optional, const_reset)
{
    score::cpp::optional<const int> fixture{23};
    EXPECT_EQ(23, fixture.value());
    fixture.reset();
    EXPECT_FALSE(fixture.has_value());
}

constexpr int some_value{999};

struct default_constructor
{
    int get_value() const { return member_; }

private:
    int member_{some_value};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(make_optional, default_constructor)
{
    const auto result = score::cpp::make_optional<default_constructor>();
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result.value().get_value(), some_value);
}

constexpr std::size_t array_size{3UL};

struct object_initializer_list
{
    object_initializer_list() = delete;
    explicit object_initializer_list(std::initializer_list<int> list) : array{}
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT(list.size() <= array_size);
        std::copy(list.begin(), list.end(), array.begin());
    }
    object_initializer_list(const object_initializer_list&) = default;
    object_initializer_list(object_initializer_list&&) = default;
    object_initializer_list& operator=(const object_initializer_list&) = delete;
    object_initializer_list& operator=(object_initializer_list&&) = delete;
    ~object_initializer_list() = default;

    std::array<int, array_size> array;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(make_optional, passing_initializer_list)
{
    constexpr std::array<int, array_size> expected_result{1, 2, 3};

    const auto result = score::cpp::make_optional<object_initializer_list>(std::initializer_list<int>{1, 2, 3});
    ASSERT_TRUE(result.has_value());

    const auto opt_object = result.value();
    EXPECT_TRUE(std::equal(std::cbegin(expected_result),
                           std::cend(expected_result),
                           std::cbegin(opt_object.array),
                           std::cend(opt_object.array)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(make_optional, invoking_initializer_list)
{
    constexpr std::array<int, array_size> expected_result{1, 2, 3};

    const auto result = score::cpp::make_optional<object_initializer_list>({1, 2, 3});
    ASSERT_TRUE(result.has_value());

    const auto opt_object = result.value();
    EXPECT_TRUE(std::equal(std::cbegin(expected_result),
                           std::cend(expected_result),
                           std::cbegin(opt_object.array),
                           std::cend(opt_object.array)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(make_optional, non_default_constructor_passing_lvalue)
{
    constexpr std::int32_t first{111};
    constexpr std::int32_t second{222};

    const auto result = score::cpp::make_optional<non_default_ctor>(first, second);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result.value().first(), first);
    EXPECT_EQ(result.value().second(), second);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(make_optional, non_default_constructor_passing_rvalue)
{
    const std::int32_t first_value{111};
    const std::int32_t second_value{222};
    std::int32_t first{first_value};
    std::int32_t second{second_value};

    const auto result = score::cpp::make_optional<non_default_ctor>(std::move(first), std::move(second));
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result.value().first(), first_value);
    EXPECT_EQ(result.value().second(), second_value);
}

struct some_copy_constructible_object
{
    explicit some_copy_constructible_object(int a) : value{a} {}
    some_copy_constructible_object(const some_copy_constructible_object&) = default;
    some_copy_constructible_object(some_copy_constructible_object&&) = default;
    some_copy_constructible_object& operator=(const some_copy_constructible_object&) = delete;
    some_copy_constructible_object& operator=(some_copy_constructible_object&&) = delete;
    ~some_copy_constructible_object() = default;
    std::int32_t value;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9337998
TEST(make_optional, copy_constructor)
{
    const std::int32_t expected_value{1001};
    const some_copy_constructible_object test{expected_value};

    const auto result = score::cpp::make_optional(test);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result.value().value, expected_value);
}

/// @requirement CB-#9337998
TEST(make_optional, creates_const_correct_optional)
{
    {
        const auto result = score::cpp::make_optional<std::int32_t>(1001);
        static_assert(std::is_same<const score::cpp::optional<std::int32_t>, decltype(result)>::value, "failed");
    }
    {
        const auto result = score::cpp::make_optional<const std::int32_t>(1001);
        static_assert(std::is_same<const score::cpp::optional<const std::int32_t>, decltype(result)>::value, "failed");
    }
}

} // namespace
