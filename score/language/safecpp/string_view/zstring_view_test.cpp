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
#include "score/language/safecpp/string_view/zstring_view.h"

#include "score/language/safecpp/string_view/details/zspan.h"

#include "score/mw/log/logging.h"

#include <score/string.hpp>

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace score::safecpp
{
namespace
{

using safecpp::literals::operator""_zsv;

TEST(ZStringView, CanConstructFromLiteral)
{
    // Given a `zstring_view` constructed from a string literal
    constexpr auto view = "hello"_zsv;

    // Then it must not be empty
    EXPECT_FALSE(view.empty());

    // And its size must be the length of the string literal minus the null-terminator
    EXPECT_EQ(view.length(), 5U);
    EXPECT_EQ(view.size(), 5U);

    // And its data must point to the start of the string literal
    EXPECT_STREQ(view.c_str(), "hello");
    EXPECT_STREQ(view.data(), "hello");

    // And its underlying character buffer must be null-terminted
    EXPECT_EQ(view.data()[view.size()], '\0');
}

TEST(ZStringView, CanImplicitlyConstructFromLiteral)
{
    // Given a function which expects a `zstring_view` as parameter
    auto test = [](safecpp::zstring_view view) {
        // Then it must not be empty
        EXPECT_FALSE(view.empty());

        // And its size must be the length of the string literal minus the null-terminator
        EXPECT_EQ(view.length(), 5U);
        EXPECT_EQ(view.size(), 5U);

        // And its data must point to the start of the string literal
        EXPECT_STREQ(view.c_str(), "hello");
        EXPECT_STREQ(view.data(), "hello");

        // And its underlying character buffer must be null-terminted
        EXPECT_EQ(view.data()[view.size()], '\0');
    };

    // When calling the above lambda with a string literal
    // Then its contained expectations must be fulfilled
    test("hello");
}

TEST(ZStringView, CanConstructFromNullTerminatedBuffer)
{
    // Given a null-terminated character buffer
    char buffer[] = {'h', 'e', 'l', 'l', 'o', '\0'};

    // When constructing a `zstring_view` from it
    safecpp::zstring_view view{buffer, sizeof(buffer)};

    // Then it must not be empty
    EXPECT_FALSE(view.empty());

    // And its size must be the length of the string literal minus the null-terminator
    EXPECT_EQ(view.length(), 5U);
    EXPECT_EQ(view.size(), 5U);

    // And its data must match the buffer's data
    EXPECT_STREQ(view.data(), "hello");
    EXPECT_EQ(view.front(), 'h');
    EXPECT_EQ(view.back(), 'o');
    EXPECT_EQ(view.at(1), 'e');
    EXPECT_EQ(view[2], 'l');
    EXPECT_EQ(view[3], 'l');

    // And its underlying character buffer must be null-terminted
    EXPECT_EQ(view.data()[view.size()], '\0');
}

TEST(ZStringView, AbortsWhenConstructingFromNonNullTerminatedBuffer)
{
    // Given a non-null-terminated character buffer
    char buffer[] = {'h', 'e', 'l', 'l', 'o'};

    // When constructing a `zstring_view` from it
    // Then immediate termination of the program is expected due to the missing null-termination
    EXPECT_EXIT(score::cpp::ignore = safecpp::zstring_view{buffer}, ::testing::KilledBySignal{SIGABRT}, "");
}

TEST(ZStringView, CanConstructFromAmpString)
{
    // Given an `score::cpp::pmr::string`
    score::cpp::pmr::string str{"hello"};

    // When constructing a `zstring_view` from it
    safecpp::zstring_view view = str;

    // Then it must not be empty
    EXPECT_FALSE(view.empty());

    // And its size must be the length of the `score::cpp::pmr::string`
    EXPECT_EQ(view.length(), 5U);
    EXPECT_EQ(view.size(), 5U);

    // And its data must match the `score::cpp::pmr::string`'s data
    EXPECT_STREQ(view.data(), "hello");
    EXPECT_EQ(view.front(), 'h');
    EXPECT_EQ(view.back(), 'o');
    EXPECT_EQ(view.at(1), 'e');
    EXPECT_EQ(view[2], 'l');
    EXPECT_EQ(view[3], 'l');

    // And its underlying character buffer must be null-terminted
    EXPECT_EQ(view.data()[view.size()], '\0');

    // When constructing a `zstring_view` from an empty `score::cpp::pmr::string`
    score::cpp::pmr::string empty_str{};
    safecpp::zstring_view empty_view = empty_str;

    // Then it must be empty
    EXPECT_TRUE(empty_view.empty());
    EXPECT_EQ(empty_view.size(), 0U);

    // But it must nonetheless reference the `score::cpp::pmr::string`'s (empty) underlying character buffer
    EXPECT_NE(empty_view.data(), nullptr);
}

TEST(ZStringView, CanConstructFromStdString)
{
    // Given an `std::string`
    std::string str{"hello"};

    // When constructing a `zstring_view` from it
    safecpp::zstring_view view = str;

    // Then it must not be empty
    EXPECT_FALSE(view.empty());

    // And its size must be the length of the `std::string`
    EXPECT_EQ(view.length(), 5U);
    EXPECT_EQ(view.size(), 5U);

    // And its data must match the `std::string`'s data
    EXPECT_STREQ(view.data(), "hello");
    EXPECT_EQ(view.front(), 'h');
    EXPECT_EQ(view.back(), 'o');
    EXPECT_EQ(view.at(1), 'e');
    EXPECT_EQ(view[2], 'l');
    EXPECT_EQ(view[3], 'l');

    // When constructing a `zstring_view` from an empty `std::string`
    std::string empty_str{};
    safecpp::zstring_view empty_view = empty_str;

    // Then it must be empty
    EXPECT_TRUE(empty_view.empty());
    EXPECT_EQ(empty_view.size(), 0U);

    // But it must nonetheless reference the `std::string`'s (empty) underlying character buffer
    EXPECT_NE(empty_view.data(), nullptr);
}

TEST(ZStringView, CanConstructFromZSpan)
{
    // Given a `zspan` constructed from a null-terminated character buffer
    details::zspan<const char> span{"hello"};

    // When constructing a `zstring_view` from it
    const safecpp::zstring_view view{span};

    // Then it must not be empty
    EXPECT_FALSE(view.empty());

    // And its size must be the length of the string literal minus the null-terminator
    EXPECT_EQ(view.length(), 5U);
    EXPECT_EQ(view.size(), 5U);

    // And its data must match the `zspan`'s one
    EXPECT_STREQ(view.c_str(), "hello");
    EXPECT_STREQ(view.data(), "hello");
    EXPECT_EQ(view.front(), 'h');
    EXPECT_EQ(view.back(), 'o');
    EXPECT_EQ(view.at(1), 'e');
    EXPECT_EQ(view[2], 'l');
    EXPECT_EQ(view[3], 'l');

    // And its underlying character buffer must be null-terminted
    EXPECT_EQ(view.data()[view.size()], '\0');
}

TEST(ZStringView, CanDefaultConstruct)
{
    // Given a default-constructed `zstring_view`
    zstring_view view{};

    // Then it must be empty
    EXPECT_TRUE(view.empty());

    // And its size must be zero
    EXPECT_EQ(view.length(), 0U);
    EXPECT_EQ(view.size(), 0U);

    // And its data must be nullptr
    EXPECT_EQ(view.data(), nullptr);
}

TEST(ZStringView, CanCopyConstruct)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When copy-constructing it based on the above one
    zstring_view copied{view};

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original view's size
    EXPECT_EQ(copied.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(copied.data(), view.data());
    EXPECT_STREQ(copied.data(), view.data());
}

TEST(ZStringView, CanMoveConstruct)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When move-constructing it based on the above one
    zstring_view moved{std::move(view)};

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original view's size
    EXPECT_EQ(moved.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(moved.data(), view.data());
    EXPECT_STREQ(moved.data(), view.data());
}

TEST(ZStringView, CanCopyAssign)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When copy-assigning it to another one
    zstring_view copied;
    copied = view;

    // Then it must not be empty
    EXPECT_FALSE(copied.empty());

    // And its size must match the original view's size
    EXPECT_EQ(copied.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(copied.data(), view.data());
    EXPECT_STREQ(copied.data(), view.data());
}

TEST(ZStringView, CanMoveAssign)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When move-assigning it to another one
    zstring_view moved;
    moved = std::move(view);

    // Then it must not be empty
    EXPECT_FALSE(moved.empty());

    // And its size must match the original view's size
    EXPECT_EQ(moved.size(), view.size());

    // And its data must match the original view's data
    EXPECT_EQ(moved.data(), view.data());
    EXPECT_STREQ(moved.data(), view.data());
}

TEST(ZStringView, CanConvertToLogString)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // Then it must be usable in conjunction with mw::log's `LogStream`
    mw::log::LogDebug() << view;
}

TEST(ZStringView, CanConvertToStdStringView)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // Then it must be implicitly convertible to `std::string_view`
    std::string_view sv = view;
    EXPECT_FALSE(sv.empty());
    EXPECT_EQ(sv.size(), view.size());
    EXPECT_EQ(sv.data(), view.data());
    EXPECT_STREQ(sv.data(), view.data());

    // And it must also be explicitly convertible to `std::string`
    auto str = std::string{view};
    EXPECT_FALSE(str.empty());
    EXPECT_EQ(str.size(), view.size());
    EXPECT_STREQ(str.data(), view.data());
}

TEST(ZStringView, CanCompareWithStringView)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // When attempting, then equality checks with `std::string_view` must be possible
    EXPECT_TRUE(view == std::string_view{"hello"});
    EXPECT_FALSE(view != std::string_view{"hello"});
    EXPECT_TRUE(view != std::string_view{"hello world"});
    EXPECT_FALSE(view == std::string_view{"hello world"});

    // When attempting, then comparison checks with `std::string_view` must be possible
    EXPECT_TRUE(view > std::string_view{"hell"});
    EXPECT_FALSE(view < std::string_view{"hello"});
    EXPECT_TRUE(view <= std::string_view{"hello"});
    EXPECT_FALSE(view > std::string_view{"hello world"});
    EXPECT_FALSE(view >= std::string_view{"hello world"});
}

TEST(ZStringView, CanOutputToOStream)
{
    // Given a preconstructed `zstring_view`
    auto view = "hello"_zsv;

    // Then it must be possible to output it to an std::ostream
    std::ostringstream oss;
    oss << view;
    EXPECT_EQ(oss.str(), "hello");

    // And it must also be possible to output an in-between null-terminated `zstring_view` correctly
    auto other_view = "hello\0world"_zsv;
    oss = std::ostringstream{};
    oss << other_view;
    EXPECT_EQ(oss.str(), (std::string{"hello\0world", 11U}));
}

TEST(ZStringView, CanSwap)
{
    // Given two preconstructed `zstring_view`s
    auto view = "got swapped"_zsv;
    auto other_view = "this view"_zsv;

    // Then it must be possible to swap them
    swap(view, other_view);
    EXPECT_EQ(view.size(), 9U);
    EXPECT_EQ(other_view.size(), 11U);
    EXPECT_STREQ(view.data(), "this view");
    EXPECT_STREQ(other_view.data(), "got swapped");
}

TEST(ZStringView, TypeTraits)
{
    EXPECT_TRUE((std::is_nothrow_default_constructible_v<safecpp::zstring_view>));
    EXPECT_TRUE((std::is_trivially_copy_constructible_v<safecpp::zstring_view>));
    EXPECT_TRUE((std::is_trivially_move_constructible_v<safecpp::zstring_view>));
    EXPECT_TRUE((std::is_trivially_destructible_v<safecpp::zstring_view>));
}

}  // namespace
}  // namespace score::safecpp
