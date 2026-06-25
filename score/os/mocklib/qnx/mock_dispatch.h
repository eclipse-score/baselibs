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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_DISPATCH_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_DISPATCH_H

#include "score/os/qnx/dispatch.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shadowing function name is intended. */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */

class MockDispatch : public Dispatch
{
  public:
    MOCK_METHOD((score::cpp::expected<name_attach_t*, score::os::Error>),
                name_attach,
                (dispatch_t * dpp, const char* path, std::uint32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                name_detach,
                (name_attach_t * attach, std::uint32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                name_open,
                (const char* name, std::int32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>), name_close, (std::int32_t fd), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<dispatch_t*, score::os::Error>), dispatch_create, (), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<dispatch_t*, score::os::Error>),
                dispatch_create_channel,
                (std::int32_t chid, std::uint32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                dispatch_destroy,
                (dispatch_t * dpp),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<dispatch_context_t*, score::os::Error>),
                dispatch_context_alloc,
                (dispatch_t * dpp),
                (const, noexcept, override));

    MOCK_METHOD((void), dispatch_context_free, (dispatch_context_t * ctp), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                dispatch_block,
                (dispatch_context_t * ctp),
                (const, noexcept, override));

    MOCK_METHOD((void), dispatch_unblock, (dispatch_context_t * ctp), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                dispatch_handler,
                (dispatch_context_t * ctp),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                resmgr_attach,
                (dispatch_t * dpp,
                 resmgr_attr_t* attr,
                 const char* path,
                 const enum _file_type file_type,
                 std::uint32_t flags,
                 const resmgr_connect_funcs_t* connect_funcs,
                 const resmgr_io_funcs_t* io_funcs,
                 RESMGR_HANDLE_T* handle),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                resmgr_detach,
                (dispatch_t * dpp, std::int32_t id, std::uint32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::size_t, score::os::Error>),
                resmgr_msgget,
                (resmgr_context_t * ctp, void* msg, std::size_t size, std::size_t offset),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                message_connect,
                (dispatch_t * dpp, std::int32_t flags),
                (const, noexcept, override));

    MOCK_METHOD(
        (score::cpp::expected_blank<score::os::Error>),
        message_attach,
        (dispatch_t * dpp,
         message_attr_t* attr,
         std::int32_t low,
         std::int32_t high,
         std::int32_t (*func)(message_context_t* ctp, std::int32_t code, std::uint32_t flags, void* handle) noexcept,
         void* handle),
        (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<thread_pool_t*, score::os::Error>),
                thread_pool_create,
                (thread_pool_attr_t * pool_attr, std::uint32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                thread_pool_start,
                (void* pool),
                (const, noexcept, override));

    MOCK_METHOD(
        (score::cpp::expected_blank<score::os::Error>),
        select_attach,
        (dispatch_t * dpp,
         select_attr_t* attr,
         std::int32_t fd,
         std::uint32_t flags,
         std::int32_t (*func)(select_context_t* ctp, std::int32_t fd, std::uint32_t flags, void* handle) noexcept,
         void* handle),
        (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                select_detach,
                (dispatch_t * dpp, std::int32_t fd),
                (const, noexcept, override));

    MOCK_METHOD(
        (score::cpp::expected<std::int32_t, score::os::Error>),
        pulse_attach,
        (dispatch_t * dpp,
         std::int32_t flags,
         std::int32_t code,
         std::int32_t (*func)(message_context_t* ctp, std::int32_t code, std::uint32_t flags, void* handle) noexcept,
         void* handle),
        (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                pulse_detach,
                (dispatch_t * dpp, std::int32_t code, std::int32_t flags),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                msg_deliver_event,
                (rcvid_t rcvid, const sigevent* event),
                (const, noexcept, override));
};

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_DISPATCH_H
