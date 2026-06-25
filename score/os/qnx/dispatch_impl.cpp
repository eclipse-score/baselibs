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
#include "score/os/qnx/dispatch_impl.h"

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{
score::cpp::expected<name_attach_t*, score::os::Error> DispatchImpl::name_attach(dispatch_t* const dpp,
                                                                        const char* const path,
                                                                        const std::uint32_t flags) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    name_attach_t* const attach = ::name_attach(dpp, path, flags);
    if (attach == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return attach;
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::name_detach(name_attach_t* const attach,
                                                              const std::uint32_t flags) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::name_detach(attach, flags) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> DispatchImpl::name_open(const char* const name,
                                                                    const std::int32_t flags) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t result = ::name_open(name, flags);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::name_close(const std::int32_t fd) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::name_close(fd) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<dispatch_t*, score::os::Error> DispatchImpl::dispatch_create(void) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Error occurs when insufficient memory to allocate
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    dispatch_t* const dpp = ::dispatch_create();
    if (dpp == nullptr)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return dpp;
}

score::cpp::expected<dispatch_t*, score::os::Error> DispatchImpl::dispatch_create_channel(
    const std::int32_t chid,
    const std::uint32_t flags) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Negative chid doesn't make sense or insufficient memory to
    // allocate.
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    dispatch_t* const dpp = ::dispatch_create_channel(chid, flags);
    if (dpp == nullptr)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return dpp;
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::dispatch_destroy(dispatch_t* const dpp) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Crashes if pass empty dpp
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::dispatch_destroy(dpp) == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return {};
}

score::cpp::expected<dispatch_context_t*, score::os::Error> DispatchImpl::dispatch_context_alloc(
    dispatch_t* const dpp) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Crashes if pass NULL parameter
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTBEGIN(score-banned-function) see comment above
    // The rule states: "Unions shall not be used."
    // the dispatch_context_t is a typedef union
    // this is defined in a qnx internal code so we cannot change it
    // coverity[autosar_cpp14_a9_5_1_violation]
    dispatch_context_t* const ctp = ::dispatch_context_alloc(dpp);
    // NOLINTEND(score-banned-function) see comment above

    // The rule states: "Unions shall not be used."
    // the dispatch_context_t is a typedef union
    // this is defined in a qnx internal code so we cannot change it
    // coverity[autosar_cpp14_a9_5_1_violation]
    if (ctp == nullptr)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    // The rule states: "Unions shall not be used."
    // the dispatch_context_t is a typedef union
    // this is defined in a qnx internal code so we cannot change it
    // coverity[autosar_cpp14_a9_5_1_violation]
    return ctp;
}

// The rule states: "Unions shall not be used."
// the dispatch_context_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
// coverity[autosar_cpp14_a9_5_1_violation]
void DispatchImpl::dispatch_context_free(dispatch_context_t* const ctp) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    ::dispatch_context_free(ctp);
}

// The rule states: "Unions shall not be used."
// the dispatch_context_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
// coverity[autosar_cpp14_a9_5_1_violation]
score::cpp::expected_blank<score::os::Error> DispatchImpl::dispatch_block(dispatch_context_t* const ctp) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Crashes if pass NULL parameter
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTBEGIN(score-banned-function) see comment above
    // The rule states: "Unions shall not be used."
    // the dispatch_context_t is a typedef union
    // this is defined in a qnx internal code so we cannot change it
    // coverity[autosar_cpp14_a9_5_1_violation]
    dispatch_context_t* const ctp_result = ::dispatch_block(ctp);
    // NOLINTEND(score-banned-function) see comment above

    // The rule states: "Unions shall not be used."
    // the dispatch_context_t is a typedef union
    // this is defined in a qnx internal code so we cannot change it
    // coverity[autosar_cpp14_a9_5_1_violation]
    if (ctp_result == nullptr)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return {};
}

// The rule states: "Unions shall not be used."
// the dispatch_context_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
// coverity[autosar_cpp14_a9_5_1_violation]
void DispatchImpl::dispatch_unblock(dispatch_context_t* const ctp) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return ::dispatch_unblock(ctp);
}

// The rule states: "Unions shall not be used."
// the dispatch_context_t is a typedef union
// this is defined in a qnx internal code so we cannot change it
// coverity[autosar_cpp14_a9_5_1_violation]
score::cpp::expected_blank<std::int32_t> DispatchImpl::dispatch_handler(dispatch_context_t* const ctp) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t res = ::dispatch_handler(ctp);
    if (res == -1)
    {
        return score::cpp::make_unexpected(res);
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> DispatchImpl::resmgr_attach(
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
    RESMGR_HANDLE_T* const handle) const noexcept
/* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */

{
    const std::int32_t res = ::resmgr_attach(dpp, attr, path, file_type, flags, connect_funcs, io_funcs, handle);
    if (res == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return res;
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::resmgr_detach(dispatch_t* const dpp,
                                                                const std::int32_t id,
                                                                const std::uint32_t flags) const noexcept
{
    if (::resmgr_detach(dpp, id, flags) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<std::size_t, score::os::Error> DispatchImpl::resmgr_msgget(resmgr_context_t* const ctp,
                                                                       void* const msg,
                                                                       const std::size_t size,
                                                                       const std::size_t offset) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Empty buffer and zero size still return non-error
    const ssize_t result = ::resmgr_msgget(ctp, msg, size, offset);
    if (result == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return static_cast<std::size_t>(result);
}

score::cpp::expected<std::int32_t, score::os::Error> DispatchImpl::message_connect(dispatch_t* const dpp,
                                                                          const std::int32_t flags) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Crashes if pass NULL parameter
    const std::int32_t coid = ::message_connect(dpp, flags);
    if (coid == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return coid;
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::message_attach(
    dispatch_t* const dpp,
    message_attr_t* const attr,
    const std::int32_t low,
    const std::int32_t high,
    std::int32_t (*const func)(message_context_t* ctp, std::int32_t code, std::uint32_t flags, void* handle) noexcept,
    void* const handle) const noexcept
{
    // Branch coverage: Error can not be reproduced in scope of unit tests. Crashes if pass NULL dpp parameter
    // The rule states: "A cast shall not convert a pointer to a function to any other pointer type, including a pointer
    // to function type." This is about the *func* pointer to a function
    // the signature matches, pointer type is the same
    // coverity[autosar_cpp14_m5_2_6_violation]
    if (::message_attach(dpp, attr, low, high, func, handle) == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return {};
}

score::cpp::expected<thread_pool_t*, score::os::Error> DispatchImpl::thread_pool_create(thread_pool_attr_t* pool_attr,
                                                                               std::uint32_t flags) const noexcept
{
    // Error can not be reproduced in scope of unit tests. Crashes if pass NULL pool_attr parameter
    const auto tpp = ::thread_pool_create(pool_attr, flags);
    if (tpp == nullptr)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
    }
    return tpp;
}

score::cpp::expected<std::int32_t, score::os::Error> DispatchImpl::thread_pool_start(void* pool) const noexcept
{
    const auto result = ::thread_pool_start(pool);
    // Error can not be reproduced in scope of unit tests. Crashes if pass NULL pool parameter
    if (result == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }

    return result;
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::select_attach(
    dispatch_t* const dpp,
    select_attr_t* const attr,
    const std::int32_t fd,
    const std::uint32_t flags,
    std::int32_t (*const func)(select_context_t* ctp, std::int32_t fd, std::uint32_t flags, void* handle) noexcept,
    void* const handle) const noexcept
{
    // The rule states: "A cast shall not convert a pointer to a function to any other pointer type, including a pointer
    // to function type." This is about the *func* pointer to a function
    // the signature matches, pointer type is the same
    // coverity[autosar_cpp14_m5_2_6_violation]
    if (::select_attach(dpp, attr, fd, flags, func, handle) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::select_detach(dispatch_t* const dpp,
                                                                const std::int32_t fd) const noexcept
{
    if (::select_detach(dpp, fd) == -1)
    {
        // it is not mentioned in the docs that select_detach() sets errno on failure
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> DispatchImpl::pulse_attach(
    dispatch_t* const dpp,
    const std::int32_t flags,
    const std::int32_t code,
    std::int32_t (*const func)(message_context_t* ctp, std::int32_t code, std::uint32_t flags, void* handle) noexcept,
    void* const handle) const noexcept
{
    // The rule states: "A cast shall not convert a pointer to a function to any other pointer type, including a pointer
    // to function type." This is about the *func* pointer to a function
    // the signature matches, pointer type is the same
    // coverity[autosar_cpp14_m5_2_6_violation]
    const std::int32_t new_code = ::pulse_attach(dpp, flags, code, func, handle);
    if (new_code == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return new_code;
}

score::cpp::expected_blank<score::os::Error> DispatchImpl::pulse_detach(dispatch_t* const dpp,
                                                               const std::int32_t code,
                                                               const std::int32_t flags) const noexcept
{
    if (::pulse_detach(dpp, code, flags) == -1)
    {
        // it is not mentioned in the docs that pulse_detach() sets errno on failure
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> DispatchImpl::msg_deliver_event(rcvid_t rcvid,
                                                                            const sigevent* event) const noexcept
{
    const std::int32_t res = ::MsgDeliverEvent(rcvid, event);
    if (res == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return res;
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
