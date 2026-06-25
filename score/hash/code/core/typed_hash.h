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

#ifndef SCORE_LIB_TYPED_HASH_H
#define SCORE_LIB_TYPED_HASH_H

#include "score/hash/code/core/hash.h"

namespace score
{
namespace hash
{

/// @brief TypedHash is a Hash-like object with a specific algorithm.
///
/// @tparam Algorithm The algorithm used by the hash. It is used when deserializing the hash from JSON and from string.
template <HashAlgorithm Algorithm>
class TypedHash final
{
  public:
    /// @brief Constructs a TypedHash from its raw binary values
    explicit TypedHash(Hash::ByteVector raw_hash);

    /// @brief Creates a TypedHash object from a Hexadecimal string.
    ///
    /// @param hex_repr The hexadecimal string representation of the hash.
    /// @return Result object that either contains a valid TypedHash or an error explaining why the string was not
    /// valid.
    static Result<TypedHash> FromString(const std::string_view& hex_repr);

    /// @brief Creates a TypedHash object from a JSON Any object.
    ///
    /// @return Result object that either contains a valid TypedHash or an error explaining why the JSON Any was not
    /// valid.
    [[nodiscard]] static score::Result<TypedHash> FromAny(score::json::Any any);

    /// @brief Converts the TypedHash object to a JSON Any object.
    [[nodiscard]] score::json::Any ToAny() const;

    /// @brief Converts the TypedHash object to a Hash object.
    [[nodiscard]] score::hash::Hash ToHash() const;

    /// @brief Retreives the raw hash value.
    ///
    /// @return score::cpp::span with the pointer holding the hash value.
    score::cpp::span<const std::uint8_t> GetBytes() const& noexcept;

    /// @brief Returns the TypedHash value's Hexadecimal string representation.
    /// @return String with the Hexadecimal representation.
    score::cpp::pmr::string ToString() const;

    /// @brief Returns the algorithm being represented by the hash instance.
    /// @return Algorithm in use
    HashAlgorithm GetAlgorithm() const noexcept;

    /// @brief Compares if two hashes use the same algorithm and if their content matches
    /// @return true, if both use the same algorithm and have the same content, false otherwise
    template <HashAlgorithm Alg>
    friend bool operator==(const TypedHash<Alg>& lhs, const TypedHash<Alg>& rhs) noexcept;
    template <HashAlgorithm Alg>
    friend bool operator!=(const TypedHash<Alg>& lhs, const TypedHash<Alg>& rhs) noexcept;

  private:
    Hash hash_;
};

template <HashAlgorithm Algorithm>
TypedHash<Algorithm>::TypedHash(Hash::ByteVector raw_hash) : hash_{Algorithm, std::move(raw_hash)}
{
}

template <HashAlgorithm Algorithm>
Result<TypedHash<Algorithm>> TypedHash<Algorithm>::FromString(const std::string_view& hex_repr)
{
    auto hash = Hash::FromString(Algorithm, hex_repr);
    if (hash.has_value())
    {
        const auto bytes = hash->GetBytes();
        return TypedHash{Hash::ByteVector{std::cbegin(bytes), std::cend(bytes)}};
    }
    return score::MakeUnexpected<TypedHash>(hash.error());
}

template <HashAlgorithm Algorithm>
score::Result<TypedHash<Algorithm>> TypedHash<Algorithm>::FromAny(score::json::Any any)
{
    if (const auto hash_string = any.As<std::string_view>(); hash_string.has_value())
    {
        return FromString(hash_string.value());
    }
    return score::MakeUnexpected(score::json::Error::kParsingError, "Expected hex string");
}

template <HashAlgorithm Algorithm>
score::json::Any TypedHash<Algorithm>::ToAny() const
{
    return score::json::Any{std::string{hash_.ToString()}};
}

template <HashAlgorithm Algorithm>
score::hash::Hash TypedHash<Algorithm>::ToHash() const
{
    return hash_;
}

template <HashAlgorithm Algorithm>
score::cpp::span<const std::uint8_t> TypedHash<Algorithm>::GetBytes() const& noexcept
{
    return hash_.GetBytes();
}

template <HashAlgorithm Algorithm>
score::cpp::pmr::string TypedHash<Algorithm>::ToString() const
{
    return hash_.ToString();
}

template <HashAlgorithm Algorithm>
HashAlgorithm TypedHash<Algorithm>::GetAlgorithm() const noexcept
{
    return hash_.GetAlgorithm();
}

template <HashAlgorithm Alg>
bool operator==(const TypedHash<Alg>& lhs, const TypedHash<Alg>& rhs) noexcept
{
    return lhs.hash_ == rhs.hash_;
}
template <HashAlgorithm Alg>
bool operator!=(const TypedHash<Alg>& lhs, const TypedHash<Alg>& rhs) noexcept
{
    return !(lhs == rhs);
}

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_TYPED_HASH_H
