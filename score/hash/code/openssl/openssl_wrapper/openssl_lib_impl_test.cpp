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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace hash
{
namespace openssl
{
namespace
{

using namespace ::testing;
using ::testing::_;

class OpensslLibTest : public ::testing::Test
{
  public:
    void SetUp() override;
    void TearDown() override;

  protected:
    OpensslLibImpl unit_{};
    const StructDigest* digest_type_;
    StructDigestCtx* digest_context_;
};

void OpensslLibTest::SetUp()
{
    digest_type_ = unit_.DigestAlgoSha256();
    digest_context_ = unit_.CreateDigestCtx();
}

void OpensslLibTest::TearDown()
{
    unit_.ResetDigestCtx(digest_context_);
    digest_context_ = nullptr;
}

TEST_F(OpensslLibTest, Sha1Test)
{
    auto result = unit_.DigestAlgoSha1();

    EXPECT_NE(result, nullptr);
}

TEST_F(OpensslLibTest, Sha256Test)
{
    EXPECT_NE(digest_context_, nullptr);
}

TEST_F(OpensslLibTest, Sha384Test)
{
    auto result = unit_.DigestAlgoSha384();

    EXPECT_NE(result, nullptr);
}

TEST_F(OpensslLibTest, Sha512Test)
{
    auto result = unit_.DigestAlgoSha512();

    EXPECT_NE(result, nullptr);
}

TEST_F(OpensslLibTest, DigestCtxNewTest)
{
    EXPECT_NE(digest_context_, nullptr);
}

TEST_F(OpensslLibTest, DigestInitExTest)
{
    auto result = unit_.InitDigestCtx(digest_context_, digest_type_, nullptr);

    EXPECT_EQ(result, 1);
}

TEST_F(OpensslLibTest, DigestUpdateTest)
{
    char msg[] = "Hello World\n";

    unit_.InitDigestCtx(digest_context_, digest_type_, nullptr);

    auto result = unit_.UpdateDigestCtx(digest_context_, msg, strlen(msg));

    EXPECT_EQ(result, 1);
}

TEST_F(OpensslLibTest, DigestFinalExTest)
{
    char msg[] = "Hello World\n";
    unsigned char data[64];
    unsigned int size;

    unit_.InitDigestCtx(digest_context_, digest_type_, nullptr);
    unit_.UpdateDigestCtx(digest_context_, msg, strlen(msg));

    auto result = unit_.FinalizeDigestValue(digest_context_, data, &size);

    EXPECT_EQ(result, 1);
}

}  // namespace
}  // namespace openssl
}  // namespace hash
}  // namespace score
