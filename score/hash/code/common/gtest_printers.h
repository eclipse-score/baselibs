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

#ifndef SCORE_LIB_HASH_CODE_COMMON_GTEST_PRINTERS_H
#define SCORE_LIB_HASH_CODE_COMMON_GTEST_PRINTERS_H

#include "score/hash/code/core/hash.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <score/span.hpp>
#include <cstdint>

/// @brief Add custom print functions for gtest. To make it compatible with deviation of score::cpp::span from the standard
/// with respect to const_iterators.
namespace score::cpp
{
std::ostream& operator<<(std::ostream& out, const score::cpp::span<const std::uint8_t> span);
void PrintTo(const score::cpp::span<const std::uint8_t>& span, std::ostream* os);
}  // namespace score::cpp

namespace score
{
namespace hash
{
std::ostream& operator<<(std::ostream& out, const Hash& hash);
void PrintTo(const Hash& hash, std::ostream* os);
}  // namespace hash
}  // namespace score

#endif  // SCORE_LIB_HASH_CODE_COMMON_GTEST_PRINTERS_H
