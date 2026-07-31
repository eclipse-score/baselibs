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
#include "flatbuffers/array.h"

#include <cstdint>
#include <cstring>

#include "flatbuffers/flatbuffer_builder.h"
#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// Magic-number sentinels used across tests.
constexpr int32_t kFirstElement = 10;
constexpr int32_t kMiddleElement = 20;
constexpr int32_t kLastElement = 30;
constexpr int32_t kMutateValue = 42;

// -------------------------------------------------------
// Minimal Point struct used across tests.
// -------------------------------------------------------
struct Point
{
  int32_t x;
  int32_t y;

  bool operator==(const Point& other) const
  {
    return x == other.x && y == other.y;
  }
};

// ---------------------------------------------------------------------------
// MISRA classification of the cast helpers (applies to the whole file)
//
// CastToArray / CastToArrayOfEnum from flatbuffers/array.h reinterpret_cast a
// raw T[length] into an Array<T, length>. This deliberately violates:
//   - MISRA C++:2023 Rule 8.2.5 / AUTOSAR C++14 A5-2-4 (use of
//     reinterpret_cast). Rule 8.2.5 exempts casts whose target is a pointer to
//     void, char, unsigned char or std::byte (possibly cv-qualified), or an
//     integer type holding the pointer value, but this cast targets a pointer
//     to an unrelated class type (Array<T, length>*), so no exception applies.
// The third-party header marks these functions as risky itself ("Use with
// care.", TODO: move to `internal`) and guarantees no defined behaviour with
// respect to object lifetime / strict aliasing; it only works because
// Array<T> is a pure layout wrapper with no data members of its own.
//
// Justification:
// 
// Array isn't a real value type; it's a reinterpret-cast view onto bytes that
// already exist inside a FlatBuffer, lifespan concerns are out of scope.
// 
// User facing is the flac generated header, which has measures 
// for correct usage.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ArrayCastTest
// Tests casting raw C arrays to flatbuffers::Array.
// ---------------------------------------------------------------------------

TEST(ArrayCastTest, CastToArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::CastToArray");
  RecordProperty("Description", "casting a raw C array to flatbuffers::Array preserves elements");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {10, 20, 30};
  auto& arr = CastToArray(raw);
  EXPECT_EQ(arr.size(), 3u);
  EXPECT_EQ(arr.Get(0), 10);
  EXPECT_EQ(arr.Get(1), 20);
  EXPECT_EQ(arr.Get(2), 30);

  int32_t raw1[1] = {42};
  auto& arr1 = CastToArray(raw1);
  EXPECT_EQ(arr1.size(), 1u);
  EXPECT_EQ(arr1.Get(0), 42);
}

TEST(ArrayCastTest, CastToArrayConst)
{
  RecordProperty("FullyVerifies", "::flatbuffers::CastToArray");
  RecordProperty("Description", "const array cast produces a const-referenced Array");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  const int32_t raw[2] = {77, 88};
  const auto& arr = CastToArray(raw);
  EXPECT_EQ(arr.size(), 2u);
  EXPECT_EQ(arr.Get(0), 77);
  EXPECT_EQ(arr.Get(1), 88);
}

// ---------------------------------------------------------------------------
// ArrayIndexTest
// Tests Array<T, N>::operator[].
// ---------------------------------------------------------------------------

TEST(ArrayIndexTest, ReturnsSameAsGet)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::operator[]");
  RecordProperty("Description", "operator[] returns same value as Get for every index");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  uint16_t raw[4] = {100, 200, 300, 400};
  auto& arr = CastToArray(raw);
  for (uint16_t i = 0; i < arr.size(); ++i)
  {
    EXPECT_EQ(arr[i], arr.Get(i));
  }
}

// ---------------------------------------------------------------------------
// ArrayMutateTest
// Tests Array<T, N>::Mutate.
// ---------------------------------------------------------------------------

TEST(ArrayMutateTest, ScalarInPlaceMutation)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::Mutate, ::flatbuffers::Array<T, N>::MutateImpl(true_type)");
  RecordProperty("Description", "in-place mutation of scalar array elements");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  int32_t raw[3] = {1, 2, 3};
  auto& arr = CastToArray(raw);

  arr.Mutate(0, 100);
  EXPECT_EQ(arr.Get(0), 100);

  arr.Mutate(2, 0);
  EXPECT_EQ(arr.Get(2), 0);

  arr.Mutate(1, std::numeric_limits<int32_t>::max());
  EXPECT_EQ(arr.Get(1), std::numeric_limits<int32_t>::max());
}

// ---------------------------------------------------------------------------
// ArrayIteratorTest
// Tests Array<T, N>::begin, Array<T, N>::end.
// ---------------------------------------------------------------------------

TEST(ArrayIteratorTest, ForwardIteration)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::begin, ::flatbuffers::Array<T, N>::end");
  RecordProperty("Description", "forward iteration yields expected elements in order");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[4] = {5, 10, 15, 20};
  const auto& arr = CastToArray(raw);

  int count = 0;
  int expected[] = {5, 10, 15, 20};
  for (auto it = arr.begin(); it != arr.end(); ++it)
  {
    EXPECT_EQ(*it, expected[count]);
    ++count;
  }
  EXPECT_EQ(count, 4);
}

// ---------------------------------------------------------------------------
// ArrayReverseIteratorTest
// Tests Array<T, N>::rbegin, Array<T, N>::rend.
// ---------------------------------------------------------------------------

TEST(ArrayReverseIteratorTest, ReverseIteration)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::rbegin, ::flatbuffers::Array<T, N>::rend");
  RecordProperty("Description", "reverse iteration yields elements in reverse order");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {1, 2, 3};
  const auto& arr = CastToArray(raw);

  std::vector<int32_t> rev;
  for (auto it = arr.rbegin(); it != arr.rend(); ++it)
  {
    rev.push_back(*it);
  }
  ASSERT_EQ(rev.size(), 3u);
  EXPECT_EQ(rev[0], 3);
  EXPECT_EQ(rev[1], 2);
  EXPECT_EQ(rev[2], 1);
}

// ---------------------------------------------------------------------------
// ArraySizeTest
// Tests Array<T, N>::size.
// ---------------------------------------------------------------------------

TEST(ArraySizeTest, ReturnsTemplateSize)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::size");
  RecordProperty("Description", "size() returns the compile-time template parameter N");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[5] = {};
  auto& arr = CastToArray(raw);
  EXPECT_EQ(arr.size(), 5u);
}

// ---------------------------------------------------------------------------
// ArrayMakeSpanTest
// Tests make_span, make_bytes_span.
// ---------------------------------------------------------------------------

TEST(ArrayMakeSpanTest, SpanFromArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::make_span(Array<T, N>&)");
  RecordProperty("Description", "make_span creates a fixed-size span over array data");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {100, 200, 300};
  auto& arr = CastToArray(raw);
  auto s = make_span(arr);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s[0], 100);
  EXPECT_EQ(s[1], 200);
  EXPECT_EQ(s[2], 300);
}

TEST(ArrayMakeSpanTest, BytesSpanFromArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::make_bytes_span(Array<T, N>&)");
  RecordProperty("Description", "make_bytes_span returns span over raw bytes");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[2] = {0, 0};
  auto& arr = CastToArray(raw);
  auto bs = make_bytes_span(arr);
  EXPECT_EQ(bs.size(), 2u * sizeof(int32_t));
}

// ---------------------------------------------------------------------------
// ArrayDataTest
// Tests Array<T, N>::Data(), Array<T, N>::data().
// ---------------------------------------------------------------------------

TEST(ArrayDataTest, DataPointers)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::Data, ::flatbuffers::Array<T, N>::data");
  RecordProperty("Description", "Data() and data() return pointers to underlying storage");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {1, 2, 3};
  auto& arr = CastToArray(raw);
  EXPECT_EQ(reinterpret_cast<const uint8_t*>(raw),
            static_cast<const uint8_t*>(arr.Data()));
  EXPECT_EQ(raw, arr.data());
}

// ---------------------------------------------------------------------------
// ArrayEqualityTest
// Tests operator==.
// ---------------------------------------------------------------------------

TEST(ArrayEqualityTest, ScalarEquality)
{
  RecordProperty("FullyVerifies", "::flatbuffers::operator==");
  RecordProperty("Description", "operator== compares all elements for equality");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t a[3] = {1, 2, 3};
  int32_t b[3] = {1, 2, 3};
  int32_t c[3] = {1, 2, 4};

  auto& aa = CastToArray(a);
  auto& ab = CastToArray(b);
  auto& ac = CastToArray(c);

  EXPECT_TRUE(aa == ab);
  EXPECT_FALSE(aa == ac);
  EXPECT_TRUE(aa == aa);
}

// ---------------------------------------------------------------------------
// ArrayEnumTest
// Tests CastToArrayOfEnum, GetEnum.
// ---------------------------------------------------------------------------

TEST(ArrayEnumTest, CastToArrayOfEnum)
{
  RecordProperty("FullyVerifies", "::flatbuffers::CastToArrayOfEnum");
  RecordProperty("Description", "CastToArrayOfEnum casts raw array to Array of enum type");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  enum class Color : int32_t { Red = 0, Green = 1, Blue = 2 };
  int32_t raw[3] = {0, 1, 2};
  auto& arr = CastToArrayOfEnum<Color>(raw);
  EXPECT_EQ(arr.size(), 3u);
  EXPECT_EQ(static_cast<int32_t>(arr.Get(0)), 0);
  EXPECT_EQ(static_cast<int32_t>(arr.Get(1)), 1);
  EXPECT_EQ(static_cast<int32_t>(arr.Get(2)), 2);
}

TEST(ArrayEnumTest, GetEnum)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::GetEnum");
  RecordProperty("Description", "GetEnum retrieves element cast to enum type");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  enum class Fruit : int32_t { Apple = 0, Banana = 1, Cherry = 2 };
  int32_t raw[3] = {0, 1, 2};
  auto& arr = CastToArray(raw);
  EXPECT_EQ(arr.GetEnum<Fruit>(0), Fruit::Apple);
  EXPECT_EQ(arr.GetEnum<Fruit>(1), Fruit::Banana);
  EXPECT_EQ(arr.GetEnum<Fruit>(2), Fruit::Cherry);
}

// ---------------------------------------------------------------------------
// ArrayConstIteratorsTest
// Tests cbegin, cend, crbegin, crend.
// ---------------------------------------------------------------------------

TEST(ArrayConstIteratorsTest, CBeginCEnd)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::cbegin, ::flatbuffers::Array<T, N>::cend");
  RecordProperty("Description", "cbegin/cend produce const forward iterators");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {10, 20, 30};
  const auto& arr = CastToArray(raw);

  int32_t expected[] = {10, 20, 30};
  int idx = 0;
  for (auto it = arr.cbegin(); it != arr.cend(); ++it)
  {
    EXPECT_EQ(*it, expected[idx]);
    ++idx;
  }
  EXPECT_EQ(idx, 3);
}

TEST(ArrayConstIteratorsTest, CRBeginCREnd)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::crbegin, ::flatbuffers::Array<T, N>::crend");
  RecordProperty("Description", "crbegin/crend produce const reverse iterators");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {1, 2, 3};
  const auto& arr = CastToArray(raw);

  std::vector<int32_t> result;
  for (auto it = arr.crbegin(); it != arr.crend(); ++it)
  {
    result.push_back(*it);
  }
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0], 3);
  EXPECT_EQ(result[1], 2);
  EXPECT_EQ(result[2], 1);
}

// ---------------------------------------------------------------------------
// ArrayMutableDataTest
// Tests mutable Data() and data().
// ---------------------------------------------------------------------------

TEST(ArrayMutableDataTest, MutableData)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::Data");
  RecordProperty("Description", "mutable Data() returns writable uint8_t pointer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[2] = {0, 0};
  auto& arr = CastToArray(raw);
  uint8_t* mutable_ptr = arr.Data();
  EXPECT_NE(mutable_ptr, nullptr);
  int32_t val = 42;
  std::memcpy(mutable_ptr, &val, sizeof(val));
  EXPECT_EQ(arr.Get(0), 42);
  EXPECT_EQ(arr.Get(1), 0);
}

TEST(ArrayMutableDataTest, MutableDataTyped)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::data");
  RecordProperty("Description", "mutable data() returns writable T* pointer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {1, 2, 3};
  auto& arr = CastToArray(raw);
  int32_t* typed_ptr = arr.data();
  EXPECT_EQ(typed_ptr, raw);
  typed_ptr[1] = 99;
  EXPECT_EQ(arr.Get(1), 99);
  EXPECT_EQ(arr.Get(0), 1);
  EXPECT_EQ(arr.Get(2), 3);
}

// ---------------------------------------------------------------------------
// ArrayStructTest
// Tests struct-related Array functionality.
// ---------------------------------------------------------------------------

TEST(ArrayStructTest, GetMutablePointerStruct)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::GetMutablePointer");
  RecordProperty("Description", "GetMutablePointer on struct array returns writable pointer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Point raw[2] = {{1, 2}, {3, 4}};
  auto& arr = CastToArray(raw);
  Point* p = arr.GetMutablePointer(0);
  ASSERT_NE(p, nullptr);
  p->x = 100;
  p->y = 200;
  EXPECT_EQ(arr.Get(0)->x, 100);
  EXPECT_EQ(arr.Get(0)->y, 200);
  EXPECT_EQ(arr.Get(1)->x, 3);
  EXPECT_EQ(arr.Get(1)->y, 4);
}

TEST(ArrayStructTest, MutateStruct)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::Mutate, ::flatbuffers::Array<T, N>::MutateImpl(false_type)");
  RecordProperty("Description", "Mutate on struct array uses MutateImpl(false_type)");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Point raw[2] = {{10, 20}, {30, 40}};
  auto& arr = CastToArray(raw);
  Point new_val = {50, 60};
  arr.Mutate(1, new_val);
  EXPECT_EQ(arr.Get(1)->x, 50);
  EXPECT_EQ(arr.Get(1)->y, 60);
  EXPECT_EQ(arr.Get(0)->x, 10);
  EXPECT_EQ(arr.Get(0)->y, 20);
}

// ---------------------------------------------------------------------------
// ArrayCopyFromSpanTest
// Tests Array<T, N>::CopyFromSpan.
// ---------------------------------------------------------------------------

TEST(ArrayCopyFromSpanTest, Scalar)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::CopyFromSpan");
  RecordProperty("Description", "CopyFromSpan with scalar (span-observable) path");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {0, 0, 0};
  auto& arr = CastToArray(raw);

  const int32_t src[3] = {7, 8, 9};
  span<const int32_t, 3> src_span(src, 3);
  arr.CopyFromSpan(src_span);

  EXPECT_EQ(arr.Get(0), 7);
  EXPECT_EQ(arr.Get(1), 8);
  EXPECT_EQ(arr.Get(2), 9);
}

TEST(ArrayCopyFromSpanTest, Struct)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::CopyFromSpan");
  RecordProperty("Description", "CopyFromSpan with struct type");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Point raw[2] = {{0, 0}, {0, 0}};
  auto& arr = CastToArray(raw);

  const Point src[2] = {{11, 22}, {33, 44}};
  span<const Point, 2> src_span(src, 2);
  arr.CopyFromSpan(src_span);

  EXPECT_EQ(arr.Get(0)->x, 11);
  EXPECT_EQ(arr.Get(0)->y, 22);
  EXPECT_EQ(arr.Get(1)->x, 33);
  EXPECT_EQ(arr.Get(1)->y, 44);
}

// ---------------------------------------------------------------------------
// ArrayConstSpanTest
// Tests make_span with const Array.
// ---------------------------------------------------------------------------

TEST(ArrayConstSpanTest, MakeSpanConst)
{
  RecordProperty("FullyVerifies", "::flatbuffers::make_span(const Array<T, N>&)");
  RecordProperty("Description", "make_span with const Array");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  const int32_t raw[3] = {5, 10, 15};
  const auto& arr = CastToArray(raw);
  auto s = make_span(arr);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s[0], 5);
  EXPECT_EQ(s[1], 10);
  EXPECT_EQ(s[2], 15);
}

TEST(ArrayConstSpanTest, MakeBytesSpanConst)
{
  RecordProperty("FullyVerifies", "::flatbuffers::make_bytes_span(const Array<T, N>&)");
  RecordProperty("Description", "make_bytes_span with const Array");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  const int32_t raw[2] = {1, 2};
  const auto& arr = CastToArray(raw);
  auto bs = make_bytes_span(arr);
  EXPECT_EQ(bs.size(), 2u * sizeof(int32_t));
  EXPECT_EQ(bs.data(), arr.Data());
}

TEST(ArrayConstSpanTest, ConstCastToArrayOfEnum)
{
  RecordProperty("FullyVerifies", "::flatbuffers::CastToArrayOfEnum");
  RecordProperty("Description", "const CastToArrayOfEnum");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  enum class Color : int32_t { Red = 0, Green = 1, Blue = 2 };
  const int32_t raw[3] = {0, 1, 2};
  const auto& arr = CastToArrayOfEnum<Color>(raw);
  EXPECT_EQ(arr.size(), 3u);
  EXPECT_EQ(static_cast<int32_t>(arr.Get(0)), 0);
  EXPECT_EQ(static_cast<int32_t>(arr.Get(1)), 1);
  EXPECT_EQ(static_cast<int32_t>(arr.Get(2)), 2);
}

// ---------------------------------------------------------------------------
// ArraySpanObservableTest
// Tests is_span_observable.
//
// Only the true path is exercised. is_span_observable is false in two cases,
// neither of which is reachable in this build:
//   1. T is a pointer. Array<T, N> is documented to carry only POD data
//      (scalars or structs), and its public factories (CastToArray /
//      CastToArrayOfEnum) cannot produce a pointer element type, so a pointer
//      T cannot be instantiated here.
//   2. T is a multi-byte scalar on a big-endian platform. FlatBuffers stores
//      scalars little-endian, so their raw bytes are only span-observable when
//      FLATBUFFERS_LITTLEENDIAN holds (or sizeof(T) == 1). The test targets are
//      little-endian, so this branch is never taken; forcing it would require a
//      big-endian toolchain that is out of scope for these unit tests.
// ---------------------------------------------------------------------------

TEST(ArraySpanObservableTest, StaticAssertions)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::is_span_observable");
  RecordProperty("Description", "is_span_observable static member for scalar and struct types");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  static_assert(Array<int32_t, 3>::is_span_observable, "int32_t should be span-observable on LE");
  static_assert(Array<uint16_t, 2>::is_span_observable, "uint16_t should be span-observable on LE");
  static_assert(Array<int64_t, 1>::is_span_observable, "int64_t should be span-observable on LE");
  static_assert(Array<uint8_t, 4>::is_span_observable, "uint8_t should always be span-observable");
  static_assert(Array<int8_t, 2>::is_span_observable, "int8_t should always be span-observable");
  static_assert(Array<Point, 2>::is_span_observable, "POD struct should be span-observable");

  EXPECT_TRUE((Array<int32_t, 3>::is_span_observable));
  EXPECT_TRUE((Array<uint8_t, 4>::is_span_observable));
  EXPECT_TRUE((Array<Point, 2>::is_span_observable));
}

// ---------------------------------------------------------------------------
// CopyFromSpanImplTest
// Direct access helper for CopyFromSpanImpl true_type and false_type paths.
// ---------------------------------------------------------------------------

// Exposes the protected CopyFromSpanImpl overloads so both paths can be tested
// regardless of platform endianness:
//   observable    -> memcpy path (raw bytes match native layout)
//   non-observable -> element-wise Mutate path (with endian conversion)
template <typename T, uint16_t N>
struct ArrayTestAccess : public Array<T, N>
{
  void CallCopyObservable(span<const T, N> src)
  {
    this->CopyFromSpanImpl(true_type(), src);
  }
  void CallCopyNonObservable(span<const T, N> src)
  {
    this->CopyFromSpanImpl(false_type(), src);
  }
};

TEST(CopyFromSpanImplTest, Observable)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::CopyFromSpanImpl(true_type)");
  RecordProperty("Description", "CopyFromSpanImpl(true_type) — memcpy path");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {0, 0, 0};
  auto& accessor = static_cast<ArrayTestAccess<int32_t, 3>&>(CastToArray(raw));

  const int32_t src[3] = {10, 20, 30};
  span<const int32_t, 3> src_span(src, 3);
  accessor.CallCopyObservable(src_span);

  EXPECT_EQ(raw[0], 10);
  EXPECT_EQ(raw[1], 20);
  EXPECT_EQ(raw[2], 30);
}

TEST(CopyFromSpanImplTest, NonObservableScalar)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::CopyFromSpanImpl(false_type)");
  RecordProperty("Description", "CopyFromSpanImpl(false_type) — element-wise Mutate path for scalars");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  int32_t raw[3] = {0, 0, 0};
  auto& accessor = static_cast<ArrayTestAccess<int32_t, 3>&>(CastToArray(raw));

  const int32_t src[3] = {100, 200, 300};
  span<const int32_t, 3> src_span(src, 3);
  accessor.CallCopyNonObservable(src_span);

  EXPECT_EQ(raw[0], 100);
  EXPECT_EQ(raw[1], 200);
  EXPECT_EQ(raw[2], 300);
}

TEST(CopyFromSpanImplTest, NonObservableStruct)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::CopyFromSpanImpl(false_type)");
  RecordProperty("Description", "CopyFromSpanImpl(false_type) with struct type");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Point raw[2] = {{0, 0}, {0, 0}};
  auto& accessor = static_cast<ArrayTestAccess<Point, 2>&>(CastToArray(raw));

  const Point src[2] = {{7, 8}, {9, 10}};
  span<const Point, 2> src_span(src, 2);
  accessor.CallCopyNonObservable(src_span);

  EXPECT_EQ(raw[0].x, 7);
  EXPECT_EQ(raw[0].y, 8);
  EXPECT_EQ(raw[1].x, 9);
  EXPECT_EQ(raw[1].y, 10);
}

// ---------------------------------------------------------------------------
// OffsetSpecializationTest
// Tests the Array<Offset<void>, N> specialization.
// ---------------------------------------------------------------------------

TEST(OffsetSpecializationTest, Data)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<Offset<void>, N>::Data");
  RecordProperty("Description", "Data() on Offset<void> specialization returns internal data pointer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  uint8_t buf[4] = {0xAA, 0xBB, 0xCC, 0xDD};
  const auto& arr = *reinterpret_cast<const Array<Offset<void>, 2>*>(buf);
  EXPECT_EQ(arr.Data(), buf);
}

TEST(OffsetSpecializationTest, StructArrayEquality)
{
  RecordProperty("FullyVerifies", "::flatbuffers::operator==");
  RecordProperty("Description", "operator== on struct arrays");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Point a[2] = {{1, 2}, {3, 4}};
  Point b[2] = {{1, 2}, {3, 4}};
  Point c[2] = {{1, 2}, {3, 5}};

  auto& aa = CastToArray(a);
  auto& ab = CastToArray(b);
  auto& ac = CastToArray(c);

  EXPECT_TRUE(aa == ab);
  EXPECT_FALSE(aa == ac);
  EXPECT_TRUE(aa == aa);
}

TEST(OffsetSpecializationTest, SizeType)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::size_type");
  RecordProperty("Description", "size_type is uint16_t");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  static_assert(std::is_same<Array<int32_t, 3>::size_type, uint16_t>::value,
                "size_type must be uint16_t");
}

// ---------------------------------------------------------------------------
// ArrayFaultInjectionTest
// Fault injection tests for Array.
// ---------------------------------------------------------------------------

TEST(ArrayFaultInjectionTest, OffsetSpecializationOperatorIndexDeathTest)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<Offset<void>, N>::operator[]");
  RecordProperty("Description", "Array<Offset<void>, N>::operator[] triggers assert(false) at runtime");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  uint8_t buf[2] = {0};
  const auto& arr = *reinterpret_cast<const Array<Offset<void>, 2>*>(buf);
  EXPECT_DEATH({arr[0]; }, "");
}

TEST(ArrayFaultInjectionTest, FaultGetOutOfBounds)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::Get");
  RecordProperty("Description", "Get() with index == size triggers assert(false)");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  int32_t raw[3] = {1, 2, 3};
  const auto& arr = CastToArray(raw);
  EXPECT_DEATH({arr.Get(3u); }, "");
}

TEST(ArrayFaultInjectionTest, FaultGetMutablePointerOutOfBounds)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::GetMutablePointer");
  RecordProperty("Description", "GetMutablePointer() with index == size triggers assert(false)");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  Point raw[2] = {{1, 2}, {3, 4}};
  auto& arr = CastToArray(raw);
  EXPECT_DEATH({arr.GetMutablePointer(2u); }, "");
}

TEST(ArrayFaultInjectionTest, FaultMutateScalarOutOfBounds)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::Mutate, ::flatbuffers::Array<T, N>::MutateImpl(true_type)");
  RecordProperty("Description", "Mutate() with index == size triggers assert(false)");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  int32_t raw[3] = {0, 0, 0};
  auto& arr = CastToArray(raw);
  EXPECT_DEATH({arr.Mutate(3u, 42); }, "");
}

TEST(ArrayFaultInjectionTest, FaultMutateStructOutOfBounds)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::MutateImpl(false_type)");
  RecordProperty("Description", "Mutate() with index == size on struct triggers assert(false) via false_type path");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  Point raw[2] = {{0, 0}, {0, 0}};
  auto& arr = CastToArray(raw);
  const Point p = {1, 2};
  EXPECT_DEATH({arr.Mutate(2u, p); }, "");
}

TEST(ArrayFaultInjectionTest, FaultCopyFromSpanOverlap)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Array<T, N>::CopyFromSpan");
  RecordProperty("Description", "CopyFromSpan with a source span overlapping the array triggers assert(false)");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  int32_t raw[3] = {1, 2, 3};
  auto& arr = CastToArray(raw);
  span<const int32_t, 3> overlapping(raw, 3);  // shares storage with arr -> p1 == p2
  EXPECT_DEATH({arr.CopyFromSpan(overlapping); }, "");
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score