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

#include "score/hash/code/openssl/openssl_hash_calculator.h"
#include "score/hash/code/common/error.h"
#include "score/hash/code/openssl/openssl_wrapper/openssl_lib_impl.h"

#include <score/static_vector.hpp>
#include <score/utility.hpp>

namespace score
{
namespace hash
{

OpensslHashCalculator::OpensslHashCalculator(const HashAlgorithm algorithm,
                                             openssl::StructDigestCtx* const message_digest_context,
                                             const openssl::IOpensslLib& openssl)
    : IHashCalculator(),
      message_digest_context_{message_digest_context,
                              [openssl_lib{std::ref(openssl)}](openssl::StructDigestCtx* ptr) noexcept {
                                  openssl_lib.get().ResetDigestCtx(ptr);
                              }},
      algorithm_{algorithm},
      openssl_{openssl}
{
}

Result<OpensslHashCalculator> OpensslHashCalculator::Create(const HashAlgorithm algorithm,
                                                            const openssl::IOpensslLib& openssl) noexcept
{
    const openssl::StructDigest* message_digest_type = nullptr;

    switch (algorithm)
    {
        case HashAlgorithm::kSha1:
            message_digest_type = openssl.DigestAlgoSha1();
            break;

        case HashAlgorithm::kSha256:
            message_digest_type = openssl.DigestAlgoSha256();
            break;

        case HashAlgorithm::kSha384:
            message_digest_type = openssl.DigestAlgoSha384();
            break;

        case HashAlgorithm::kSha512:
            message_digest_type = openssl.DigestAlgoSha512();
            break;

        case HashAlgorithm::kNone:
        case HashAlgorithm::kCrc32:
        case HashAlgorithm::kCrc32Unused:
        case HashAlgorithm::kLast:
        default:
            mw::log::LogError() << "OpensslHashCalculator::Unknown digest algorithm";
            break;
    }

    if (nullptr == message_digest_type)
    {
        mw::log::LogError() << "OpensslHashCalculator::Digest algorithm not created";
        return MakeUnexpected(ErrorCode::kCouldNotCreateDigest);
    }

    auto* const message_digest_context = openssl.CreateDigestCtx();

    if (nullptr == message_digest_context)
    {
        mw::log::LogError() << "OpensslHashCalculator::Could not create message digest context.";
        return MakeUnexpected(ErrorCode::kCouldNotCreateDigest);
    }

    if (0 == openssl.InitDigestCtx(message_digest_context, message_digest_type, nullptr))
    {
        mw::log::LogError() << "OpensslHashCalculator::Could not create message digest algorithm.";
        return MakeUnexpected(ErrorCode::kCouldNotCreateDigest);
    }

    return OpensslHashCalculator(algorithm, message_digest_context, openssl);
}

ResultBlank OpensslHashCalculator::Update(const score::cpp::span<const std::uint8_t> data) noexcept
{
    if (nullptr == data.data())
    {
        mw::log::LogError() << "OpensslHashCalculator::Invalid parameters";
        return MakeUnexpected(ErrorCode::kInvalidParameters);
    }

    if (0 == openssl_.get().UpdateDigestCtx(message_digest_context_.get(),
                                            static_cast<const void*>(data.data()),
                                            static_cast<std::size_t>(data.size())))
    {
        mw::log::LogError() << "OpensslHashCalculator::Unable to update the digest context." << __LINE__;
        return MakeUnexpected(ErrorCode::kCouldNotUpdateDigest);
    }

    return {};
}

Hash OpensslHashCalculator::Finalize() noexcept
{
    Hash result{HashAlgorithm::kNone, {}};
    std::uint32_t message_digest_size{0U};
    score::cpp::static_vector<std::uint8_t, kMaxDigestSize> message_digest_value(kMaxDigestSize);

    if (0 == openssl_.get().FinalizeDigestValue(
                 message_digest_context_.get(), message_digest_value.data(), &message_digest_size))
    {
        mw::log::LogError() << "OpensslHashCalculator::Could not finalize message digest algorithm";
    }
    else
    {
        message_digest_value.resize(static_cast<std::size_t>(message_digest_size));
        result = Hash{algorithm_, std::move(message_digest_value)};
    }

    return result;
}

const openssl::IOpensslLib& OpensslHashCalculator::GetDefaultOpenSslImpl()
{
    static const openssl::OpensslLibImpl impl{};
    return impl;
}

}  // namespace hash
}  // namespace score
