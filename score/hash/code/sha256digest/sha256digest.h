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

// DISCLAIMER: This implementation of SHA-256 is intended strictly for non-cryptographic purposes
// (e.g., data integrity checks, checksums).
// It must NOT be used for any security-sensitive or cryptographic applications,
// including but not limited to password hashing, digital signatures, or key derivation.

#ifndef SCORE_LIB_HASH_SHA256DIGEST_SHA256DIGEST_H
#define SCORE_LIB_HASH_SHA256DIGEST_SHA256DIGEST_H

#include "score/hash/code/core/i_hash_calculator.h"

#include <score/utility.hpp>

#include <cstdint>

namespace score
{
namespace hash
{

namespace detail
{

template <typename T, std::size_t N>
class MessageBuffer final
{
  public:
    using SizeType = std::uint8_t;

    constexpr MessageBuffer() noexcept : buffer_{}, size_{} {}

    constexpr SizeType size() const noexcept
    {
        return size_;
    }

    constexpr void clear() noexcept
    {
        size_ = SizeType{};
    }

    constexpr void push_back(const T& val) noexcept
    {
        const auto size_casted = static_cast<std::size_t>(size_);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(size_casted < N);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bounds checked above
        buffer_[size_casted] = val;
        ++size_;
    }

    constexpr T& operator[](std::size_t pos) noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(pos < N);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bounds checked above
        return buffer_[pos];
    }

    constexpr const T& operator[](std::size_t pos) const noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(pos < N);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bounds checked above
        return buffer_[pos];
    }

    constexpr SizeType fill_from(const score::cpp::span<const T>& data)
    {
        const auto data_size = std::min(static_cast<std::size_t>(kMaxSize), static_cast<std::size_t>(data.size()));
        const auto size_casted = static_cast<std::size_t>(size_);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(size_casted < N);
        const auto remaining = N - size_casted;
        const auto count = std::min(remaining, data_size);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index) bounds checked above
        score::cpp::ignore = std::copy_n(data.begin(), count, &(buffer_[size_casted]));

        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(size_casted + count <= std::numeric_limits<SizeType>::max());
        size_ = static_cast<SizeType>(size_ + static_cast<SizeType>(count));
        return static_cast<SizeType>(count);
    }

  private:
    std::array<T, N> buffer_;
    SizeType size_;

    static constexpr SizeType kMaxSize{std::numeric_limits<SizeType>::max()};
    static_assert(N <= std::size_t{kMaxSize}, "Maximum size of MessageBuffer exceeded");
};

}  // namespace detail

class Sha256Digest final : public IHashCalculator
{
  public:
    Sha256Digest() noexcept;

    ResultBlank Update(const score::cpp::span<const std::uint8_t> data) noexcept override;
    Hash Finalize() noexcept override;

  private:
    // False positive, variable is used.
    // coverity[autosar_cpp14_a0_1_1_violation]
    static constexpr std::size_t kHashSize{8U};
    static constexpr std::size_t kBufferSize{64U};

    using MessageBuffer = detail::MessageBuffer<std::uint8_t, kBufferSize>;

    void UpdateHash() noexcept;

    MessageBuffer buffer_;
    std::array<std::uint32_t, kHashSize> hash_;
    std::uint64_t message_len_;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_SHA256DIGEST_SHA256DIGEST_H
