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

#include "score/os/qnx/fs_crypto_impl.h"

namespace score
{
namespace os
{
namespace qnx
{
score::cpp::expected_blank<Error> FsCryptoImpl::fs_crypto_domain_remove(const char* path,
                                                                        int domain,
                                                                        int* preply) const noexcept
{
    const std::int32_t err = ::fs_crypto_domain_remove(path, domain, preply);
    if (0U != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

score::cpp::expected_blank<Error> FsCryptoImpl::fs_crypto_domain_add(
    const char* path,
    int domain,
    int type,
    int state,
    int length,
    const uint8_t bytes[],  // NOLINT(modernize-avoid-c-arrays) see comment above
    int* preply) const noexcept
{
    const std::int32_t err = ::fs_crypto_domain_add(path, domain, type, state, length, bytes, preply);
    if (0U != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

score::cpp::expected_blank<Error> FsCryptoImpl::fs_crypto_domain_query(const char* path,
                                                                       int domain,
                                                                       int* preply) const noexcept
{
    const std::int32_t err = ::fs_crypto_domain_query(path, domain, preply);
    if (0U != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

score::cpp::expected_blank<Error> FsCryptoImpl::fs_crypto_domain_unlock(
    const char* path,
    int domain,
    int length,
    const uint8_t bytes[],  // NOLINT(modernize-avoid-c-arrays) see comment above
    int* preply) const noexcept
{
    const std::int32_t err = ::fs_crypto_domain_unlock(path, domain, length, bytes, preply);
    if (0U != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

score::cpp::expected_blank<Error> FsCryptoImpl::fs_crypto_file_set_domain(const char* path,
                                                                          int domain,
                                                                          int* preply) const noexcept
{
    const std::int32_t err = ::fs_crypto_file_set_domain(path, domain, preply);
    if (0U != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

}  // namespace qnx
}  // namespace os
}  // namespace score
