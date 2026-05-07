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

#ifndef SCORE_LIB_HASH_CODE_CRC_CRC32_MOCK_H
#define SCORE_LIB_HASH_CODE_CRC_CRC32_MOCK_H

#include "score/hash/code/crc/i_crc32.h"

#include "gmock/gmock.h"

namespace score
{
namespace hash
{

class Crc32HashCalculatorMock final : public ICrc32HashCalculator
{
  public:
    Crc32HashCalculatorMock() = default;
    ~Crc32HashCalculatorMock() = default;

    MOCK_METHOD(ResultBlank, Update, (const score::cpp::span<const std::uint8_t>), (noexcept, override));
    MOCK_METHOD(ResultBlank, UpdateFromStream, (std::istream & input), (noexcept, override));
    MOCK_METHOD(Hash, Finalize, (), (noexcept, override));
    MOCK_METHOD(std::uint_fast32_t, GetChecksum, (), (const, noexcept, override));
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_CRC_CRC32_MOCK_H
