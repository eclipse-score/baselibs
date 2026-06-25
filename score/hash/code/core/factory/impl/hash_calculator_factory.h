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

#ifndef SCORE_LIB_HASH_CODE_CORE_FACTORY_IMPL_HASHCALCULATOR_FATCORY_H
#define SCORE_LIB_HASH_CODE_CORE_FACTORY_IMPL_HASHCALCULATOR_FATCORY_H

#include "score/hash/code/core/factory/i_hash_calculator_factory.h"
#include "score/hash/code/openssl/openssl_hash_calculator.h"

namespace score
{
namespace hash
{
/// @brief Factory class implementation providing the required objects of HashCalculator and Hash object with the hash
/// value for the given input.
class HashCalculatorFactory final : public IHashCalculatorFactory
{
  public:
    explicit HashCalculatorFactory(
        score::cpp::optional<std::reference_wrapper<const openssl::IOpensslLib>> openssl_lib = {}) noexcept;

    Result<std::unique_ptr<IHashCalculator>> CreateHashCalculator(
        const HashAlgorithm algorithm) const noexcept override;

  private:
    score::cpp::optional<std::reference_wrapper<const openssl::IOpensslLib>> openssl_lib_;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_CORE_FACTORY_IMPL_HASHCALCULATOR_FATCORY_H
