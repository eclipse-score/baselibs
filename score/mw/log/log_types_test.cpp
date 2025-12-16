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
#include "score/mw/log/log_types.h"

#include <score/string.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <string>
#include <string_view>

namespace score
{
namespace mw
{
namespace log
{

namespace
{

TEST(LogStringTest, ConstructFromCharArray)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the detection of null-/non-null-terminated char array");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // When constructing our LogString type from an empty char[] literal
    const auto empty_log_str = mw::log::LogStr("");
    const auto empty_str = LogString{""};

    // Then it must work appropriately
    EXPECT_STREQ("", empty_log_str.Data());
    EXPECT_STREQ("", empty_str.Data());
    EXPECT_EQ(0U, empty_log_str.Size());
    EXPECT_EQ(0U, empty_str.Size());

    // When constructing our LogString type from nullptr
    const auto null_str = LogString{nullptr, 0U};

    // Then it must work appropriately
    EXPECT_EQ(nullptr, null_str.Data());
    EXPECT_EQ(0U, null_str.Size());

    // Given a null-terminated array of characters
    const char null_terminated[] = {'M', 'y', 'A', 'r', 'r', 'a', 'y', '\0'};

    // When constructing our LogString type using that one
    const auto log_str = mw::log::LogStr(null_terminated);
    const auto str = LogString{null_terminated};

    // Then it must work appropriately
    EXPECT_STREQ("MyArray", log_str.Data());
    EXPECT_STREQ("MyArray", str.Data());
    EXPECT_EQ(7U, log_str.Size());
    EXPECT_EQ(7U, str.Size());

    // Given a an array of characters which is not null-terminated at the end
    const char non_null_terminated[] = {'M', 'y', '\0', 'A', 'r', 'r', 'a', 'y'};

    // When constructing our LogString type using that one
    const auto test_implicit_conversion_from = [](LogString param) {
        std::ignore = param;
    };

    // Then immediate terminating is expected
    EXPECT_DEATH(test_implicit_conversion_from(non_null_terminated), ".*");
    EXPECT_DEATH(mw::log::LogStr(non_null_terminated), ".*");
}

TEST(LogStringTest, CanImplicitlyConvertFromStringLikeTypes)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of creating our LogString type view from string-like types");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto kExpected = "MyString";

    const auto test_implicit_conversion_from = [](LogString log_str) {
        EXPECT_STREQ(log_str.Data(), kExpected);
        EXPECT_EQ(log_str.Size(), 8U);
    };

    const auto test = [&test_implicit_conversion_from](auto str) {
        const auto& ref = str;
        test_implicit_conversion_from(ref);

        std::reference_wrapper wrapper{ref};
        test_implicit_conversion_from(wrapper);

        test_implicit_conversion_from(str);
        test_implicit_conversion_from(std::move(str));
    };

    struct MyStringView
    {
        constexpr const char* data() const noexcept
        {
            return kExpected;
        }
        constexpr std::size_t size() const noexcept
        {
            return 8U;
        }
    };

    test_implicit_conversion_from("MyString");
    test(score::cpp::pmr::string{kExpected});
    test(std::string_view{kExpected});
    test(std::string{kExpected});
    test(MyStringView{});
}

TEST(MakeLogRawBufferTest, MakeBufferFromInteger)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of creating a buffer from integer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::int32_t value{15};
    LogRawBuffer log_raw_buffer{MakeLogRawBuffer(value)};
    ASSERT_EQ(log_raw_buffer.size(), sizeof(value));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr const char expected[]{0, 0, 0, 15};
#else
    constexpr const char expected[]{15, 0, 0, 0};
#endif
    EXPECT_TRUE(std::equal(log_raw_buffer.cbegin(), log_raw_buffer.cend(), std::begin(expected)));
}

TEST(MakeLogRawBufferTest, MakeBufferFromIntegerStdArray)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of creating a buffer from array of integers.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::array<std::int32_t, 2> values{15, 16};
    LogRawBuffer log_raw_buffer{MakeLogRawBuffer(values)};
    ASSERT_EQ(log_raw_buffer.size(), sizeof(values));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr const char expected[]{0, 0, 0, 15, 0, 0, 0, 16};
#else
    constexpr const char expected[]{15, 0, 0, 0, 16, 0, 0, 0};
#endif
    EXPECT_TRUE(std::equal(log_raw_buffer.cbegin(), log_raw_buffer.cend(), std::begin(expected)));
}

TEST(MakeLogRawBufferTest, MakeBufferFromSpan)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of creating a buffer from score::cpp::span.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::int32_t values[]{15, 16};
    const score::cpp::span<const std::int32_t> span{values, 2};
    LogRawBuffer log_raw_buffer{MakeLogRawBuffer(span)};
    ASSERT_EQ(log_raw_buffer.size(), sizeof(values));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr const char expected[]{0, 0, 0, 15, 0, 0, 0, 16};
#else
    constexpr const char expected[]{15, 0, 0, 0, 16, 0, 0, 0};
#endif
    EXPECT_TRUE(std::equal(log_raw_buffer.cbegin(), log_raw_buffer.cend(), std::begin(expected)));
}

TEST(MakeLogRawBufferTest, MakeBufferFromVector)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of creating a buffer from vector of integers");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::vector<std::int32_t> values{15, 16};
    LogRawBuffer log_raw_buffer{MakeLogRawBuffer(values)};
    ASSERT_EQ(log_raw_buffer.size(), sizeof(std::int32_t) * values.size());
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    constexpr const char expected[]{0, 0, 0, 15, 0, 0, 0, 16};
#else
    constexpr const char expected[]{15, 0, 0, 0, 16, 0, 0, 0};
#endif
    EXPECT_TRUE(std::equal(log_raw_buffer.cbegin(), log_raw_buffer.cend(), std::begin(expected)));
}

}  // namespace

}  // namespace log
}  // namespace mw
}  // namespace score
