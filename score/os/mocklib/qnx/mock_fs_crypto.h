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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_FS_CRYPTO_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_FS_CRYPTO_H

#include "score/os/qnx/fs_crypto.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

namespace qnx
{

class MockFsCrypto : public FsCrypto
{
  public:
    ~MockFsCrypto() override = default;

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                fs_crypto_domain_remove,
                (const char*, int, int*),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                fs_crypto_domain_add,
                (const char*, int, int, int, int, const uint8_t bytes[], int*),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                fs_crypto_domain_query,
                (const char*, int, int*),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                fs_crypto_domain_unlock,
                (const char*, int, int, const uint8_t bytes[], int*),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                fs_crypto_file_set_domain,
                (const char*, int, int*),
                (const, noexcept, override));
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_FS_CRYPTO_H
