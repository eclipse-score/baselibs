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

#include "score/serialization/error/serialization_error.h"

#include <gtest/gtest.h>

namespace score
{
namespace serialization
{
namespace
{

void TestMessage(SerializationError error, const char* message)
{
    EXPECT_EQ(MakeError(error).Message(), message);
}

TEST(SerializationErrorTest, CanConvertToString)
{
    TestMessage(SerializationError::kOutOfBoundsWrite, "Tried to write out of bounds!");
    TestMessage(SerializationError::kStringTooBig, "Provided string exceeds maximum values!");
    TestMessage(SerializationError::kNotAllDataDeserialized,
                "Some data still left in input buffer after deserialization!");
    TestMessage(static_cast<SerializationError>(0xFF), "Unknown Error!");
}

}  // namespace
}  // namespace serialization
}  // namespace score
