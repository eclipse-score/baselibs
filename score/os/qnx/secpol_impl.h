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
#ifndef SCORE_LIB_OS_QNX_SECPOL_IMPL_H
#define SCORE_LIB_OS_QNX_SECPOL_IMPL_H

#include "score/os/qnx/secpol.h"
#include <secpol/secpol.h>

namespace score
{
namespace os
{
namespace qnx
{

namespace details
{
class SecpolImpl final : public Secpol
{
  public:
    constexpr SecpolImpl() = default;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<secpol_file_t*, score::os::Error> secpol_open(
        const char* const path,
        const std::uint32_t flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> secpol_close(secpol_file_t* const handle) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> secpol_posix_spawnattr_settypeid(
        secpol_file_t* const handle,
        posix_spawnattr_t* const attrp,
        const char* const name,
        const std::uint32_t flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> secpol_transition_type(
        secpol_file_t* const handle,
        const char* const name,
        const std::uint32_t flags) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
};
}  // namespace details

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SECPOL_IMPL_H
