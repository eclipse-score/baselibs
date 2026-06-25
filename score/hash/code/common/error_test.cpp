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

#include "score/hash/code/common/error.h"

#include <gtest/gtest.h>

namespace score
{
namespace hash
{
namespace
{

void TestMessage(hash::ErrorCode error, const char* message)
{
    EXPECT_EQ(MakeError(error).Message(), message);
}

TEST(HashErrorTest, CanConvertToString)
{
    TestMessage(hash::ErrorCode::kCouldNotCreateDigest, "Could not create digest algorithm");
    TestMessage(hash::ErrorCode::kCouldNotUpdateDigest, "Could not update digest");
    TestMessage(hash::ErrorCode::kCouldNotUpdateDigestFromStream, "Could not update digest from stream");
    TestMessage(hash::ErrorCode::kCouldNotFinalizeDigest, "Could not finalize digest");
    TestMessage(hash::ErrorCode::kInvalidParameters, "Invalid parameters");
    TestMessage(hash::ErrorCode::kNotFound, "Object not found");
    TestMessage(hash::ErrorCode::kStreamError, "Stream error");
    TestMessage(hash::ErrorCode::kUnknownError, "Unknown Error!");
    TestMessage(static_cast<hash::ErrorCode>(0xFF), "Unknown Error!");
}

}  // namespace
}  // namespace hash
}  // namespace score
