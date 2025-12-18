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
#include "score/os/qnx/fs_crypto.h"

#include <fs_crypto_api.h>
#include <sys/fs_crypto.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;

namespace
{

struct fsCryptoImplTest : public ::testing::Test
{

    static constexpr std::size_t bytes_length = 64;
    uint8_t bytes[bytes_length] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
                                   0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
                                   0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                   0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34,
                                   0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40};
    void SetUp() override
    {
        fscrypto_ptr_ = std::make_unique<score::os::qnx::FsCryptoImpl>();
    }
    void TearDown() override
    {
        fscrypto_ptr_.release();
    }

    std::unique_ptr<score::os::qnx::FsCrypto> fscrypto_ptr_;
};

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_add_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_add Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent";
    std::int32_t domain{6};
    std::int32_t type{FS_CRYPTO_TYPE_XTS};
    std::int32_t state{0};

    std::int32_t preply{0};
    auto res = fscrypto_ptr_->fs_crypto_domain_add(path, domain, type, state, bytes_length, bytes, &preply);
    EXPECT_TRUE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_set_domain_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_file_set_domain Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent/test";
    std::int32_t domain{6};
    std::int32_t preply{0};
    mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    auto res = fscrypto_ptr_->fs_crypto_file_set_domain(path, domain, &preply);
    EXPECT_TRUE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_add_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_add Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t bytes_length = 64;
    const char* path = "/persistent/test";
    std::int32_t domain{-1};
    std::int32_t type{FS_CRYPTO_TYPE_XTS};
    std::int32_t state{0};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_domain_add(path, domain, type, state, bytes_length, bytes, &preply);
    EXPECT_FALSE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_query_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_query Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent";
    std::int32_t domain{6};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_domain_query(path, domain, &preply);
    EXPECT_TRUE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_query_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_query Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent/test";
    std::int32_t domain{-1};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_domain_query(path, domain, &preply);
    EXPECT_FALSE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_unlock_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_unlock Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent";
    std::int32_t domain{6};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_domain_unlock(path, domain, bytes_length, bytes, &preply);
    EXPECT_TRUE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_unlock_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_unlock Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent/test";
    std::int32_t domain{-1};
    std::int32_t length{5};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_domain_unlock(path, domain, length, bytes, &preply);
    EXPECT_FALSE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_set_domain_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_file_set_domain Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent/test";
    std::int32_t domain{-1};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_file_set_domain(path, domain, &preply);
    EXPECT_FALSE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_remove_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_remove Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent/test";
    std::int32_t domain{-1};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_domain_remove(path, domain, &preply);
    EXPECT_FALSE(res.has_value());
}

TEST_F(fsCryptoImplTest, TestFunction_fs_crypto_domain_remove_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function fs_crypto_domain_remove Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* path = "/persistent";
    std::int32_t domain{6};
    std::int32_t preply{0};

    auto res = fscrypto_ptr_->fs_crypto_domain_remove(path, domain, &preply);
    EXPECT_TRUE(res.has_value());
}

}  // namespace
