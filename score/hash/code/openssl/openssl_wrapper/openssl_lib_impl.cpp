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

#include "score/hash/code/openssl/openssl_wrapper/openssl_lib_impl.h"

#include <openssl/evp.h>

namespace score
{
namespace hash
{
namespace openssl
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
const StructDigest* OpensslLibImpl::DigestAlgoSha1() const noexcept
{
    return ::EVP_sha1();
}

const StructDigest* OpensslLibImpl::DigestAlgoSha256() const noexcept
{
    return ::EVP_sha256();
}

const StructDigest* OpensslLibImpl::DigestAlgoSha384() const noexcept
{
    return ::EVP_sha384();
}

const StructDigest* OpensslLibImpl::DigestAlgoSha512() const noexcept
{
    return ::EVP_sha512();
}

StructDigestCtx* OpensslLibImpl::CreateDigestCtx() const noexcept
{
    return ::EVP_MD_CTX_new();
}

/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: False positive */
std::int32_t OpensslLibImpl::InitDigestCtx(StructDigestCtx* ctx, const StructDigest* type, Engine* impl) const noexcept
{
    return ::EVP_DigestInit_ex(ctx, type, impl);
}

std::int32_t OpensslLibImpl::UpdateDigestCtx(StructDigestCtx* ctx, const void* data, std::size_t count) const noexcept
{
    return ::EVP_DigestUpdate(ctx, data, count);
}

/* KW_SUPPRESS_START:UNUSED.BUILTIN_NUMERIC: The function requires the input param to be basic numeric type*/
std::int32_t OpensslLibImpl::FinalizeDigestValue(StructDigestCtx* ctx,
                                                 unsigned char* digest_value,
                                                 unsigned int* digest_size) const noexcept
{
    return ::EVP_DigestFinal_ex(ctx, digest_value, digest_size);
}
/* KW_SUPPRESS_END:UNUSED.BUILTIN_NUMERIC */
void OpensslLibImpl::ResetDigestCtx(StructDigestCtx* ctx) const noexcept
{
    ::EVP_MD_CTX_free(ctx);
}
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: */
}  // namespace openssl
}  // namespace hash
}  // namespace score
