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
#include "score/os/socket.h"
#include "score/os/socket_impl.h"

score::os::Socket& score::os::Socket::instance() noexcept
{
    static score::os::SocketImpl instance;
    return select_instance(instance);
}

/* KW_SUPPRESS_START:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST: */
/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<score::os::Socket> score::os::Socket::Default(
    score::cpp::pmr::memory_resource* memory_resource) noexcept
/* KW_SUPPRESS_END:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST */
{
    return score::cpp::pmr::make_unique<score::os::SocketImpl>(memory_resource);
}
