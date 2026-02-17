/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Hash component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_HASH_HPP
#define SCORE_LANGUAGE_FUTURECPP_HASH_HPP

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <type_traits>

namespace score::cpp
{
namespace detail
{
// parameters are defined according to http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param
// or here: https://www.ietf.org/id/draft-eastlake-fnv-20.html
template <std::size_t Size>
struct fnv_params
{
};

template <>
struct fnv_params<4U> // 32 bit
{
    using value_type = std::uint32_t;
    static constexpr std::uint32_t prime = 16777619U;
    static constexpr std::uint32_t offset_basis = 2166136261U;
    static constexpr std::uint32_t next(std::uint32_t state, unsigned char octet)
    {
        state ^= octet;
        state *= fnv_params::prime;
        return state;
    }
};

template <>
struct fnv_params<8U> // 64 bit
{
    using value_type = std::uint64_t;
    static constexpr std::uint64_t prime = 1099511628211UL;
    static constexpr std::uint64_t offset_basis = 14695981039346656037UL;
    static constexpr std::uint64_t next(std::uint64_t state, unsigned char octet)
    {
        state ^= octet;
        state *= fnv_params::prime;
        return state;
    }
};

} // namespace detail

/// \brief Perform a 32 or 64 bit Fowler/Noll/Vo FNV-1a hash (continued) on a buffer
///
/// Fowler / Noll / Vo (FNV) hashes (type FNV-1a) are designed to be fast while maintaining a low collision
/// rate. The FNV speed allows one to quickly hash lots of data while maintaining a reasonable collision rate.
///
/// \param data input which is treated as bytestream and used for hashing.
/// \param size number of bytes within data for creating the hash.
/// \param hash hash value already calculated for previous data.
/// \return combined hashed value of previous and given data.
template <typename T>
constexpr T hash_bytes_fnv1a(const void* data, std::size_t size, T hash)
{
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "FNV1a is only implemented for 32 and 64 bit platforms");
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    const auto* const data_begin{static_cast<const unsigned char*>(data)};
    const auto* const data_end{data_begin + size};
    return std::accumulate(data_begin, data_end, hash, detail::fnv_params<sizeof(T)>::next);
}

/// \brief Perform a 32 or 64 bit Fowler/Noll/Vo FNV-1a hash on a buffer
///
/// Fowler / Noll / Vo (FNV) hashes (type FNV-1a) are designed to be fast while maintaining a low collision
/// rate. The FNV speed allows one to quickly hash lots of data while maintaining a reasonable collision rate.
///
/// \param data input which is treated as bytestream and used for hashing.
/// \param size number of bytes within data for creating the hash.
/// \return hashed value of given data.
template <typename T = std::size_t>
constexpr T hash_bytes_fnv1a(const void* data, std::size_t size)
{
    return score::cpp::hash_bytes_fnv1a(data, size, detail::fnv_params<sizeof(T)>::offset_basis);
}

/// \brief Perform a 32 or 64 bit hash on a buffer
///
/// Note: do not rely on a specific hash algorithm.
///
/// \param data input which is treated as bytestream and used for hashing.
/// \param size number of bytes within data for creating the hash.
/// \return hashed value of given data.
constexpr std::size_t hash_bytes(const void* data, std::size_t size) { return score::cpp::hash_bytes_fnv1a(data, size); }

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_HASH_HPP
