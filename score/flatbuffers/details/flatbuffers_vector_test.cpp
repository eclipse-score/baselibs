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
#include "flatbuffers/vector.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <utility>
#include <vector>

#include "flatbuffers/flatbuffer_builder.h"
#include "gtest/gtest.h"
#include "score/flatbuffers/details/vector_lookup_fixture_generated.h"
#include "score/quality/compiler_warnings/warnings.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

// These tests compare against native values, so they assume a little-endian host, matching FlatBuffers' wire format.
static_assert(FLATBUFFERS_LITTLEENDIAN, "flatbuffers vector tests assume a little-endian host");

using namespace ::flatbuffers;

// The slot number for a table's first field. The test tables built in this file (e.g.
// BuildScalarVector below) have only one field and no .fbs schema, so flatc can't generate this
// slot number for us; we hardcode it here instead, everywhere that field is written or read.
constexpr ::flatbuffers::voffset_t kSingleFieldSlot = 4;

namespace
{

// Builds a table whose slot kSingleFieldSlot holds the given vector offset, finishes the
// buffer, and returns a pointer to the vector rooted in `fbb`'s buffer. The
// FlatBufferBuilder must outlive the returned pointer (it owns the buffer).
template <typename T>
const Vector<T>* BuildScalarVector(FlatBufferBuilder& fbb, const std::vector<T>& data)
{
    auto vec = fbb.CreateVector(data);
    auto start = fbb.StartTable();
    fbb.AddOffset(kSingleFieldSlot, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    const auto* table = GetRoot<Table>(fbb.GetBufferPointer());
    return table->GetPointer<const Vector<T>*>(kSingleFieldSlot);
}

// Round-trips a scalar vector and checks size, ordered access, and both valid
// index boundaries (0 and size()-1). Used to give equivalent coverage across a
// range of scalar element types with their min/zero/max boundary values.
template <typename T>
void CheckScalarRoundTrip(const std::vector<T>& data)
{
    FlatBufferBuilder fbb(256);
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);
    ASSERT_EQ(v->size(), data.size());
    for (uint32_t i = 0; i < data.size(); ++i)
    {
        EXPECT_EQ(v->Get(i), data[i]) << "mismatch at index " << i;
    }
    // Boundary indices.
    EXPECT_EQ(v->Get(0), data.front());
    EXPECT_EQ(v->Get(static_cast<uint32_t>(data.size() - 1)), data.back());
}

enum class Color : int32_t
{
    kRed = 10,
    kGreen = 20,
    kBlue = 30,
};

// Builds a Container holding a key-sorted vector of Item{id, label} using the
// generated fixture schema. Items are inserted out of key order on purpose so
// CreateVectorOfSortedTables() (and hence LookupByKey's precondition) is
// actually exercised. The FlatBufferBuilder must outlive the returned pointer.
// Hand-writing Item instead of generating it from the schema would work too,
// but it would just reimplement what flatc does. This approach is far less
// error-prone.
const fixture::Container* BuildSortedItemContainer(FlatBufferBuilder& fbb)
{
    std::vector<Offset<fixture::Item>> items = {
        fixture::CreateItemDirect(fbb, 30, "thirty"),
        fixture::CreateItemDirect(fbb, 10, "ten"),
        fixture::CreateItemDirect(fbb, 20, "twenty"),
    };
    auto items_vec = fbb.CreateVectorOfSortedTables(&items);
    auto container = fixture::CreateContainer(fbb, items_vec);
    fbb.Finish(container);
    return fixture::GetContainer(fbb.GetBufferPointer());
}

}  // namespace

// ---------------------------------------------------------------------------
// VectorGetTest
// ---------------------------------------------------------------------------

TEST(VectorGetTest, Scalars)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access, comp_req__flatbuffers__serialization");
    RecordProperty("Description", "Get, operator[] access elements; size/empty report correct state");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {10, 20, 30, 40, 50};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    EXPECT_EQ(v->size(), 5U);
    EXPECT_FALSE(v->empty());
    EXPECT_EQ(v->Get(0), 10);  // lower boundary
    EXPECT_EQ(v->Get(1), 20);
    EXPECT_EQ(v->Get(2), 30);
    EXPECT_EQ(v->Get(3), 40);
    EXPECT_EQ(v->Get(4), 50);  // upper boundary
    EXPECT_EQ((*v)[0], 10);    // lower boundary
    EXPECT_EQ((*v)[1], 20);
    EXPECT_EQ((*v)[2], 30);
    EXPECT_EQ((*v)[3], 40);
    EXPECT_EQ((*v)[4], 50);  // upper boundary
}

// ---------------------------------------------------------------------------
// VectorEmptyTest
// ---------------------------------------------------------------------------

TEST(VectorEmptyTest, ZeroElements)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access, comp_req__flatbuffers__serialization");
    RecordProperty("Description", "empty vector has size 0, empty() is true, and begin()==end()");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data;
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 0U);
    EXPECT_TRUE(v->empty());
    // Boundary: iteration over an empty vector must be a no-op.
    EXPECT_TRUE(v->begin() == v->end());
    EXPECT_EQ(v->end() - v->begin(), 0);
    // Same boundary, reverse direction.
    EXPECT_TRUE(v->rbegin() == v->rend());
}

// ---------------------------------------------------------------------------
// VectorLengthTest
// ---------------------------------------------------------------------------

TEST(VectorLengthTest, NullAndNotNull)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "null/empty vector returns 0, non-null returns actual size");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // null vector
    EXPECT_EQ(VectorLength<int32_t>(nullptr), 0U);

    // empty vector
    FlatBufferBuilder fbb_empty(256);
    const std::vector<int32_t> data_empty{};
    const auto* empty_vec = BuildScalarVector(fbb_empty, data_empty);
    EXPECT_EQ(VectorLength(empty_vec), 0U);

    // non-null
    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {1, 2, 3};
    const auto* v = BuildScalarVector(fbb, data);
    EXPECT_EQ(VectorLength(v), 3U);
}

// ---------------------------------------------------------------------------
// VectorDeprecatedLengthTest
// ---------------------------------------------------------------------------

TEST(VectorDeprecatedLengthTest, MatchesSize)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "deprecated Length() returns the same value as size()");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // empty vector
    FlatBufferBuilder fbb_empty(256);
    const std::vector<int32_t> data_empty{};
    const auto* empty_vec = BuildScalarVector(fbb_empty, data_empty);
    ASSERT_NE(empty_vec, nullptr);

    // clang-format off
    DISABLE_WARNING_PUSH
    DISABLE_WARNING(-Wdeprecated-declarations)
    EXPECT_EQ(empty_vec->Length(), empty_vec->size());
    DISABLE_WARNING_POP
    // clang-format on

    // not empty vector
    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {7, 8, 9, 10};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    // clang-format off
    DISABLE_WARNING_PUSH
    DISABLE_WARNING(-Wdeprecated-declarations)
    EXPECT_EQ(v->Length(), v->size());
    DISABLE_WARNING_POP
    // clang-format on
}

// ---------------------------------------------------------------------------
// VectorMutateTest
// ---------------------------------------------------------------------------

TEST(VectorMutateTest, InPlace)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "in-place mutation of vector elements including type extremes");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {100, 200, 300};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<int32_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    v->Mutate(0, 999);
    EXPECT_EQ(v->Get(0), 999);
    v->Mutate(2, 0);
    EXPECT_EQ(v->Get(2), 0);
    // Boundary values: type min and max.
    v->Mutate(1, std::numeric_limits<int32_t>::max());
    EXPECT_EQ(v->Get(1), std::numeric_limits<int32_t>::max());
    v->Mutate(1, std::numeric_limits<int32_t>::min());
    EXPECT_EQ(v->Get(1), std::numeric_limits<int32_t>::min());
}

// ---------------------------------------------------------------------------
// VectorScalarTypesTest
// ---------------------------------------------------------------------------

TEST(VectorScalarTypesTest, MinZeroMaxAcrossTypes)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access, comp_req__flatbuffers__serialization");
    RecordProperty("Description", "round-trip of representative scalar types at min/zero/max boundaries");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    CheckScalarRoundTrip<int8_t>({std::numeric_limits<int8_t>::min(), 0, std::numeric_limits<int8_t>::max()});
    CheckScalarRoundTrip<uint8_t>({0, 0x7F, std::numeric_limits<uint8_t>::max()});
    CheckScalarRoundTrip<int16_t>({std::numeric_limits<int16_t>::min(), 0, std::numeric_limits<int16_t>::max()});
    CheckScalarRoundTrip<uint16_t>({0, 0x7FFF, std::numeric_limits<uint16_t>::max()});
    CheckScalarRoundTrip<int32_t>({std::numeric_limits<int32_t>::min(), 0, std::numeric_limits<int32_t>::max()});
    CheckScalarRoundTrip<uint32_t>({0u, 0x7FFFFFFFu, std::numeric_limits<uint32_t>::max()});
    CheckScalarRoundTrip<int64_t>({std::numeric_limits<int64_t>::min(), 0, std::numeric_limits<int64_t>::max()});
    CheckScalarRoundTrip<uint64_t>({0u, 0x7FFFFFFFFFFFFFFFull, std::numeric_limits<uint64_t>::max()});
    CheckScalarRoundTrip<float>({std::numeric_limits<float>::lowest(), 0.0f, std::numeric_limits<float>::max()});
    CheckScalarRoundTrip<double>({std::numeric_limits<double>::lowest(), 0.0, std::numeric_limits<double>::max()});
}

// ---------------------------------------------------------------------------
// VectorEnumTest
// ---------------------------------------------------------------------------

TEST(VectorEnumTest, GetEnum)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetEnum reinterprets the stored scalar as the requested enum type");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {10, 20, 30};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    EXPECT_EQ(v->GetEnum<Color>(0), Color::kRed);
    EXPECT_EQ(v->GetEnum<Color>(1), Color::kGreen);
    EXPECT_EQ(v->GetEnum<Color>(2), Color::kBlue);
}

// ---------------------------------------------------------------------------
// VectorDataTest
// ---------------------------------------------------------------------------

TEST(VectorDataTest, RawAndTypedPointers)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "Data()/data()/GetStructFromOffset expose the backing storage consistently");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {11, 22, 33};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    // Typed data() must alias the raw Data() and agree with Get().
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(v->data()), v->Data());
    EXPECT_EQ(v->data()[0], v->Get(0));
    EXPECT_EQ(v->data()[2], v->Get(2));

    // GetStructFromOffset(o) returns Data()+o.
    EXPECT_EQ(v->GetStructFromOffset(0), static_cast<const void*>(v->Data()));
    EXPECT_EQ(v->GetStructFromOffset(sizeof(int32_t)), static_cast<const void*>(v->Data() + sizeof(int32_t)));
}

TEST(VectorDataTest, MutableRawAndTypedPointers)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "non-const Data()/data() overloads expose writable backing storage");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {11, 22, 33};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<int32_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    // Non-const data() aliases non-const Data(), same as the const overloads.
    EXPECT_EQ(reinterpret_cast<uint8_t*>(v->data()), v->Data());
    EXPECT_EQ(v->Get(0), 11);

    // Writable: mutate through the raw pointer and observe via Get().
    v->data()[0] = 99;
    EXPECT_EQ(v->Get(0), 99);
}

// ---------------------------------------------------------------------------
// VectorForwardIteratorTest
// ---------------------------------------------------------------------------

TEST(VectorForwardIteratorTest, RangeBased)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "forward iteration collects elements in order");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {5, 10, 15, 20};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    std::vector<int32_t> collected;
    for (auto it = v->begin(); it != v->end(); ++it)
    {
        collected.push_back(*it);
    }
    ASSERT_EQ(collected.size(), 4U);
    EXPECT_EQ(collected[0], 5);
    EXPECT_EQ(collected[1], 10);
    EXPECT_EQ(collected[2], 15);
    EXPECT_EQ(collected[3], 20);
}

// ---------------------------------------------------------------------------
// VectorReverseIteratorTest
// ---------------------------------------------------------------------------

TEST(VectorReverseIteratorTest, RangeBased)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "reverse iteration collects elements in reverse order");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {1, 2, 3};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    std::vector<int32_t> rev;
    for (auto it = v->rbegin(); it != v->rend(); ++it)
    {
        rev.push_back(*it);
    }
    ASSERT_EQ(rev.size(), 3U);
    EXPECT_EQ(rev[0], 3);
    EXPECT_EQ(rev[1], 2);
    EXPECT_EQ(rev[2], 1);
}

TEST(VectorReverseIteratorTest, ArrowOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "const_reverse_iterator::operator-> reaches through to the pointee");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    // As with the forward iterator (see VectorOfStringsTest.IteratorArrow),
    // operator->() is only meaningful when the element type is pointer-like.
    FlatBufferBuilder fbb(256);
    auto s1 = fbb.CreateString("alpha");
    auto s2 = fbb.CreateString("beta");
    const std::vector<Offset<String>> str_offsets = {s1, s2};
    auto vec = fbb.CreateVector(str_offsets);
    auto start = fbb.StartTable();
    fbb.AddOffset(kSingleFieldSlot, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    const auto* table = GetRoot<Table>(fbb.GetBufferPointer());
    const auto* v = table->GetPointer<const Vector<Offset<String>>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    auto rit = v->rbegin();
    EXPECT_STREQ(rit->c_str(), "beta");
    ++rit;
    EXPECT_STREQ(rit->c_str(), "alpha");
}

// ---------------------------------------------------------------------------
// VectorMutableIteratorTest
// Tests the non-const begin()/end()/rbegin()/rend() overloads, which return
// (reverse_)iterator rather than const_(reverse_)iterator. Every other
// iterator test above goes through a const Vector<T>* and so only exercises
// the const overloads.
// ---------------------------------------------------------------------------

TEST(VectorMutableIteratorTest, BeginEndRBeginREnd)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "non-const begin/end/rbegin/rend return mutable iterators over the vector");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {1, 2, 3};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<int32_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    std::vector<int32_t> fwd;
    for (auto it = v->begin(); it != v->end(); ++it)
    {
        fwd.push_back(*it);
    }
    ASSERT_EQ(fwd.size(), 3U);
    EXPECT_EQ(fwd.front(), 1);
    EXPECT_EQ(fwd.back(), 3);

    std::vector<int32_t> rev;
    for (auto it = v->rbegin(); it != v->rend(); ++it)
    {
        rev.push_back(*it);
    }
    ASSERT_EQ(rev.size(), 3U);
    EXPECT_EQ(rev.front(), 3);
    EXPECT_EQ(rev.back(), 1);
}

// ---------------------------------------------------------------------------
// VectorConstIteratorTest
// ---------------------------------------------------------------------------

TEST(VectorConstIteratorTest, CbeginCendCrbeginCrend)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "const iterator accessors traverse forward and reverse");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {2, 4, 6, 8};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    std::vector<int32_t> fwd;
    for (auto it = v->cbegin(); it != v->cend(); ++it)
    {
        fwd.push_back(*it);
    }
    ASSERT_EQ(fwd.size(), 4U);
    EXPECT_EQ(fwd.front(), 2);
    EXPECT_EQ(fwd.back(), 8);

    std::vector<int32_t> rev;
    for (auto it = v->crbegin(); it != v->crend(); ++it)
    {
        rev.push_back(*it);
    }
    ASSERT_EQ(rev.size(), 4U);
    EXPECT_EQ(rev.front(), 8);
    EXPECT_EQ(rev.back(), 2);
}

// ---------------------------------------------------------------------------
// VectorIteratorTest
// One test case per VectorIterator (vector.h) member. operator->() is
// exercised separately by VectorOfStringsTest.IteratorArrow and
// VectorReverseIteratorTest.ArrowOperator, since it only compiles/is
// meaningful for pointer-like element types, unlike the scalar vector used
// throughout this suite.
// ---------------------------------------------------------------------------

class VectorIteratorTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        v_ = BuildScalarVector(fbb_, data_);
        ASSERT_NE(v_, nullptr);
    }

    FlatBufferBuilder fbb_{256};
    const std::vector<int32_t> data_ = {10, 20, 30, 40};
    const Vector<int32_t>* v_ = nullptr;
};

TEST_F(VectorIteratorTest, DefaultConstructor)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "VectorIterator() default-constructs a null iterator");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    using Iter = Vector<int32_t>::const_iterator;

    Iter a;
    Iter b;
    EXPECT_TRUE(a == b);
}

TEST_F(VectorIteratorTest, CopyConstructor)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "VectorIterator(const VectorIterator&) copies the source position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto source = v_->begin() + 1;
    Vector<int32_t>::const_iterator copy(source);
    EXPECT_TRUE(copy == source);
    EXPECT_EQ(*copy, 20);
}

TEST_F(VectorIteratorTest, CopyAssignment)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator=(const VectorIterator&) repoints an existing iterator");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    using Iter = Vector<int32_t>::const_iterator;
    // The source must be an lvalue: an rvalue (e.g. `v_->begin()` directly) would
    // instead bind to the move-assignment overload below.
    Iter source = v_->begin();
    Iter target;
    target = source;
    EXPECT_TRUE(target == source);
    EXPECT_EQ(*target, 10);
}

TEST_F(VectorIteratorTest, MoveAssignment)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator=(VectorIterator&&) repoints an existing iterator");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    using Iter = Vector<int32_t>::const_iterator;
    Iter source = v_->begin() + 1;
    Iter target;
    target = std::move(source);
    EXPECT_EQ(*target, 20);
}

TEST_F(VectorIteratorTest, EqualityOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator== compares iterator positions");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_TRUE(v_->begin() == v_->begin());
    EXPECT_FALSE(v_->begin() == v_->end());
}

TEST_F(VectorIteratorTest, InequalityOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator!= compares iterator positions");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_TRUE(v_->begin() != v_->end());
    EXPECT_FALSE(v_->begin() != v_->begin());
}

TEST_F(VectorIteratorTest, LessThanOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator< orders iterators by position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_TRUE(v_->begin() < v_->end());
    EXPECT_FALSE(v_->end() < v_->begin());
}

TEST_F(VectorIteratorTest, GreaterThanOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator> orders iterators by position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_TRUE(v_->end() > v_->begin());
    EXPECT_FALSE(v_->begin() > v_->end());
}

TEST_F(VectorIteratorTest, LessOrEqualOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator<= orders iterators by position, inclusive of equality");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_TRUE(v_->begin() <= v_->begin());
    EXPECT_TRUE(v_->begin() <= v_->end());
    EXPECT_FALSE(v_->end() <= v_->begin());
}

TEST_F(VectorIteratorTest, GreaterOrEqualOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator>= orders iterators by position, inclusive of equality");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_TRUE(v_->end() >= v_->begin());
    EXPECT_TRUE(v_->begin() >= v_->begin());
    EXPECT_FALSE(v_->begin() >= v_->end());
}

TEST_F(VectorIteratorTest, DifferenceOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator-(iterator) returns the distance between two positions");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_EQ(v_->end() - v_->begin(), 4);
}

TEST_F(VectorIteratorTest, DereferenceOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator* reads the element at the iterator's position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    EXPECT_EQ(*v_->begin(), 10);
    EXPECT_EQ(*(v_->begin() + 2), 30);
}

TEST_F(VectorIteratorTest, PreIncrementOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator++() advances the iterator and returns the new position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto it = v_->begin();
    auto& result = ++it;
    EXPECT_EQ(&result, &it);
    EXPECT_EQ(*it, 20);
}

TEST_F(VectorIteratorTest, PostIncrementOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator++(int) advances the iterator and returns the previous position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto it = v_->begin();
    auto prev = it++;
    EXPECT_EQ(*prev, 10);
    EXPECT_EQ(*it, 20);
}

TEST_F(VectorIteratorTest, PreDecrementOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator--() moves the iterator back and returns the new position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto it = v_->begin() + 1;
    auto& result = --it;
    EXPECT_EQ(&result, &it);
    EXPECT_EQ(*it, 10);
}

TEST_F(VectorIteratorTest, PostDecrementOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator--(int) moves the iterator back and returns the previous position");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto it = v_->begin() + 1;
    auto prev = it--;
    EXPECT_EQ(*prev, 20);
    EXPECT_EQ(*it, 10);
}

TEST_F(VectorIteratorTest, PlusOffsetOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator+(offset) returns an advanced iterator, leaving the original unchanged");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto it = v_->begin();
    auto advanced = it + 2;
    EXPECT_EQ(*advanced, 30);
    EXPECT_EQ(*it, 10);
}

TEST_F(VectorIteratorTest, PlusEqualsOffsetOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator+=(offset) advances the iterator in place");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto it = v_->begin();
    it += 3;
    EXPECT_EQ(*it, 40);
}

TEST_F(VectorIteratorTest, MinusOffsetOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator-(offset) returns a receded iterator, leaving the original unchanged");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto end = v_->end();
    auto last = end - 1;
    EXPECT_EQ(*last, 40);
    EXPECT_EQ(end - v_->begin(), 4);
}

TEST_F(VectorIteratorTest, MinusEqualsOffsetOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "operator-=(offset) moves the iterator back in place");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    auto it = v_->begin() + 3;
    it -= 2;
    EXPECT_EQ(*it, 20);
}

// ---------------------------------------------------------------------------
// VectorMakeSpanTest
// ---------------------------------------------------------------------------

TEST(VectorMakeSpanTest, SpanOverData)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "make_span creates a span over vector data");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<uint8_t> data = {0xAA, 0xBB, 0xCC};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    auto s = make_span(*v);
    EXPECT_EQ(s.size(), 3U);
    EXPECT_EQ(s[0], 0xAA);
    EXPECT_EQ(s[1], 0xBB);
    EXPECT_EQ(s[2], 0xCC);
}

TEST(VectorMakeSpanTest, PointerOverloadNullAndNonNull)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "pointer overload yields an empty span for null and a full span otherwise");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Null path: this is the safety-relevant guard.
    const Vector<uint8_t>* null_vec = nullptr;
    auto empty = make_span(null_vec);
    EXPECT_EQ(empty.size(), 0U);

    // Non-null path.
    FlatBufferBuilder fbb(256);
    const std::vector<uint8_t> data = {0x01, 0x02};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);
    auto s = make_span(v);
    ASSERT_EQ(s.size(), 2U);
    EXPECT_EQ(s[0], 0x01);
    EXPECT_EQ(s[1], 0x02);
}

TEST(VectorMakeSpanTest, MutableSpanOverData)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "non-const make_span(Vector<U>&) yields a writable span aliasing the vector");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<uint8_t> data = {0xAA, 0xBB, 0xCC};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<uint8_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    auto s = make_span(*v);
    ASSERT_EQ(s.size(), 3U);
    EXPECT_EQ(v->Get(0), 0xAA);
    s[0] = 0x11;
    EXPECT_EQ(v->Get(0), 0x11);
    EXPECT_EQ(v->Get(1), 0xBB);
    EXPECT_EQ(v->Get(2), 0xCC);
}

TEST(VectorMakeSpanTest, MutablePointerOverloadNullAndNonNull)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "non-const pointer overload of make_span yields an empty span for null and a full span otherwise");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Null path: this is the safety-relevant guard.
    Vector<uint8_t>* null_vec = nullptr;
    auto empty = make_span(null_vec);
    EXPECT_EQ(empty.size(), 0U);

    // Non-null path.
    FlatBufferBuilder fbb(256);
    const std::vector<uint8_t> data = {0x01, 0x02};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<uint8_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);
    auto s = make_span(v);
    ASSERT_EQ(s.size(), 2U);
    EXPECT_EQ(v->Get(0), 0x01);
    EXPECT_EQ(v->Get(1), 0x02);
    s[1] = 0x99;
    EXPECT_EQ(v->Get(1), 0x99);
}

TEST(VectorMakeBytesSpanTest, ByteView)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access, comp_req__flatbuffers__serialization");
    RecordProperty("Description", "make_bytes_span exposes size()*sizeof(T) raw bytes");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {0x04030201, 0x08070605};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    auto bytes = make_bytes_span(*v);
    ASSERT_EQ(bytes.size(), data.size() * sizeof(int32_t));
    // Little-endian layout of the first element.
    EXPECT_EQ(bytes[0], 0x01);
    EXPECT_EQ(bytes[1], 0x02);
    EXPECT_EQ(bytes[2], 0x03);
    EXPECT_EQ(bytes[3], 0x04);
    // Little-endian layout of the second element.
    EXPECT_EQ(bytes[4], 0x05);
    EXPECT_EQ(bytes[5], 0x06);
    EXPECT_EQ(bytes[6], 0x07);
    EXPECT_EQ(bytes[7], 0x08);
}

TEST(VectorMakeBytesSpanTest, MutableByteView)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "non-const make_bytes_span(Vector<U>&) yields a writable byte span");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {0x04030201, 0x08070605};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<int32_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    auto bytes = make_bytes_span(*v);
    ASSERT_EQ(bytes.size(), data.size() * sizeof(int32_t));
    bytes[0] = 0xFF;
    bytes[1] = 0xFF;
    bytes[2] = 0xFF;
    bytes[3] = 0xFF;
    // Little-endian: byte 0 is the low byte of the first element.
    EXPECT_EQ(v->Get(0), static_cast<int32_t>(0xFFFFFFFF));
    bytes[4] = 0xAA;
    bytes[5] = 0xAA;
    bytes[6] = 0xAA;
    bytes[7] = 0xAA;

    EXPECT_EQ(v->Get(1), static_cast<int32_t>(0xAAAAAAAA));
}

// ---------------------------------------------------------------------------
// VectorOfStringsTest
// ---------------------------------------------------------------------------

TEST(VectorOfStringsTest, Offsets)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access, comp_req__flatbuffers__serialization");
    RecordProperty("Description", "vector of Offset<String> elements accessed via Get/GetAs/GetAsString");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    auto s1 = fbb.CreateString("alpha");
    auto s2 = fbb.CreateString("beta");
    auto s3 = fbb.CreateString("gamma");
    const std::vector<Offset<String>> str_offsets = {s1, s2, s3};
    auto vec = fbb.CreateVector(str_offsets);
    auto start = fbb.StartTable();
    fbb.AddOffset(kSingleFieldSlot, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    const auto* table = GetRoot<Table>(fbb.GetBufferPointer());
    const auto* v = table->GetPointer<const Vector<Offset<String>>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 3U);
    EXPECT_STREQ(v->Get(0)->c_str(), "alpha");
    EXPECT_STREQ(v->Get(1)->c_str(), "beta");
    EXPECT_STREQ(v->Get(2)->c_str(), "gamma");

    // GetAs<String>/GetAsString reinterpret the same element pointer.
    EXPECT_EQ(static_cast<const void*>(v->GetAs<String>(1)), static_cast<const void*>(v->Get(1)));
    EXPECT_EQ(static_cast<const void*>(v->GetAsString(1)), static_cast<const void*>(v->Get(1)));
    EXPECT_STREQ(v->GetAsString(1)->c_str(), "beta");
}

TEST(VectorOfStringsTest, IteratorArrow)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "const_iterator::operator-> reaches through to the pointee for pointer-like IT");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    // operator->() only compiles/is meaningful when IT is pointer-like (as for
    // Offset<String>/Offset<Table> element types); for scalar vectors IT is the
    // scalar itself and `it->` has no members to reach through.
    FlatBufferBuilder fbb(256);
    auto s1 = fbb.CreateString("alpha");
    auto s2 = fbb.CreateString("beta");
    const std::vector<Offset<String>> str_offsets = {s1, s2};
    auto vec = fbb.CreateVector(str_offsets);
    auto start = fbb.StartTable();
    fbb.AddOffset(kSingleFieldSlot, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    const auto* table = GetRoot<Table>(fbb.GetBufferPointer());
    const auto* v = table->GetPointer<const Vector<Offset<String>>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    auto it = v->begin();
    EXPECT_STREQ(it->c_str(), "alpha");
    ++it;
    EXPECT_STREQ(it->c_str(), "beta");
}

// ---------------------------------------------------------------------------
// VectorMutableObjectTest
// ---------------------------------------------------------------------------

TEST(VectorMutableObjectTest, GetMutableObjectAndMutateOffset)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "mutable object access and repointing an offset element");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    auto s1 = fbb.CreateString("first");
    auto s2 = fbb.CreateString("second");
    const std::vector<Offset<String>> str_offsets = {s1, s2};
    auto vec = fbb.CreateVector(str_offsets);
    auto start = fbb.StartTable();
    fbb.AddOffset(kSingleFieldSlot, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<Offset<String>>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    // GetMutableObject returns a mutable view of the same element as Get.
    ASSERT_NE(v->GetMutableObject(0), nullptr);
    EXPECT_EQ(static_cast<const void*>(v->GetMutableObject(0)), static_cast<const void*>(v->Get(0)));
    EXPECT_STREQ(v->GetMutableObject(0)->c_str(), "first");

    // Repoint element 0 at the target of element 1; both must then resolve equal.
    const auto* target = reinterpret_cast<const uint8_t*>(v->Get(1));
    v->MutateOffset(0, target);
    EXPECT_STREQ(v->Get(0)->c_str(), "second");
    EXPECT_STREQ(v->Get(1)->c_str(), "second");
}

// ---------------------------------------------------------------------------
// Vector64Test
// ---------------------------------------------------------------------------

TEST(Vector64Test, SizeGetAndIterate)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "Vector64 (64-bit size field) reports size and reads elements");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Vector64<T> is Vector<T, uoffset64_t>: an 8-byte little-endian length field
    // followed by the elements. Like every flatbuffers vector it is only ever
    // used as a reinterpret_cast view over an existing buffer (see GetRoot /
    // CastToArray), so we lay out that buffer by hand rather than standing up a
    // full 64-bit FlatBufferBuilder buffer just to reach the accessors.
    constexpr uint64_t kCount = 3;
    // Buffer size: 8-byte length field + kCount elements, with no gap between them.
    alignas(alignof(uint64_t)) uint8_t buf[sizeof(uint64_t) + kCount * sizeof(int32_t)] = {};
    const uint64_t length = kCount;
    std::memcpy(buf, &length, sizeof(length));
    const int32_t elems[kCount] = {111, 222, 333};
    std::memcpy(buf + sizeof(uint64_t), elems, sizeof(elems));

    const auto* v = reinterpret_cast<const Vector64<int32_t>*>(buf);
    EXPECT_EQ(v->size(), kCount);
    EXPECT_FALSE(v->empty());
    EXPECT_EQ(v->Get(0), 111);  // lower boundary
    EXPECT_EQ(v->Get(2), 333);  // upper boundary
    EXPECT_EQ(v->end() - v->begin(), 3);
}

TEST(Vector64Test, Empty)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "Vector64 with a zero length field is empty");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    alignas(alignof(uint64_t)) uint8_t buf[sizeof(uint64_t)] = {};  // length == 0
    const auto* v = reinterpret_cast<const Vector64<int32_t>*>(buf);
    EXPECT_EQ(v->size(), 0U);
    EXPECT_TRUE(v->empty());
}

// ---------------------------------------------------------------------------
// VectorOfAnyTest
// ---------------------------------------------------------------------------

TEST(VectorOfAnyTest, SizeAndDataAliasTypedVector)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "VectorOfAny views the same length/data layout as a typed vector");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {7, 8, 9, 10};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    // VectorOfAny is the type-erased view to inspect a vector
    // without knowing its element type; it shares the [length][data]
    // layout of any Vector<T>.
    const auto* any = reinterpret_cast<const VectorOfAny*>(v);
    EXPECT_EQ(any->size(), v->size());
    EXPECT_EQ(any->Data(), v->Data());
}

TEST(VectorOfAnyTest, MutableDataAliasesTypedVector)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "non-const VectorOfAny::Data() aliases the non-const typed Vector::Data()");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {7, 8, 9, 10};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<int32_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    auto* any = reinterpret_cast<VectorOfAny*>(v);
    EXPECT_EQ(any->Data(), v->Data());
}

// ---------------------------------------------------------------------------
// VectorSingleElementTest
// ---------------------------------------------------------------------------

TEST(VectorSingleElementTest, OneElement)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access, comp_req__flatbuffers__serialization");
    RecordProperty("Description", "vector with exactly 1 element");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {42};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 1U);
    EXPECT_EQ(v->Get(0), 42);
    EXPECT_FALSE(v->empty());
    EXPECT_EQ(v->end() - v->begin(), 1);
}

// ---------------------------------------------------------------------------
// VectorLookupByKeyTest
// ---------------------------------------------------------------------------

TEST(VectorLookupByKeyTest, FindsAndMissesKeys)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access, comp_req__flatbuffers__serialization");
    RecordProperty("Description", "LookupByKey returns the matching keyed table and nullptr for a missing key");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const auto* c = BuildSortedItemContainer(fbb);
    ASSERT_NE(c, nullptr);
    const auto* v = c->items();
    ASSERT_NE(v, nullptr);
    ASSERT_EQ(v->size(), 3U);

    const fixture::Item* found = v->LookupByKey(20);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->id(), 20);
    EXPECT_STREQ(found->label()->c_str(), "twenty");

    // Boundary keys (smallest and largest present).
    ASSERT_NE(v->LookupByKey(10), nullptr);
    EXPECT_EQ(v->LookupByKey(10)->id(), 10);
    ASSERT_NE(v->LookupByKey(30), nullptr);
    EXPECT_EQ(v->LookupByKey(30)->id(), 30);

    // Missing keys: below range, above range, and a gap.
    EXPECT_EQ(v->LookupByKey(5), nullptr);
    EXPECT_EQ(v->LookupByKey(15), nullptr);
    EXPECT_EQ(v->LookupByKey(99), nullptr);
}

TEST(VectorLookupByKeyTest, MutableLookup)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "MutableLookupByKey returns a mutable pointer to the matching keyed table");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const auto* c = BuildSortedItemContainer(fbb);
    ASSERT_NE(c, nullptr);
    auto* v = const_cast<Vector<Offset<fixture::Item>>*>(c->items());
    ASSERT_NE(v, nullptr);

    // `found` is non-const, but there is nothing it can actually mutate: flatc is invoked
    // without --gen-mutable for this schema (see codegen.bzl), so Item has no mutate_id()/
    // mutate_label(). This test only checks that the lookup finds the right table, not that
    // its value can be changed.
    fixture::Item* found = v->MutableLookupByKey(10);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->id(), 10);
    EXPECT_EQ(v->MutableLookupByKey(99), nullptr);
}

// ---------------------------------------------------------------------------
// VectorCastTest
// ---------------------------------------------------------------------------

TEST(VectorCastTest, UpcastToBaseTable)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "VectorCast reinterprets a Vector<Offset<Derived>> as Vector<Offset<Base>>");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder fbb(256);
    const auto* c = BuildSortedItemContainer(fbb);
    ASSERT_NE(c, nullptr);
    const auto* v = c->items();
    ASSERT_NE(v, nullptr);

    // Item derives from ::flatbuffers::Table, so the upcast satisfies the
    // is_base_of static_assert. The cast is a pure reinterpretation, so the
    // address must be preserved. (const overload)
    const auto* as_table = ::flatbuffers::VectorCast<::flatbuffers::Table>(v);
    EXPECT_EQ(static_cast<const void*>(as_table), static_cast<const void*>(v));

    // Non-const overload.
    auto* mv = const_cast<Vector<Offset<fixture::Item>>*>(v);
    auto* mas_table = ::flatbuffers::VectorCast<::flatbuffers::Table>(mv);
    EXPECT_EQ(static_cast<void*>(mas_table), static_cast<void*>(mv));
}

// ---------------------------------------------------------------------------
// VectorFaultDeathTest - robustness of the FLATBUFFERS_ASSERT bounds checks
// ---------------------------------------------------------------------------

TEST(VectorFaultDeathTest, GetOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "Get with index == size() triggers the bounds assertion");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {1, 2, 3};
    const auto* v = BuildScalarVector(fbb, data);
    ASSERT_NE(v, nullptr);

    EXPECT_DEATH(
        {
            volatile auto x = v->Get(v->size());
            (void)x;
        },
        "");
}

TEST(VectorFaultDeathTest, MutateOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "Mutate with index == size() triggers the bounds assertion");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    const std::vector<int32_t> data = {1, 2, 3};
    BuildScalarVector(fbb, data);
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<int32_t>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    EXPECT_DEATH({ v->Mutate(v->size(), 0); }, "");
}

TEST(VectorFaultDeathTest, MutateOffsetOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "MutateOffset with index == size() triggers the bounds assertion");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    auto s1 = fbb.CreateString("only");
    const std::vector<Offset<String>> str_offsets = {s1};
    auto vec = fbb.CreateVector(str_offsets);
    auto start = fbb.StartTable();
    fbb.AddOffset(kSingleFieldSlot, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<Offset<String>>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    const auto* target = reinterpret_cast<const uint8_t*>(v->Get(0));
    EXPECT_DEATH({ v->MutateOffset(v->size(), target); }, "");
}

TEST(VectorFaultDeathTest, GetMutableObjectOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetMutableObject with index == size() triggers the bounds assertion");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder fbb(256);
    auto s1 = fbb.CreateString("only");
    const std::vector<Offset<String>> str_offsets = {s1};
    auto vec = fbb.CreateVector(str_offsets);
    auto start = fbb.StartTable();
    fbb.AddOffset(kSingleFieldSlot, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    auto* v = table->GetPointer<Vector<Offset<String>>*>(kSingleFieldSlot);
    ASSERT_NE(v, nullptr);

    EXPECT_DEATH(
        {
            auto* x = v->GetMutableObject(v->size());
            (void)x;
        },
        "");
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score
