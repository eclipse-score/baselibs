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
#include "score/language/safecpp/string_view/null_termination_violation_policies.h"

#include "score/language/safecpp/string_view/details/zspan.h"
#include "score/language/safecpp/string_view/zstring_view.h"

#include <score/string.hpp>
#include <score/utility.hpp>

#include <gtest/gtest.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace score::safecpp::details
{
namespace
{

constexpr const char kNullTerminatedCharArray[] = "hello";
constexpr zspan<const char> kSpan{kNullTerminatedCharArray};
constexpr zspan<const char> kOtherSpan{"hello world"};

using safecpp::literals::operator""_zsv;

TEST(ZSpan, CanAssignElements)
{
    // Given a non-const `zspan` to a modifiable range
    char buffer[] = "hello world";
    zspan<char> span{buffer};

    // When accessing elements
    // Then it must work as expected
    EXPECT_EQ(span[0], 'h');
    EXPECT_EQ(span[1], 'e');
    EXPECT_EQ(span[2], 'l');
    EXPECT_EQ(span[3], 'l');
    EXPECT_EQ(span[4], 'o');
    EXPECT_EQ(span[5], ' ');
    EXPECT_EQ(span[6], 'w');
    EXPECT_EQ(span[7], 'o');
    EXPECT_EQ(span[8], 'r');
    EXPECT_EQ(span[9], 'l');
    EXPECT_EQ(span[10], 'd');

    // When accessing an element out of the `zspan`'s range
    // Then immediate termination is expected when invoking `operator[]`
    ASSERT_EQ(span.size(), 11U);
    EXPECT_EXIT((void)span[11], ::testing::KilledBySignal{SIGABRT}, "");

    // Whereas `std::out_of_range` is expected when invoking `at()`
    ASSERT_EQ(span.size(), 11U);
    EXPECT_THROW((void)span.at(11), std::out_of_range);

    // When modifying some of the `zspan`'s elements
    span.front() = 'f';
    span[6] = 'f';
    span[7] = 'o';
    span[8] = 'l';
    span[9] = 'k';
    span.back() = 's';

    // Then the `zspan`'s underlying buffer must have gotten modified
    EXPECT_STREQ(buffer, "fello folks");

    // When modifying one of the `zspan`'s elements with another one
    span[0] = span[9];

    // Then the `zspan`'s underlying buffer must have gotten modified accordingly
    EXPECT_STREQ(buffer, "kello folks");

    // When modifying one of the `zspan`'s elements with itself (self assignment)
    span[1] = span[1];

    // Then the `zspan`'s underlying buffer must remain unchanged
    EXPECT_STREQ(buffer, "kello folks");

    // When swapping two of the `zspan`'s elements
    swap(span[1], span[7]);

    // Then the `zspan`'s underlying buffer must have gotten modified accordingly
    EXPECT_STREQ(buffer, "kollo felks");
}

TEST(ZSpan, CanAccessUnderlyingSequenceOnlyViaPointerToConst)
{
    // Given a non-const `zspan` to a modifiable range
    char buffer[] = "hello world";
    zspan<char> span{buffer};

    // When requesting a pointer to the `zspan`'s underlying sequence
    // Then a pointer to const is expected to get returned.
    // Reason is that the underlying sequence shall not be modifiable via raw pointer.
    ASSERT_TRUE((std::is_pointer_v<decltype(span.data())>));
    ASSERT_TRUE((std::is_const_v<std::remove_pointer_t<decltype(span.data())>>));
}

TEST(ZSpan, CanConstructConstexpr)
{
    // Given a `zspan`s constructed from a constexpr buffer
    // see declarations at the top of this file

    // Then they must not be empty
    EXPECT_FALSE(kSpan.empty());
    EXPECT_FALSE(kOtherSpan.empty());

    // And their size must be the length of the respective string literal minus the null-terminator
    EXPECT_EQ(kSpan.size(), 5U);
    EXPECT_EQ(kOtherSpan.size(), 11U);

    // And their data must point to the start of the respective string literal
    EXPECT_STREQ(kSpan.data(), "hello");
    EXPECT_STREQ(kOtherSpan.data(), "hello world");

    // And their underlying character buffers must be null-terminated
    EXPECT_EQ(kSpan.data()[kSpan.size()], '\0');
    EXPECT_EQ(kOtherSpan.data()[kOtherSpan.size()], '\0');

    // And accessing their elements via operator[] in a constexpr context must work as expected
    constexpr auto element = kSpan[1];
    constexpr auto other_element = kOtherSpan[8];
    EXPECT_EQ(element, 'e');
    EXPECT_EQ(other_element, 'r');
}

TEST(ZSpan, CanConstructFromBuffer)
{
    // Given a null-terminated character buffer
    char buffer[] = {'h', 'e', 'l', 'l', 'o', '\0'};

    // When constructing a `zspan` from it
    zspan<char> span{buffer, sizeof(buffer)};

    // Then it must not be empty
    EXPECT_FALSE(span.empty());

    // And its size must be the length of the buffer minus the null-terminator
    EXPECT_EQ(span.size(), 5U);

    // And its data must match the buffer's data
    EXPECT_STREQ(span.data(), "hello");
    EXPECT_EQ(span.front(), 'h');
    EXPECT_EQ(span.back(), 'o');

    // When constructing a `zspan` from it in conjunction with a size of zero & the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW(
        (span =
             zspan<char>{
                 buffer, 0U, safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
        std::invalid_argument);

    // When constructing a `zspan` from a nullptr in conjunction with the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW(
        (span = zspan<char>{nullptr,
                            sizeof(buffer),
                            safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
        std::invalid_argument);

    // Given a non-null-terminated character buffer
    char invalid_buffer[] = {'h', 'e', 'l', 'l', 'o'};

    // When constructing a `zspan` from it in conjunction with the `set_empty` violation policy
    span =
        zspan<char>{invalid_buffer, sizeof(invalid_buffer), safecpp::null_termination_violation_policies::set_empty{}};

    // Then it must be empty
    EXPECT_TRUE(span.empty());
    EXPECT_EQ(span.size(), 0U);
    EXPECT_EQ(span.data(), nullptr);

    // When constructing a `zspan` from it in conjunction with the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW(
        (span = zspan<char>{invalid_buffer,
                            sizeof(invalid_buffer),
                            safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
        std::invalid_argument);
}

TEST(ZSpan, CanConstructFromRange)
{
    // Given a null-terminated character range
    char range[] = {'h', 'e', 'l', 'l', 'o', '\0'};

    // When constructing a `zspan` from it
    zspan<char> span{range};

    // Then it must not be empty
    EXPECT_FALSE(span.empty());

    // And its size must be the length of the range minus the null-terminator
    EXPECT_EQ(span.size(), 5U);

    // And its data must match the buffer's data
    EXPECT_STREQ(span.data(), "hello");
    EXPECT_EQ(span.front(), 'h');
    EXPECT_EQ(span.back(), 'o');

    // Given a non-null-terminated character range
    char invalid_range[] = {'h', 'e', 'l', 'l', 'o'};

    // When constructing a `zspan` from it in conjunction with the `set_empty` violation policy
    span = zspan<char>{invalid_range, safecpp::null_termination_violation_policies::set_empty{}};

    // Then it must be empty
    EXPECT_TRUE(span.empty());
    EXPECT_EQ(span.size(), 0U);
    EXPECT_EQ(span.data(), nullptr);

    // When constructing a `zspan` from it in conjunction with the `throw_exception` violation policy
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THROW(
        (span = zspan<char>{invalid_range,
                            safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}}),
        std::invalid_argument);
}

TEST(ZSpan, CanConstructFromAmpString)
{
    // Given a preconstructed `std::string`
    score::cpp::pmr::string str{"hello"};

    // When constructing it based on the above one
    zspan<const char> span{str};

    // Then it must have worked
    ASSERT_FALSE(span.empty());
    EXPECT_EQ(span.size(), 5U);
    EXPECT_STREQ(span.data(), "hello");
}

TEST(ZSpan, CanConstructFromStdString)
{
    // Given a preconstructed `std::string`
    std::string str{"hello"};

    // When constructing it based on the above one
    zspan<const char> span{str};

    // Then it must have worked
    ASSERT_FALSE(span.empty());
    EXPECT_EQ(span.size(), 5U);
    EXPECT_STREQ(span.data(), "hello");
}

TEST(ZSpan, CanConstructFromZStringView)
{
    // Given a preconstructed `zstring_view`
    constexpr static auto view = "hello"_zsv;

    // When constructing it based on the above one
    constexpr zspan<const char> span = view;

    // Then it must have worked
    ASSERT_FALSE(span.empty());
    EXPECT_EQ(span.size(), 5U);
    EXPECT_STREQ(span.data(), "hello");
}

TEST(ZSpan, CanConstructFromOtherZSpan)
{
    // Given an empty preconstructed `zspan`
    zspan<char> empty_span{};

    // When constructing a `zspan` based on the above one
    const zspan<const char> copied_empty{empty_span};

    // Then it must have worked
    EXPECT_TRUE(copied_empty.empty());
    EXPECT_EQ(copied_empty.data(), nullptr);

    // Given an non-empty preconstructed `zspan`
    char range[] = {'h', 'e', 'l', 'l', 'o', '\0'};
    zspan<char> span{range};

    // When constructing a `zspan` based on the above one
    zspan<const char> copied{span};

    // Then it must have worked correctly
    EXPECT_FALSE(copied.empty());
    EXPECT_EQ(copied.data(), std::data(range));
    EXPECT_STREQ(copied.data(), "hello");
    EXPECT_EQ(copied.front(), 'h');
    EXPECT_EQ(copied.back(), 'o');
    EXPECT_EQ(copied.at(1), 'e');
    EXPECT_EQ(copied[2], 'l');
    EXPECT_EQ(copied[3], 'l');
}

TEST(ZSpan, CanDefaultConstruct)
{
    // Given a default-constructed `zspan`
    zspan<char> span{};

    // Then it must be empty
    EXPECT_TRUE(span.empty());

    // And its size must be zero
    EXPECT_EQ(span.size(), 0U);

    // And its data must be nullptr
    EXPECT_EQ(span.data(), nullptr);
}

TEST(ZSpan, CanCopyConstruct)
{
    // Given a preconstructed `zspan`
    zspan<const char> span{"hello"};

    // When copy-constructing it based on the above one
    zspan<const char> copied{span};

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original span's size
    EXPECT_EQ(copied.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(copied.data(), span.data());
    EXPECT_STREQ(copied.data(), span.data());
}

TEST(ZSpan, CanMoveConstruct)
{
    // Given a preconstructed `zspan`
    zspan<const char> span{"hello"};

    // When move-constructing it based on the above one
    zspan<const char> moved{std::move(span)};

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original span's size
    EXPECT_EQ(moved.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(moved.data(), span.data());
    EXPECT_STREQ(moved.data(), span.data());
}

TEST(ZSpan, CanCopyAssign)
{
    // Given a preconstructed `zspan`
    zspan<const char> span{"hello"};

    // When copy-assigning it to another one
    zspan<const char> copied;
    copied = span;

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original span's size
    EXPECT_EQ(copied.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(copied.data(), span.data());
    EXPECT_STREQ(copied.data(), span.data());
}

TEST(ZSpan, CanMoveAssign)
{
    // Given a preconstructed `zspan`
    zspan<const char> span{"hello"};

    // When move-assigning it to another one
    zspan<const char> moved;
    moved = std::move(span);

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original span's size
    EXPECT_EQ(moved.size(), span.size());

    // And its data must match the original span's data
    EXPECT_EQ(moved.data(), span.data());
    EXPECT_STREQ(moved.data(), span.data());
}

TEST(ZSpan, TypeTraits)
{
    using zspan = zspan<char>;
    EXPECT_TRUE((std::is_nothrow_default_constructible_v<zspan>));
    EXPECT_TRUE((std::is_trivially_copy_constructible_v<zspan>));
    EXPECT_TRUE((std::is_trivially_move_constructible_v<zspan>));
    EXPECT_TRUE((std::is_trivially_destructible_v<zspan>));
}

TEST(ZSpan, FrontInvokedWhileViolatingPrecondition)
{
    // Given a default-constructed modifiable `zspan`
    zspan<char> span{};

    // When invoking `front()`
    // Then immediate termination is expected due to precondition violation
    EXPECT_EXIT(score::cpp::ignore = span.front(), ::testing::KilledBySignal{SIGABRT}, "");

    // Given a default-constructed non-modifiable `zspan`
    const zspan<char> const_span{};

    // When invoking `front()`
    // Then immediate termination is expected due to precondition violation
    EXPECT_EXIT(score::cpp::ignore = const_span.front(), ::testing::KilledBySignal{SIGABRT}, "");
}

TEST(ZSpan, BackInvokedWhileViolatingPrecondition)
{
    // Given a default-constructed modifiable `zspan`
    zspan<char> span{};

    // When invoking `back()`
    // Then immediate termination is expected due to precondition violation
    EXPECT_EXIT(score::cpp::ignore = span.back(), ::testing::KilledBySignal{SIGABRT}, "");

    // Given a default-constructed non-modifiable `zspan`
    const zspan<char> const_span{};

    // When invoking `back()`
    // Then immediate termination is expected due to precondition violation
    EXPECT_EXIT(score::cpp::ignore = const_span.back(), ::testing::KilledBySignal{SIGABRT}, "");
}

TEST(ZSpan, AtInvokedWhileViolatingPrecondition)
{
    // Given a default-constructed modifiable `zspan`
    zspan<char> span{};

    // When invoking `at()`
    // Then immediate termination is expected due to precondition violation
    EXPECT_THROW(score::cpp::ignore = span.at(0), std::out_of_range);

    // Given a default-constructed non-modifiable `zspan`
    const zspan<char> const_span{};

    // When invoking `at()`
    // Then immediate termination is expected due to precondition violation
    EXPECT_THROW(score::cpp::ignore = const_span.at(0), std::out_of_range);
}

TEST(ZSpan, SubscriptOperatorInvokedWhileViolatingPrecondition)
{
    // Given a default-constructed modifiable `zspan`
    zspan<char> span{};

    // When invoking `operator[]`
    // Then immediate termination is expected due to precondition violation
    EXPECT_EXIT(score::cpp::ignore = span[0], ::testing::KilledBySignal{SIGABRT}, "");

    // Given a default-constructed non-modifiable `zspan`
    const zspan<char> const_span{};

    // When invoking `operator[]`
    // Then immediate termination is expected due to precondition violation
    EXPECT_EXIT(score::cpp::ignore = const_span[0], ::testing::KilledBySignal{SIGABRT}, "");
}

}  // namespace
}  // namespace score::safecpp::details
