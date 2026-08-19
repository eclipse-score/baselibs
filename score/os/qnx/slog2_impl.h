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
#ifndef SCORE_LIB_OS_QNX_SLOG2_IMPL_H
#define SCORE_LIB_OS_QNX_SLOG2_IMPL_H

#include "score/os/qnx/slog2.h"

namespace score
{
namespace os
{
namespace qnx
{

class Slog2Impl : public Slog2
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> slog2_register(
        const slog2_buffer_set_config_t* const config,
        slog2_buffer_t* const handles,
        const std::uint32_t flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    std::int32_t slog2_set_verbosity(const slog2_buffer_t buffer, const uint8_t verbosity) const noexcept override;

    std::int32_t slog2_reset() const noexcept override;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> slog2c(const slog2_buffer_t buffer,
                                                                const std::uint16_t code,
                                                                const std::uint8_t severity,
                                                                const char* const data) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.FUNC.VARARG:Required for wrapper method */
    // coverity[autosar_cpp14_a8_4_1_violation]: see above
    score::cpp::expected<std::int32_t, score::os::Error> slog2f(const slog2_buffer_t buffer,
                                                                const std::uint16_t code,
                                                                const std::uint8_t severity,
                                                                const char* const format,
                                                                ...) const noexcept override
        __attribute__((format(printf, 5, 6)));
    /* KW_SUPPRESS_END:MISRA.FUNC.VARARG:Required for wrapper method */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SLOG2_IMPL_H
