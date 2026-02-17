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
#ifndef SCORE_LIB_OS_QNX_DEVCTL_H
#define SCORE_LIB_OS_QNX_DEVCTL_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <devctl.h>
#include <cstdint>

namespace score
{
namespace os
{

///
/// @brief Device control functions OSAL class
/// [QNX devctl documentation](http://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/d/devctl.html)
///
class Devctl : public ObjectSeam<Devctl>
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    static Devctl& instance() noexcept;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> devctl(const std::int32_t fd,
                                              const std::int32_t dev_cmd,
                                              void* const dev_data_ptr,
                                              const std::size_t n_bytes,
                                              std::int32_t* const dev_info_ptr) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> devctlv(const std::int32_t fd,
                                               const std::int32_t dev_cmd,
                                               const std::int32_t sparts,
                                               const std::int32_t rparts,
                                               const iovec* const sv,
                                               const iovec* const rv,
                                               std::int32_t* const dev_info_ptr) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    Devctl() = default;
    virtual ~Devctl() = default;

  protected:
    Devctl(const Devctl&) = default;
    Devctl& operator=(const Devctl&) = default;
    Devctl(Devctl&&) = default;
    Devctl& operator=(Devctl&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_DEVCTL_H
