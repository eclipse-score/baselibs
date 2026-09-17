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
#include "score/os/qnx/inout_impl.h"
#include <hw/inout.h>

score::cpp::expected<std::uint8_t, score::os::Error> score::os::qnx::InOutQnx::in8(const uintptr_t port) const noexcept
{
    return ::in8(static_cast<std::uint16_t>(port));
}

score::cpp::expected<std::uint16_t, score::os::Error> score::os::qnx::InOutQnx::in16(
    const uintptr_t port) const noexcept
{
    return ::in16(static_cast<std::uint16_t>(port));
}

score::cpp::expected<std::uint32_t, score::os::Error> score::os::qnx::InOutQnx::in32(
    const uintptr_t port) const noexcept
{
    return ::in32(static_cast<std::uint16_t>(port));
}

score::cpp::expected_blank<score::os::Error> score::os::qnx::InOutQnx::out8(const uintptr_t port,
                                                                            const uint8_t val) noexcept
{
    ::out8(static_cast<std::uint16_t>(port), val);
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::qnx::InOutQnx::out16(const uintptr_t port,
                                                                             const uint16_t val) noexcept
{
    ::out16(static_cast<std::uint16_t>(port), val);
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::qnx::InOutQnx::out32(const uintptr_t port,
                                                                             const uint32_t val) noexcept
{
    ::out32(static_cast<std::uint16_t>(port), val);
    return {};
}
