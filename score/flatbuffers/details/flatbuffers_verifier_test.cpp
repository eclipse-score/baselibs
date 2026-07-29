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
#include "flatbuffers/verifier.h"

#include <cstdint>
#include <cstring>
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

// Minimal table type that can be verified.
struct VerifiableTable : public Table
{
  bool Verify(Verifier& verifier) const
  {
    return VerifyTableStart(verifier) && verifier.EndTable();
  }
  bool Verify(SizeVerifier& verifier) const
  {
    return VerifyTableStart(verifier) && verifier.EndTable();
  }
};

// Helper: build a minimal valid buffer with one int32 field.
static std::vector<uint8_t> BuildSimpleBuffer(const char* identifier = nullptr)
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
  auto* p = fbb.GetBufferPointer();
  return std::vector<uint8_t>(p, p + fbb.GetSize());
}

// ---------------------------------------------------------------------------
// VerifierOptionsDefaultTest
// ---------------------------------------------------------------------------

TEST(VerifierOptionsDefaultTest, Values)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::Options");
  RecordProperty("Description", "default verifier options");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Verifier::Options opts;
  EXPECT_GT(opts.max_depth, 0u);
  EXPECT_GT(opts.max_tables, 0u);
  EXPECT_EQ(opts.assert, false);
}

// ---------------------------------------------------------------------------
// VerifierCheckTest
// ---------------------------------------------------------------------------

TEST(VerifierCheckTest, PassFail)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::Check");
  RecordProperty("Description", "Check returns the value passed");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildSimpleBuffer();
  Verifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.Check(true));
  EXPECT_FALSE(v.Check(false));
}

// ---------------------------------------------------------------------------
// VerifierVerifyRangeTest
// ---------------------------------------------------------------------------

TEST(VerifierVerifyRangeTest, Boundary)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::Verify (range)");
  RecordProperty("Description", "range verification including boundary at buf.size() (strict less-than)");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  auto buf = BuildSimpleBuffer();
  Verifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.Verify(static_cast<size_t>(0), static_cast<size_t>(1)));
  EXPECT_TRUE(v.Verify(static_cast<size_t>(0), static_cast<size_t>(buf.size() - 1)));
  EXPECT_FALSE(v.Verify(static_cast<size_t>(0), static_cast<size_t>(buf.size())));
}

// ---------------------------------------------------------------------------
// VerifierVerifyAlignmentTest
// ---------------------------------------------------------------------------

TEST(VerifierVerifyAlignmentTest, Checks)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::VerifyAlignment");
  RecordProperty("Description", "alignment checks return true for valid alignments");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildSimpleBuffer();
  Verifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.VerifyAlignment(0, 1));
  EXPECT_TRUE(v.VerifyAlignment(0, 4));
}

// ---------------------------------------------------------------------------
// VerifierVerifyStringTest
// ---------------------------------------------------------------------------

TEST(VerifierVerifyStringTest, InBuffer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::VerifyString");
  RecordProperty("Description", "string in buffer verifies successfully");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto s = fbb.CreateString("hello");
  auto start = fbb.StartTable();
  fbb.AddOffset(4, s);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* p = fbb.GetBufferPointer();
  Verifier v(p, fbb.GetSize());
  EXPECT_TRUE(v.VerifyBuffer<VerifiableTable>(nullptr));

  auto* table = GetRoot<Table>(p);
  auto* str = table->GetPointer<const String*>(4);
  ASSERT_NE(str, nullptr);
  EXPECT_TRUE(v.VerifyString(str));
}

// ---------------------------------------------------------------------------
// VerifierVerifyVectorTest
// ---------------------------------------------------------------------------

TEST(VerifierVerifyVectorTest, Scalars)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::VerifyVector");
  RecordProperty("Description", "vector of scalars in buffer verifies");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  std::vector<int32_t> data = {1, 2, 3};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* p = fbb.GetBufferPointer();
  Verifier v(p, fbb.GetSize());
  EXPECT_TRUE(v.VerifyBuffer<VerifiableTable>(nullptr));

  auto* table = GetRoot<Table>(p);
  auto* vp = table->GetPointer<const Vector<int32_t>*>(4);
  ASSERT_NE(vp, nullptr);
  EXPECT_TRUE(v.VerifyVector(vp));
}

// ---------------------------------------------------------------------------
// VerifierVerifyBufferValidTest
// ---------------------------------------------------------------------------

TEST(VerifierVerifyBufferValidTest, Passes)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::VerifyBuffer<T>");
  RecordProperty("Description", "valid buffer passes VerifyBuffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildSimpleBuffer();
  Verifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.VerifyBuffer<VerifiableTable>(nullptr));
}

// ---------------------------------------------------------------------------
// VerifierVerifyBufferIdentifierTest
// ---------------------------------------------------------------------------

TEST(VerifierVerifyBufferIdentifierTest, MatchMismatch)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::VerifyBuffer<T>");
  RecordProperty("Description", "identifier check: match passes, mismatch fails");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildSimpleBuffer("TEST");
  Verifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.VerifyBuffer<VerifiableTable>("TEST"));
  EXPECT_FALSE(v.VerifyBuffer<VerifiableTable>("NOPE"));
}

// ---------------------------------------------------------------------------
// VerifierVerifyBufferInvalidTest
// ---------------------------------------------------------------------------

TEST(VerifierVerifyBufferInvalidTest, Truncated)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::VerifyBuffer<T>");
  RecordProperty("Description", "truncated buffer fails");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  std::vector<uint8_t> buf = {0x00, 0x01, 0x02, 0x03};
  Verifier v(buf.data(), buf.size());
  EXPECT_FALSE(v.VerifyBuffer<VerifiableTable>(nullptr));
}

// ---------------------------------------------------------------------------
// VerifierDepthLimitTest
// ---------------------------------------------------------------------------

TEST(VerifierDepthLimitTest, ZeroDepthFails)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::Options::max_depth");
  RecordProperty("Description", "depth limit of 0 causes verification to fail");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  auto buf = BuildSimpleBuffer();
  Verifier::Options opts;
  opts.max_depth = 0;
  Verifier v(buf.data(), buf.size(), opts);
  EXPECT_FALSE(v.VerifyBuffer<VerifiableTable>(nullptr));
}

// ---------------------------------------------------------------------------
// VerifierSizePrefixedTest
// ---------------------------------------------------------------------------

TEST(VerifierSizePrefixedTest, Works)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::VerifySizePrefixedBuffer<T>");
  RecordProperty("Description", "size-prefixed buffer verifies");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start);
  fbb.FinishSizePrefixed(Offset<Table>(root));

  auto* p = fbb.GetBufferPointer();
  Verifier v(p, fbb.GetSize());
  EXPECT_TRUE(v.VerifySizePrefixedBuffer<VerifiableTable>(nullptr));
}

// ---------------------------------------------------------------------------
// VerifierSizeVerifierTest
// ---------------------------------------------------------------------------

TEST(VerifierSizeVerifierTest, Works)
{
  RecordProperty("FullyVerifies", "::flatbuffers::SizeVerifier::VerifyBuffer<T>");
  RecordProperty("Description", "SizeVerifier works correctly");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildSimpleBuffer();
  SizeVerifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.VerifyBuffer<VerifiableTable>(nullptr));
}

// ---------------------------------------------------------------------------
// VerifierDeprecatedConstructorTest
// ---------------------------------------------------------------------------

TEST(VerifierDeprecatedConstructorTest, Works)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Verifier::Verifier(const uint8_t*, size_t)");
  RecordProperty("Description", "deprecated two-arg constructor still works");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildSimpleBuffer();
  Verifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.VerifyBuffer<VerifiableTable>(nullptr));
}

// ---------------------------------------------------------------------------
// VerifierSizeVerifierCheckFailTest
// ---------------------------------------------------------------------------

TEST(VerifierSizeVerifierCheckFailTest, ResetUpperBound)
{
  RecordProperty("FullyVerifies", "::flatbuffers::SizeVerifier::Check");
  RecordProperty("Description", "Check(false) on SizeVerifier resets upper_bound_ to 0");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildSimpleBuffer();
  SizeVerifier v(buf.data(), buf.size());
  EXPECT_TRUE(v.Check(true));
  EXPECT_FALSE(v.Check(false));
}

// ---------------------------------------------------------------------------
// VerifierVectorLengthOverflowTest
// ---------------------------------------------------------------------------

TEST(VerifierVectorLengthOverflowTest, OverflowProtection)
{
  RecordProperty("FullyVerifies", "::flatbuffers::SizeVerifier::VerifyVectorOrString");
  RecordProperty("Description", "vector with stored length >= max_elems triggers overflow guard");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  std::vector<uint8_t> buf(5, 0xFF);
  buf[4] = 0;
  SizeVerifier::Options opts;
  opts.max_size = 8;
  SizeVerifier v(buf.data(), buf.size(), opts);
  const auto* vec_ptr = reinterpret_cast<const Vector<int32_t>*>(buf.data());
  EXPECT_FALSE(v.VerifyVector(vec_ptr));
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score