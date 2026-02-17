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

#include "score/os/utils/qnx/resource_manager/include/resource.h"

namespace score
{
namespace os
{

Resource::Resource()
    : resource_path_{""}, resource_flags_(0), resource_file_type_(_FTYPE_ANY), io_funcs_{}, connect_funcs_{}
{
}

/// @brief initializer list for all the private members of the Resource instance.
Resource::Resource(const std::string& resource_path,
                   const std::uint32_t resource_flags,
                   const _file_type resource_ftype,
                   const extended_dev_attr_t& resource_attributes)
    : resource_path_(resource_path),
      resource_flags_(resource_flags),
      resource_file_type_(resource_ftype),
      resource_attributes_(resource_attributes),
      io_funcs_{},
      connect_funcs_{}
{
}

// setters and getters

std::string Resource::GetResourcePath() const
{
    return this->resource_path_;
}

void Resource::SetResourcePath(const std::string& resource_path)
{
    this->resource_path_ = resource_path;
}

std::uint32_t Resource::GetResourceFlags() const
{
    return this->resource_flags_;
}

void Resource::SetResourceFlags(const std::uint32_t resource_flags)
{
    this->resource_flags_ = resource_flags;
}

_file_type Resource::GetResourceFileType() const
{
    return this->resource_file_type_;
}

void Resource::SetResourceFileType(const _file_type resource_ftype)
{
    this->resource_file_type_ = resource_ftype;
}

const extended_dev_attr_t& Resource::GetResourceAttributes() const
{
    return this->resource_attributes_;
}

void Resource::SetRequestHandler(const std::shared_ptr<IReadFunction> read_handler)
{
    this->resource_attributes_.handlers.read_handler = read_handler;
}

void Resource::SetRequestHandler(const std::shared_ptr<IWriteFunction> write_handler)
{
    this->resource_attributes_.handlers.write_handler = write_handler;
}

void Resource::SetRequestHandler(const std::shared_ptr<ISeekFunction> lseek_handler)
{
    this->resource_attributes_.handlers.lseek_handler = lseek_handler;
}

void Resource::SetRequestHandler(const std::shared_ptr<IOpenFunction> open_handler)
{
    this->resource_attributes_.handlers.open_handler = open_handler;
}

const resmgr_connect_funcs_t& Resource::GetResourceConnectFunctions() const
{
    return this->connect_funcs_;
}

const resmgr_io_funcs_t& Resource::GetResourceIoFunctions() const
{
    return this->io_funcs_;
}

/// @brief register all configured handlers to QNX system.
/// Only the following requests is considered till now, read(),write(),lseek() and open()
/// if there is no handler is provided, the default qnx handler should be considered.
void Resource::AttachRegisteredHandlers()
{
    if (this->resource_attributes_.handlers.open_handler != nullptr)
    {
        connect_funcs_.open = &connect_open_handler;
    }

    if (this->resource_attributes_.handlers.lseek_handler != nullptr)
    {
        io_funcs_.lseek = &io_lseek_handler;
    }

    // The io_[read/write]_handler is set anyway,
    // which they are returning ENOSYS error in case of this->resource_attributes_.handlers.[read/write]_handler is
    // nullptr which means this operation is not supported otherwise call the attached callback
    // this->resource_attributes_.handlers.[read/write]_handler
    io_funcs_.read = &io_read_handler;
    io_funcs_.write = &io_write_handler;
}

}  // namespace os
}  // namespace score
