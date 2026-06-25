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
#ifndef SCORE_LIB_OS_QNX_DISPATCH_IMPL_H
#define SCORE_LIB_OS_QNX_DISPATCH_IMPL_H

#include "score/os/qnx/dispatch.h"

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

namespace score
{
namespace os
{

class DispatchImpl final : public Dispatch
{
  public:
    score::cpp::expected<name_attach_t*, score::os::Error> name_attach(dispatch_t* const dpp,
                                                              const char* const path,
                                                              const std::uint32_t flags) const noexcept override;

    score::cpp::expected_blank<score::os::Error> name_detach(name_attach_t* const attach,
                                                    const std::uint32_t flags) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> name_open(const char* const name,
                                                          const std::int32_t flags) const noexcept override;

    score::cpp::expected_blank<score::os::Error> name_close(const std::int32_t fd) const noexcept override;

    score::cpp::expected<dispatch_t*, score::os::Error> dispatch_create(void) const noexcept override;

    score::cpp::expected<dispatch_t*, score::os::Error> dispatch_create_channel(
        const std::int32_t chid,
        const std::uint32_t flags) const noexcept override;

    score::cpp::expected_blank<score::os::Error> dispatch_destroy(dispatch_t* const dpp) const noexcept override;

    score::cpp::expected<dispatch_context_t*, score::os::Error> dispatch_context_alloc(
        dispatch_t* const dpp) const noexcept override;

    void dispatch_context_free(dispatch_context_t* const ctp) const noexcept override;

    score::cpp::expected_blank<score::os::Error> dispatch_block(dispatch_context_t* const ctp) const noexcept override;

    void dispatch_unblock(dispatch_context_t* const ctp) const noexcept override;

    score::cpp::expected_blank<std::int32_t> dispatch_handler(dispatch_context_t* const ctp) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> resmgr_attach(
        dispatch_t* const dpp,
        resmgr_attr_t* const attr,
        const char* const path,
        /* KW_SUPPRESS_START:AUTOSAR.ENUM.EXPLICIT_BASE_TYPE:*/
        /* Enum _file_type is defined outside our code domain */
        const enum _file_type file_type,
        /* KW_SUPPRESS_END:AUTOSAR.ENUM.EXPLICIT_BASE_TYPE:*/
        /* Enum _file_type is defined outside our code domain */
        const std::uint32_t flags,
        const resmgr_connect_funcs_t* const connect_funcs,
        const resmgr_io_funcs_t* const io_funcs,
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        RESMGR_HANDLE_T* const handle) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */

    score::cpp::expected_blank<score::os::Error> resmgr_detach(dispatch_t* const dpp,
                                                      const std::int32_t id,
                                                      const std::uint32_t flags) const noexcept override;

    score::cpp::expected<std::size_t, score::os::Error> resmgr_msgget(resmgr_context_t* const ctp,
                                                             void* const msg,
                                                             const std::size_t size,
                                                             const std::size_t offset) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> message_connect(dispatch_t* const dpp,
                                                                const std::int32_t flags) const noexcept override;

    score::cpp::expected_blank<score::os::Error> message_attach(dispatch_t* const dpp,
                                                       message_attr_t* const attr,
                                                       const std::int32_t low,
                                                       const std::int32_t high,
                                                       std::int32_t (*const func)(message_context_t* ctp,
                                                                                  std::int32_t code,
                                                                                  std::uint32_t flags,
                                                                                  void* handle) noexcept,
                                                       void* const handle) const noexcept override;

    score::cpp::expected<thread_pool_t*, score::os::Error> thread_pool_create(thread_pool_attr_t* pool_attr,
                                                                     std::uint32_t flags) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> thread_pool_start(void* pool) const noexcept override;

    score::cpp::expected_blank<score::os::Error> select_attach(
        dispatch_t* const dpp,
        select_attr_t* const attr,
        const std::int32_t fd,
        const std::uint32_t flags,
        std::int32_t (*const func)(select_context_t* ctp, std::int32_t fd, std::uint32_t flags, void* handle) noexcept,
        void* const handle) const noexcept override;

    score::cpp::expected_blank<score::os::Error> select_detach(dispatch_t* const dpp,
                                                      const std::int32_t fd) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> pulse_attach(dispatch_t* const dpp,
                                                             const std::int32_t flags,
                                                             const std::int32_t code,
                                                             std::int32_t (*const func)(message_context_t* ctp,
                                                                                        std::int32_t code,
                                                                                        std::uint32_t flags,
                                                                                        void* handle) noexcept,
                                                             void* const handle) const noexcept override;

    score::cpp::expected_blank<score::os::Error> pulse_detach(dispatch_t* const dpp,
                                                     const std::int32_t code,
                                                     const std::int32_t flags) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> msg_deliver_event(rcvid_t recvid,
                                                                  const sigevent* client_event) const noexcept override;
};

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

#endif  // SCORE_LIB_OS_QNX_DISPATCH_IMPL_H
