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

#include "score/flatbuffers/error.hpp"

#include <gtest/gtest.h>

namespace score
{
namespace flatbuffers
{
namespace unit_test
{

TEST(FlatbuffersErrorDomainTest, ProvidesMessageForEveryErrorCode)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "MakeError produces an Error whose Message() returns the expected human-readable string "
                   "for every defined ErrorCode, exercising all branches of FlatbuffersErrorDomain::MessageFor");

    EXPECT_EQ(MakeError(score::flatbuffers::ErrorCode::kNullDataPointer).Message(),
              "Data pointer or derived root table pointer is null");
    EXPECT_EQ(MakeError(score::flatbuffers::ErrorCode::kVerificationFailed).Message(),
              "Structural buffer verification failed");
    EXPECT_EQ(MakeError(score::flatbuffers::ErrorCode::kVersionInfoNotPresent).Message(),
              "No version info available in buffer");
    EXPECT_EQ(MakeError(score::flatbuffers::ErrorCode::kVersionMismatch).Message(),
              "Buffer version does not match expected version");
    // Default branch: a code value that does not map to any named enumerator.
    EXPECT_EQ(MakeError(static_cast<score::flatbuffers::ErrorCode>(99)).Message(), "Unknown FlatBuffers error");
}

}  // namespace unit_test
}  // namespace flatbuffers
}  // namespace score
