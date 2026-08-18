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
#ifndef SCORE_LIB_OS_QNX_SECPOL_H
#define SCORE_LIB_OS_QNX_SECPOL_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <secpol/secpol.h>

namespace score
{
namespace os
{
namespace qnx
{

class Secpol : public ObjectSeam<Secpol>
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    static Secpol& instance() noexcept;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<secpol_file_t*, score::os::Error> secpol_open(
        const char* const path,
        const std::uint32_t flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<score::os::Error> secpol_close(secpol_file_t* const handle) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> secpol_posix_spawnattr_settypeid(
        secpol_file_t* const handle,
        posix_spawnattr_t* const attrp,
        const char* const name,
        const std::uint32_t flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> secpol_transition_type(
        secpol_file_t* const handle,
        const char* const name,
        const std::uint32_t flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~Secpol() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Secpol(const Secpol&) = delete;
    Secpol& operator=(const Secpol&) = delete;
    Secpol(Secpol&& other) = delete;
    Secpol& operator=(Secpol&& other) = delete;

  protected:
    Secpol() = default;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SECPOL_H
