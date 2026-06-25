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

#ifndef SCORE_LIB_HASH_CODE_CRC_I_CRC32_H
#define SCORE_LIB_HASH_CODE_CRC_I_CRC32_H

#include "score/hash/code/core/i_hash_calculator.h"

#include <cstdint>

namespace score
{
namespace hash
{

/// Base class for all implementations of a CRC32 digest.
class ICrc32HashCalculator : public IHashCalculator
{
  public:
    ICrc32HashCalculator() noexcept = default;
    virtual ~ICrc32HashCalculator() noexcept = default;
    ICrc32HashCalculator(const ICrc32HashCalculator&) noexcept = delete;
    ICrc32HashCalculator(ICrc32HashCalculator&&) noexcept = delete;
    ICrc32HashCalculator& operator=(const ICrc32HashCalculator&) noexcept = delete;
    ICrc32HashCalculator& operator=(ICrc32HashCalculator&&) noexcept = delete;

    /// @brief Returns the calculated CRC32 as one 32 bit value.
    ///
    /// This is equivalent to calling `Finalize` and reassembling the value in little endian mode.
    /// @return Checksum as a 32 bit value.
    virtual std::uint_fast32_t GetChecksum() const noexcept = 0;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_CRC_I_CRC32_H
