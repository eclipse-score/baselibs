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
#ifndef SCORE_FLATBUFFERS_DETAILS_FULL_TABLE_FIXTURE_HPP
#define SCORE_FLATBUFFERS_DETAILS_FULL_TABLE_FIXTURE_HPP

#include "flatbuffers/flatbuffer_builder.h"
#include "flatbuffers/table.h"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>

namespace score
{

namespace flatbuffers
{

namespace test
{

// These tests compare against native values, so they assume a little-endian host, matching FlatBuffers' wire format.
static_assert(FLATBUFFERS_LITTLEENDIAN, "flatbuffers tests assume a little-endian host");

// A vtable is an array of 2-byte voffset_t entries, each holding a byte count.
// Entry 0 holds the vtable's own size, entry 1 the size of the table object's inline data.
// Each further entry holds one field's byte offset from the table start, in declaration order.
// A stored offset of 0 marks the field as absent.
// Offset 0 never names a real field, because the table's soffset_t back-pointer to its vtable occupies it.
// So field index i lives at byte position 4 + 2 * i within the vtable.
// This is the same formula FlatBufferBuilder::FieldIndexToOffset() uses internally.
// The slot constants below are these byte positions within the vtable, not the offsets stored there.
// The fixture table has no .fbs schema, so the slot numbers are hardcoded here.
inline constexpr ::flatbuffers::voffset_t kScalarFieldSlot{4};         // Field 0: int32 scalar.
inline constexpr ::flatbuffers::voffset_t kOptionalFieldSlot{6};       // Field 1: optional int32.
inline constexpr ::flatbuffers::voffset_t kBoolFieldSlot{8};           // Field 2: optional bool (stored as uint8).
inline constexpr ::flatbuffers::voffset_t kStringFieldSlot{10};        // Field 3: string offset.
inline constexpr ::flatbuffers::voffset_t kSecondStringFieldSlot{12};  // Field 4: second string offset.
inline constexpr ::flatbuffers::voffset_t kVectorFieldSlot{14};        // Field 5: vector<int32> offset.
inline constexpr ::flatbuffers::voffset_t kStructFieldSlot{16};        // Field 6: inline struct.
inline constexpr ::flatbuffers::voffset_t kVector64FieldSlot{18};      // Field 7: 64-bit-offset vector<int32>.
// BuildFullTable never writes this slot, and it lies beyond the vtable, so lookups take the "field not present" path.
inline constexpr ::flatbuffers::voffset_t kAbsentFieldSlot{100};

// Initial capacity hint for each test's FlatBufferBuilder; the builder grows as needed.
inline constexpr std::size_t kInitialBufferCapacity{256U};

inline constexpr std::int32_t kScalarFieldValue{42};
inline constexpr std::int32_t kOptionalFieldValue{7};
inline constexpr std::uint8_t kBoolFieldValue{1U};
inline constexpr char kStringFieldValue[]{"hello"};
inline constexpr char kSecondStringFieldValue[]{"world"};
inline constexpr std::int32_t kVectorValues[]{1, 2, 3};
inline constexpr std::int32_t kVector64Values[]{100, 200, 300};
inline constexpr std::int32_t kStructFieldX{11};
inline constexpr std::int32_t kStructFieldY{22};

/// @brief Inline struct stored in the fixture table's struct field.
struct Point2D
{
    std::int32_t x;
    std::int32_t y;
};

/// @brief Builds a single table exercising every field kind Table can read.
/// @param fbb Builder that owns the resulting buffer; it must outlive the returned pointer.
/// @return Mutable pointer to the table rooted in the builder's buffer; never nullptr.
///
/// The i-th AddElement/AddOffset/AddStruct call is field index i and uses the matching slot constant above.
/// The values written are independent of the slot numbers:
///
///   index  slot constant           slot  value written
///     0    kScalarFieldSlot           4  kScalarFieldValue (42)
///     1    kOptionalFieldSlot         6  kOptionalFieldValue (7)
///     2    kBoolFieldSlot             8  kBoolFieldValue (1, i.e. true)
///     3    kStringFieldSlot          10  kStringFieldValue ("hello")
///     4    kSecondStringFieldSlot    12  kSecondStringFieldValue ("world")
///     5    kVectorFieldSlot          14  kVectorValues ({1, 2, 3})
///     6    kStructFieldSlot          16  {kStructFieldX, kStructFieldY} (11, 22)
///     7    kVector64FieldSlot        18  kVector64Values ({100, 200, 300})
///
/// FlatBufferBuilder fills the buffer back to front, which currently yields this layout:
///
///   bytes    content                                         size
///    0-3     root offset (points forward to table)              4   <- fbb.GetBufferPointer()
///    4-7     alignment padding                                  4
///    8-27    vtable: 2 size entries, then 8 field entries      20
///   28-31    table header: soffset_t back to vtable             4   <- table start, Table::data_[0]
///   32-39    vec64 field (8-byte offset A, slot 18)             8   <- in-table offset 4
///   40-47    struct field (Point2D inline: x, y)                8   <- in-table offset 12
///   48-51    vec field (4-byte offset D, slot 14)               4   <- in-table offset 20
///   52-55    str2 field (4-byte offset C, slot 12)              4   <- in-table offset 24
///   56-59    str1 field (4-byte offset B, slot 10)              4   <- in-table offset 28
///   60-62    padding (aligns the int32 fields below)            3
///    63      bool field (1 byte, slot 8)                        1   <- in-table offset 35
///   64-67    optional field (int32, slot 6)                     4   <- in-table offset 36
///   68-71    scalar field (int32, slot 4)                       4   <- in-table offset 40
///   72-87    vec data                                          16   <- D
///   88-99    str2 "world"                                      12   <- C
///  100-111   str1 "hello"                                      12   <- B
///  112-135   vec64 data (built first, stored last)             24   <- A
///
/// The vtable at bytes 8-27 holds these voffset_t values:
///
///   vtable byte  entry                                  stored value
///        0       vtable size                              20   (10 entries, 2 B each)
///        2       table object size                        44   (bytes 28-71)
///        4       kScalarFieldSlot                         40
///        6       kOptionalFieldSlot                       36
///        8       kBoolFieldSlot                           35
///       10       kStringFieldSlot                         28
///       12       kSecondStringFieldSlot                   24
///       14       kVectorFieldSlot                         20
///       16       kStructFieldSlot                         12
///       18       kVector64FieldSlot                        4
///
/// The field entries are the "in-table offset" column of the layout above.
/// A lookup reads the entry at the slot's vtable byte, then reads the field at that offset from the table start.
/// A slot at or beyond the vtable size reads as absent, which is how kAbsentFieldSlot behaves.
///
/// The byte layout is an implementation detail of the builder and illustrates the offset indirection only.
/// Tests look fields up by slot and derive any byte positions they need at runtime.
inline ::flatbuffers::Table* BuildFullTable(::flatbuffers::FlatBufferBuilder64& fbb)
{
    // Create... writes to the tail now and hands back a lightweight handle.
    // All 64-bit-offset content must be serialized before any 32-bit-offset content, so CreateVector64 comes first.
    const auto vec64 =
        fbb.CreateVector64(std::vector<std::int32_t>(std::begin(kVector64Values), std::end(kVector64Values)));
    const auto str1 = fbb.CreateString(kStringFieldValue);
    const auto str2 = fbb.CreateString(kSecondStringFieldValue);
    const auto vec = fbb.CreateVector(kVectorValues, std::size(kVectorValues));
    const Point2D point{kStructFieldX, kStructFieldY};

    const auto start = fbb.StartTable();

    // AddElement writes the value itself directly into the table.
    fbb.AddElement<std::int32_t>(kScalarFieldSlot, kScalarFieldValue, 0);
    fbb.AddElement<std::int32_t>(kOptionalFieldSlot, kOptionalFieldValue, 0);
    fbb.AddElement<std::uint8_t>(kBoolFieldSlot, kBoolFieldValue, 0U);

    // AddOffset writes a number to the table saying where the data is.
    fbb.AddOffset(kStringFieldSlot, str1);
    fbb.AddOffset(kSecondStringFieldSlot, str2);
    fbb.AddOffset(kVectorFieldSlot, vec);
    // AddStruct is like AddElement, not AddOffset: it copies the whole struct directly into the
    // table, since Point2D is fixed-size and needs no tail storage.
    fbb.AddStruct(kStructFieldSlot, &point);
    fbb.AddOffset(kVector64FieldSlot, vec64);
    const auto root = fbb.EndTable(start);
    fbb.Finish(::flatbuffers::Offset<::flatbuffers::Table>(root));

    return ::flatbuffers::GetMutableRoot<::flatbuffers::Table>(fbb.GetBufferPointer());
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_FLATBUFFERS_DETAILS_FULL_TABLE_FIXTURE_HPP
