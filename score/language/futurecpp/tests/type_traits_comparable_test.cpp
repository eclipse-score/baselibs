/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#include <score/type_traits.hpp>
#include <score/type_traits.hpp> // test include guard

#include <gtest/gtest.h>

namespace
{

class test_less_than_comparable_1
{
public:
    test_less_than_comparable_1() = delete;
    test_less_than_comparable_1(int j) : i_(j) {}

    bool operator<(const test_less_than_comparable_1& rhs) const { return (i_ < rhs.i_); }

private:
    int i_{};
};

enum class test_less_than_comparable_2
{
    test,
    test2
};

enum test_less_than_comparable_3
{
    test_less_than_comparable_3_test,
    test_less_than_comparable_3_test2
};

using test_less_than_comparable_4 = std::pair<test_less_than_comparable_1, test_less_than_comparable_1>;
using test_less_than_comparable_5 = std::tuple<test_less_than_comparable_1, test_less_than_comparable_1>;

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770042
TEST(is_less_than_comparable, when_less_than_comparable)
{
    static_assert(score::cpp::is_less_than_comparable_v<test_less_than_comparable_1>,
                  "Error: the given data type should be LessThanComparable.");
    static_assert(score::cpp::is_less_than_comparable_v<test_less_than_comparable_2>,
                  "Error: the given data type should be LessThanComparable.");
    static_assert(score::cpp::is_less_than_comparable_v<test_less_than_comparable_3>,
                  "Error: the given data type should be LessThanComparable.");
    static_assert(score::cpp::is_less_than_comparable_v<test_less_than_comparable_4>,
                  "Error: the given data type should be LessThanComparable.");
    static_assert(score::cpp::is_less_than_comparable_v<test_less_than_comparable_5>,
                  "Error: the given data type should be LessThanComparable.");
    static_assert(score::cpp::is_less_than_comparable_v<double>, "Error: the given data type should be LessThanComparable.");
}

class test_not_less_than_comparable
{
public:
    bool operator<(const test_not_less_than_comparable& rhs) = delete;
    bool operator<(test_not_less_than_comparable& rhs) = delete;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770042
TEST(is_less_than_comparable, when_not_less_than_comparable)
{
    static_assert(!score::cpp::is_less_than_comparable_v<test_not_less_than_comparable>,
                  "Error: the given data type should NOT be LessThanComparable.");
}

class test_less_than_or_equal_to_comparable_1
{
public:
    test_less_than_or_equal_to_comparable_1() = delete;
    test_less_than_or_equal_to_comparable_1(int j) : i_(j) {}

    bool operator<=(const test_less_than_or_equal_to_comparable_1& rhs) const { return (i_ <= rhs.i_); }

private:
    int i_{};
};

enum class test_less_than_or_equal_to_comparable_2
{
    test,
    test2
};

enum test_less_than_or_equal_to_comparable_3
{
    test_less_than_or_equal_to_comparable_3_test,
    test_less_than_or_equal_to_comparable_3_test2
};

class test_less_than_or_equal_to_comparable_pair_element
{
public:
    test_less_than_or_equal_to_comparable_pair_element() = delete;
    test_less_than_or_equal_to_comparable_pair_element(int j) : i_(j) {}

    bool operator<(const test_less_than_or_equal_to_comparable_pair_element& rhs) const { return (i_ < rhs.i_); }
    bool operator<=(const test_less_than_or_equal_to_comparable_pair_element& rhs) const { return (i_ <= rhs.i_); }

private:
    int i_{};
};

using test_less_than_or_equal_to_comparable_4 =
    std::pair<test_less_than_or_equal_to_comparable_pair_element, test_less_than_or_equal_to_comparable_pair_element>;
using test_less_than_or_equal_to_comparable_5 =
    std::tuple<test_less_than_or_equal_to_comparable_pair_element, test_less_than_or_equal_to_comparable_pair_element>;

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770042
TEST(is_less_than_or_equal_to_comparable, when_less_than_or_equal_to_comparable)
{
    static_assert(score::cpp::is_less_than_or_equal_to_comparable_v<test_less_than_or_equal_to_comparable_1>,
                  "Error: the given data type should be LessThanOrEqualToComparable.");
    static_assert(score::cpp::is_less_than_or_equal_to_comparable_v<test_less_than_or_equal_to_comparable_2>,
                  "Error: the given data type should be LessThanOrEqualToComparable.");
    static_assert(score::cpp::is_less_than_or_equal_to_comparable_v<test_less_than_or_equal_to_comparable_3>,
                  "Error: the given data type should be LessThanOrEqualToComparable.");
    static_assert(score::cpp::is_less_than_or_equal_to_comparable_v<test_less_than_or_equal_to_comparable_4>,
                  "Error: the given data type should be LessThanOrEqualToComparable.");
    static_assert(score::cpp::is_less_than_or_equal_to_comparable_v<test_less_than_or_equal_to_comparable_5>,
                  "Error: the given data type should be LessThanOrEqualToComparable.");
    static_assert(score::cpp::is_less_than_or_equal_to_comparable_v<double>,
                  "Error: the given data type should be LessThanOrEqualToComparable.");
}

class test_not_less_than_or_equal_to_comparable
{
public:
    bool operator<=(const test_not_less_than_or_equal_to_comparable& rhs) = delete;
    bool operator<=(test_not_less_than_or_equal_to_comparable& rhs) = delete;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17770042
TEST(is_less_than_or_equal_to_comparable, when_not_less_than_or_equal_to_comparable)
{
    static_assert(!score::cpp::is_less_than_comparable_v<test_not_less_than_or_equal_to_comparable>,
                  "Error: the given data type should NOT be LessThanOrEqualToComparable.");
}

} // namespace
