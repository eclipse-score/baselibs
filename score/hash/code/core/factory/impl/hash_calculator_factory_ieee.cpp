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
#include "score/hash/code/core/factory/impl/hash_calculator_factory.h"
#include "score/hash/code/crc/crc32_ieee.h"
#include "score/hash/code/sha256digest/sha256digest.h"

#include "score/mw/log/logging.h"

namespace score
{
namespace hash
{

HashCalculatorFactory::HashCalculatorFactory(
    score::cpp::optional<std::reference_wrapper<const openssl::IOpensslLib>> openssl_lib) noexcept
    : IHashCalculatorFactory(), openssl_lib_{openssl_lib}
{
}

Result<std::unique_ptr<IHashCalculator>> HashCalculatorFactory::CreateHashCalculator(
    const HashAlgorithm algorithm) const noexcept
{
    mw::log::LogDebug() << "HashCalculatorFactory::" << __func__;

    Result<std::unique_ptr<IHashCalculator>> result{
        MakeUnexpected(ErrorCode::kCouldNotCreateDigest, "Unknown hash algorithm specified")};

    switch (algorithm)
    {
        case HashAlgorithm::kCrc32:
            result = static_cast<std::unique_ptr<IHashCalculator>>(std::make_unique<Crc32IeeeHashCalculator>());
            break;
        case HashAlgorithm::kSha256:
            result = static_cast<std::unique_ptr<IHashCalculator>>(std::make_unique<Sha256Digest>());
            break;
        case HashAlgorithm::kSha1:
        case HashAlgorithm::kSha384:
        case HashAlgorithm::kSha512:
        {
            auto digest = openssl_lib_.has_value() ? OpensslHashCalculator::Create(algorithm, openssl_lib_->get())
                                                   : OpensslHashCalculator::Create(algorithm);
            if (!digest.has_value())
            {
                mw::log::LogError() << "HashCalculatorFactory::Could not create OpenSSL based digest";
                return MakeUnexpected<std::unique_ptr<IHashCalculator>>(digest.error());
            }

            result = static_cast<std::unique_ptr<IHashCalculator>>(
                std::make_unique<OpensslHashCalculator>(std::move(*digest)));
            break;
        }
        case HashAlgorithm::kCrc32Unused:
        case HashAlgorithm::kNone:
        case HashAlgorithm::kLast:
        default:
        {
            mw::log::LogError() << "HashCalculatorFactory::Could not create digest, unknown hash algorithm specified";
            break;
        }
    }

    return result;
}

}  // namespace hash
}  // namespace score
