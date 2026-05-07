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

#ifndef SCORE_LIB_HASH_CODE_I_OPENSSL_LIB_H
#define SCORE_LIB_HASH_CODE_I_OPENSSL_LIB_H

#include "score/expected.hpp"

#include <openssl/evp.h>

#include <cstdint>

namespace score
{
namespace hash
{
namespace openssl
{
/// @brief Type-alias for better readability
using StructDigest = ::EVP_MD;
using StructDigestCtx = ::EVP_MD_CTX;
using Engine = ::ENGINE;

/// @brief IOpensslLib is an abstract class that defines the interface to the openssl library as it is used to
/// calculate hash values. Its purpose is to allow the interface to be mocked for testing.
class IOpensslLib
{
  public:
    IOpensslLib() noexcept = default;
    virtual ~IOpensslLib() noexcept = default;
    IOpensslLib(const IOpensslLib&) = delete;
    IOpensslLib(IOpensslLib&&) noexcept = delete;
    IOpensslLib& operator=(const IOpensslLib&) = delete;
    IOpensslLib& operator=(IOpensslLib&&) noexcept = delete;

    /// @brief Calculates the hash value with SHA1 algorithm.
    virtual const StructDigest* DigestAlgoSha1() const noexcept = 0;

    /// @brief Calculates the hash value with SHA256 algorithm.
    virtual const StructDigest* DigestAlgoSha256() const noexcept = 0;

    /// @brief Calculates the hash value with SHA384 algorithm.
    virtual const StructDigest* DigestAlgoSha384() const noexcept = 0;

    /// @brief Calculates the hash value with SHA512 algorithm.
    virtual const StructDigest* DigestAlgoSha512() const noexcept = 0;

    /// @brief Allocates and returns a digest context
    virtual StructDigestCtx* CreateDigestCtx() const noexcept = 0;

    /// @brief Sets up digest context ctx to use a digest type from ENGINE impl
    virtual std::int32_t InitDigestCtx(StructDigestCtx* ctx, const StructDigest* type, Engine* impl) const noexcept = 0;

    /// @brief Hashes count bytes of data at into the digest context ctx.
    virtual std::int32_t UpdateDigestCtx(StructDigestCtx* ctx, const void* data, std::size_t count) const noexcept = 0;

    /// @brief Retrieves the digest value from ctx and places it in digest_value and stores the length of digest in
    /// digest_size
    virtual std::int32_t FinalizeDigestValue(StructDigestCtx* ctx,
                                             unsigned char* digest_value,
                                             unsigned int* digest_size) const noexcept = 0;

    /// @brief Cleans up digest context ctx and frees up the space allocated to it.
    virtual void ResetDigestCtx(StructDigestCtx* ctx) const noexcept = 0;
};

}  // namespace openssl
}  // namespace hash
}  // namespace score
#endif  // SCORE_LIB_HASH_CODE_I_OPENSSL_LIB_H
