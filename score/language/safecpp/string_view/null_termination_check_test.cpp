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
#include "score/language/safecpp/string_view/null_termination_check.h"
#include "score/language/safecpp/string_view/null_termination_violation_policies.h"

#include "score/language/safecpp/string_view/details/zspan.h"
#include "score/language/safecpp/string_view/zstring_view.h"

#include <score/span.hpp>
#include <score/string.hpp>
#include <score/string_view.hpp>
#include <score/utility.hpp>

#include <gtest/gtest.h>

#include <csignal>
#include <cstddef>
#include <cstdint>
#if defined(__has_include) && __has_include(<memory_resource>)
#include <memory_resource>
#endif
#if defined(__has_include) && __has_include(<span>)
#include <span>
#endif
#include <stdexcept>
#include <string>
#include <string_view>

namespace score
{
namespace
{

using safecpp::literals::operator""_zsv;

TEST(IsNullTerminatedViewType, CheckTypes)
{
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<std::string>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const std::string>());

#if defined(__cpp_lib_polymorphic_allocator)
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<std::pmr::string>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const std::pmr::string>());
#endif

    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<score::cpp::pmr::string>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const score::cpp::pmr::string>());

    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::zstring_view>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::zstring_view>());

    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<unsigned char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<std::uint8_t>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<std::byte>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<const char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<const unsigned char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<const std::uint8_t>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<safecpp::details::zspan<const std::byte>>());

    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<unsigned char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<std::uint8_t>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<std::byte>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<const char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<const unsigned char>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<const std::uint8_t>>());
    EXPECT_TRUE(safecpp::IsNullTerminatedViewType<const safecpp::details::zspan<const std::byte>>());

    EXPECT_FALSE(safecpp::IsNullTerminatedViewType<std::string_view>());
    EXPECT_FALSE(safecpp::IsNullTerminatedViewType<const std::string_view>());

    EXPECT_FALSE(safecpp::IsNullTerminatedViewType<score::cpp::string_view>());
    EXPECT_FALSE(safecpp::IsNullTerminatedViewType<const score::cpp::string_view>());
}

TEST(NullTerminationCheck, ForDefaultAmpStringView)
{
    score::cpp::string_view view{};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

TEST(NullTerminationCheck, ForEmptyAmpStringView)
{
    score::cpp::string_view view{""};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TEST(NullTerminationCheck, ForNonEmptyAmpStringView)
{
    score::cpp::string_view view{"hello"};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TEST(NullTerminationCheck, ForNonNullTerminatedAmpStringView)
{
    char buffer[] = {'h', 'e', 'l', 'l', 'o'};  // not null-terminated
    score::cpp::string_view view{buffer, sizeof(buffer)};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TEST(NullTerminationCheck, ForDefaultStdStringView)
{
    std::string_view view{};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

TEST(NullTerminationCheck, ForEmptyStdStringView)
{
    std::string_view view{""};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TEST(NullTerminationCheck, ForNonEmptyStdStringView)
{
    std::string_view view{"hello"};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TEST(NullTerminationCheck, ForNonNullTerminatedStdStringView)
{
    char buffer[] = {'h', 'e', 'l', 'l', 'o'};  // not null-terminated
    std::string_view view{buffer, sizeof(buffer)};
    // works just for now, is expected to report a violation once Ticket-214240 got resolved
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TEST(NullTerminationCheck, ForDefaultStdString)
{
    std::string str{};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(str), str.data());

    EXPECT_NE(nullptr, safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(std::string{}));
}

TEST(NullTerminationCheck, ForEmptyStdString)
{
    std::string str{""};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(str), str.data());

    EXPECT_NE(nullptr, safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(std::string{""}));
}

TEST(NullTerminationCheck, ForNonEmptyStdString)
{
    std::string str{"hello"};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(str), str.data());

    const std::string const_str{"hello"};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(const_str), const_str.data());
}

TEST(NullTerminationCheck, ForEmptyZStringView)
{
    EXPECT_EXIT(score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(safecpp::zstring_view{}),
                ::testing::KilledBySignal{SIGABRT},
                "");
}

TEST(NullTerminationCheck, ForNonEmptyZStringView)
{
    auto view = "hello"_zsv;
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TEST(NullTerminationCheck, ForEmptyZSpan)
{
    EXPECT_EXIT(score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(safecpp::details::zspan<char>{}),
                ::testing::KilledBySignal{SIGABRT},
                "");

    EXPECT_EXIT(score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(safecpp::details::zspan<const char>{}),
                ::testing::KilledBySignal{SIGABRT},
                "");
}

TEST(NullTerminationCheck, ForNonEmptyZSpan)
{
    char buffer[] = "hello";
    safecpp::details::zspan<char> span{buffer, sizeof(buffer)};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(span), span.data());

    safecpp::details::zspan<const char> span_const = span;
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(span_const), span_const.data());

    const safecpp::details::zspan<char> const_span = span;
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(const_span), const_span.data());

    const safecpp::details::zspan<const char> const_span_const = span;
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(span_const), const_span_const.data());
}

TEST(NullTerminationCheck, ViolationPolicies)
{
    auto perform_test = [](auto violating_view) {
        // When used in conjunction with `abort` policy, immediate termination is expected
        EXPECT_EXIT(score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(
                        violating_view, safecpp::null_termination_violation_policies::abort{}),
                    ::testing::KilledBySignal{SIGABRT},
                    "");

        // When used in conjunction with `set_empty` policy, nullptr is expected as result
        EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(
                      violating_view, safecpp::null_termination_violation_policies::set_empty{}),
                  nullptr);

        // When used in conjunction with `throw_exception` policy, the specified exception type must get thrown
        EXPECT_THROW(
            score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(
                violating_view, safecpp::null_termination_violation_policies::throw_exception<std::invalid_argument>{}),
            std::invalid_argument);

        // When used in conjunction with `throw_exception` policy, the specified exception type must get thrown
        EXPECT_THROW(
            score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(
                violating_view, safecpp::null_termination_violation_policies::throw_exception<std::out_of_range>{}),
            std::out_of_range);
    };

    auto test_with = [&perform_test](auto view_type_instance) {
        char buffer[] = {'h', 'e', 'l', 'l', 'o'};  // not null-terminated

        decltype(view_type_instance) violating_view{buffer, sizeof(buffer)};
        ASSERT_NE(nullptr, violating_view.data());
        ASSERT_FALSE(violating_view.empty());
        perform_test(violating_view);

        decltype(view_type_instance) empty_view{buffer, 0U};
        ASSERT_NE(nullptr, empty_view.data());
        ASSERT_TRUE(empty_view.empty());
        perform_test(empty_view);

        decltype(view_type_instance) null_view{};
        ASSERT_EQ(nullptr, null_view.data());
        perform_test(null_view);
    };

    test_with(score::cpp::span<char>{});
    test_with(score::cpp::span<const char>{});
    test_with(std::add_const_t<score::cpp::span<char>>{});
    test_with(std::add_const_t<score::cpp::span<const char>>{});

#if defined(__cpp_lib_span)
    test_with(std::span<char>{});
    test_with(std::span<const char>{});
    test_with(std::add_const_t<std::span<char>>{});
    test_with(std::add_const_t<std::span<const char>>{});
#endif
}

///
/// @brief typed tests for various span types
///
template <typename ViewType>
class NullTerminationCheckTest : public ::testing::Test
{
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

using ViewTypes = ::testing::Types<score::cpp::span<char>,
                                   score::cpp::span<const char>,
                                   score::cpp::span<unsigned char>,
                                   score::cpp::span<std::int8_t>,
                                   score::cpp::span<std::uint8_t>,
                                   score::cpp::span<const std::int8_t>,
                                   score::cpp::span<const std::uint8_t>,
                                   score::cpp::span<const unsigned char>
#if defined(__cpp_lib_span)
                                   ,
                                   std::span<char>,
                                   std::span<const char>,
                                   std::span<unsigned char>,
                                   std::span<std::int8_t>,
                                   std::span<std::uint8_t>,
                                   std::span<const std::int8_t>,
                                   std::span<const std::uint8_t>,
                                   std::span<const unsigned char>
#endif
                                   >;

TYPED_TEST_SUITE(NullTerminationCheckTest, ViewTypes, /* unused */);

TYPED_TEST(NullTerminationCheckTest, NoUnderlyingBuffer)
{
    TypeParam view{};
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

TYPED_TEST(NullTerminationCheckTest, NullptrUnderlyingBuffer)
{
    TypeParam view{nullptr, 3U};
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

TYPED_TEST(NullTerminationCheckTest, WithUnderlyingBufferButZeroSizedView)
{
    typename TypeParam::value_type buffer[] = {'x'};
    TypeParam view{buffer, 0U};
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

TYPED_TEST(NullTerminationCheckTest, NullTerminatedEmtpyUnderlyingCharBufferButZeroSizedView)
{
    typename TypeParam::value_type buffer[] = {'\0'};
    TypeParam view{buffer, 0U};
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

TYPED_TEST(NullTerminationCheckTest, NullTerminatedEmtpyUnderlyingCharBuffer)
{
    typename TypeParam::value_type buffer[] = {'\0'};
    TypeParam view{buffer, 1U};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TYPED_TEST(NullTerminationCheckTest, NullTerminatedUnderlyingCharBuffer)
{
    typename TypeParam::value_type buffer[] = "hello";
    TypeParam view{buffer};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TYPED_TEST(NullTerminationCheckTest, NullTerminatedUnderlyingBufferButStringViewSmallerThanBuffer)
{
    typename TypeParam::value_type buffer[] = "hello world";
    TypeParam view{buffer, 5U};
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

TYPED_TEST(NullTerminationCheckTest, NullTerminationEmbeddedInUnderlyingBufferAtLastViewElement)
{
    typename TypeParam::value_type buffer[] = "hello\0world";
    TypeParam view{buffer, 6U};
    EXPECT_EQ(safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), view.data());
}

TYPED_TEST(NullTerminationCheckTest, NullTerminationEmbeddedInUnderlyingBufferButNotAtLastViewElement)
{
    typename TypeParam::value_type buffer[] = "hello\0world";
    TypeParam view{buffer, 7U};
    EXPECT_EXIT(
        score::cpp::ignore = safecpp::GetPtrToNullTerminatedUnderlyingBufferOf(view), ::testing::KilledBySignal{SIGABRT}, "");
}

}  // namespace
}  // namespace score
