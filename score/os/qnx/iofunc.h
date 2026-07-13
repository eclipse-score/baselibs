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
#ifndef SCORE_LIB_OS_QNX_IOFUNC_H
#define SCORE_LIB_OS_QNX_IOFUNC_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/qnx/types/types.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

// The rule states: "The pre-processor shall only be used for unconditional and conditional file inclusion
// and include guards, and using the following directives: (1) #ifndef, (2) #ifdef, (3) #if, (4) #if defined,
// (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include."
// We are removing dangerous and unneeded macro
// coverity[autosar_cpp14_a16_0_1_violation]
#undef iofunc_attr_init

namespace score
{
namespace os
{

class IoFunc : public ObjectSeam<IoFunc>
{
  public:
    static IoFunc& instance() noexcept;

    static score::cpp::pmr::unique_ptr<IoFunc> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    virtual void iofunc_attr_init(iofunc_attr_t* const attr,
                                  const mode_t mode,
                                  iofunc_attr_t* const dattr,
                                  struct _client_info* const info) const noexcept = 0;

    virtual void iofunc_func_init(const std::uint32_t nconnect,
                                  resmgr_connect_funcs_t* const connect,
                                  const std::uint32_t nio,
                                  resmgr_io_funcs_t* const io) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> iofunc_mount_init(iofunc_mount_t* const mountp,
                                                         const size_t size) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, std::int32_t> iofunc_close_ocb_default(
        resmgr_context_t* const ctp,
        void* const reserved,
        iofunc_ocb_t* const ocb) const noexcept = 0;

    // non-error return values are EOK (no data to reply), -1 (data to reply), _RESMGR_DEFAULT (to be handled by client)
    virtual score::cpp::expected<std::int32_t, std::int32_t> iofunc_devctl_default(resmgr_context_t* const ctp,
                                                                            io_devctl_t* const msg,
                                                                            iofunc_ocb_t* const ocb) const noexcept = 0;

    // the use case for the error is to feed it back to the framework, so, std::int32_t
    virtual score::cpp::expected_blank<std::int32_t> iofunc_write_verify(resmgr_context_t* const ctp,
                                                                  io_write_t* const msg,
                                                                  iofunc_ocb_t* const ocb,
                                                                  std::int32_t* const nonblock) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_read_verify(resmgr_context_t* const ctp,
                                                                 io_read_t* const msg,
                                                                 iofunc_ocb_t* const ocb,
                                                                 std::int32_t* const nonblock) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, std::int32_t> iofunc_lseek_default(resmgr_context_t* const ctp,
                                                                           io_lseek_t* const msg,
                                                                           iofunc_ocb_t* const ocb) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_client_info_ext(resmgr_context_t* ctp,
                                                                     const std::int32_t ioflag,
                                                                     struct _client_info** info) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_client_info_ext_free(
        struct _client_info** info) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_check_access(resmgr_context_t* ctp,
                                                                  const iofunc_attr_t* attr,
                                                                  mode_t checkmode,
                                                                  const struct _client_info* info) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_attr_lock(iofunc_attr_t* const attr) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_attr_unlock(iofunc_attr_t* const attr) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_open(resmgr_context_t* const ctp,
                                                          io_open_t* const msg,
                                                          iofunc_attr_t* const attr,
                                                          iofunc_attr_t* const dattr,
                                                          struct _client_info* const info) const noexcept = 0;

    virtual score::cpp::expected_blank<std::int32_t> iofunc_ocb_attach(
        resmgr_context_t* const ctp,
        io_open_t* const msg,
        iofunc_ocb_t* const ocb,
        iofunc_attr_t* const attr,
        const resmgr_io_funcs_t* const io_funcs) const noexcept = 0;

    // return value is a bitset
    virtual std::int32_t iofunc_ocb_detach(resmgr_context_t* const ctp, iofunc_ocb_t* const ocb) const noexcept = 0;

    // return value is to be directly returned by _IO_NOTIFY handler
    virtual std::int32_t iofunc_notify(resmgr_context_t* const ctp,
                                       io_notify_t* const msg,
                                       iofunc_notify_t* const nop,
                                       const std::int32_t trig,
                                       const std::int32_t* const notifycounts,
                                       std::int32_t* const armed) const noexcept = 0;

    virtual void iofunc_notify_trigger(iofunc_notify_t* const nop,
                                       const std::int32_t count,
                                       const std::int32_t index) const noexcept = 0;

    virtual void iofunc_notify_trigger_strict(resmgr_context_t* const ctp,
                                              iofunc_notify_t* const nop,
                                              const std::int32_t count,
                                              const std::int32_t index) const noexcept = 0;

    virtual void iofunc_notify_remove(resmgr_context_t* const ctp, iofunc_notify_t* const nop) const noexcept = 0;


    virtual std::int32_t iofunc_close_dup_default(resmgr_context_t* ctp,
                                                  io_close_t* msg,
                                                  iofunc_ocb_t* ocb) const noexcept = 0;
    
    virtual std::int32_t iofunc_lock_ocb_default(resmgr_context_t* ctp,
                                                 void* reserved,
                                                 iofunc_ocb_t* ocb) const noexcept = 0;
                                                 
    virtual std::int32_t iofunc_unlock_ocb_default(resmgr_context_t* ctp,
                                                   void* reserved,
                                                   iofunc_ocb_t* ocb) const noexcept = 0;

    IoFunc() = default;
    virtual ~IoFunc() = default;

  protected:
    IoFunc(const IoFunc&) = default;
    IoFunc& operator=(const IoFunc&) = default;
    IoFunc(IoFunc&&) = default;
    IoFunc& operator=(IoFunc&&) = default;
};

class IoFuncQnx final : public IoFunc
{

  public:
    void iofunc_func_init(const std::uint32_t nconnect,
                          resmgr_connect_funcs_t* const connect,
                          const std::uint32_t nio,
                          resmgr_io_funcs_t* const io) const noexcept override;

    void iofunc_attr_init(iofunc_attr_t* const attr,
                          const mode_t mode,
                          iofunc_attr_t* const dattr,
                          _client_info* const info) const noexcept override;

    score::cpp::expected_blank<Error> iofunc_mount_init(iofunc_mount_t* const mountp,
                                                 const size_t size) const noexcept override;

    score::cpp::expected<std::int32_t, std::int32_t> iofunc_close_ocb_default(resmgr_context_t* const ctp,
                                                                       void* const reserved,
                                                                       iofunc_ocb_t* const ocb) const noexcept override;

    score::cpp::expected<std::int32_t, std::int32_t> iofunc_devctl_default(resmgr_context_t* const ctp,
                                                                    io_devctl_t* const msg,
                                                                    iofunc_ocb_t* const ocb) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_write_verify(resmgr_context_t* const ctp,
                                                          io_write_t* const msg,
                                                          iofunc_ocb_t* const ocb,
                                                          std::int32_t* const nonblock) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_read_verify(resmgr_context_t* const ctp,
                                                         io_read_t* const msg,
                                                         iofunc_ocb_t* const ocb,
                                                         std::int32_t* const nonblock) const noexcept override;

    score::cpp::expected<std::int32_t, std::int32_t> iofunc_lseek_default(resmgr_context_t* const ctp,
                                                                   io_lseek_t* const msg,
                                                                   iofunc_ocb_t* const ocb) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_client_info_ext(resmgr_context_t* ctp,
                                                             const std::int32_t ioflag,
                                                             struct _client_info** info) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_client_info_ext_free(struct _client_info** info) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_check_access(resmgr_context_t* ctp,
                                                          const iofunc_attr_t* attr,
                                                          mode_t checkmode,
                                                          const struct _client_info* info) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_attr_lock(iofunc_attr_t* const attr) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_attr_unlock(iofunc_attr_t* const attr) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_open(resmgr_context_t* const ctp,
                                                  io_open_t* const msg,
                                                  iofunc_attr_t* const attr,
                                                  iofunc_attr_t* const dattr,
                                                  struct _client_info* const info) const noexcept override;

    score::cpp::expected_blank<std::int32_t> iofunc_ocb_attach(
        resmgr_context_t* const ctp,
        io_open_t* const msg,
        iofunc_ocb_t* const ocb,
        iofunc_attr_t* const attr,
        const resmgr_io_funcs_t* const io_funcs) const noexcept override;

    std::int32_t iofunc_ocb_detach(resmgr_context_t* const ctp, iofunc_ocb_t* const ocb) const noexcept override;

    std::int32_t iofunc_notify(resmgr_context_t* const ctp,
                               io_notify_t* const msg,
                               iofunc_notify_t* const nop,
                               const std::int32_t trig,
                               const std::int32_t* const notifycounts,
                               std::int32_t* const armed) const noexcept override;

    void iofunc_notify_trigger(iofunc_notify_t* const nop,
                               const std::int32_t count,
                               const std::int32_t index) const noexcept override;

    void iofunc_notify_trigger_strict(resmgr_context_t* const ctp,
                                      iofunc_notify_t* const nop,
                                      const std::int32_t count,
                                      const std::int32_t index) const noexcept override;

    void iofunc_notify_remove(resmgr_context_t* const ctp, iofunc_notify_t* const nop) const noexcept override;

    std::int32_t iofunc_close_dup_default(resmgr_context_t* ctp,
                                                  io_close_t* msg,
                                                  iofunc_ocb_t* ocb) const noexcept override;

    std::int32_t iofunc_lock_ocb_default(resmgr_context_t* ctp,
                                        void* reserved,
                                        iofunc_ocb_t* ocb) const noexcept override;
                                                 
    std::int32_t iofunc_unlock_ocb_default(resmgr_context_t* ctp,
                                            void* reserved,
                                            iofunc_ocb_t* ocb) const noexcept override;
};

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

#endif  // SCORE_LIB_OS_QNX_IOFUNC_H
