/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#include "flatbuffers/string.h"

#include <cstring>
#include <string>
#include <vector>

#include "flatbuffers/flatbuffer_builder.h"
#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// Helper: build buffer with a single string field.
static std::vector<uint8_t> BuildStringTable(const char* str)
{
  FlatBufferBuilder fbb(256);
  auto s = fbb.CreateString(str);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, s);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));
  auto* ptr = fbb.GetBufferPointer();
  return std::vector<uint8_t>(ptr, ptr + fbb.GetSize());
}

// ---------------------------------------------------------------------------
// StringCStrTest
// ---------------------------------------------------------------------------

TEST(StringCStrTest, Content)
{
  RecordProperty("FullyVerifies", "::flatbuffers::String::c_str, ::flatbuffers::String::size");
  RecordProperty("Description", "c_str() and size() return correct content for non-empty, empty, and single-char strings");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  {
    auto buf = BuildStringTable("hello");
    auto* table = GetRoot<Table>(buf.data());
    const String* s = table->GetPointer<const String*>(4);
    ASSERT_NE(s, nullptr);
    EXPECT_STREQ(s->c_str(), "hello");
    EXPECT_EQ(s->size(), 5u);
  }
  {
    auto buf = BuildStringTable("");
    auto* table = GetRoot<Table>(buf.data());
    const String* s = table->GetPointer<const String*>(4);
    ASSERT_NE(s, nullptr);
    EXPECT_STREQ(s->c_str(), "");
    EXPECT_EQ(s->size(), 0u);
  }
  {
    auto buf = BuildStringTable("X");
    auto* table = GetRoot<Table>(buf.data());
    const String* s = table->GetPointer<const String*>(4);
    ASSERT_NE(s, nullptr);
    EXPECT_STREQ(s->c_str(), "X");
    EXPECT_EQ(s->size(), 1u);
  }
}

// ---------------------------------------------------------------------------
// StringStrTest
// ---------------------------------------------------------------------------

TEST(StringStrTest, StdString)
{
  RecordProperty("FullyVerifies", "::flatbuffers::String::str");
  RecordProperty("Description", "str() returns std::string with correct content");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildStringTable("world");
  auto* table = GetRoot<Table>(buf.data());
  const String* s = table->GetPointer<const String*>(4);
  ASSERT_NE(s, nullptr);
  std::string cpp_str = s->str();
  EXPECT_STREQ(cpp_str.c_str(), "world");
  EXPECT_EQ(cpp_str.size(), 5u);
}

// ---------------------------------------------------------------------------
// StringViewTest
// ---------------------------------------------------------------------------

TEST(StringViewTest, View)
{
  RecordProperty("FullyVerifies", "::flatbuffers::String::string_view");
  RecordProperty("Description", "string_view returns correct view into string content");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildStringTable("viewme");
  auto* table = GetRoot<Table>(buf.data());
  const String* s = table->GetPointer<const String*>(4);
  ASSERT_NE(s, nullptr);
  auto sv = s->string_view();
  EXPECT_EQ(sv.size(), 6u);
  EXPECT_EQ(sv[0], 'v');
  EXPECT_EQ(sv[5], 'e');
}

// ---------------------------------------------------------------------------
// StringComparisonTest
// ---------------------------------------------------------------------------

TEST(StringComparisonTest, Ordering)
{
  RecordProperty("FullyVerifies", "::flatbuffers::String::operator<");
  RecordProperty("Description", "operator< compares strings lexicographically");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto sa = fbb.CreateString("abc");
  auto sb = fbb.CreateString("abd");
  auto sc = fbb.CreateString("abc");
  auto sd = fbb.CreateString("");

  auto start = fbb.StartTable();
  fbb.AddOffset(4, sa);
  fbb.AddOffset(6, sb);
  fbb.AddOffset(8, sc);
  fbb.AddOffset(10, sd);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);

  const String* s_abc = table->GetPointer<const String*>(4);
  const String* s_abd = table->GetPointer<const String*>(6);
  const String* s_abc2 = table->GetPointer<const String*>(8);
  const String* s_empty = table->GetPointer<const String*>(10);

  ASSERT_NE(s_abc, nullptr);
  ASSERT_NE(s_abd, nullptr);
  ASSERT_NE(s_abc2, nullptr);
  ASSERT_NE(s_empty, nullptr);

  EXPECT_TRUE(*s_abc < *s_abd);
  EXPECT_FALSE(*s_abd < *s_abc);
  EXPECT_FALSE(*s_abc < *s_abc2);
  EXPECT_FALSE(*s_abc2 < *s_abc);
  EXPECT_TRUE(*s_empty < *s_abc);
}

// ---------------------------------------------------------------------------
// GetStringFunctionsTest
// ---------------------------------------------------------------------------

TEST(GetStringFunctionsTest, NullSafe)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetCstring, ::flatbuffers::GetStringView");
  RecordProperty("Description", "GetCstring and GetStringView handle null pointer gracefully");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_STREQ(GetCstring(nullptr), "");

  auto buf = BuildStringTable("hello");
  auto* table = GetRoot<Table>(buf.data());
  const String* s = table->GetPointer<const String*>(4);
  EXPECT_STREQ(GetCstring(s), "hello");

  auto sv = GetStringView(s);
  EXPECT_EQ(sv.size(), 5u);

  auto sv_null = GetStringView(nullptr);
  EXPECT_EQ(sv_null.size(), 0u);
}

// ---------------------------------------------------------------------------
// StringSpecialCharsTest
// ---------------------------------------------------------------------------

TEST(StringSpecialCharsTest, SpecialAndLong)
{
  RecordProperty("FullyVerifies", "::flatbuffers::String::c_str, ::flatbuffers::String::size");
  RecordProperty("Description", "strings with special characters and long strings");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  {
    auto buf = BuildStringTable("line1\nline2\ttab");
    auto* table = GetRoot<Table>(buf.data());
    const String* s = table->GetPointer<const String*>(4);
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->size(), 15u);
    EXPECT_STREQ(s->c_str(), "line1\nline2\ttab");
  }
  {
    std::string long_str(1000, 'A');
    auto buf = BuildStringTable(long_str.c_str());
    auto* table = GetRoot<Table>(buf.data());
    const String* s = table->GetPointer<const String*>(4);
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(s->size(), 1000u);
  }
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score