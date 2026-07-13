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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_IOFUNC_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_IOFUNC_H

#include "score/os/qnx/iofunc.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

class MockIoFunc : public IoFunc
{
  public:
    MOCK_METHOD((void),
                iofunc_attr_init,
                (iofunc_attr_t * attr, const mode_t mode, iofunc_attr_t* dattr, struct _client_info* info),
                (const, noexcept, override));

    MOCK_METHOD(
        (void),
        iofunc_func_init,
        (const std::uint32_t nconnect, resmgr_connect_funcs_t* connect, const std::uint32_t nio, resmgr_io_funcs_t* io),
        (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<Error>),
                iofunc_mount_init,
                (iofunc_mount_t* const mountp, const size_t size),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, std::int32_t>),
                iofunc_close_ocb_default,
                (resmgr_context_t * ctp, void* reserved, iofunc_ocb_t* ocb),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, std::int32_t>),
                iofunc_devctl_default,
                (resmgr_context_t * ctp, io_devctl_t* msg, iofunc_ocb_t* ocb),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_write_verify,
                (resmgr_context_t * ctp, io_write_t* msg, iofunc_ocb_t* ocb, std::int32_t* nonblock),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_read_verify,
                (resmgr_context_t * ctp, io_read_t* msg, iofunc_ocb_t* ocb, std::int32_t* nonblock),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, std::int32_t>),
                iofunc_lseek_default,
                (resmgr_context_t * ctp, io_lseek_t* msg, iofunc_ocb_t* ocb),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_client_info_ext,
                (resmgr_context_t * ctp, std::int32_t ioflag, struct _client_info** info),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_client_info_ext_free,
                (struct _client_info * *info),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_check_access,
                (resmgr_context_t * ctp, const iofunc_attr_t* attr, mode_t checkmode, const struct _client_info* info),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_attr_lock,
                (iofunc_attr_t * attr),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_attr_unlock,
                (iofunc_attr_t * attr),
                (const, noexcept, override));

    MOCK_METHOD(
        (score::cpp::expected_blank<std::int32_t>),
        iofunc_open,
        (resmgr_context_t * ctp, io_open_t* msg, iofunc_attr_t* attr, iofunc_attr_t* dattr, struct _client_info* info),
        (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<std::int32_t>),
                iofunc_ocb_attach,
                (resmgr_context_t * ctp,
                 io_open_t* msg,
                 iofunc_ocb_t* ocb,
                 iofunc_attr_t* attr,
                 const resmgr_io_funcs_t* io_funcs),
                (const, noexcept, override));

    MOCK_METHOD((std::int32_t),
                iofunc_ocb_detach,
                (resmgr_context_t * ctp, iofunc_ocb_t* ocb),
                (const, noexcept, override));

    MOCK_METHOD((std::int32_t),
                iofunc_notify,
                (resmgr_context_t * ctp,
                 io_notify_t* msg,
                 iofunc_notify_t* nop,
                 std::int32_t trig,
                 const std::int32_t* notifycounts,
                 std::int32_t* armed),
                (const, noexcept, override));

    MOCK_METHOD((void),
                iofunc_notify_trigger,
                (iofunc_notify_t * nop, std::int32_t count, std::int32_t index),
                (const, noexcept, override));

    MOCK_METHOD((void),
                iofunc_notify_trigger_strict,
                (resmgr_context_t * ctp, iofunc_notify_t* nop, std::int32_t count, std::int32_t index),
                (const, noexcept, override));

    MOCK_METHOD((void),
                iofunc_notify_remove,
                (resmgr_context_t * ctp, iofunc_notify_t* nop),
                (const, noexcept, override));

    MOCK_METHOD((std::int32_t),
                iofunc_close_dup_default,
                (resmgr_context_t * ctp, io_close_t* msg, iofunc_ocb_t* ocb),
                (const, noexcept, override));

    MOCK_METHOD((std::int32_t),
                iofunc_lock_ocb_default,
                (resmgr_context_t * ctp, void* reserved, iofunc_ocb_t* ocb),
                (const, noexcept, override));

    MOCK_METHOD((std::int32_t),
                iofunc_unlock_ocb_default,
                (resmgr_context_t * ctp, void* reserved, iofunc_ocb_t* ocb),
                (const, noexcept, override));
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_IOFUNC_H
