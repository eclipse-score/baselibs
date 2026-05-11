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

#ifndef SCORE_LIB_HASH_CODE_CORE_FACTORY_HASHCALCULATOR_FATCORY_MOCK_H
#define SCORE_LIB_HASH_CODE_CORE_FACTORY_HASHCALCULATOR_FATCORY_MOCK_H

#include "score/hash/code/core/factory/i_hash_calculator_factory.h"

#include "gmock/gmock.h"

namespace score
{
namespace hash
{

class HashCalculatorFactoryMock : public IHashCalculatorFactory
{
  public:
    HashCalculatorFactoryMock() = default;
    ~HashCalculatorFactoryMock() = default;

    MOCK_METHOD(Result<std::unique_ptr<IHashCalculator>>,
                CreateHashCalculator,
                (const HashAlgorithm algorithm),
                (const, noexcept, override));
    MOCK_METHOD(Result<Hash>,
                CalculateHash,
                (const HashAlgorithm algorithm, const score::cpp::span<const std::uint8_t> data),
                (const, noexcept, override));
    MOCK_METHOD(Result<Hash>,
                CalculateHash,
                (const HashAlgorithm algorithm, std::istream& input_stream),
                (const, noexcept, override));
    MOCK_METHOD(Result<Hash>,
                CalculateHash,
                (const HashAlgorithm algorithm, std::istream& input_stream, std::int64_t max_read),
                (const, noexcept, override));
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_CORE_FACTORY_HASHCALCULATOR_FATCORY_MOCK_H
