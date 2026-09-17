/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_OS_QNX_INOUT_H
#define SCORE_LIB_OS_QNX_INOUT_H

#include "score/expected.hpp"
#include "score/os/errno.h"

namespace score
{
namespace os
{
namespace qnx
{

class InOut
{
  public:
    virtual score::cpp::expected<std::uint8_t, score::os::Error> in8(const uintptr_t port) const noexcept = 0;

    virtual score::cpp::expected<std::uint16_t, score::os::Error> in16(const uintptr_t port) const noexcept = 0;

    virtual score::cpp::expected<std::uint32_t, score::os::Error> in32(const uintptr_t port) const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> out8(const uintptr_t port, const uint8_t val) noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> out16(const uintptr_t port, const uint16_t val) noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> out32(const uintptr_t port, const uint32_t val) noexcept = 0;

    virtual ~InOut() = default;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_INOUT_H
