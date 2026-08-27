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
#include "score/os/qnx/slog2_impl.h"

namespace score
{
namespace os
{
namespace qnx
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> Slog2Impl::slog2_register(
    const slog2_buffer_set_config_t* const config,
    slog2_buffer_t* const handles,
    const std::uint32_t flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::slog2_register(config, handles, flags);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

std::int32_t Slog2Impl::slog2_set_verbosity(const slog2_buffer_t buffer, const uint8_t verbosity) const noexcept
{
    return ::slog2_set_verbosity(buffer, verbosity);
}

std::int32_t Slog2Impl::slog2_reset() const noexcept
{
    return ::slog2_reset();
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> Slog2Impl::slog2c(const slog2_buffer_t buffer,
                                                                       const std::uint16_t code,
                                                                       const std::uint8_t severity,
                                                                       const char* const data) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::slog2c(buffer, code, severity, data);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:MISRA.FUNC.VARARG:Required for wrapper method */
// coverity[autosar_cpp14_a8_4_1_violation]: see above
score::cpp::expected<std::int32_t, score::os::Error> Slog2Impl::slog2f(const slog2_buffer_t buffer,
                                                                       const std::uint16_t code,
                                                                       const std::uint8_t severity,
                                                                       const char* const format,
                                                                       ...) const noexcept
/* KW_SUPPRESS_END:MISRA.FUNC.VARARG:Required for wrapper method */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
    va_list args;
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) see comment above
    va_start(args, format);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) see comment above
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(*array-decay, *pro-type-vararg,*array-to-pointer-decay) see comment above
    const auto result = ::vslog2f(buffer, code, severity, format, args);
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) see comment above
    va_end(args);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) see comment above
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

}  // namespace qnx
}  // namespace os
}  // namespace score
