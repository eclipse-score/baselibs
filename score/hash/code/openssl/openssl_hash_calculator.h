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

#ifndef SCORE_LIB_HASH_CODE_OPENSSL_HASHCALCULATOR_IMPL_H
#define SCORE_LIB_HASH_CODE_OPENSSL_HASHCALCULATOR_IMPL_H

#include "score/hash/code/common/algorithms.h"
#include "score/hash/code/core/i_hash_calculator.h"
#include "score/hash/code/openssl/openssl_wrapper/i_openssl_lib.h"

#include "score/mw/log/logging.h"

#include <score/callback.hpp>

#include <memory>

namespace score
{
namespace hash
{

/// @brief OpenSSL-based implementation for hash calculation
class OpensslHashCalculator final : public IHashCalculator
{
  public:
    OpensslHashCalculator(const OpensslHashCalculator&) noexcept = delete;
    OpensslHashCalculator(OpensslHashCalculator&& other) noexcept = default;
    OpensslHashCalculator& operator=(const OpensslHashCalculator&) & = delete;
    OpensslHashCalculator& operator=(OpensslHashCalculator&& other) & noexcept = default;

    ~OpensslHashCalculator() noexcept override = default;

    /// @brief Create hash calculation to use the given algorithm
    ///
    /// @param[in] algorithm algorithm to use for hash calculation
    /// @param[in] openssl reference to the OpenSSL library call wrapper
    ///
    /// @return OpensslHashCalculator upon successful initialization, error otherwise
    static Result<OpensslHashCalculator> Create(const HashAlgorithm algorithm,
                                                const openssl::IOpensslLib& openssl = GetDefaultOpenSslImpl()) noexcept;

    ResultBlank Update(const score::cpp::span<const std::uint8_t> data) noexcept override;

    Hash Finalize() noexcept override;

  private:
    /// Returns the default implementation for the OpenSSL call wrapper which uses the real OpenSSL library calls.
    static const openssl::IOpensslLib& GetDefaultOpenSslImpl();

    OpensslHashCalculator(const HashAlgorithm algorithm,
                          openssl::StructDigestCtx* const message_digest_context,
                          const openssl::IOpensslLib& openssl);

    std::unique_ptr<openssl::StructDigestCtx, score::cpp::callback<void(openssl::StructDigestCtx*)>> message_digest_context_;
    HashAlgorithm algorithm_;

    std::reference_wrapper<const openssl::IOpensslLib> openssl_;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_OPENSSL_HASHCALCULATOR_IMPL_H
