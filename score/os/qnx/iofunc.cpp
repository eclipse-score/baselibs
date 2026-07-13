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
#include "score/os/qnx/iofunc.h"

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

void score::os::IoFuncQnx::iofunc_func_init(const std::uint32_t nconnect,
                                          resmgr_connect_funcs_t* const connect,
                                          const std::uint32_t nio,
                                          resmgr_io_funcs_t* const io) const noexcept
{
    ::iofunc_func_init(nconnect, connect, nio, io);
}

void score::os::IoFuncQnx::iofunc_attr_init(iofunc_attr_t* const attr,
                                          const mode_t mode,
                                          iofunc_attr_t* const dattr,
                                          _client_info* const info) const noexcept
{
    ::iofunc_attr_init_sized(attr, mode, dattr, info, sizeof(iofunc_attr_t));
}

score::cpp::expected_blank<score::os::Error> score::os::IoFuncQnx::iofunc_mount_init(iofunc_mount_t* const mountp,
                                                                          const size_t size) const noexcept
{
    if (EOK != ::iofunc_mount_init(mountp, size))
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<int32_t, int32_t> score::os::IoFuncQnx::iofunc_close_ocb_default(resmgr_context_t* const ctp,
                                                                             void* const reserved,
                                                                             iofunc_ocb_t* const ocb) const noexcept
{
    return ::iofunc_close_ocb_default(ctp, reserved, ocb);
}

// The rule states: "Unions shall not be used."
// the io_devctl_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
score::cpp::expected<std::int32_t, int32_t> score::os::IoFuncQnx::iofunc_devctl_default(
    resmgr_context_t* const ctp,
    // coverity[autosar_cpp14_a9_5_1_violation]
    io_devctl_t* const msg,
    iofunc_ocb_t* const ocb) const noexcept
{
    const std::int32_t result = ::iofunc_devctl_default(ctp, msg, ocb);
    // Suppress "AUTOSAR C++14 A5-2-2": "Traditional C-style casts shall not be used."
    // the C-style cast is used in _RESMGR_DEFAULT, this is defined in a qnx internal code so we cannot change it
    // Suppress "AUTOSAR C++14 A5-2-6": "The operands of a logical && or || shall be parenthesized
    // if the operands contain binary operators."
    // the operands are used in _RESMGR_DEFAULT, this is defined in a qnx internal code so we cannot change it
    // Suppress "AUTOSAR C++14 M5-0-10": "If the bitwise operators ~ and << are applied to an operand with an
    // underlying type of unsigned char or unsigned short, the result shall be immediately cast to the underlying type
    // of the operand."
    // the bitwise operators are used in _RESMGR_DEFAULT, this is defined in a qnx internal code so we cannot change it
    // coverity[autosar_cpp14_a5_2_2_violation]
    // coverity[autosar_cpp14_a5_2_6_violation]
    // coverity[autosar_cpp14_m5_0_10_violation]
    if ((result != EOK) && (result != _RESMGR_NPARTS(1)) && (result != _RESMGR_DEFAULT))
    {
        return score::cpp::make_unexpected(result);
    }
    return result;
}

// The rule states: "Unions shall not be used."
// the io_write_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_write_verify(resmgr_context_t* const ctp,
                                                                          // coverity[autosar_cpp14_a9_5_1_violation]
                                                                          io_write_t* const msg,
                                                                          iofunc_ocb_t* const ocb,
                                                                          std::int32_t* const nonblock) const noexcept
{
    const std::int32_t result = ::iofunc_write_verify(ctp, msg, ocb, nonblock);
    if (result != EOK) /* KW_SUPPRESS:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    {
        return score::cpp::make_unexpected(result);
    }
    return {};
}

// The rule states: "Unions shall not be used."
// the io_read_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_read_verify(resmgr_context_t* const ctp,
                                                                         // coverity[autosar_cpp14_a9_5_1_violation]
                                                                         io_read_t* const msg,
                                                                         iofunc_ocb_t* const ocb,
                                                                         std::int32_t* const nonblock) const noexcept
{
    const std::int32_t result = ::iofunc_read_verify(ctp, msg, ocb, nonblock);
    if (result != EOK) /* KW_SUPPRESS:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    {
        return score::cpp::make_unexpected(result);
    }
    return {};
}

// The rule states: "Unions shall not be used."
// the io_lseek_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
score::cpp::expected<std::int32_t, std::int32_t> score::os::IoFuncQnx::iofunc_lseek_default(
    resmgr_context_t* const ctp,
    // coverity[autosar_cpp14_a9_5_1_violation]
    io_lseek_t* const msg,
    iofunc_ocb_t* const ocb) const noexcept
{
    const std::int32_t result = ::iofunc_lseek_default(ctp, msg, ocb);
    // -1 result considered as success
    // check
    // http://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.lib_ref/topic/i/iofunc_lseek_default.html
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    if ((result != EOK) && (result != -1))
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    {
        return score::cpp::make_unexpected(result);
    }
    return result;
}

score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_client_info_ext(resmgr_context_t* ctp,
                                                                             const std::int32_t ioflag,
                                                                             struct _client_info** info) const noexcept
{
    const std::int32_t result = ::iofunc_client_info_ext(ctp, ioflag, info, IOFUNC_CLIENTINFO_GETGROUPS);

    // LCOV_EXCL_BR_START (Creation of valid resmgr_context_t struct is not possible through unit tests)
    if (result != EOK) /* KW_SUPPRESS:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    {
        return score::cpp::make_unexpected(result);
    }
    // LCOV_EXCL_BR_STOP (Creation of valid resmgr_context_t struct is not possible through unit tests)
    return {};  // LCOV_EXCL_LINE (Creation of valid resmgr_context_t struct is not possible through unit tests)
}

score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_client_info_ext_free(
    struct _client_info** info) const noexcept
{
    // LCOV_EXCL_BR_START (Creation of invalid _client_info struct is not possible through unit tests)
    const std::int32_t result = ::iofunc_client_info_ext_free(info);
    if (result != EOK) /* KW_SUPPRESS:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    {
        // LCOV_EXCL_START (Creation of invalid _client_info struct is not possible through unit tests)
        return score::cpp::make_unexpected(result);
        // LCOV_EXCL_STOP
    }
    // LCOV_EXCL_BR_STOP (Creation of invalid _client_info struct is not possible through unit tests)
    return {};
}

score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_check_access(
    resmgr_context_t* ctp,
    const iofunc_attr_t* attr,
    mode_t checkmode,
    const struct _client_info* info) const noexcept
{
    const std::int32_t result = ::iofunc_check_access(ctp, attr, checkmode, info);
    if (result != EOK) /* KW_SUPPRESS:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    {
        return score::cpp::make_unexpected(result);
    }
    return {};
}

score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_attr_lock(iofunc_attr_t* const attr) const noexcept
{
    const std::int32_t result = ::iofunc_attr_lock(attr);
    if (result != EOK)
    {
        return score::cpp::make_unexpected(result);
    }
    return {};
}

score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_attr_unlock(iofunc_attr_t* const attr) const noexcept
{
    const std::int32_t result = ::iofunc_attr_unlock(attr);
    if (result != EOK)
    {
        return score::cpp::make_unexpected(result);
    }
    return {};
}

// The rule states: "Unions shall not be used."
// the io_open_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_open(resmgr_context_t* const ctp,
                                                                  // coverity[autosar_cpp14_a9_5_1_violation]
                                                                  io_open_t* const msg,
                                                                  iofunc_attr_t* const attr,
                                                                  iofunc_attr_t* const dattr,
                                                                  struct _client_info* const info) const noexcept
{
    const std::int32_t result = ::iofunc_open(ctp, msg, attr, dattr, info);
    if (result != EOK)
    {
        return score::cpp::make_unexpected(result);
    }
    return {};
}

// The rule states: "Unions shall not be used."
// the io_open_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
score::cpp::expected_blank<std::int32_t> score::os::IoFuncQnx::iofunc_ocb_attach(
    resmgr_context_t* const ctp,
    // coverity[autosar_cpp14_a9_5_1_violation]
    io_open_t* const msg,
    iofunc_ocb_t* const ocb,
    iofunc_attr_t* const attr,
    const resmgr_io_funcs_t* const io_funcs) const noexcept
{
    const std::int32_t result = ::iofunc_ocb_attach(ctp, msg, ocb, attr, io_funcs);
    if (result != EOK)
    {
        return score::cpp::make_unexpected(result);
    }
    return {};
}

std::int32_t score::os::IoFuncQnx::iofunc_ocb_detach(resmgr_context_t* const ctp, iofunc_ocb_t* const ocb) const noexcept
{
    return ::iofunc_ocb_detach(ctp, ocb);
}

// The rule states: "Unions shall not be used."
// the io_notify_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
std::int32_t score::os::IoFuncQnx::iofunc_notify(resmgr_context_t* const ctp,
                                               // coverity[autosar_cpp14_a9_5_1_violation]
                                               io_notify_t* const msg,
                                               iofunc_notify_t* const nop,
                                               const std::int32_t trig,
                                               const std::int32_t* const notifycounts,
                                               std::int32_t* const armed) const noexcept
{
    return ::iofunc_notify(ctp, msg, nop, trig, notifycounts, armed);
}

void score::os::IoFuncQnx::iofunc_notify_trigger(iofunc_notify_t* const nop,
                                               const std::int32_t count,
                                               const std::int32_t index) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    ::iofunc_notify_trigger(nop, count, index);
}

void score::os::IoFuncQnx::iofunc_notify_trigger_strict(resmgr_context_t* const ctp,
                                                      iofunc_notify_t* const nop,
                                                      const std::int32_t count,
                                                      const std::int32_t index) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    ::iofunc_notify_trigger_strict(ctp, nop, count, index);
}

void score::os::IoFuncQnx::iofunc_notify_remove(resmgr_context_t* const ctp, iofunc_notify_t* const nop) const noexcept
{
    ::iofunc_notify_remove(ctp, nop);
}

std::int32_t score::os::IoFuncQnx::iofunc_close_dup_default(resmgr_context_t* ctp,
                                                          io_close_t* msg,
                                                          iofunc_ocb_t* ocb) const noexcept
{
    return ::iofunc_close_dup_default(ctp, msg, ocb);
}

std::int32_t score::os::IoFuncQnx::iofunc_lock_ocb_default(resmgr_context_t* ctp,
                                                         void* reserved,
                                                         iofunc_ocb_t* ocb) const noexcept
{
    return ::iofunc_lock_ocb_default(ctp, reserved, ocb);
}

std::int32_t score::os::IoFuncQnx::iofunc_unlock_ocb_default(resmgr_context_t* ctp,
                                                           void* reserved,
                                                           iofunc_ocb_t* ocb) const noexcept
{
    return ::iofunc_unlock_ocb_default(ctp, reserved, ocb);
}

score::os::IoFunc& score::os::IoFunc::instance() noexcept
{
    // The rule states: Static and thread-local objects shall be constant-initialized.
    // It's a singleton by design hence cannot be made const
    // coverity[autosar_cpp14_a3_3_2_violation]
    static score::os::IoFuncQnx instance; /* LCOV_EXCL_BR_LINE */
    /* All branches are generated by certified compiler, no additional check necessary. */
    return select_instance(instance);
}

/* KW_SUPPRESS_START:MISRA.PPARAM.NEEDS.CONST,MISRA.VAR.NEEDS.CONST: */
/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<score::os::IoFunc> score::os::IoFunc::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
/* KW_SUPPRESS_END:MISRA.PPARAM.NEEDS.CONST,MISRA.VAR.NEEDS.CONST */
{
    return score::cpp::pmr::make_unique<score::os::IoFuncQnx>(memory_resource);
}

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
