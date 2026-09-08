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
#include <limits>
#include <type_traits>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

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

TEST(ArrayCastTest, CastToArray)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "casting a raw C array to flatbuffers::Array preserves elements");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "const array cast produces a const-referenced Array");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const int32_t raw[2] = {77, 88};
    static_assert(std::is_const_v<std::remove_reference_t<decltype(CastToArray(raw))>>);
    SUCCEED();
}

// ---------------------------------------------------------------------------
// ArrayIndexTest
// Tests Array<T, N>::operator[].
// ---------------------------------------------------------------------------

TEST(ArrayIndexTest, ReturnsSameAsGet)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "operator[] returns same value as Get for every index");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "in-place mutation of scalar array elements");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "forward iteration yields expected elements in order");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "reverse iteration yields elements in reverse order");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "size() returns the template parameter N");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    int32_t raw[5] = {};
    auto& arr = CastToArray(raw);
    EXPECT_EQ(arr.size(), 5u);

    // size() returns the template parameter N, which is part of the Array type.
    // CastToArray is not constexpr (it reinterpret_casts, which is banned in
    // constant expressions), so we cannot form a constexpr Array object. But N
    // is fixed at compile time regardless: assert it through the type, using
    // decltype's unevaluated context so no cast is actually performed.
    using ArrayType = std::remove_reference_t<decltype(CastToArray(raw))>;
    static_assert(std::is_same_v<ArrayType, Array<int32_t, 5>>, "CastToArray must bake N==5 into the Array type");
}

TEST(ArraySizeTest, SizeType)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "size_type is uint16_t");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    static_assert(std::is_same<Array<int32_t, 3>::size_type, uint16_t>::value, "size_type must be uint16_t");
}

// ---------------------------------------------------------------------------
// ArrayMakeSpanTest
// Tests make_span, make_bytes_span.
// ---------------------------------------------------------------------------

TEST(ArrayMakeSpanTest, SpanFromArray)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "make_span creates a fixed-size span over array data");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "make_bytes_span returns span over raw bytes");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "Data() and data() return pointers to underlying storage");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    int32_t raw[3] = {1, 2, 3};
    auto& arr = CastToArray(raw);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(raw), static_cast<const uint8_t*>(arr.Data()));
    EXPECT_EQ(raw, arr.data());
}

// ---------------------------------------------------------------------------
// ArrayEqualityTest
// Tests operator==.
// ---------------------------------------------------------------------------

TEST(ArrayEqualityTest, ScalarEquality)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "operator== compares all elements for equality");
    RecordProperty("TestType", "interface-test");
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

TEST(ArrayEqualityTest, StructArrayEquality)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "operator== on struct arrays");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Point a[2] = {{1, 2}, {3, 4}};
    Point b[2] = {{1, 2}, {3, 4}};
    Point c[2] = {{1, 2}, {3, 5}};

    auto& aa = CastToArray(a);
    auto& ab = CastToArray(b);
    auto& ac = CastToArray(c);

    EXPECT_EQ(aa, ab);
    EXPECT_FALSE(aa == ac);  // EXPECT_NE cannot be used "no match for 'operator!='"
    EXPECT_EQ(aa, aa);
}

// ---------------------------------------------------------------------------
// ArrayEnumTest
// Tests CastToArrayOfEnum, GetEnum.
// ---------------------------------------------------------------------------

TEST(ArrayEnumTest, CastToArrayOfEnum)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "CastToArrayOfEnum casts raw array to Array of enum type");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    enum class Color : int32_t
    {
        Red = 0,
        Green = 1,
        Blue = 2
    };
    int32_t raw[3] = {0, 1, 2};
    auto& arr = CastToArrayOfEnum<Color>(raw);
    EXPECT_EQ(arr.size(), 3u);
    EXPECT_EQ(static_cast<int32_t>(arr.Get(0)), 0);
    EXPECT_EQ(static_cast<int32_t>(arr.Get(1)), 1);
    EXPECT_EQ(static_cast<int32_t>(arr.Get(2)), 2);
}

TEST(ArrayEnumTest, GetEnum)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "GetEnum retrieves element cast to enum type");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    enum class Fruit : int32_t
    {
        Apple = 0,
        Banana = 1,
        Cherry = 2
    };
    int32_t raw[3] = {0, 1, 2};
    auto& arr = CastToArray(raw);
    EXPECT_EQ(arr.GetEnum<Fruit>(0), Fruit::Apple);
    EXPECT_EQ(arr.GetEnum<Fruit>(1), Fruit::Banana);
    EXPECT_EQ(arr.GetEnum<Fruit>(2), Fruit::Cherry);
}

// ---------------------------------------------------------------------------
// ArrayConstIteratorsTest
// Tests cbegin, cend, crbegin, crend.
//
// Note on iterators in flatbuffers (why "const_iterator" behaves oddly):
// Unlike the STL, a flatbuffers::Array exposes no mutable-vs-const iterator
// pair. begin/end/cbegin/cend all return the SAME const_iterator type (and the
// reverse variants the same const_reverse_iterator); there is no non-const
// iterator, even on a non-const Array. Mutation is done via Array::Mutate, not
// through iterators.
//
// What operator* yields depends on the element kind, via
// IndirectHelper<T>::return_type -- it is NOT the STL's `const T&`:
//   * scalar T (e.g. int32_t): returns T BY VALUE -- a copy, not a reference
//     into the buffer. Immutability comes from value semantics, not const.
//   * struct/non-scalar T (e.g. Point): returns const T* -- a pointer into the
//     buffer. Structs are stored inline, so the pointer aliases the buffer;
//     no copy is made. (See ArrayStructTest.IterationYieldsPointers.)
// This is why `*it = x` never compiles: for scalars *it is a prvalue, and for
// structs it is a pointer, not an assignable lvalue element.
// ---------------------------------------------------------------------------

TEST(ArrayConstIteratorsTest, CBeginCEnd)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "cbegin/cend produce read-only forward iterators");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    int32_t raw[3] = {10, 20, 30};
    const auto& arr = CastToArray(raw);

    int32_t expected[] = {10, 20, 30};
    int idx = 0;
    // For scalars, cbegin yields values, not references into the buffer —
    // the buffer cannot be mutated through the iterator.
    static_assert(!std::is_reference_v<decltype(*arr.cbegin())>, "cbegin dereferences to a value, not a reference");
    for (auto it = arr.cbegin(); it != arr.cend(); ++it)
    {
        EXPECT_EQ(*it, expected[idx]);
        ++idx;
    }
    EXPECT_EQ(idx, 3);
}

TEST(ArrayConstIteratorsTest, IteratorTypesMatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description",
                   "begin/end/cbegin/cend share one const_iterator type; the reverse variants share "
                   "one const_reverse_iterator type -- there is no separate mutable iterator");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Deliberately a non-const Array: even so, begin()/end() return const_iterator.
    int32_t raw[3] = {1, 2, 3};
    auto& arr = CastToArray(raw);
    using ArrayType = std::remove_reference_t<decltype(arr)>;

    static_assert(std::is_same_v<decltype(arr.begin()), ArrayType::const_iterator>, "begin must return const_iterator");
    static_assert(std::is_same_v<decltype(arr.end()), ArrayType::const_iterator>, "end must return const_iterator");
    static_assert(std::is_same_v<decltype(arr.cbegin()), ArrayType::const_iterator>,
                  "cbegin must return const_iterator");
    static_assert(std::is_same_v<decltype(arr.cend()), ArrayType::const_iterator>, "cend must return const_iterator");

    static_assert(std::is_same_v<decltype(arr.rbegin()), ArrayType::const_reverse_iterator>,
                  "rbegin must return const_reverse_iterator");
    static_assert(std::is_same_v<decltype(arr.rend()), ArrayType::const_reverse_iterator>,
                  "rend must return const_reverse_iterator");
    static_assert(std::is_same_v<decltype(arr.crbegin()), ArrayType::const_reverse_iterator>,
                  "crbegin must return const_reverse_iterator");
    static_assert(std::is_same_v<decltype(arr.crend()), ArrayType::const_reverse_iterator>,
                  "crend must return const_reverse_iterator");

    SUCCEED();
}

TEST(ArrayConstIteratorsTest, CRBeginCREnd)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "crbegin/crend produce const reverse iterators");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "mutable Data() returns writable uint8_t pointer");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "mutable data() returns writable T* pointer");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "GetMutablePointer on struct array returns writable pointer");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "Mutate replaces a struct element in place, leaving others unchanged");
    RecordProperty("TestType", "interface-test");
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

TEST(ArrayStructTest, IterationYieldsPointers)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "for non-scalar element types operator* yields a const T* into the buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Point raw[3] = {{1, 2}, {3, 4}, {5, 6}};
    const auto& arr = CastToArray(raw);

    // Unlike scalar arrays (operator* returns T by value) and the STL
    // (operator* returns const T&), a struct Array's iterator dereferences
    // to IndirectHelper<T>::return_type == const T* -- a pointer into the
    // buffer. Elements are stored inline, so the pointer aliases the source.
    static_assert(std::is_pointer_v<decltype(*arr.begin())>, "struct iterator dereferences to a pointer");
    static_assert(std::is_same_v<decltype(*arr.begin()), const Point*>, "struct iterator dereferences to const T*");

    int idx = 0;
    const Point expected[] = {{1, 2}, {3, 4}, {5, 6}};
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        const Point* p = *it;
        ASSERT_NE(p, nullptr);
        EXPECT_EQ(*p, expected[idx]);
        // the pointer aliases the underlying buffer, no copy is made
        EXPECT_EQ(p, &raw[idx]);
        ++idx;
    }
    EXPECT_EQ(idx, 3);
}

// ---------------------------------------------------------------------------
// ArrayCopyFromSpanTest
// Tests Array<T, N>::CopyFromSpan.
// ---------------------------------------------------------------------------

TEST(ArrayCopyFromSpanTest, Scalar)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "CopyFromSpan with scalar (span-observable) path");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "CopyFromSpan with struct type");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "make_span with const Array");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const int32_t raw[3] = {5, 10, 15};
    const auto& arr = CastToArray(raw);
    auto s = make_span(arr);

    // make_span on a const Array yields a read-only span: its element type is
    // const, so elements cannot be written through it.
    static_assert(std::is_const_v<decltype(s)::element_type>,
                  "make_span(const Array) must produce a span of const elements");
    static_assert(std::is_const_v<std::remove_reference_t<decltype(s[0])>>,
                  "operator[] on a const span yields a const reference");

    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s[0], 5);
    EXPECT_EQ(s[1], 10);
    EXPECT_EQ(s[2], 15);
}

TEST(ArrayConstSpanTest, MakeBytesSpanConst)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "make_bytes_span with const Array");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const int32_t raw[2] = {1, 2};
    const auto& arr = CastToArray(raw);
    auto bs = make_bytes_span(arr);
    EXPECT_EQ(bs.size(), 2u * sizeof(int32_t));
    EXPECT_EQ(bs.data(), arr.Data());
}

TEST(ArrayConstSpanTest, ConstCastToArrayOfEnum)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "const CastToArrayOfEnum");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    enum class Color : int32_t
    {
        Red = 0,
        Green = 1,
        Blue = 2
    };
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "is_span_observable static member for scalar and struct types");
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "CopyFromSpanImpl(true_type) — memcpy path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    int32_t raw[3] = {0, 0, 0};
    auto& accessor = *reinterpret_cast<ArrayTestAccess<int32_t, 3>*>(raw);

    const int32_t src[3] = {10, 20, 30};
    span<const int32_t, 3> src_span(src, 3);
    accessor.CallCopyObservable(src_span);

    EXPECT_EQ(raw[0], 10);
    EXPECT_EQ(raw[1], 20);
    EXPECT_EQ(raw[2], 30);
}

TEST(CopyFromSpanImplTest, NonObservableScalar)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "CopyFromSpanImpl(false_type) — element-wise Mutate path for scalars");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    int32_t raw[3] = {0, 0, 0};
    auto& accessor = *reinterpret_cast<ArrayTestAccess<int32_t, 3>*>(raw);

    const int32_t src[3] = {100, 200, 300};
    span<const int32_t, 3> src_span(src, 3);
    accessor.CallCopyNonObservable(src_span);

    EXPECT_EQ(raw[0], 100);
    EXPECT_EQ(raw[1], 200);
    EXPECT_EQ(raw[2], 300);
}

TEST(CopyFromSpanImplTest, NonObservableStruct)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "CopyFromSpanImpl(false_type) with struct type");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Point raw[2] = {{0, 0}, {0, 0}};
    auto& accessor = *reinterpret_cast<ArrayTestAccess<Point, 2>*>(raw);

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
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty(
        "Description",
        "Data() on the Array<Offset<void>,N> specialization exposes the underlying offset bytes for reading");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    constexpr uint16_t kCount = 2;
    const uint8_t expected[kCount * sizeof(uoffset_t)] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    alignas(Array<Offset<void>, kCount>) uint8_t buf[sizeof(expected)];
    std::memcpy(buf, expected, sizeof(buf));

    const auto& arr = *reinterpret_cast<const Array<Offset<void>, kCount>*>(buf);

    // Data() points at the underlying storage...
    ASSERT_EQ(arr.Data(), buf);
    // ...and every raw offset byte is readable through it.
    EXPECT_EQ(std::memcmp(arr.Data(), expected, sizeof(expected)), 0);
}

// ---------------------------------------------------------------------------
// ArrayFaultInjectionTest
// Fault injection tests for Array.
// ---------------------------------------------------------------------------

TEST(ArrayFaultInjectionTest, OffsetSpecializationOperatorIndexDeathTest)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "Array<Offset<void>, N>::operator[] triggers assert(false) at runtime");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    alignas(Array<Offset<void>, 2>) uint8_t buf[sizeof(Array<Offset<void>, 2>)] = {};
    const auto& arr = *reinterpret_cast<const Array<Offset<void>, 2>*>(buf);
    EXPECT_DEATH({ arr[0]; }, "");
}

TEST(ArrayFaultInjectionTest, FaultGetOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "Get() with index == size triggers assert(false)");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    int32_t raw[3] = {1, 2, 3};
    const auto& arr = CastToArray(raw);
    EXPECT_DEATH({ arr.Get(3u); }, "");
}

TEST(ArrayFaultInjectionTest, FaultGetMutablePointerOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "GetMutablePointer() with index == size triggers assert(false)");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    Point raw[2] = {{1, 2}, {3, 4}};
    auto& arr = CastToArray(raw);
    EXPECT_DEATH({ arr.GetMutablePointer(2u); }, "");
}

TEST(ArrayFaultInjectionTest, FaultMutateScalarOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "Mutate() with index == size triggers assert(false)");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    int32_t raw[3] = {0, 0, 0};
    auto& arr = CastToArray(raw);
    EXPECT_DEATH({ arr.Mutate(3u, 42); }, "");
}

TEST(ArrayFaultInjectionTest, FaultMutateStructOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "Mutate() with index == size on struct triggers assert(false) via false_type path");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    Point raw[2] = {{0, 0}, {0, 0}};
    auto& arr = CastToArray(raw);
    const Point p = {1, 2};
    EXPECT_DEATH({ arr.Mutate(2u, p); }, "");
}

TEST(ArrayFaultInjectionTest, FaultCopyFromSpanOverlap)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "CopyFromSpan with a source span overlapping the array triggers assert(false)");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    int32_t raw[3] = {1, 2, 3};
    auto& arr = CastToArray(raw);
    span<const int32_t, 3> overlapping(raw, 3);  // shares storage with arr -> p1 == p2
    EXPECT_DEATH({ arr.CopyFromSpan(overlapping); }, "");
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score
