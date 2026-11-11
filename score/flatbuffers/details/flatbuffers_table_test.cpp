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
#include "flatbuffers/table.h"

#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

#include "flatbuffers/flatbuffer_builder.h"
#include "flatbuffers/verifier.h"
#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// These tests compare against native values, so they assume a little-endian host, matching FlatBuffers' wire format.
static_assert(FLATBUFFERS_LITTLEENDIAN, "flatbuffers table tests assume a little-endian host");

namespace
{

// A vtable is an array of 2-byte voffset_t entries.
// Entry 0 holds the vtable's own byte size, entry 1 holds the table object's byte size.
// Entry 2 onward holds one byte offset per field, in declaration order.
// So field index i lives at byte offset 4 + 2 * i within the vtable.
// This is the same formula FlatBufferBuilder::FieldIndexToOffset() uses internally.
// The tables in this file have no .fbs schema, so the slot numbers are hardcoded here.
constexpr voffset_t kScalarFieldSlot{4};         // Field 0: int32 scalar.
constexpr voffset_t kOptionalFieldSlot{6};       // Field 1: optional int32 (generic GetOptional).
constexpr voffset_t kBoolFieldSlot{8};           // Field 2: optional bool (stored as uint8).
constexpr voffset_t kStringFieldSlot{10};        // Field 3: string offset.
constexpr voffset_t kSecondStringFieldSlot{12};  // Field 4: second string offset.
constexpr voffset_t kVectorFieldSlot{14};        // Field 5: vector<int32> offset.
constexpr voffset_t kStructFieldSlot{16};        // Field 6: inline struct.
constexpr voffset_t kVector64FieldSlot{18};      // Field 7: 64-bit-offset vector<int32>.
// BuildFullTable never writes this slot, and it lies beyond the vtable, so lookups take the "field not present" path.
constexpr voffset_t kAbsentFieldSlot{100};

// Initial capacity hint for each test's FlatBufferBuilder64; the builder grows as needed.
constexpr size_t kInitialBufferCapacity{256U};

constexpr int32_t kScalarFieldValue{42};
constexpr int32_t kOptionalFieldValue{7};
constexpr uint8_t kBoolFieldValue{1U};
constexpr char kStringFieldValue[]{"hello"};
constexpr char kSecondStringFieldValue[]{"world"};
constexpr int32_t kVectorValues[]{1, 2, 3};
constexpr int32_t kVector64Values[]{100, 200, 300};
constexpr int32_t kStructFieldX{11};
constexpr int32_t kStructFieldY{22};

// Relative offset written into a field to make it resolve far outside any buffer built in this file.
constexpr uint64_t kOutOfBoundsOffset{10'000'000ULL};

struct Point2D
{
    int32_t x;
    int32_t y;
};

// Overwrites the stored relative offset of a present offset field with a raw value.
// Writing the raw bytes avoids forming an out-of-bounds pointer, which SetPointer would require.
template <typename OffsetType>
bool WriteRawOffset(Table& table, const voffset_t field, const OffsetType value)
{
    auto* const field_address = table.GetAddressOf(field);
    if (field_address == nullptr)
    {
        return false;
    }
    std::memcpy(field_address, &value, sizeof(value));
    return true;
}

// Builds a single table exercising every field kind Table can read.
// The i-th AddElement/AddOffset/AddStruct call is field index i and uses the matching slot constant above.
// The values written are independent of the slot numbers:
//
//   index  slot constant           slot  value written
//     0    kScalarFieldSlot           4  kScalarFieldValue (42)
//     1    kOptionalFieldSlot         6  kOptionalFieldValue (7)
//     2    kBoolFieldSlot             8  kBoolFieldValue (1, i.e. true)
//     3    kStringFieldSlot          10  kStringFieldValue ("hello")
//     4    kSecondStringFieldSlot    12  kSecondStringFieldValue ("world")
//     5    kVectorFieldSlot          14  kVectorValues ({1, 2, 3})
//     6    kStructFieldSlot          16  {kStructFieldX, kStructFieldY} (11, 22)
//     7    kVector64FieldSlot        18  kVector64Values ({100, 200, 300})
//
// FlatBufferBuilder fills the buffer back to front, which currently yields this layout:
//
//   bytes    content                                         size
//    0-3     root offset (points forward to table)              4   <- fbb.GetBufferPointer()
//    4-23    vtable header, then 8 field slots (2 B each)      20
//   24-27    alignment padding                                  4
//   28-31    table header: soffset_t back to vtable             4   <- table start, Table::data_[0]
//   32-39    vec64 field (8-byte offset A, slot 18)             8   <- in-table offset 4
//   40-47    struct field (Point2D inline: x, y)                8   <- in-table offset 12
//   48-51    vec field (4-byte offset D, slot 14)               4   <- in-table offset 20
//   52-55    str2 field (4-byte offset C, slot 12)              4   <- in-table offset 24
//   56-59    str1 field (4-byte offset B, slot 10)              4   <- in-table offset 28
//   60-62    padding (aligns the int32 fields below)            3
//    63      bool field (1 byte, slot 8)                        1   <- in-table offset 35
//   64-67    optional field (int32, slot 6)                     4   <- in-table offset 36
//   68-71    scalar field (int32, slot 4)                       4   <- in-table offset 40
//   72-87    vec data                                          16   <- D
//   88-99    str2 "world"                                      12   <- C
//  100-111   str1 "hello"                                      12   <- B
//  112-135   vec64 data (built first, stored last)             24   <- A
//
// The byte layout is an implementation detail of the builder and illustrates the offset indirection only.
// Tests look fields up by slot and derive any byte positions they need at runtime.
Table* BuildFullTable(FlatBufferBuilder64& fbb)
{
    // Create... writes to the tail now and hands back a lightweight handle.
    // All 64-bit-offset content must be serialized before any 32-bit-offset content, so CreateVector64 comes first.
    const auto vec64 = fbb.CreateVector64(std::vector<int32_t>(std::begin(kVector64Values), std::end(kVector64Values)));
    const auto str1 = fbb.CreateString(kStringFieldValue);
    const auto str2 = fbb.CreateString(kSecondStringFieldValue);
    const auto vec = fbb.CreateVector(kVectorValues, std::size(kVectorValues));
    const Point2D point{kStructFieldX, kStructFieldY};

    const auto start = fbb.StartTable();

    // AddElement writes the value itself directly into the table.
    fbb.AddElement<int32_t>(kScalarFieldSlot, kScalarFieldValue, 0);
    fbb.AddElement<int32_t>(kOptionalFieldSlot, kOptionalFieldValue, 0);
    fbb.AddElement<uint8_t>(kBoolFieldSlot, kBoolFieldValue, 0U);

    // AddOffset writes a number to the table saying where the data is.
    fbb.AddOffset(kStringFieldSlot, str1);
    fbb.AddOffset(kSecondStringFieldSlot, str2);
    fbb.AddOffset(kVectorFieldSlot, vec);
    // AddStruct is like AddElement, not AddOffset: it copies the whole struct directly into the
    // table, since Point2D is fixed-size and needs no tail storage.
    fbb.AddStruct(kStructFieldSlot, &point);
    fbb.AddOffset(kVector64FieldSlot, vec64);
    const auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    return GetMutableRoot<Table>(fbb.GetBufferPointer());
}

}  // namespace

// ScalarFieldTest
// GetField / SetField (3-arg and 2-arg overloads).

TEST(ScalarFieldTest, GetFieldReturnsStoredValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetField returns the stored value when the field is present");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    EXPECT_EQ(table->GetField<int32_t>(kScalarFieldSlot, -1), kScalarFieldValue);
}

// BuildFullTableTest
// Fixture-regression guard: BuildFullTable is the shared fixture
TEST(BuildFullTableTest, WritesEveryFieldWithItsExpectedValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "BuildFullTable's fixture table round-trips every field kind with its expected value");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);

    EXPECT_EQ(table->GetField<int32_t>(kScalarFieldSlot, -1), kScalarFieldValue);

    const auto optional_value = table->GetOptional<int32_t, int32_t>(kOptionalFieldSlot);
    ASSERT_TRUE(optional_value.has_value());
    EXPECT_EQ(*optional_value, kOptionalFieldValue);

    const auto bool_value = table->GetOptional<uint8_t, bool>(kBoolFieldSlot);
    ASSERT_TRUE(bool_value.has_value());
    EXPECT_EQ(*bool_value, kBoolFieldValue != 0U);

    const auto* string_value = table->GetPointer<const String*>(kStringFieldSlot);
    ASSERT_NE(string_value, nullptr);
    EXPECT_STREQ(string_value->c_str(), kStringFieldValue);

    const auto* second_string_value = table->GetPointer<const String*>(kSecondStringFieldSlot);
    ASSERT_NE(second_string_value, nullptr);
    EXPECT_STREQ(second_string_value->c_str(), kSecondStringFieldValue);

    const auto* vec = table->GetVectorPointerOrEmpty<int32_t>(kVectorFieldSlot);
    ASSERT_NE(vec, nullptr);
    ASSERT_EQ(vec->size(), std::size(kVectorValues));
    for (size_t i = 0U; i < std::size(kVectorValues); ++i)
    {
        EXPECT_EQ(vec->Get(static_cast<uoffset_t>(i)), kVectorValues[i]) << "mismatch at vector index " << i;
    }

    const auto* point = table->GetStruct<const Point2D*>(kStructFieldSlot);
    ASSERT_NE(point, nullptr);
    EXPECT_EQ(point->x, kStructFieldX);
    EXPECT_EQ(point->y, kStructFieldY);

    const auto* vec64 = table->GetPointer64<const Vector64<int32_t>*>(kVector64FieldSlot);
    ASSERT_NE(vec64, nullptr);
    ASSERT_EQ(vec64->size(), std::size(kVector64Values));
    for (size_t i = 0U; i < std::size(kVector64Values); ++i)
    {
        EXPECT_EQ(vec64->Get(static_cast<uoffset64_t>(i)), kVector64Values[i]) << "mismatch at vector64 index " << i;
    }
}

// ScalarFieldTest
// GetField / SetField (3-arg and 2-arg overloads).

TEST(ScalarFieldTest, GetFieldReturnsDefaultWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetField returns the supplied default when the field is absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);

    EXPECT_EQ(table->GetField<int32_t>(static_cast<voffset_t>(kVector64FieldSlot + sizeof(voffset_t)), -1), -1);
    EXPECT_EQ(table->GetField<int32_t>(kAbsentFieldSlot, -1), -1);
}

TEST(ScalarFieldTest, AllAccessorsTreatSkippedSlotInsideVtableAsAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetField, CheckField, GetAddressOf and GetOptional all treat a vtable slot that stores "
                   "offset 0 as absent, without disturbing an adjacent written field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto start = fbb.StartTable();
    fbb.AddElement<int32_t>(kScalarFieldSlot, kScalarFieldValue, 0);
    fbb.AddElement<uint8_t>(kBoolFieldSlot, kBoolFieldValue, 0U);
    const auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    const auto* table = GetRoot<Table>(fbb.GetBufferPointer());

    EXPECT_EQ(table->GetField<int32_t>(kOptionalFieldSlot, -1), -1);
    EXPECT_FALSE(table->CheckField(kOptionalFieldSlot));
    EXPECT_EQ(table->GetAddressOf(kOptionalFieldSlot), nullptr);
    EXPECT_FALSE((table->GetOptional<int32_t, int32_t>(kOptionalFieldSlot).has_value()));
    EXPECT_EQ(table->GetField<int32_t>(kScalarFieldSlot, -1), kScalarFieldValue);
}

TEST(ScalarFieldTest, SetFieldWithDefaultWhenAbsentReturnsWhetherValEqualsDefault)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "SetField(field,val,def) on an absent field returns IsTheSameAs(val,def) without writing");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    // An absent field already reads back as the default, so setting it to that value needs no write and succeeds.
    EXPECT_TRUE(table->SetField<int32_t>(kAbsentFieldSlot, 5, 5));
    EXPECT_FALSE(table->SetField<int32_t>(kAbsentFieldSlot, 5, 9));
}

TEST(ScalarFieldTest, SetFieldWithDefaultWhenPresentWritesAndReturnsTrue)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description", "SetField(field,val,def) on a present field writes the value and returns true");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    EXPECT_TRUE(table->SetField<int32_t>(kScalarFieldSlot, 99, 0));
    EXPECT_EQ(table->GetField<int32_t>(kScalarFieldSlot, -1), 99);
}

TEST(ScalarFieldTest, SetFieldWithoutDefaultWhenAbsentReturnsFalse)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description", "SetField(field,val) on an absent field does not write and returns false");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    EXPECT_FALSE(table->SetField<int32_t>(kAbsentFieldSlot, 123));
}

TEST(ScalarFieldTest, SetFieldWithoutDefaultWhenPresentWritesAndReturnsTrue)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description", "SetField(field,val) on a present field writes the value and returns true");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    EXPECT_TRUE(table->SetField<int32_t>(kScalarFieldSlot, 55));
    EXPECT_EQ(table->GetField<int32_t>(kScalarFieldSlot, -1), 55);
}

// PointerFieldTest
// GetPointer / GetAddressOf / SetPointer (string-backed fields).

TEST(PointerFieldTest, GetPointerReturnsTargetWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetPointer resolves a present offset field, through both const and non-const");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const Table* const_table = table;

    const auto* string_value = table->GetPointer<const String*>(kStringFieldSlot);
    ASSERT_NE(string_value, nullptr);
    EXPECT_STREQ(string_value->c_str(), kStringFieldValue);

    const auto* const_string_value = const_table->GetPointer<const String*>(kStringFieldSlot);
    ASSERT_NE(const_string_value, nullptr);
    EXPECT_STREQ(const_string_value->c_str(), kStringFieldValue);
}

TEST(PointerFieldTest, GetPointerReturnsNullWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetPointer returns nullptr for an absent offset field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const Table* const_table = table;

    EXPECT_EQ(table->GetPointer<const String*>(kAbsentFieldSlot), nullptr);
    EXPECT_EQ(const_table->GetPointer<const String*>(kAbsentFieldSlot), nullptr);
}

TEST(PointerFieldTest, GetAddressOfReturnsFieldAddressWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetAddressOf returns a non-null address for a present field, const and non-const");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const Table* const_table = table;

    EXPECT_NE(table->GetAddressOf(kScalarFieldSlot), nullptr);
    EXPECT_NE(const_table->GetAddressOf(kScalarFieldSlot), nullptr);
}

TEST(PointerFieldTest, GetAddressOfReturnsNullWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetAddressOf returns nullptr for an absent field, const and non-const");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const Table* const_table = table;

    EXPECT_EQ(table->GetAddressOf(kAbsentFieldSlot), nullptr);
    EXPECT_EQ(const_table->GetAddressOf(kAbsentFieldSlot), nullptr);
}

TEST(PointerFieldTest, SetPointerWhenAbsentReturnsFalse)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description", "SetPointer on an absent field does not write and returns false");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const auto* target = table->GetPointer<const String*>(kStringFieldSlot);
    ASSERT_NE(target, nullptr);
    EXPECT_FALSE(table->SetPointer(kAbsentFieldSlot, reinterpret_cast<const uint8_t*>(target)));
}

TEST(PointerFieldTest, SetPointerWhenPresentRetargetsField)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description", "SetPointer repoints a present offset field at a new target and returns true");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const auto* new_target = table->GetPointer<const String*>(kSecondStringFieldSlot);
    ASSERT_NE(new_target, nullptr);

    EXPECT_TRUE(table->SetPointer(kStringFieldSlot, reinterpret_cast<const uint8_t*>(new_target)));
    const auto* read_back = table->GetPointer<const String*>(kStringFieldSlot);
    ASSERT_NE(read_back, nullptr);
    EXPECT_STREQ(read_back->c_str(), kSecondStringFieldValue);
}

// Pointer64Test
// GetPointer64 (64-bit-offset field access).

TEST(Pointer64Test, GetPointer64ReturnsTargetWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetPointer64 resolves a present 64-bit-offset field, const and non-const");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const Table* const_table = table;

    const auto* vec = table->GetPointer64<const Vector64<int32_t>*>(kVector64FieldSlot);
    ASSERT_NE(vec, nullptr);
    EXPECT_EQ(vec->size(), 3U);
    EXPECT_EQ(vec->Get(0), kVector64Values[0]);

    const auto* vec_const = const_table->GetPointer64<const Vector64<int32_t>*>(kVector64FieldSlot);
    ASSERT_NE(vec_const, nullptr);
    EXPECT_EQ(vec_const->Get(2), kVector64Values[2]);
}

TEST(Pointer64Test, GetPointer64ReturnsNullWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetPointer64 returns nullptr for an absent 64-bit-offset field, const and non-const");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    const Table* const_table = table;

    EXPECT_EQ(table->GetPointer64<const Vector64<int32_t>*>(kAbsentFieldSlot), nullptr);
    EXPECT_EQ(const_table->GetPointer64<const Vector64<int32_t>*>(kAbsentFieldSlot), nullptr);
}

// VectorFieldTest
// GetVectorPointerOrEmpty / GetMutableVectorPointerOrEmpty (32-bit-offset vectors).

TEST(VectorFieldTest, GetVectorPointerOrEmptyReturnsVectorWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetVectorPointerOrEmpty returns the stored vector when the field is present");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto* vec = table->GetVectorPointerOrEmpty<int32_t>(kVectorFieldSlot);
    ASSERT_NE(vec, nullptr);
    ASSERT_EQ(vec->size(), 3U);
    EXPECT_EQ(vec->Get(0), kVectorValues[0]);
    EXPECT_EQ(vec->Get(2), kVectorValues[2]);
}

TEST(VectorFieldTest, GetVectorPointerOrEmptyReturnsEmptyVectorWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetVectorPointerOrEmpty returns a zero-length vector when the field is absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto* vec = table->GetVectorPointerOrEmpty<int32_t>(kAbsentFieldSlot);
    ASSERT_NE(vec, nullptr);
    EXPECT_EQ(vec->size(), 0U);
}

TEST(VectorFieldTest, GetMutableVectorPointerOrEmptyAllowsMutationWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description", "GetMutableVectorPointerOrEmpty returns a mutable view of a present vector field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    auto* vec = table->GetMutableVectorPointerOrEmpty<int32_t>(kVectorFieldSlot);
    ASSERT_NE(vec, nullptr);
    vec->Mutate(0, 999);
    const auto* read_back = table->GetVectorPointerOrEmpty<int32_t>(kVectorFieldSlot);
    ASSERT_NE(read_back, nullptr);
    EXPECT_EQ(read_back->Get(0), 999);
}

TEST(VectorFieldTest, GetMutableVectorPointerOrEmptyReturnsEmptyVectorWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetMutableVectorPointerOrEmpty returns a zero-length vector when the field is absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    auto* vec = table->GetMutableVectorPointerOrEmpty<int32_t>(kAbsentFieldSlot);
    ASSERT_NE(vec, nullptr);
    EXPECT_EQ(vec->size(), 0U);
}

// VectorField64Test
// GetVectorPointer64OrEmpty / GetMutableVectorPointer64OrEmpty (64-bit-offset vectors).

TEST(VectorField64Test, GetVectorPointer64OrEmptyReturnsVectorWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetVectorPointer64OrEmpty returns the stored vector when the field is present");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto* vec = table->GetVectorPointer64OrEmpty<int32_t, uoffset64_t>(kVector64FieldSlot);
    ASSERT_NE(vec, nullptr);
    ASSERT_EQ(vec->size(), 3U);
    EXPECT_EQ(vec->Get(1), kVector64Values[1]);
}

TEST(VectorField64Test, GetVectorPointer64OrEmptyReturnsEmptyVectorWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetVectorPointer64OrEmpty returns a zero-length vector when the field is absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto* vec = table->GetVectorPointer64OrEmpty<int32_t, uoffset64_t>(kAbsentFieldSlot);
    ASSERT_NE(vec, nullptr);
    EXPECT_EQ(vec->size(), 0U);
}

TEST(VectorField64Test, GetMutableVectorPointer64OrEmptyAllowsMutationWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization, comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetMutableVectorPointer64OrEmpty returns a mutable view of a present 64-bit-offset vector field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    auto* vec = table->GetMutableVectorPointer64OrEmpty<int32_t, uoffset64_t>(kVector64FieldSlot);
    ASSERT_NE(vec, nullptr);
    vec->Mutate(0, 999);
    const auto* read_back = table->GetVectorPointer64OrEmpty<int32_t, uoffset64_t>(kVector64FieldSlot);
    ASSERT_NE(read_back, nullptr);
    EXPECT_EQ(read_back->Get(0), 999);
}

TEST(VectorField64Test, GetMutableVectorPointer64OrEmptyReturnsEmptyVectorWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetMutableVectorPointer64OrEmpty returns a zero-length vector when the field is absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);
    auto* vec = table->GetMutableVectorPointer64OrEmpty<int32_t, uoffset64_t>(kAbsentFieldSlot);
    ASSERT_NE(vec, nullptr);
    EXPECT_EQ(vec->size(), 0U);
}

// StructFieldTest
// GetStruct.

TEST(StructFieldTest, GetStructReturnsInlineStructWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetStruct returns a pointer to the inline struct when the field is present");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto* point = table->GetStruct<const Point2D*>(kStructFieldSlot);
    ASSERT_NE(point, nullptr);
    EXPECT_EQ(point->x, kStructFieldX);
    EXPECT_EQ(point->y, kStructFieldY);
}

TEST(StructFieldTest, GetStructReturnsNullWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetStruct returns nullptr when the field is absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    EXPECT_EQ(table->GetStruct<const Point2D*>(kAbsentFieldSlot), nullptr);
}

// OptionalFieldTest
// GetOptional, generic template and the <uint8_t, bool> specialization.

TEST(OptionalFieldTest, GetOptionalGenericReturnsValueWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetOptional<Raw,Face> (generic template) returns an engaged Optional when present");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto value = table->GetOptional<int32_t, int32_t>(kOptionalFieldSlot);
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, kOptionalFieldValue);
}

TEST(OptionalFieldTest, GetOptionalGenericReturnsEmptyWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetOptional<Raw,Face> (generic template) returns a disengaged Optional when absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto value = table->GetOptional<int32_t, int32_t>(kAbsentFieldSlot);
    EXPECT_FALSE(value.has_value());
}

TEST(OptionalFieldTest, GetOptionalBoolSpecializationReturnsValueWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetOptional<uint8_t,bool> specialization returns an engaged Optional when present");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto value = table->GetOptional<uint8_t, bool>(kBoolFieldSlot);
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(*value);
}

TEST(OptionalFieldTest, GetOptionalBoolSpecializationReturnsFalseWhenPresentAndZero)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetOptional<uint8_t,bool> specialization returns an engaged Optional(false) when the field is "
                   "present but its stored byte is 0, distinguishing field presence from the stored value");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    // ForceDefaults makes the builder serialize this field even though its value equals the default.
    // Without it, AddElement would omit the field and GetOptional would report it as absent.
    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    fbb.ForceDefaults(true);
    const auto start = fbb.StartTable();
    fbb.AddElement<uint8_t>(kBoolFieldSlot, 0U, 0U);
    const auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    const auto* table = GetRoot<Table>(fbb.GetBufferPointer());

    const auto value = table->GetOptional<uint8_t, bool>(kBoolFieldSlot);
    ASSERT_TRUE(value.has_value());
    EXPECT_FALSE(*value);
}

TEST(OptionalFieldTest, GetOptionalBoolSpecializationReturnsEmptyWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetOptional<uint8_t,bool> specialization returns a disengaged Optional when absent");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    const auto value = table->GetOptional<uint8_t, bool>(kAbsentFieldSlot);
    EXPECT_FALSE(value.has_value());
}

// CheckFieldTest

TEST(CheckFieldTest, ReturnsTrueWhenPresentAndFalseWhenAbsent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "CheckField reports whether the field's offset is set in the vtable");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    EXPECT_TRUE(table->CheckField(kScalarFieldSlot));
    EXPECT_FALSE(table->CheckField(kAbsentFieldSlot));
}

// VerifyTest
// Table verification helpers, including the Required, 64-bit-offset and WithDefault variants.

TEST(VerifyTest, VerifyTableStartSucceeds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyTableStart succeeds for a well-formed table");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());
    EXPECT_TRUE(table->VerifyTableStart(verifier));
}

TEST(VerifyTest, VerifyTableStartFailsForATruncatedBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyTableStart fails when the verifier's buffer is truncated before the table");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);

    // The verifier's buffer ends exactly where the table's 4-byte vtable offset begins.
    // VerifyTableStart's first bounds check therefore fails before the vtable is inspected.
    const auto table_offset = static_cast<size_t>(reinterpret_cast<const uint8_t*>(table) - fbb.GetBufferPointer());
    Verifier verifier(fbb.GetBufferPointer(), table_offset);
    EXPECT_FALSE(table->VerifyTableStart(verifier));
}

TEST(VerifyTest, VerifyFieldShortCircuitsWhenAbsentAndDelegatesWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description",
                   "VerifyField returns true immediately for an absent field, else delegates to the verifier");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    // An absent field has no stored data to check, so verification succeeds.
    // VerifyFieldRequired is the counterpart that fails on absence.
    EXPECT_TRUE(table->VerifyField<int32_t>(verifier, kAbsentFieldSlot, sizeof(int32_t)));
    EXPECT_TRUE(table->VerifyField<int32_t>(verifier, kScalarFieldSlot, sizeof(int32_t)));
}

TEST(VerifyTest, VerifyFieldRequiredFailsWhenAbsentAndSucceedsWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyFieldRequired fails for an absent field and succeeds for a present one");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    EXPECT_FALSE(table->VerifyFieldRequired<int32_t>(verifier, kAbsentFieldSlot, sizeof(int32_t)));
    EXPECT_TRUE(table->VerifyFieldRequired<int32_t>(verifier, kScalarFieldSlot, sizeof(int32_t)));
}

TEST(VerifyTest, VerifyOffsetShortCircuitsWhenAbsentAndDelegatesWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description",
                   "VerifyOffset returns true immediately for an absent field, else delegates to the verifier");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    // An absent field has no stored offset to follow, so verification succeeds.
    // VerifyOffsetRequired is the counterpart that fails on absence.
    EXPECT_TRUE(table->VerifyOffset(verifier, kAbsentFieldSlot));
    EXPECT_TRUE(table->VerifyOffset(verifier, kStringFieldSlot));
}

TEST(VerifyTest, VerifyOffsetRequiredFailsWhenAbsentAndSucceedsWhenPresent)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyOffsetRequired fails for an absent field and succeeds for a present one");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    EXPECT_FALSE(table->VerifyOffsetRequired(verifier, kAbsentFieldSlot));
    EXPECT_TRUE(table->VerifyOffsetRequired(verifier, kStringFieldSlot));
}

TEST(VerifyTest, VerifyOffset64AndRequiredSucceedForA64BitOffsetField)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description",
                   "VerifyOffset64 and VerifyOffset64Required succeed for a present 64-bit-offset field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    EXPECT_TRUE(table->VerifyOffset64(verifier, kVector64FieldSlot));
    EXPECT_TRUE(table->VerifyOffset64Required(verifier, kVector64FieldSlot));
}

TEST(VerifyTest, VerifyOffset64FamilyFailsWhenAbsentOrOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description",
                   "VerifyOffset64Required fails for an absent field, and both VerifyOffset64 and "
                   "VerifyOffset64Required fail when a present 64-bit-offset field resolves outside the buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);

    {
        Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());
        // VerifyOffset64 accepts an absent field, while VerifyOffset64Required treats absence as a failure.
        EXPECT_TRUE(table->VerifyOffset64(verifier, kAbsentFieldSlot));
        EXPECT_FALSE(table->VerifyOffset64Required(verifier, kAbsentFieldSlot));
    }

    // The full 8-byte relative offset now resolves far outside the buffer.
    ASSERT_TRUE(WriteRawOffset(*table, kVector64FieldSlot, static_cast<uoffset64_t>(kOutOfBoundsOffset)));

    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());
    EXPECT_FALSE(table->VerifyOffset64(verifier, kVector64FieldSlot));
    EXPECT_FALSE(table->VerifyOffset64Required(verifier, kVector64FieldSlot));
}

TEST(VerifyTest, VerifyStringWithDefaultHandlesAbsentAndPresentFields)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyStringWithDefault succeeds for both an absent field and a valid string field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    // An absent string field falls back to its default, so there is no stored content to verify.
    EXPECT_TRUE(table->VerifyStringWithDefault(verifier, kAbsentFieldSlot));
    EXPECT_TRUE(table->VerifyStringWithDefault(verifier, kStringFieldSlot));
}

TEST(VerifyTest, VerifyVectorWithDefaultHandlesAbsentAndPresentFields)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyVectorWithDefault succeeds for both an absent field and a valid vector field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    // An absent vector field falls back to an empty vector, so there is no stored content to verify.
    EXPECT_TRUE(table->VerifyVectorWithDefault<int32_t>(verifier, kAbsentFieldSlot));
    EXPECT_TRUE(table->VerifyVectorWithDefault<int32_t>(verifier, kVectorFieldSlot));
}

TEST(VerifyTest, VerifyVector64WithDefaultSucceedsForA64BitVectorField)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyVector64WithDefault succeeds for a present 64-bit-offset vector field");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    const bool verified = table->VerifyVector64WithDefault<int32_t, uoffset64_t>(verifier, kVector64FieldSlot);
    EXPECT_TRUE(verified);
}

TEST(VerifyTest, VerifyFieldAndVerifyFieldRequiredFailWhenAlignmentCannotBeSatisfied)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description", "VerifyField and VerifyFieldRequired fail when the verifier's alignment check fails");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    const auto* table = BuildFullTable(fbb);
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());

    // The verifier checks alignment relative to the buffer start.
    // Twice the lowest set bit of the field's buffer offset is an alignment that offset does not satisfy.
    const auto* field_address = table->GetAddressOf(kScalarFieldSlot);
    ASSERT_NE(field_address, nullptr);
    const auto field_offset = static_cast<size_t>(field_address - fbb.GetBufferPointer());
    ASSERT_NE(field_offset, 0U);
    const size_t unsatisfied_alignment{(field_offset & (~field_offset + 1U)) << 1U};
    EXPECT_FALSE(table->VerifyField<int32_t>(verifier, kScalarFieldSlot, unsatisfied_alignment));
    EXPECT_FALSE(table->VerifyFieldRequired<int32_t>(verifier, kScalarFieldSlot, unsatisfied_alignment));
}

TEST(VerifyTest, VerifyOffsetFamilyFailWhenTargetIsOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description",
                   "VerifyOffset, VerifyOffsetRequired and VerifyStringWithDefault fail when a present field's "
                   "offset resolves outside the buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);

    // The string field stays present but its offset now resolves far outside the buffer.
    ASSERT_TRUE(WriteRawOffset(*table, kStringFieldSlot, static_cast<uoffset_t>(kOutOfBoundsOffset)));

    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());
    EXPECT_FALSE(table->VerifyOffset(verifier, kStringFieldSlot));
    EXPECT_FALSE(table->VerifyOffsetRequired(verifier, kStringFieldSlot));
    EXPECT_FALSE(table->VerifyStringWithDefault(verifier, kStringFieldSlot));
}

TEST(VerifyTest, VerifyVectorWithDefaultFamilyFailsWhenTargetIsOutOfBounds)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__verification");
    RecordProperty("Description",
                   "VerifyVectorWithDefault and VerifyVector64WithDefault fail when a present vector field's "
                   "offset resolves outside the buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb(kInitialBufferCapacity);
    auto* table = BuildFullTable(fbb);

    ASSERT_TRUE(WriteRawOffset(*table, kVectorFieldSlot, static_cast<uoffset_t>(kOutOfBoundsOffset)));
    ASSERT_TRUE(WriteRawOffset(*table, kVector64FieldSlot, static_cast<uoffset64_t>(kOutOfBoundsOffset)));

    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());
    EXPECT_FALSE(table->VerifyVectorWithDefault<int32_t>(verifier, kVectorFieldSlot));
    EXPECT_FALSE((table->VerifyVector64WithDefault<int32_t, uoffset64_t>(verifier, kVector64FieldSlot)));
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score
