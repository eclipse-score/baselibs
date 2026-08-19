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
#ifndef SCORE_LIB_OS_QNX_SLOG2_H
#define SCORE_LIB_OS_QNX_SLOG2_H

#include "score/os/errno.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <sys/slog2.h>

namespace score
{
namespace os
{
namespace qnx
{

class Slog2
{
  public:
    static score::cpp::pmr::unique_ptr<Slog2> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> slog2_register(
        const slog2_buffer_set_config_t* const config,
        slog2_buffer_t* const handles,
        const std::uint32_t flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual std::int32_t slog2_set_verbosity(const slog2_buffer_t buffer, const uint8_t verbosity) const noexcept = 0;

    virtual std::int32_t slog2_reset() const noexcept = 0;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> slog2c(const slog2_buffer_t buffer,
                                                                        const std::uint16_t code,
                                                                        const std::uint8_t severity,
                                                                        const char* const data) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.FUNC.VARARG:Required for wrapper method */
    virtual score::cpp::expected<std::int32_t, score::os::Error> slog2f(const slog2_buffer_t buffer,
                                                                        const std::uint16_t code,
                                                                        const std::uint8_t severity,
                                                                        const char* const format,
                                                                        ...) const noexcept
        /* KW_SUPPRESS_END:MISRA.FUNC.VARARG:Required for wrapper method */
        /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

        __attribute__((format(printf, 5, 6))) = 0;

    /// @brief Construct a new Slog2 object
    ///
    Slog2() = default;

    virtual ~Slog2() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Slog2(const Slog2&) = delete;
    Slog2& operator=(const Slog2&) = delete;
    Slog2(Slog2&& other) = delete;
    Slog2& operator=(Slog2&& other) = delete;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SLOG2_H
