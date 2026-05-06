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

#ifndef SCORE_LIB_HASH_HASH_H
#define SCORE_LIB_HASH_HASH_H

#include "score/hash/code/common/algorithms.h"
#include "score/json/internal/model/any.h"
#include "score/result/result.h"

#include <score/span.hpp>
#include <score/static_vector.hpp>
#include <score/string_view.hpp>

namespace score
{
namespace hash
{

/// @brief Encapsulates a hash by owning its value and type
class Hash final
{
  public:
    using ByteVector = score::cpp::static_vector<std::uint8_t, kMaxDigestSize>;

    /// @brief Constructs a hash from its raw binary values and its algorithm type
    Hash(const HashAlgorithm algorithm, ByteVector raw_hash);

    /// @brief Retreives the raw hash value.
    ///
    /// @return score::cpp::span with the pointer holding the hash value.
    score::cpp::span<const std::uint8_t> GetBytes() const& noexcept;

    /// @brief Creates a Hash object from a Hexadecimal string.
    ///
    /// @return Result object that either contains a valid Hash or an error explaining why the string was not valid.
    static Result<Hash> FromString(const HashAlgorithm algorithm, const score::cpp::string_view& hex_repr);

    /// @brief Returns the Hash value's Hexadecimal string representation.
    /// @return String with the Hexadecimal representation.
    score::cpp::pmr::string ToString() const;

    /// @brief Returns the algorithm being represented by the hash instance.
    /// @return Algorithm in use
    HashAlgorithm GetAlgorithm() const noexcept;

    /// @brief Compares if two hashes use the same algorithm and if their content matches
    /// @return true, if both use the same algorithm and have the same content, false otherwise
    friend bool operator==(const Hash& lhs, const Hash& rhs) noexcept;
    friend bool operator!=(const Hash& lhs, const Hash& rhs) noexcept;

  private:
    HashAlgorithm algorithm_;
    score::cpp::static_vector<std::uint8_t, kMaxDigestSize> raw_hash_;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_HASH_H
