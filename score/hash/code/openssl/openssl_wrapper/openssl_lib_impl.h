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

#ifndef SCORE_LIB_HASH_CODE_OPENSSL_LIB_IMPL_H
#define SCORE_LIB_HASH_CODE_OPENSSL_LIB_IMPL_H

#include "score/hash/code/openssl/openssl_wrapper/i_openssl_lib.h"

#include <cstdint>

namespace score
{
namespace hash
{
namespace openssl
{

class OpensslLibImpl final : public IOpensslLib
{
  public:
    constexpr OpensslLibImpl() noexcept = default;
    ~OpensslLibImpl() noexcept override = default;

    OpensslLibImpl(const OpensslLibImpl&) = delete;
    OpensslLibImpl(OpensslLibImpl&&) noexcept = delete;
    OpensslLibImpl& operator=(const OpensslLibImpl&) = delete;
    OpensslLibImpl& operator=(OpensslLibImpl&&) noexcept = delete;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
    const StructDigest* DigestAlgoSha1() const noexcept override;
    const StructDigest* DigestAlgoSha256() const noexcept override;
    const StructDigest* DigestAlgoSha384() const noexcept override;
    const StructDigest* DigestAlgoSha512() const noexcept override;
    StructDigestCtx* CreateDigestCtx() const noexcept override;
    std::int32_t InitDigestCtx(StructDigestCtx* ctx, const StructDigest* type, Engine* impl) const noexcept override;
    std::int32_t UpdateDigestCtx(StructDigestCtx* ctx, const void* data, std::size_t count) const noexcept override;
    /* KW_SUPPRESS_START:UNUSED.BUILTIN_NUMERIC: The function requires the input param to be basic numeric type*/
    std::int32_t FinalizeDigestValue(StructDigestCtx* ctx,
                                     unsigned char* digest_value,
                                     unsigned int* digest_size) const noexcept override;
    /* KW_SUPPRESS_END:UNUSED.BUILTIN_NUMERIC*/
    void ResetDigestCtx(StructDigestCtx* ctx) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN */
};

}  // namespace openssl
}  // namespace hash
}  // namespace score
#endif  // SCORE_LIB_HASH_CODE_OPENSSL_LIB_IMPL_H
