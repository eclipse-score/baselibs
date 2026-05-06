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

#ifndef SCORE_LIB_HASH_CODE_OPENSSL_LIB_MOCK_H
#define SCORE_LIB_HASH_CODE_OPENSSL_LIB_MOCK_H

#include "score/hash/code/openssl/openssl_wrapper/i_openssl_lib.h"

#include <gmock/gmock.h>

namespace score
{
namespace hash
{
namespace openssl
{

/// @brief This class mocks the Openssl library for the purpose of unit testing.
class OpensslLibMock : public IOpensslLib
{
  public:
    /* KW_SUPPRESS_START:MISRA.MEMB.NOT_PRIVATE: caused by MOCK_METHOD macros */
    MOCK_METHOD((const StructDigest*), DigestAlgoSha1, (), (const, noexcept, override));
    MOCK_METHOD((const StructDigest*), DigestAlgoSha256, (), (const, noexcept, override));
    MOCK_METHOD((const StructDigest*), DigestAlgoSha384, (), (const, noexcept, override));
    MOCK_METHOD((const StructDigest*), DigestAlgoSha512, (), (const, noexcept, override));
    MOCK_METHOD(StructDigestCtx*, CreateDigestCtx, (), (const, noexcept, override));
    MOCK_METHOD(std::int32_t,
                InitDigestCtx,
                (StructDigestCtx * ctx, const StructDigest* type, Engine* impl),
                (const, noexcept, override));
    MOCK_METHOD(std::int32_t,
                UpdateDigestCtx,
                (StructDigestCtx * ctx, const void* data, size_t count),
                (const, noexcept, override));
    MOCK_METHOD(std::int32_t,
                FinalizeDigestValue,
                (StructDigestCtx * ctx, unsigned char* digest_value, unsigned int* digest_size),
                (const, noexcept, override));
    MOCK_METHOD(void, ResetDigestCtx, (StructDigestCtx * ctx), (const, noexcept, override));
    /* KW_SUPPRESS_END:MISRA.MEMB.NOT_PRIVATE: caused by MOCK_METHOD macros */
};

}  // namespace openssl
}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_OPENSSL_LIB_MOCK_H
