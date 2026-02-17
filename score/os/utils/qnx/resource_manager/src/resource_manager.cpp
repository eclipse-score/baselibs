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

#include "score/os/utils/qnx/resource_manager/include/resource_manager.h"
#include "score/os/errno.h"

#include <cstring>

namespace score
{
namespace os
{

namespace
{
constexpr std::int32_t kResourceManagerNoError = 0;
constexpr std::int32_t kResourceManagerError = -1;
}  // namespace

/// @brief initializer list for all the private members of the Resource instance.
ResourceManager::ResourceManager(const ResourceManagerConfig& resource_manager_configuration,
                                 const std::vector<std::shared_ptr<Resource>> resources,
                                 const DispatchPtr dispatch,
                                 const IoFuncPtr io_func)
    : resmgr_attributes_{},
      dispatch_handle_(nullptr),
      dispatch_context_(nullptr),
      resources_(resources),
      iofunc_calls_(io_func),
      dispatch_calls_(dispatch)
{
    // Initialize the resource manager general attributes.
    this->resmgr_attributes_.flags = resource_manager_configuration.flags;
    this->resmgr_attributes_.nparts_max = resource_manager_configuration.nparts_max;
    this->resmgr_attributes_.msg_max_size = resource_manager_configuration.msg_max_size;

    // Inform the starting of the resource manager
    this->logger_.LogInfo() << "Resource Manager is starting...";
}

/// @brief The fixed sequence to setup/attach ther resource manager.
/// @return kResourceManagerNoError(-1) if it fails at any step, otherwise return kResourceManagerNoError(0)
std::int32_t ResourceManager::Initialize()
{
    // Create Dispatch structure.
    // Note : The Dispatch structure should contain the communication ID.
    // However, the ch id will be loaded only after resmgr_attach () is called
    if (kResourceManagerNoError != this->InitializeDispatchInterface())
    {
        return kResourceManagerError;
    }

    // Register the I/O callbacks to the system.
    this->InitializeHandlers();

    // Initialize the iofunc layer to control the permissions and user/group information.
    this->InitializeIoFuncParameters();

    // Register the file in the system and link it the manager channel.
    if (kResourceManagerNoError != this->AttachResource())
    {
        return kResourceManagerError;
    }

    // Allocate the messaging context structure
    if (kResourceManagerNoError != this->AllocateContextStructure())
    {
        return kResourceManagerError;
    }

    this->logger_.LogDebug() << "Resource Manager is initialized successfully";

    return kResourceManagerNoError;
}

/// @brief The fixed sequence to run ther resource manager process.
/// @return kResourceManagerError(-1) if it fails at any step, otherwise return kResourceManagerNoError(0)
std::int32_t ResourceManager::Run(const score::cpp::stop_token& exit_token) const
{
    // Super loop of the resource manager process
    while (!exit_token.stop_requested())
    {
        // wait till receive new request
        // NOLINTNEXTLINE(score-banned-function) it is among safety headers.
        const auto res = dispatch_calls_->dispatch_block(this->dispatch_context_);
        /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
        if ((!res.has_value())) /* KW_SUPPRESS:MISRA.STMT.COND.NOT_BOOLEAN: false positive */
        /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */
        {
            this->logger_.LogError() << "Failed to send dispatch block request. Error:"
                                     << strerror(score::os::geterrno());

            return kResourceManagerError;
        }

        // call the right handler of the request
        // NOLINTNEXTLINE(score-banned-function) it is among safety headers.
        const auto ret = this->dispatch_calls_->dispatch_handler(this->dispatch_context_);
        /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
        if ((!ret.has_value())) /* KW_SUPPRESS:MISRA.STMT.COND.NOT_BOOLEAN: false positive */
        /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */
        {
            this->logger_.LogError() << "Failed to send the dispatch handler request.";
            return kResourceManagerError;
        }
    }
    this->logger_.LogDebug() << "Received exit request";

    return kResourceManagerNoError;
}

/// @brief Create the dispatch interface structure and allocate it to the system
std::int32_t ResourceManager::InitializeDispatchInterface()
{
    // dispatch create
    // NOLINTNEXTLINE(score-banned-function) it is among safety headers.
    auto res = this->dispatch_calls_->dispatch_create();

    // handle the return value and exit if error
    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    if (res.has_value()) /* KW_SUPPRESS:MISRA.STMT.COND.NOT_BOOLEAN: false positive */
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */
    {
        this->dispatch_handle_ = res.value();
        this->logger_.LogDebug() << "Dispatch Interface is created";
        return kResourceManagerNoError;
    }
    else
    {
        this->logger_.LogError() << "Failed to initialize dispatch interface, Error:" << strerror(score::os::geterrno());
        return kResourceManagerError;
    }
}

/// @brief Register the configured I/O and connect handlers to QNX system
void ResourceManager::InitializeHandlers()
{
    for (const auto& resource : resources_)
    {
        // Get the resource specific info
        const resmgr_io_funcs_t& io_funcs_ptr = resource->GetResourceIoFunctions();
        const resmgr_connect_funcs_t& connect_funcs_ptr = resource->GetResourceConnectFunctions();

        // Initialize the i/o and connect structures with the default handlers provided by QNX system
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
        this->iofunc_calls_->iofunc_func_init(_RESMGR_CONNECT_NFUNCS,
                                              const_cast<resmgr_connect_funcs_t*>(&connect_funcs_ptr),
                                              _RESMGR_IO_NFUNCS,
                                              const_cast<resmgr_io_funcs_t*>(&io_funcs_ptr));
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
        // Attach the register handlers to the system and overwrite the default functions
        resource->AttachRegisteredHandlers();
    }

    this->logger_.LogDebug() << "I/O Handlers are initialized for" << resources_.size() << "resource(s).";
}

/// @brief Initialize the resource I/O attributes
void ResourceManager::InitializeIoFuncParameters()
{
    for (const auto& resource : resources_)
    {
        const extended_dev_attr_t& device_attributes_ptr = resource->GetResourceAttributes();
        const std::uint32_t file_mode = resource->GetResourceFlags();

        this->iofunc_calls_->iofunc_attr_init(
            const_cast<iofunc_attr_t*>(&(device_attributes_ptr.attr)), file_mode, nullptr, nullptr);
    }
    this->logger_.LogDebug() << "I/O Attributes are initialized for" << resources_.size() << "resource(s).";
}

/// @brief Attach all resources should be managed to the system.
std::int32_t ResourceManager::AttachResource()
{
    std::uint8_t resource_index = 0;

    for (const auto& resource : resources_)
    {
        // Get the resource specific info
        const resmgr_io_funcs_t& io_funcs_ptr = resource->GetResourceIoFunctions();
        const resmgr_connect_funcs_t& connect_funcs_ptr = resource->GetResourceConnectFunctions();
        const extended_dev_attr_t& device_attributes_ptr = resource->GetResourceAttributes();
        const _file_type file_type = resource->GetResourceFileType();

        // Attach the resource
        const auto res = this->dispatch_calls_->resmgr_attach(this->dispatch_handle_,
                                                              &(this->resmgr_attributes_),
                                                              resource->GetResourcePath().c_str(),
                                                              file_type,
                                                              this->resmgr_attributes_.flags,
                                                              &connect_funcs_ptr,
                                                              &io_funcs_ptr,
                                                              const_cast<extended_dev_attr_t*>(&device_attributes_ptr));
        resource_index++;
        // Handle the returned error for the system
        /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
        if (!res.has_value())
        /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */
        {
            /* KW_SUPPRESS_START:MISRA.CAST.UNSIGNED_BITS: Allowed for Logging*/
            this->logger_.LogError() << "Failed to attach the Resource number" << resource_index
                                     << "to the system! Error:" << strerror(score::os::geterrno());
            /* KW_SUPPRESS_END:MISRA.CAST.UNSIGNED_BITS: Allowed for Logging*/
            return kResourceManagerError;
        }

        // log
        /*KW_SUPPRESS_START:MISRA.CAST.UNSIGNED_BITS: Allowed for Logging*/
        this->logger_.LogDebug() << "Resource number" << resource_index << "is attached.";
        /*KW_SUPPRESS_END:MISRA.CAST.UNSIGNED_BITS: Allowed for Logging*/
    }
    return kResourceManagerNoError;
}

/// @brief Allocate the context structure in QNX system
std::int32_t ResourceManager::AllocateContextStructure()
{
    // Allocate the context structure
    // NOLINTNEXTLINE(score-banned-function) it is among safety headers.
    auto res = this->dispatch_calls_->dispatch_context_alloc(this->dispatch_handle_);
    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    if (res.has_value()) /* KW_SUPPRESS:MISRA.STMT.COND.NOT_BOOLEAN: false positive */
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */
    {
        this->dispatch_context_ = res.value();
        this->logger_.LogDebug() << "The context structure for the resource manager is allocated";
        return kResourceManagerNoError;
    }
    else
    {
        this->logger_.LogError() << "Failed to allocate the context structure. Erro:" << strerror(score::os::geterrno());
        return kResourceManagerError;
    }
}

}  // namespace os
}  // namespace score
