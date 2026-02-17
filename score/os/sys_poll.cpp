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
#include "score/os/sys_poll.h"
#include "score/os/sys_poll_impl.h"

namespace score
{
namespace os
{

score::os::SysPoll& score::os::SysPoll::instance() noexcept
{
    static SysPollImpl syspoll_instance;
    return select_instance(syspoll_instance);
}

/* KW_SUPPRESS_START:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST: */
/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<score::os::SysPoll> score::os::SysPoll::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
/* KW_SUPPRESS_END:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST */
{
    return score::cpp::pmr::make_unique<score::os::SysPollImpl>(memory_resource);
}

}  // namespace os
}  // namespace score
