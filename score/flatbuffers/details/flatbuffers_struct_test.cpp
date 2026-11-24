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
#include "flatbuffers/struct.h"

#include <cstddef>
#include <cstdint>

#include "flatbuffers/base.h"
#include "flatbuffers/flatbuffer_builder.h"
#include "flatbuffers/table.h"
#include "gtest/gtest.h"
#include "score/flatbuffers/details/full_table_fixture.hpp"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

namespace
{

// Byte offsets of Point2D's two members within the inline struct stored at kStructFieldSlot.
// A struct has no vtable, so its members are addressed by their fixed offset rather than by a slot number.
constexpr uoffset_t kStructMemberXOffset{0U};
constexpr uoffset_t kStructMemberYOffset{sizeof(std::int32_t)};

// Value written by the mutation test; it differs from every value BuildFullTable stores.
constexpr std::int32_t kMutatedValue{99};

// Returns the fixture table's inline struct field viewed as a flatbuffers::Struct.
// Table::GetStruct casts away constness internally, so this yields a mutable view even though the struct
// lives in the buffer the builder owns. The FlatBufferBuilder must outlive the returned pointer.
Struct* GetFixtureStruct(FlatBufferBuilder64& fbb)
{
    return BuildFullTable(fbb)->GetStruct<Struct*>(kStructFieldSlot);
}

// A struct of mixed scalar types and widths, declared in an order that leaves gaps a compiler
// fills with padding. Its members are therefore addressed below by the real offsetof(),
// not by hand-computed byte positions, the same way generated FlatBuffers struct accessors
// use the offsets the schema compiler derives from the layout.
struct MixedFieldsStruct
{
    std::uint8_t flag;
    std::int32_t id;
    std::int64_t timestamp;
};

constexpr std::uint8_t kMixedFlagValue{1U};
constexpr std::int32_t kMixedIdValue{123456};
constexpr std::int64_t kMixedTimestampValue{9'876'543'210LL};

// This table has only one field, so it reuses the first slot regardless of what the fixture table above uses.
constexpr voffset_t kMixedStructFieldSlot{4};

// Builds a table whose only field is a mixed-type struct, and returns a pointer to that struct's
// inline storage. The FlatBufferBuilder must outlive the returned pointer.
Struct* BuildMixedFieldsTable(FlatBufferBuilder64& fbb, const MixedFieldsStruct& value)
{
    const auto start = fbb.StartTable();
    fbb.AddStruct(kMixedStructFieldSlot, &value);
    const auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    auto* table = GetMutableRoot<Table>(fbb.GetBufferPointer());
    return table->GetStruct<Struct*>(kMixedStructFieldSlot);
}

// An outer struct whose two members are themselves structs, the schema-realistic shape of a nested
// struct: flatc compiles a struct field of struct type to an accessor that calls GetStruct at that
// field's offset, exactly as BuildRectTable's caller does below.
struct Rect
{
    Point2D top_left;
    Point2D bottom_right;
};

constexpr Point2D kRectTopLeft{10, 20};
constexpr Point2D kRectBottomRight{30, 40};

// This table has only one field, so it reuses the first slot regardless of what the other tables above use.
constexpr voffset_t kRectFieldSlot{4};

// Builds a table whose only field is a Rect, and returns a pointer to that struct's inline storage.
// The FlatBufferBuilder must outlive the returned pointer.
const Struct* BuildRectTable(FlatBufferBuilder64& fbb, const Rect& value)
{
    const auto start = fbb.StartTable();
    fbb.AddStruct(kRectFieldSlot, &value);
    const auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    const auto* table = GetRoot<Table>(fbb.GetBufferPointer());
    return table->GetStruct<const Struct*>(kRectFieldSlot);
}

}  // namespace

// StructFieldAccessTest
// Struct::GetField.

TEST(StructFieldAccessTest, GetFieldReturnsEachMemberAtItsOffset)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "GetField reads each member of an inline struct from the member's byte offset");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb{kInitialBufferCapacity};
    const auto* point = GetFixtureStruct(fbb);
    ASSERT_NE(point, nullptr);

    EXPECT_EQ(point->GetField<std::int32_t>(kStructMemberXOffset), kStructFieldX);
    EXPECT_EQ(point->GetField<std::int32_t>(kStructMemberYOffset), kStructFieldY);
}

// StructNestedAccessTest
// Struct::GetStruct.

TEST(StructNestedAccessTest, GetStructReturnsANestedStructMembers)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetStruct on an outer struct whose two members are themselves structs returns each member at its "
                   "offsetof-derived offset, matching how a struct field of struct type is accessed");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb{kInitialBufferCapacity};
    const Rect value{kRectTopLeft, kRectBottomRight};
    const auto* rect = BuildRectTable(fbb, value);
    ASSERT_NE(rect, nullptr);

    const auto offset_top_left = offsetof(Rect, top_left);
    ASSERT_EQ(offset_top_left, 0U);

    const auto* top_left = rect->GetStruct<const Point2D*>(offset_top_left);
    ASSERT_NE(top_left, nullptr);
    EXPECT_EQ(top_left->x, kRectTopLeft.x);
    EXPECT_EQ(top_left->y, kRectTopLeft.y);

    const auto offset_bottom_right = offsetof(Rect, bottom_right);
    EXPECT_GT(offset_bottom_right, offset_top_left);
    const auto* bottom_right = rect->GetStruct<const Point2D*>(offset_bottom_right);
    ASSERT_NE(bottom_right, nullptr);
    EXPECT_EQ(bottom_right->x, kRectBottomRight.x);
    EXPECT_EQ(bottom_right->y, kRectBottomRight.y);
}

// StructAddressTest
// Struct::GetAddressOf, const and non-const.

TEST(StructAddressTest, GetAddressOfReturnsTheMemberAddressForReading)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description", "The const GetAddressOf returns an address from which the member reads back");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb{kInitialBufferCapacity};
    const Struct* point = GetFixtureStruct(fbb);
    ASSERT_NE(point, nullptr);

    const auto* address = point->GetAddressOf(kStructMemberYOffset);
    ASSERT_NE(address, nullptr);
    EXPECT_EQ(ReadScalar<std::int32_t>(address), kStructFieldY);
}

TEST(StructAddressTest, GetAddressOfReturnsTheMemberAddressForWriting)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "The non-const GetAddressOf returns a writable address, and a value written there reads back "
                   "through GetField without disturbing the neighbouring member");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb{kInitialBufferCapacity};
    auto* point = GetFixtureStruct(fbb);
    ASSERT_NE(point, nullptr);

    auto* address = point->GetAddressOf(kStructMemberXOffset);
    ASSERT_NE(address, nullptr);
    WriteScalar(address, kMutatedValue);

    EXPECT_EQ(point->GetField<std::int32_t>(kStructMemberXOffset), kMutatedValue);
    EXPECT_EQ(point->GetField<std::int32_t>(kStructMemberYOffset), kStructFieldY);
}

// StructMixedFieldTypesTest
// GetField and GetAddressOf across members of different scalar widths within the same struct.

TEST(StructMixedFieldTypesTest, GetFieldReadsEachMemberAtItsRealOffsetRegardlessOfWidth)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetField reads a 1-byte, a 4-byte and an 8-byte member of the same struct, each from its own "
                   "offsetof-derived offset");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    FlatBufferBuilder64 fbb{kInitialBufferCapacity};
    const MixedFieldsStruct value{kMixedFlagValue, kMixedIdValue, kMixedTimestampValue};
    const auto* mixed = BuildMixedFieldsTable(fbb, value);
    ASSERT_NE(mixed, nullptr);

    EXPECT_EQ(mixed->GetField<std::uint8_t>(offsetof(MixedFieldsStruct, flag)), kMixedFlagValue);
    EXPECT_EQ(mixed->GetField<std::int32_t>(offsetof(MixedFieldsStruct, id)), kMixedIdValue);
    EXPECT_EQ(mixed->GetField<std::int64_t>(offsetof(MixedFieldsStruct, timestamp)), kMixedTimestampValue);
}

TEST(StructMixedFieldTypesTest, GetAddressOfWritesAnEightByteMemberWithoutDisturbingItsNeighbours)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__access");
    RecordProperty("Description",
                   "GetAddressOf on a non-default-width member (8 bytes) yields a writable address; the write reads "
                   "back through GetField and leaves the narrower neighbouring members unchanged");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    FlatBufferBuilder64 fbb{kInitialBufferCapacity};
    const MixedFieldsStruct value{kMixedFlagValue, kMixedIdValue, kMixedTimestampValue};
    auto* mixed = BuildMixedFieldsTable(fbb, value);
    ASSERT_NE(mixed, nullptr);

    constexpr std::int64_t kMutatedTimestamp{-1LL};
    auto* address = mixed->GetAddressOf(offsetof(MixedFieldsStruct, timestamp));
    ASSERT_NE(address, nullptr);
    WriteScalar(address, kMutatedTimestamp);

    EXPECT_EQ(mixed->GetField<std::int64_t>(offsetof(MixedFieldsStruct, timestamp)), kMutatedTimestamp);
    EXPECT_EQ(mixed->GetField<std::uint8_t>(offsetof(MixedFieldsStruct, flag)), kMixedFlagValue);
    EXPECT_EQ(mixed->GetField<std::int32_t>(offsetof(MixedFieldsStruct, id)), kMixedIdValue);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score
