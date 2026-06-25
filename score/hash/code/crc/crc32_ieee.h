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

#ifndef SCORE_LIB_HASH_CODE_CRC_CRC32_IEEE_H
#define SCORE_LIB_HASH_CODE_CRC_CRC32_IEEE_H

#include "score/hash/code/crc/i_crc32.h"

#include <cstdint>

namespace score
{
namespace hash
{

/// @brief CRC32 hash calculator using the IEEE 802.3 polynomial (0x04C11DB7).
class Crc32IeeeHashCalculator final : public ICrc32HashCalculator
{
  public:
    Crc32IeeeHashCalculator() noexcept;
    ResultBlank Update(const score::cpp::span<const std::uint8_t> data) noexcept override;
    Hash Finalize() noexcept override;
    std::uint_fast32_t GetChecksum() const noexcept override;

  private:
    std::uint_fast32_t checksum_;
};

}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_CRC_CRC32_IEEE_H
