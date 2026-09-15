/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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

#ifndef SCORE_LIB_OS_QNX_FS_CRYPTO_IMPL_H
#define SCORE_LIB_OS_QNX_FS_CRYPTO_IMPL_H

#include "score/os/qnx/fs_crypto.h"

namespace score
{
namespace os
{

namespace qnx
{
/// @brief fs_crypto functions OSAL class
/// [QNX fs_crypto
/// documentation](https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.utilities/topic/f/fsencrypt.html)
class FsCryptoImpl final : public FsCrypto
{
  public:
    score::cpp::expected_blank<Error> fs_crypto_domain_remove(const char* path,
                                                              int domain,
                                                              int* preply) const noexcept override;

    score::cpp::expected_blank<Error> fs_crypto_domain_add(
        const char* path,
        int domain,
        int type,
        int state,
        int length,
        const uint8_t bytes[],  // NOLINT(modernize-avoid-c-arrays) see comment above
        int* preply) const noexcept override;

    score::cpp::expected_blank<Error> fs_crypto_domain_query(const char* path,
                                                             int domain,
                                                             int* preply) const noexcept override;

    score::cpp::expected_blank<Error> fs_crypto_domain_unlock(
        const char* path,
        int domain,
        int length,
        const uint8_t bytes[],  // NOLINT(modernize-avoid-c-arrays) see comment above
        int* preply) const noexcept override;

    score::cpp::expected_blank<Error> fs_crypto_file_set_domain(const char* path,
                                                                int domain,
                                                                int* preply) const noexcept override;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_FS_CRYPTO_IMPL_H
