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
#include "flatbuffers/buffer_ref.h"

#include <cstdint>
#include <cstring>

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

// A minimal "verifiable" wrapper so BufferRef<VerifiableTable>::Verify()
// can call table->Verify(verifier).
struct VerifiableTable : public Table
{
  bool Verify(Verifier& verifier) const
  {
    return VerifyTableStart(verifier) && verifier.EndTable();
  }
};

static DetachedBuffer BuildVerifiable(const char* identifier = nullptr)
{
  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start);
  if (identifier)
  {
    fbb.Finish(Offset<Table>(root), identifier);
  }
  else
  {
    fbb.Finish(Offset<Table>(root));
  }
  return fbb.Release();
}

// ---------------------------------------------------------------------------
// BufferRefValidTest
// Tests that a valid buffer verifies successfully.
// ---------------------------------------------------------------------------

TEST(BufferRefValidTest, ValidBuffer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::BufferRef<T>::Verify");
  RecordProperty("Description", "BufferRef with valid buffer data verifies successfully");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto db = BuildVerifiable();
  BufferRef<VerifiableTable> ref;
  ref.buf = db.data();
  ref.len = static_cast<uoffset_t>(db.size());
  EXPECT_TRUE(ref.Verify());
}

// ---------------------------------------------------------------------------
// BufferRefDefaultTest
// Tests that a default-constructed BufferRef is not valid.
// ---------------------------------------------------------------------------

TEST(BufferRefDefaultTest, Empty)
{
  RecordProperty("FullyVerifies", "::flatbuffers::BufferRef<T>::BufferRef");
  RecordProperty("Description", "default-constructed BufferRef has null pointer and zero length");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  BufferRef<VerifiableTable> ref;
  EXPECT_EQ(ref.buf, nullptr);
  EXPECT_EQ(ref.len, 0u);
}

// ---------------------------------------------------------------------------
// BufferRefInvalidTest
// Tests that truncated / corrupted buffer fails verification.
// ---------------------------------------------------------------------------

TEST(BufferRefInvalidTest, TruncatedBuffer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::BufferRef<T>::Verify");
  RecordProperty("Description", "truncated buffer fails verification");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  uint8_t tiny[] = {0x00, 0x01, 0x02, 0x03};
  BufferRef<VerifiableTable> ref;
  ref.buf = tiny;
  ref.len = 4;
  EXPECT_FALSE(ref.Verify());
}

// ---------------------------------------------------------------------------
// BufferRefGetRootTest
// Tests BufferRef<T>::GetRoot.
// ---------------------------------------------------------------------------

TEST(BufferRefGetRootTest, ReturnsRoot)
{
  RecordProperty("FullyVerifies", "::flatbuffers::BufferRef<T>::GetRoot");
  RecordProperty("Description", "GetRoot returns a usable pointer for valid buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto db = BuildVerifiable();
  BufferRef<VerifiableTable> ref;
  ref.buf = db.data();
  ref.len = static_cast<uoffset_t>(db.size());
  EXPECT_TRUE(ref.Verify());

  const auto* root = ref.GetRoot();
  ASSERT_NE(root, nullptr);
}

// ---------------------------------------------------------------------------
// BufferRefWithIdentifierTest
// Tests buffer with identifier.
// ---------------------------------------------------------------------------

TEST(BufferRefWithIdentifierTest, IdentifierPresent)
{
  RecordProperty("FullyVerifies", "::flatbuffers::BufferRef<T>::Verify, ::flatbuffers::BufferHasIdentifier");
  RecordProperty("Description", "buffer with matching identifier verifies and passes BufferHasIdentifier");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto db = BuildVerifiable("ABCD");
  BufferRef<VerifiableTable> ref;
  ref.buf = db.data();
  ref.len = static_cast<uoffset_t>(db.size());
  EXPECT_TRUE(ref.Verify());
  EXPECT_TRUE(BufferHasIdentifier(ref.buf, "ABCD"));
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score