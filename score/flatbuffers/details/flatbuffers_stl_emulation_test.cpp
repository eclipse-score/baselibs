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
#include "flatbuffers/stl_emulation.h"

#include <array>
#include <vector>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// ---------------------------------------------------------------------------
// OptionalDefaultConstructTest
// ---------------------------------------------------------------------------

TEST(OptionalDefaultConstructTest, Empty)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::Optional");
  RecordProperty("Description", "default-constructed Optional is empty");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Optional<int> o;
  EXPECT_FALSE(o.has_value());
  EXPECT_FALSE(static_cast<bool>(o));
  EXPECT_EQ(o.value_or(42), 42);
}

// ---------------------------------------------------------------------------
// OptionalNulloptTest
// ---------------------------------------------------------------------------

TEST(OptionalNulloptTest, FromNullopt)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::Optional(nullopt_t)");
  RecordProperty("Description", "Optional constructed from nullopt is empty");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Optional<int> o(nullopt);
  EXPECT_FALSE(o.has_value());
}

// ---------------------------------------------------------------------------
// OptionalWithValueTest
// ---------------------------------------------------------------------------

TEST(OptionalWithValueTest, ContainsValue)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::has_value, ::flatbuffers::Optional<T>::operator*, ::flatbuffers::Optional<T>::value_or");
  RecordProperty("Description", "Optional with value: checks value_or returns stored value");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  {
    Optional<int> o(0);
    EXPECT_TRUE(o.has_value());
    EXPECT_EQ(*o, 0);
    EXPECT_EQ(o.value_or(99), 0);
  }
  {
    Optional<int> o(42);
    EXPECT_TRUE(o.has_value());
    EXPECT_EQ(*o, 42);
  }
  {
    Optional<int> o(-1);
    EXPECT_TRUE(o.has_value());
    EXPECT_EQ(*o, -1);
  }
  {
    Optional<uint32_t> o(std::numeric_limits<uint32_t>::max());
    EXPECT_TRUE(o.has_value());
    EXPECT_EQ(*o, std::numeric_limits<uint32_t>::max());
  }
}

// ---------------------------------------------------------------------------
// OptionalCopyTest
// ---------------------------------------------------------------------------

TEST(OptionalCopyTest, Construction)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::Optional(const Optional&)");
  RecordProperty("Description", "copy construction copies value state");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Optional<int> a(7);
  Optional<int> b(a);
  EXPECT_TRUE(b.has_value());
  EXPECT_EQ(*b, 7);

  Optional<int> c;
  Optional<int> d(c);
  EXPECT_FALSE(d.has_value());
}

// ---------------------------------------------------------------------------
// OptionalAssignmentTest
// ---------------------------------------------------------------------------

TEST(OptionalAssignmentTest, Operations)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::operator=");
  RecordProperty("Description", "assignment from value and from nullopt");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Optional<int> o;
  o = 10;
  EXPECT_TRUE(o.has_value());
  EXPECT_EQ(*o, 10);

  o = nullopt;
  EXPECT_FALSE(o.has_value());
}

// ---------------------------------------------------------------------------
// OptionalResetTest
// ---------------------------------------------------------------------------

TEST(OptionalResetTest, ClearValue)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::reset");
  RecordProperty("Description", "reset clears the stored value");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Optional<int> o(5);
  EXPECT_TRUE(o.has_value());
  o.reset();
  EXPECT_FALSE(o.has_value());
  o.reset();
  EXPECT_FALSE(o.has_value());
}

// ---------------------------------------------------------------------------
// OptionalSwapTest
// ---------------------------------------------------------------------------

TEST(OptionalSwapTest, ExchangeValues)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::swap");
  RecordProperty("Description", "swap exchanges values between Optionals");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Optional<int> a(1);
  Optional<int> b;
  a.swap(b);
  EXPECT_FALSE(a.has_value());
  EXPECT_TRUE(b.has_value());
  EXPECT_EQ(*b, 1);
}

// ---------------------------------------------------------------------------
// OptionalNulloptComparisonTest
// ---------------------------------------------------------------------------

TEST(OptionalNulloptComparisonTest, Equality)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Optional<T>::operator==, ::flatbuffers::Optional<T>::operator!=");
  RecordProperty("Description", "comparison with nullopt");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Optional<int> empty;
  Optional<int> full(3);
  EXPECT_TRUE(empty == nullopt);
  EXPECT_TRUE(nullopt == empty);
  EXPECT_FALSE(full == nullopt);
  EXPECT_TRUE(full != nullopt);
}

// ---------------------------------------------------------------------------
// SpanFromArrayTest
// ---------------------------------------------------------------------------

TEST(SpanFromArrayTest, FromCArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::make_span");
  RecordProperty("Description", "make_span creates span from C-array");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int arr[] = {10, 20, 30};
  auto s = make_span(arr);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s[0], 10);
  EXPECT_EQ(s[1], 20);
  EXPECT_EQ(s[2], 30);
  EXPECT_FALSE(s.empty());
}

// ---------------------------------------------------------------------------
// SpanFromPointerTest
// ---------------------------------------------------------------------------

TEST(SpanFromPointerTest, RawPtrAndCount)
{
  RecordProperty("FullyVerifies", "::flatbuffers::span<T>::span(pointer, size_type)");
  RecordProperty("Description", "span from raw pointer + count");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  std::vector<int> v = {1, 2, 3, 4, 5};
  span<int> s(v.data(), v.size());
  EXPECT_EQ(s.size(), 5u);
  EXPECT_EQ(s[0], 1);
  EXPECT_EQ(s[4], 5);
}

// ---------------------------------------------------------------------------
// SpanEmptyTest
// ---------------------------------------------------------------------------

TEST(SpanEmptyTest, DefaultConstructed)
{
  RecordProperty("FullyVerifies", "::flatbuffers::span<T>::span()");
  RecordProperty("Description", "empty span via default constructor");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  span<int> s;
  EXPECT_EQ(s.size(), 0u);
  EXPECT_TRUE(s.empty());
}

// ---------------------------------------------------------------------------
// SpanPtrCountTest
// ---------------------------------------------------------------------------

TEST(SpanPtrCountTest, PartialArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::span<T>::span(pointer, size_type)");
  RecordProperty("Description", "span from pointer + smaller count");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int arr[] = {7, 8, 9};
  span<int> s(&arr[0], 2);
  EXPECT_EQ(s.size(), 2u);
  EXPECT_EQ(s[0], 7);
  EXPECT_EQ(s[1], 8);
}

// ---------------------------------------------------------------------------
// TypeTraitsTest
// ---------------------------------------------------------------------------

TEST(TypeTraitsTest, CompileTimeTraits)
{
  RecordProperty("FullyVerifies", "::flatbuffers::is_scalar, is_same, is_floating_point, is_unsigned");
  RecordProperty("Description", "type traits aliases work correctly");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_TRUE((is_scalar<int>::value));
  EXPECT_FALSE((is_scalar<std::string>::value));
  EXPECT_TRUE((is_same<int, int>::value));
  EXPECT_FALSE((is_same<int, float>::value));
  EXPECT_TRUE((is_floating_point<float>::value));
  EXPECT_FALSE((is_floating_point<int>::value));
  EXPECT_TRUE((is_unsigned<unsigned>::value));
  EXPECT_FALSE((is_unsigned<int>::value));
}

// ---------------------------------------------------------------------------
// NumericLimitsTest
// ---------------------------------------------------------------------------

TEST(NumericLimitsTest, ValueRanges)
{
  RecordProperty("FullyVerifies", "::flatbuffers::numeric_limits<T>");
  RecordProperty("Description", "numeric_limits alias provides correct min/max values");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(numeric_limits<uint8_t>::max(), static_cast<uint8_t>(255));
  EXPECT_EQ(numeric_limits<int8_t>::min(), static_cast<int8_t>(-128));
  EXPECT_EQ(numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max());
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score