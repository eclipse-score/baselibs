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
#ifndef SCORE_LIB_OS_STAT_IMPL_H
#define SCORE_LIB_OS_STAT_IMPL_H

#include "score/os/stat.h"

namespace score
{
namespace os
{

class StatImpl final : public Stat
{
  public:
    constexpr StatImpl() = default;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    // This is a wrapper function and it simulates the behaviour of OS function
    // NOLINTNEXTLINE(google-default-arguments) see comment above
    score::cpp::expected_blank<Error> stat(const char* const file,
                                           StatBuffer& buf,
                                           const bool resolve_symlinks = true) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> fstat(const std::int32_t fd, StatBuffer& buf) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> mkdir(const char* const path, const Mode mode) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> chmod(const char* const path, const Mode mode) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> fchmod(const std::int32_t fd, const Mode mode) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<Mode, Error> umask(const Mode umask) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> fchmodat(const std::int32_t fd,
                                               const char* const path,
                                               const Mode mode,
                                               const bool resolve_symlinks) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
  private:
    void stat_to_statbuffer(const struct stat& native_stat, StatBuffer& stat_buffer) const noexcept;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_STAT_IMPL_H
