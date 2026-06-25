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

#ifndef SCORE_LIB_HASH_CODE_CORE_HASHCALCULATOR_H
#define SCORE_LIB_HASH_CODE_CORE_HASHCALCULATOR_H

#include "score/hash/code/core/hash.h"
#include "score/result/result.h"

#include <score/span.hpp>

#include <cstdint>
#include <istream>

namespace score
{
namespace hash
{

/// @brief This is an interface that calculates cryptographic hash over blocks of data.
///
/// Depending on the algorithm selected, the hash value is calculated.
/// Example : Input data = 123abc sha256sum = DD130A849D7B29E5541B05D2F7F86A4ACD4F1EC598C1C9438783F56BC4F0FF80
class IHashCalculator
{
  public:
    IHashCalculator() noexcept = default;
    virtual ~IHashCalculator() noexcept = default;
    IHashCalculator(const IHashCalculator&) noexcept = delete;
    IHashCalculator& operator=(const IHashCalculator&) noexcept = delete;

    /// @brief Update current hash calculation with some more data
    /// This method can be called multiple times for hash calculation
    /// over more than one block of data. The data should not be empty.
    ///
    /// @param[in] data pointer to the block of data that should be added.
    ///
    /// @return score::cpp::blank upon successful update, error otherwise
    virtual ResultBlank Update(const score::cpp::span<const std::uint8_t> data) noexcept = 0;

    /// @brief Update current hash calculation with input stream
    /// This method can be called multiple times for hash calculation
    /// over more than one file.
    ///
    /// By default, this method will read the given stream and feed it into the Update method.
    ///
    /// @param[in] input istream whose data should be used for update
    ///
    /// @return score::cpp::blank upon successful update, error otherwise
    virtual ResultBlank UpdateFromStream(std::istream& input);

    /// @brief Update current hash calculation with input stream
    /// This method can be called multiple times for hash calculation
    /// over more than one file.
    ///
    /// By default, this method will read the given stream and feed it into the Update method.
    ///
    /// @param[in] input istream whose data should be used for update
    /// @param[in] max_read the number of bytes that shall be read from the stream
    ///
    /// @return score::cpp::blank upon successful update, error otherwise
    virtual ResultBlank UpdateFromStream(std::istream& input, const std::int64_t max_read);

    /// @brief Finalize hash calculation and retrieve computed hash value
    ///
    /// @return string with the computed hash data, empty if not successful
    virtual Hash Finalize() noexcept = 0;

  protected:
    IHashCalculator(IHashCalculator&&) noexcept = default;
    IHashCalculator& operator=(IHashCalculator&&) noexcept = default;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_CORE_HASHCALCULATOR_H
