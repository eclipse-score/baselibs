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

#include "score/serialization/strategies/hex_string_to_bytes.h"

#include <gtest/gtest.h>

namespace score
{
namespace serialization
{

TEST(HexToString, CorrectInput)
{
    const auto result = HexStringToBytes("0F02A3");
    const auto expected = score::cpp::pmr::vector<uint8_t>{0x0F, 0x02, 0xa3};
    EXPECT_EQ(result, expected);
}

TEST(HexToString, InputNotVerbose)
{
    EXPECT_DEATH(HexStringToBytes("10203"), "");
}

TEST(HexToString, InputNotHex)
{
    EXPECT_DEATH(HexStringToBytes("zebra!"), "");
}

}  // namespace serialization
}  // namespace score
