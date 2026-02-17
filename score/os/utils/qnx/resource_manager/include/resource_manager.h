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

#ifndef SCORE_LIB_OS_QNX_RESOURCE_MANAGER_H
#define SCORE_LIB_OS_QNX_RESOURCE_MANAGER_H

#include "score/os/utils/qnx/resource_manager/include/resource.h"
#include "score/mw/log/logger.h"

#include "score/stop_token.hpp"
#include "score/os/qnx/dispatch_impl.h"
#include "score/os/qnx/iofunc.h"

#include <memory>
#include <string>
#include <vector>

namespace score
{
namespace os
{

using DispatchPtr = std::shared_ptr<score::os::Dispatch>;
using IoFuncPtr = std::shared_ptr<score::os::IoFunc>;

/// @brief ResourceManager class encapsulates the skeleton of QNX resource manager.
///
/// This class providing the common implementation to setup and run QNX resource manager
/// with flexibility to configure specific configuration per Resource.
///
class ResourceManager final
{
  public:
    /// @brief constructor of the resource manager instance.
    ///
    /// @param resource_manager_configuration The general configuration applied on all resources.
    /// @param resources all the resources to be managed by the resource manager instance.
    ///
    explicit ResourceManager(const ResourceManagerConfig& resource_manager_configuration,
                             const std::vector<std::shared_ptr<Resource>> resources,
                             const DispatchPtr dispatch,
                             const IoFuncPtr io_func);

    ~ResourceManager() = default;

    /// @brief The sequence/procedure to attach and setup the resource manager as recommended by QNX.
    ///
    /// @return Should return NOERROR(0) if there is no issues or ERROR(-1) in case of any failure.
    std::int32_t Initialize();

    /// @brief The process of the resource manager itself, wait for request then handle it and so.. in super loop.
    ///
    /// @param exit_token To enable the exit from the resource manager process
    /// @return Should return NOERROR(0) if there is no issues or ERROR(-1) in case of any failure.
    std::int32_t Run(const score::cpp::stop_token& exit_token) const;

  private:
    /// @brief create and allocate the dispatch structure that should contain the communication channel id.
    ///
    /// @return Should return NOERROR(0) if there is no issues or ERROR(-1) in case of any failure.
    std::int32_t InitializeDispatchInterface();

    /// @brief register handlers for the I/O POSIX requests (ex, read and write ...).
    void InitializeHandlers();

    /// @brief  initialize the attribute structure contains information about all resources associated with pathname.
    void InitializeIoFuncParameters();

    /// @brief Register all the resources and link it to the dispatch strucure and unique pathname.
    ///
    /// @return Should return NOERROR(0) if there is no issues or ERROR(-1) in case of any failure.
    std::int32_t AttachResource();

    /// @brief Allocate context structure contains a buffer where messages will be received.
    ///
    /// @return Should return NOERROR(0) if there is no issues or ERROR(-1) in case of any failure.
    std::int32_t AllocateContextStructure();

  private:
    /// @brief contain the general configuration of the resources manager in general.
    ///
    /// This configuration should be process-based to be applied on all the managed resources.
    /// examples : the size of the messages, the I/O parts and the flags to direct running the process.
    resmgr_attr_t resmgr_attributes_;

    /// @brief reference to the dispatch structure interface.
    ///
    /// This structure should contain the Channel Id of the communication and to be set after resmgr_attach().
    dispatch_t* dispatch_handle_;

    /// @brief The actual allocation of the messages data of the requests.
    dispatch_context_t* dispatch_context_;

    /// @brief all the resources to be managed by single resource manager process.
    std::vector<std::shared_ptr<Resource>> resources_;

    /// @brief unique pointer to all the system call of the "iofunc" layer provided by OSAL.
    IoFuncPtr iofunc_calls_;

    /// @brief unique pointer to all the system call of the "dispatch" layer provided by OSAL.
    DispatchPtr dispatch_calls_;

    /// @brief logger to used monitor the status of the process.
    ///
    /// QNX slog2 should eventually be used as mandated by the safety specs.
    /* KW_SUPPRESS_START:AUTOSAR.CTOR.NSDMI_INIT_LIST: Deviation for overloaded ctor ok */
    /* KW_SUPPRESS_START:MISRA.INIT.BRACES: False positive, constructor used correctly */
    score::mw::log::Logger logger_{"ResourceManager"};
    /* KW_SUPPRESS_END:MISRA.INIT.BRACES: False positive, constructor used correctly */
    /* KW_SUPPRESS_END:AUTOSAR.CTOR.NSDMI_INIT_LIST: Deviation for overloaded ctor ok */
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_RESOURCE_MANAGER_H
