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
#ifndef SCORE_LIB_OS_QNX_FS_CRYPTO_H
#define SCORE_LIB_OS_QNX_FS_CRYPTO_H

#include "score/os/errno.h"

#include "score/expected.hpp"

#include <fs_crypto_api.h>
#include <sys/fs_crypto.h>
#include <memory>

namespace score
{
namespace os
{
namespace qnx
{

///
/// @brief fs_crypto functions OSAL class
/// [QNX fs_crypto
/// documentation](https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.utilities/topic/f/fsencrypt.html)
///
class FsCrypto
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    static std::unique_ptr<score::os::qnx::FsCrypto> createFsCryptoInstance() noexcept;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> fs_crypto_domain_remove(const char* path,
                                                               int domain,
                                                               int* preply) const noexcept = 0;

    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
    virtual score::cpp::expected_blank<Error> fs_crypto_domain_add(
        const char* path,
        int domain,
        int type,
        int state,
        int length,
        const uint8_t bytes[],  // NOLINT(modernize-avoid-c-arrays) see comment above
        int* preply) const noexcept = 0;

    /* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> fs_crypto_domain_query(const char* path,
                                                              int domain,
                                                              int* preply) const noexcept = 0;

    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
    virtual score::cpp::expected_blank<Error> fs_crypto_domain_unlock(
        const char* path,
        int domain,
        int length,
        const uint8_t bytes[],  // NOLINT(modernize-avoid-c-arrays) see comment above
        int* preply) const noexcept = 0;

    /* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> fs_crypto_file_set_domain(const char* path,
                                                                 int domain,
                                                                 int* preply) const noexcept = 0;

    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~FsCrypto() = default;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_FS_CRYPTO_H
