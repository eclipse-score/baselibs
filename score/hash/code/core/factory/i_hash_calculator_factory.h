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

#ifndef SCORE_LIB_HASH_CODE_CORE_FACTORY_I_HASHCALCULATOR_FATCORY_H
#define SCORE_LIB_HASH_CODE_CORE_FACTORY_I_HASHCALCULATOR_FATCORY_H

#include "score/hash/code/common/algorithms.h"
#include "score/hash/code/core/hash.h"
#include "score/hash/code/core/i_hash_calculator.h"
#include "score/result/result.h"

namespace score
{
namespace hash
{

/// @brief Interface class for factory providing required object of HashCalculator and the Hash object with the hash
/// value for the given input.
class IHashCalculatorFactory
{
  public:
    IHashCalculatorFactory() noexcept = default;
    virtual ~IHashCalculatorFactory() noexcept = default;
    IHashCalculatorFactory(const IHashCalculatorFactory&) noexcept = delete;
    IHashCalculatorFactory(IHashCalculatorFactory&&) noexcept = delete;
    IHashCalculatorFactory& operator=(const IHashCalculatorFactory&) noexcept = delete;
    IHashCalculatorFactory& operator=(IHashCalculatorFactory&&) noexcept = delete;

    /// @brief Method for creating a hash calculator object.
    ///
    /// @param[in] algorithm algorithm to use for hash calculation
    /// @return unique_ptr to the hash calculator object, otherwise error.
    ///
    virtual Result<std::unique_ptr<IHashCalculator>> CreateHashCalculator(
        const HashAlgorithm algorithm) const noexcept = 0;
    /// @brief Encapsulates a hashcalculator. Creates an instance of the hashcalculator and calculates the hash value of
    /// given data.
    ///
    /// @param[in] algorithm algorithm to use for hash calculation
    /// @param[in] data pointer to the input data whose hash value has to calculated.
    /// @return Hash object that owns the value and type, otherwise error.
    virtual Result<Hash> CalculateHash(const HashAlgorithm algorithm,
                                       const score::cpp::span<const std::uint8_t> data) const noexcept;
    /// @brief Encapsulates a hashcalculator. Creates an instance of the hashcalculator and calculates the hash value of
    /// given data.
    ///
    /// @param[in] algorithm algorithm to use for hash calculation
    /// @param[in] input_stream istream to the input data whose hash value has to calculated.
    /// @return Hash object that owns the value and type, otherwise error.
    virtual Result<Hash> CalculateHash(const HashAlgorithm algorithm, std::istream& input_stream) const noexcept;

    /// @brief Encapsulates a hashcalculator. Creates an instance of the hashcalculator and calculates the hash value of
    /// given data.
    ///
    /// @param[in] algorithm algorithm to use for hash calculation
    /// @param[in] input_stream istream to the input data whose hash value has to calculated.
    /// @param[in] max_read the maximum number of bytes to read from the stream
    /// @return Hash object that owns the value and type, otherwise error.
    virtual Result<Hash> CalculateHash(const HashAlgorithm algorithm,
                                       std::istream& input_stream,
                                       std::int64_t max_read) const noexcept;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_CORE_FACTORY_I_HASHCALCULATOR_FATCORY_H
