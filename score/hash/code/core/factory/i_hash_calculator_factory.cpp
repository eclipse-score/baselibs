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

#include "score/hash/code/core/factory/i_hash_calculator_factory.h"

#include "score/mw/log/logging.h"

namespace score
{
namespace hash
{

// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown, as we call has_value() before calling value()
// Suppress "UNUSED C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called, as we call has_value() before
// calling value()
//
// coverity[fun_call_w_exception]
// coverity[exn_spec_violation]
// coverity[autosar_cpp14_a15_5_3_violation]
Result<Hash> IHashCalculatorFactory::CalculateHash(const HashAlgorithm algorithm,
                                                   const score::cpp::span<const std::uint8_t> data) const noexcept
{
    auto hash_calculator = CreateHashCalculator(algorithm);

    if (!hash_calculator.has_value())
    {
        return MakeUnexpected<Hash>(hash_calculator.error());
    }

    auto update_result = hash_calculator.value()->Update(data);
    if (!update_result.has_value())
    {
        mw::log::LogError() << "HashCalculatorFactory::Could not update digest";
        return MakeUnexpected<Hash>(update_result.error());
    }

    return hash_calculator.value()->Finalize();
}

Result<Hash> IHashCalculatorFactory::CalculateHash(const HashAlgorithm algorithm,
                                                   std::istream& input_stream) const noexcept
{
    return CalculateHash(algorithm, input_stream, -1);
}

// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown, as we call has_value() before calling value()
// Suppress "UNUSED C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called, as we call has_value() before
// calling value()
//
// coverity[fun_call_w_exception]
// coverity[exn_spec_violation]
// coverity[autosar_cpp14_a15_5_3_violation]
Result<Hash> IHashCalculatorFactory::CalculateHash(const HashAlgorithm algorithm,
                                                   std::istream& input_stream,
                                                   std::int64_t max_read) const noexcept
{
    auto hash_calculator = CreateHashCalculator(algorithm);

    if (!hash_calculator.has_value())
    {
        return MakeUnexpected<Hash>(hash_calculator.error());
    }

    auto update_result = hash_calculator.value()->UpdateFromStream(input_stream, max_read);
    if (!update_result.has_value())
    {
        mw::log::LogError() << "HashCalculatorFactory::Could not update digest";
        return MakeUnexpected<Hash>(update_result.error());
    }

    return hash_calculator.value()->Finalize();
}

}  // namespace hash
}  // namespace score
