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

#include <score/variant.hpp>
#include <score/variant.hpp> // test include guard

#include <cstdint>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

using score::cpp::get;
using score::cpp::get_if;
using score::cpp::holds_alternative;
using score::cpp::variant;
using score::cpp::visit;

namespace
{

class argh
{
public:
    argh() : id_(next_id++) { ++default_ctor_counter; }
    argh(argh const& rhs) : id_(rhs.id()) { ++copy_ctor_counter; }
    argh(argh&& rhs) : id_(rhs.id()) { ++move_ctor_counter; }
    ~argh() { ++dtor_counter; }
    argh& operator=(argh const& rhs)
    {
        id_ = rhs.id();
        ++copy_op_counter;
        return *this;
    }
    argh& operator=(argh&& rhs)
    {
        id_ = rhs.id();
        ++move_op_counter;
        return *this;
    }

    static std::uint32_t default_ctor_calls() { return default_ctor_counter; }
    static std::uint32_t copy_ctor_calls() { return copy_ctor_counter; }
    static std::uint32_t copy_op_calls() { return copy_op_counter; }
    static std::uint32_t move_ctor_calls() { return move_ctor_counter; }
    static std::uint32_t move_op_calls() { return move_op_counter; }
    static std::uint32_t dtor_calls() { return dtor_counter; }

    std::uint32_t id() const { return id_; }

    static void reset();

private:
    static std::uint32_t next_id;
    static std::uint32_t default_ctor_counter;
    static std::uint32_t copy_ctor_counter;
    static std::uint32_t copy_op_counter;
    static std::uint32_t move_ctor_counter;
    static std::uint32_t move_op_counter;
    static std::uint32_t dtor_counter;
    std::uint32_t id_;
};

void argh::reset()
{
    argh::default_ctor_counter = 0;
    argh::copy_ctor_counter = 0;
    argh::copy_op_counter = 0;
    argh::move_ctor_counter = 0;
    argh::move_op_counter = 0;
    argh::dtor_counter = 0;
}

std::uint32_t argh::next_id = 0;
std::uint32_t argh::default_ctor_counter = 0;
std::uint32_t argh::copy_ctor_counter = 0;
std::uint32_t argh::copy_op_counter = 0;
std::uint32_t argh::move_ctor_counter = 0;
std::uint32_t argh::move_op_counter = 0;
std::uint32_t argh::dtor_counter = 0;

enum found_type
{
    found_nothing,
    found_int,
    found_double,
    found_argh
};

struct int_double_argh_visitor
{
    found_type operator()(int const&) { return found_int; }
    found_type operator()(double const&) { return found_double; }
    found_type operator()(argh const&) { return found_argh; }
};

struct int_double_argh_mutable_visitor
{
    found_type operator()(int&) { return found_int; }
    found_type operator()(double&) { return found_double; }
    found_type operator()(argh&) { return found_argh; }
};

class variant_fixture : public ::testing::Test
{
protected:
    variant_fixture() { argh::reset(); }

    void expected_argh_counters(std::uint32_t default_ctor_calls,
                                std::uint32_t copy_ctor_calls,
                                std::uint32_t copy_op_calls,
                                std::uint32_t move_ctor_calls,
                                std::uint32_t move_op_calls,
                                std::uint32_t dtor_calls)
    {
        EXPECT_EQ(default_ctor_calls, argh::default_ctor_calls());
        EXPECT_EQ(copy_ctor_calls, argh::copy_ctor_calls());
        EXPECT_EQ(copy_op_calls, argh::copy_op_calls());
        EXPECT_EQ(move_ctor_calls, argh::move_ctor_calls());
        EXPECT_EQ(move_op_calls, argh::move_op_calls());
        EXPECT_EQ(dtor_calls, argh::dtor_calls());
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, variant_capacity)
{
    typedef variant<argh, double, int> v;
    const std::size_t storage_size = v::storage_size;
    EXPECT_EQ(storage_size, sizeof(double));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432448
TEST_F(variant_fixture, variant_alternative)
{
    using v = score::cpp::variant<argh, double, int>;
    static_assert(std::is_same<argh, score::cpp::variant_alternative_t<0U, v>>::value, "failed");
    static_assert(std::is_same<double, score::cpp::variant_alternative_t<1U, v>>::value, "failed");
    static_assert(std::is_same<int, score::cpp::variant_alternative_t<2U, v>>::value, "failed");
    static_assert(std::is_same<const argh, score::cpp::variant_alternative_t<0U, const v>>::value, "failed");
    static_assert(std::is_same<const double, score::cpp::variant_alternative_t<1U, const v>>::value, "failed");
    static_assert(std::is_same<const int, score::cpp::variant_alternative_t<2U, const v>>::value, "failed");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, default_ctor_argh)
{
    {
        variant<argh, double, int> v1;
        EXPECT_TRUE(holds_alternative<argh>(v1));
        EXPECT_EQ(0, v1.index());
    }

    expected_argh_counters(1, 0, 0, 0, 0, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, default_ctor_not_argh)
{
    {
        variant<int, double, argh> v1;
        EXPECT_TRUE(holds_alternative<int>(v1));
        EXPECT_EQ(0, v1.index());
    }

    expected_argh_counters(0, 0, 0, 0, 0, 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, converting_ctor)
{
    {
        variant<argh, double, int> v1{argh{}};
        EXPECT_TRUE(holds_alternative<argh>(v1));
        EXPECT_EQ(0, v1.index());
    }

    expected_argh_counters(1, 0, 0, 1, 0, 2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, inplace_ctor)
{
    {
        variant<argh, double, int> v1{score::cpp::in_place_type<argh>};
        EXPECT_TRUE(holds_alternative<argh>(v1));
        EXPECT_EQ(0, v1.index());
    }

    expected_argh_counters(1, 0, 0, 0, 0, 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, variant_copy_ctor)
{
    {
        variant<argh, double, int> v1;
        variant<argh, double, int> v2(v1);
    }

    expected_argh_counters(1, 1, 0, 0, 0, 2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, variant_copy_operator)
{
    {
        variant<argh, double, int> v1;
        variant<argh, double, int> v2;
        v1 = v2;
    }

    expected_argh_counters(2, 1, 0, 0, 0, 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, variant_move_ctor)
{
    {
        variant<argh, double, int> v1;
        variant<argh, double, int> v2(std::move(v1));
    }

    expected_argh_counters(1, 0, 0, 1, 0, 2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, variant_move_operator)
{
    {
        variant<argh, double, int> v1;
        variant<argh, double, int> v2;
        v1 = std::move(v2);
    }

    expected_argh_counters(2, 0, 0, 1, 0, 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, copy_operator)
{
    variant<int, double, argh> v1;
    variant<int, double, argh> v2(23.42);

    EXPECT_EQ(0, v1.index());
    EXPECT_EQ(1, v2.index());

    v1 = v2;

    EXPECT_EQ(1, v1.index());
    EXPECT_EQ(1, v2.index());

    v2 = 0;

    EXPECT_EQ(1, v1.index());
    EXPECT_EQ(0, v2.index());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, assign_argh)
{
    variant<int, double, argh> v1;

    expected_argh_counters(0, 0, 0, 0, 0, 0);

    v1 = argh();

    expected_argh_counters(1, 0, 0, 1, 0, 1);

    v1 = 0;

    expected_argh_counters(1, 0, 0, 1, 0, 2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, isa_and_index)
{
    variant<int, double, argh> v1;

    v1 = 23;
    EXPECT_TRUE(holds_alternative<int>(v1));
    EXPECT_EQ(0, v1.index());

    v1 = 47.11;
    EXPECT_TRUE(holds_alternative<double>(v1));
    EXPECT_EQ(1, v1.index());

    v1 = argh();
    EXPECT_TRUE(holds_alternative<argh>(v1));
    EXPECT_EQ(2, v1.index());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17431441
TEST_F(variant_fixture, const_visitor)
{
    variant<int, double, argh> v1;
    variant<int, double, argh> const& cv1 = v1;

    v1 = 23;
    EXPECT_EQ(found_int, visit(int_double_argh_visitor(), cv1));

    v1 = 47.11;
    EXPECT_EQ(found_double, visit(int_double_argh_visitor(), cv1));

    v1 = argh();
    EXPECT_EQ(found_argh, visit(int_double_argh_visitor(), cv1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17431441
TEST_F(variant_fixture, mutable_visitor)
{
    variant<int, double, argh> v1;

    v1 = 23;
    EXPECT_EQ(found_int, visit(int_double_argh_mutable_visitor(), v1));

    v1 = 47.11;
    EXPECT_EQ(found_double, visit(int_double_argh_mutable_visitor(), v1));

    v1 = argh();
    EXPECT_EQ(found_argh, visit(int_double_argh_mutable_visitor(), v1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, placement)
{
    variant<int, double, argh> v1;

    const auto v = v1.emplace<double>(47.11);

    EXPECT_TRUE(holds_alternative<double>(v1));
    EXPECT_EQ(47.11, get<double>(v1));
    EXPECT_EQ(47.11, v);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, const_get)
{
    variant<int, double, argh> v1(23);
    variant<int, double, argh> const& cv1 = v1;

    EXPECT_EQ(23, get<int>(cv1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, mutable_get)
{
    variant<int, double, argh> v1(23);

    get<int>(v1) = 42;

    EXPECT_EQ(42, get<int>(v1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, const_get_if)
{
    const variant<int, double, argh> v1(23);

    EXPECT_EQ(nullptr, get_if<double>(&v1));

    ASSERT_NE(nullptr, get_if<int>(&v1));
    EXPECT_EQ(23, *get_if<int>(&v1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, get_if)
{
    variant<int, double, argh> v1(23);

    EXPECT_EQ(nullptr, get_if<double>(&v1));

    ASSERT_NE(nullptr, get_if<int>(&v1));
    EXPECT_EQ(23, *get_if<int>(&v1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, variant_with_vector)
{
    std::vector<int> vec(1024, 42);
    variant<std::vector<int>, double, int> v1(vec);
    EXPECT_TRUE(holds_alternative<std::vector<int>>(v1));
    EXPECT_EQ(0, v1.index());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17432434
TEST_F(variant_fixture, variant_with_big_object)
{
    typedef variant<int[100 * 100 * 10], double, int> big_variant;
    std::unique_ptr<big_variant> v1(new big_variant);
    EXPECT_TRUE(holds_alternative<int[100 * 100 * 10]>(*v1));
    EXPECT_EQ(0, v1->index());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, sizeof_variant)
{
    typedef variant<int[100 * 100 * 10], double, int> big_variant;
    static_assert(sizeof(big_variant::storage_type) == (sizeof(int) * 100 * 100 * 10), "");
    static_assert(big_variant::storage_size == (sizeof(int) * 100 * 100 * 10), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, alignment)
{
    typedef variant<std::uint8_t, std::uint32_t*, std::uint16_t> variant_a;
    static_assert(variant_a::storage_alignment == sizeof(std::uint32_t*), "");
    typedef variant<std::uint8_t, std::uint16_t, std::uint32_t> variant_b;
    static_assert(variant_b::storage_alignment == 4, "");
    typedef variant<std::uint16_t, std::uint8_t> variant_c;
    static_assert(variant_c::storage_alignment == 2, "");
    typedef variant<std::uint8_t> variant_d;
    static_assert(variant_d::storage_alignment == 1, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, variant_alternatives)
{
    typedef variant<char, short, int, long int, float, double> variant_a;
    static_assert(score::cpp::variant_size<variant_a>::value == 6, "");
    typedef variant<char> variant_b;
    static_assert(score::cpp::variant_size<variant_b>::value == 1, "");
}

struct variant_visitor
{
    variant_visitor(bool value) : toggle(value) {}
    void operator()(int) { toggle = (toggle ? false : true); }
    bool toggle;

private:
    variant_visitor& operator=(const variant_visitor&);
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17431441
TEST_F(variant_fixture, Visit_void_result_type)
{
    variant<int> variant_object;

    variant_visitor visitor(true);
    EXPECT_TRUE(visitor.toggle);
    visit(visitor, variant_object);
    EXPECT_FALSE(visitor.toggle);
    visit(visitor, variant_object);
    EXPECT_TRUE(visitor.toggle);
}

namespace
{
enum constness
{
    const_visitor_const_variant,
    const_visitor_non_const_variant,
    non_const_visitor_const_variant,
    non_const_visitor_non_const_variant
};

struct constness_visitor
{
    constness_visitor() {}
    constness operator()(int const&) const { return const_visitor_const_variant; }
    constness operator()(int&) const { return const_visitor_non_const_variant; }
    constness operator()(int const&) { return non_const_visitor_const_variant; }
    constness operator()(int&) { return non_const_visitor_non_const_variant; }
};
} // namespace

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17431441
TEST_F(variant_fixture, Visit_const_correct_lvalue)
{
    const variant<int> const_variant;
    variant<int> non_const_variant;

    const constness_visitor const_visitor;
    constness_visitor non_const_visitor;

    EXPECT_EQ(const_visitor_const_variant, visit(const_visitor, const_variant));
    EXPECT_EQ(const_visitor_non_const_variant, visit(const_visitor, non_const_variant));
    EXPECT_EQ(non_const_visitor_const_variant, visit(non_const_visitor, const_variant));
    EXPECT_EQ(non_const_visitor_non_const_variant, visit(non_const_visitor, non_const_variant));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17431441
TEST_F(variant_fixture, Visit_const_correct_rvalue)
{
    const variant<int> const_variant;
    variant<int> non_const_variant;

    const constness_visitor const_visitor;
    constness_visitor non_const_visitor;

    EXPECT_EQ(const_visitor_const_variant, visit(std::move(const_visitor), std::move(const_variant)));
    EXPECT_EQ(const_visitor_non_const_variant, visit(std::move(const_visitor), std::move(non_const_variant)));
    EXPECT_EQ(non_const_visitor_const_variant, visit(std::move(non_const_visitor), std::move(const_variant)));
    EXPECT_EQ(non_const_visitor_non_const_variant, visit(std::move(non_const_visitor), std::move(non_const_variant)));
    EXPECT_EQ(non_const_visitor_non_const_variant, visit(constness_visitor(), variant<int>()));
}

class forward_counter
{
    std::int32_t copy_count_;
    std::int32_t move_count_;

public:
    forward_counter() : copy_count_(0), move_count_(0) {}
    forward_counter(const forward_counter& other) : copy_count_(other.copy_count_ + 1), move_count_(other.move_count_)
    {
    }
    forward_counter(forward_counter&& other) : copy_count_(other.copy_count_), move_count_(other.move_count_ + 1) {}

    std::int32_t copy_calls() const { return copy_count_; }
    std::int32_t move_calls() const { return move_count_; }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, perfect_forward_converting_constructor)
{
    forward_counter value;

    variant<forward_counter> fixture_1(std::move(value));
    EXPECT_TRUE(holds_alternative<forward_counter>(fixture_1));
    EXPECT_EQ(1, get<forward_counter>(fixture_1).move_calls());
    EXPECT_EQ(0, get<forward_counter>(fixture_1).copy_calls());

    variant<forward_counter> fixture_2(value);
    EXPECT_TRUE(holds_alternative<forward_counter>(fixture_2));
    EXPECT_EQ(1, get<forward_counter>(fixture_2).copy_calls());
    EXPECT_EQ(0, get<forward_counter>(fixture_2).move_calls());

    variant<forward_counter> fixture_3(forward_counter{});
    EXPECT_TRUE(holds_alternative<forward_counter>(fixture_3));
    EXPECT_EQ(1, get<forward_counter>(fixture_3).move_calls());
    EXPECT_EQ(0, get<forward_counter>(fixture_3).copy_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688
TEST_F(variant_fixture, perfect_forward_converting_assignment)
{
    forward_counter value;

    variant<forward_counter> fixture_1;
    fixture_1 = std::move(value);
    EXPECT_TRUE(holds_alternative<forward_counter>(fixture_1));
    EXPECT_EQ(1, get<forward_counter>(fixture_1).move_calls());
    EXPECT_EQ(0, get<forward_counter>(fixture_1).copy_calls());

    variant<forward_counter> fixture_2;
    fixture_2 = value;
    EXPECT_TRUE(holds_alternative<forward_counter>(fixture_2));
    EXPECT_EQ(1, get<forward_counter>(fixture_2).copy_calls());
    EXPECT_EQ(0, get<forward_counter>(fixture_2).move_calls());

    variant<forward_counter> fixture_3;
    fixture_3 = forward_counter{};
    EXPECT_TRUE(holds_alternative<forward_counter>(fixture_3));
    EXPECT_EQ(1, get<forward_counter>(fixture_3).move_calls());
    EXPECT_EQ(0, get<forward_counter>(fixture_3).copy_calls());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17431468
TEST(variant_equality_operators, equality_checks_for_internal_and_library_type)
{
    using test_variant = variant<int, std::string>;

    const test_variant elem1_1{1};
    const test_variant elem1_2{1};
    const test_variant elem1_3{2};
    const test_variant elem1_4{std::string{"test"}};

    EXPECT_EQ(elem1_1, elem1_2);
    EXPECT_EQ(elem1_1, elem1_1);
    EXPECT_NE(elem1_1, elem1_3);
    EXPECT_FALSE(elem1_1 == elem1_3);
    EXPECT_NE(elem1_1, elem1_4);
    EXPECT_EQ(elem1_4, elem1_4);
}

struct type_with_internal_operator_equals
{
    explicit type_with_internal_operator_equals(std::int32_t n) : i{n} {}
    bool operator==(const type_with_internal_operator_equals& other) const { return i == other.i; }
    bool operator!=(const type_with_internal_operator_equals& other) const { return i != other.i; }
    std::int32_t i;
};

struct type_with_external_operator_equals
{
    explicit type_with_external_operator_equals(std::int32_t n) : i{n} {}
    std::int32_t i;
};
inline bool operator==(const type_with_external_operator_equals& lhs, const type_with_external_operator_equals& rhs)
{
    return lhs.i == rhs.i;
}
inline bool operator!=(const type_with_external_operator_equals& lhs, const type_with_external_operator_equals& rhs)
{
    return lhs.i != rhs.i;
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17431468
TEST(variant_equality_operators, custom_type_with_equal_operator)
{
    using test_variant = variant<type_with_internal_operator_equals, type_with_external_operator_equals>;

    EXPECT_EQ(test_variant{type_with_internal_operator_equals{1}}, test_variant{type_with_internal_operator_equals{1}});
    EXPECT_NE(test_variant{type_with_internal_operator_equals{1}},
              test_variant{type_with_internal_operator_equals{100}});

    EXPECT_EQ(test_variant{type_with_external_operator_equals{1}}, test_variant{type_with_external_operator_equals{1}});
    EXPECT_NE(test_variant{type_with_external_operator_equals{1}},
              test_variant{type_with_external_operator_equals{200}});

    EXPECT_NE(test_variant{type_with_internal_operator_equals{1}}, test_variant{type_with_external_operator_equals{1}});
}

struct type_without_operator_equals
{
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9487688, CB-#17431468
TEST(variant_equality_operators, types_without_equality_operators_still_compile_if_not_called)
{
    using test_variant = variant<type_without_operator_equals>;
    const test_variant test_input{};

    EXPECT_EQ(test_input.index(), 0);
}

} // namespace
