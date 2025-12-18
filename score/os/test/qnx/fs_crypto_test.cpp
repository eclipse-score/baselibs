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
#include "score/os/qnx/fs_crypto.h"
#include "score/os/mocklib/qnx/mock_fs_crypto.h"

#include <fs_crypto_api.h>
#include <sys/fs_crypto.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Return;

namespace
{

TEST(FsCrypto, CreateObjectSuccessful)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Create Object Successful");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto fscrypto = score::os::qnx::FsCrypto::createFsCryptoInstance();
    EXPECT_TRUE(fscrypto);
    auto another_fscrypto = score::os::qnx::FsCrypto::createFsCryptoInstance();
    EXPECT_TRUE(another_fscrypto);
    EXPECT_NE(fscrypto.get(), another_fscrypto.get());
    EXPECT_NO_THROW(fscrypto.reset());
    EXPECT_NO_THROW(another_fscrypto.reset());
}

struct fsCryptoMockTest : public ::testing::Test
{
    void SetUp() override
    {
        fscryptoMock = std::make_shared<score::os::qnx::MockFsCrypto>();
        fscrypto = fscryptoMock;
    }
    void TearDown() override
    {
        fscrypto.reset();
        fscryptoMock.reset();
    }

    std::shared_ptr<score::os::qnx::MockFsCrypto> fscryptoMock;
    std::shared_ptr<score::os::qnx::FsCrypto> fscrypto;
};

TEST_F(fsCryptoMockTest, Test_fs_crypto_domain_remove_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_remove Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_remove(path, domain, preply))
        .WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    auto result = fscrypto->fs_crypto_domain_remove(path, domain, preply);
    EXPECT_TRUE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_remove_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_remove Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_remove(path, domain, preply))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));
    auto result = fscrypto->fs_crypto_domain_remove(path, domain, preply);
    EXPECT_FALSE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_domain_add_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_add Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::size_t bytes_length = 512;
    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t type{FS_CRYPTO_TYPE_XTS};
    std::int32_t state{0};
    const uint8_t bytes[bytes_length] = {};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_add(path, domain, type, state, bytes_length, bytes, preply))
        .WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    auto result = fscrypto->fs_crypto_domain_add(path, domain, type, state, bytes_length, bytes, preply);
    EXPECT_TRUE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_domain_add_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_add Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::size_t bytes_length = 1;
    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t type{FS_CRYPTO_TYPE_XTS};
    std::int32_t state{0};
    const uint8_t bytes[bytes_length] = {};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_add(path, domain, type, state, bytes_length, bytes, preply))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));
    auto result = fscrypto->fs_crypto_domain_add(path, domain, type, state, bytes_length, bytes, preply);
    EXPECT_FALSE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_domain_query_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_query Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_query(path, domain, preply))
        .WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    auto result = fscrypto->fs_crypto_domain_query(path, domain, preply);
    EXPECT_TRUE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_domain_query_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_query Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_query(path, domain, preply))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));
    auto result = fscrypto->fs_crypto_domain_query(path, domain, preply);
    EXPECT_FALSE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_domain_unlock_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_unlock Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t length{3};
    const uint8_t bytes[1] = {};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_unlock(path, domain, length, bytes, preply))
        .WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    auto result = fscrypto->fs_crypto_domain_unlock(path, domain, length, bytes, preply);
    EXPECT_TRUE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_domain_unlock_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_domain_unlock Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t length{0};
    const uint8_t bytes[1] = {};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_domain_unlock(path, domain, length, bytes, preply))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));
    auto result = fscrypto->fs_crypto_domain_unlock(path, domain, length, bytes, preply);
    EXPECT_FALSE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_file_set_domain_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_file_set_domain Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_file_set_domain(path, domain, preply))
        .WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    auto result = fscrypto->fs_crypto_file_set_domain(path, domain, preply);
    EXPECT_TRUE(result.has_value());
}

TEST_F(fsCryptoMockTest, Test_fs_crypto_file_set_domain_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test fs_crypto_file_set_domain Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* path = "/persistent";
    std::int32_t domain{3};
    std::int32_t* preply{nullptr};

    EXPECT_CALL(*fscryptoMock, fs_crypto_file_set_domain(path, domain, preply))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));
    auto result = fscrypto->fs_crypto_file_set_domain(path, domain, preply);
    EXPECT_FALSE(result.has_value());
}

}  // namespace
