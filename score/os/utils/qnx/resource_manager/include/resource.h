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

#ifndef SCORE_LIB_OS_QNX_RESOURCE_H
#define SCORE_LIB_OS_QNX_RESOURCE_H

#include "score/os/utils/qnx/resource_manager/include/static_c_wrappers.h"

#include <string>

namespace score
{
namespace os
{

/// @brief Resource class encapsulates the resource/device based configuration.
///
/// This class providing the needed setters and getters for all resource parameters to be managed .
///
class Resource final
{
  public:
    /// @brief the default version of the constructor which is used in case that the attributes of
    /// the Resource are not ready at the phase of the instantiation.
    Resource();

    /// @brief Constructor of the Resource instance, should initialize all the resource-specific attributes.
    ///
    /// @param resource_path The file pathname should be known for the resource/device
    /// @param resource_flags permissions flags for the file.
    /// @param resource_ftype The file type to be identified for the system.
    /// @param resource_attr reference to the resource attributes (iofunc_attr_t) and the configured I/O and Connect
    /// handlers.
    explicit Resource(const std::string& resource_path,
                      const std::uint32_t resource_flags,
                      const _file_type resource_ftype,
                      const extended_dev_attr_t& resource_attributes);

    ~Resource() = default;

    /// @brief Getter method for the resource filename.
    ///
    /// @return The file pathname of the resource.
    std::string GetResourcePath() const;

    /// @brief Setter method for the resource filename.
    ///
    /// @param resource_path The file pathname of the resource.
    void SetResourcePath(const std::string& resource_path);

    /// @brief Getter method for the permission flags of the resource.
    ///
    /// @return The permission flags of the resource.
    std::uint32_t GetResourceFlags() const;

    /// @brief Setter method for the permission flags of the resource.
    ///
    /// @param resource_flags The the permission flags of the resource.
    void SetResourceFlags(const std::uint32_t resource_flags);

    /// @brief Getter method for the file type of the resource.
    ///
    /// @return The file type  of the resource.
    _file_type GetResourceFileType() const;

    /// @brief Setter method for the file type of the resource.
    ///
    /// @param resource_ftype The file type  of the resource.
    void SetResourceFileType(const _file_type resource_ftype);

    /// @brief Getter method for the references of the resource attributes and I/O handlers.
    ///
    /// @return The address of the device/resource extended attributes structure.
    const extended_dev_attr_t& GetResourceAttributes() const;

    /// @brief overloaded Setter method for the I/O handlers.
    ///
    /// @param <posix_handler> Handler/callback for each supported POSIX request.
    void SetRequestHandler(const std::shared_ptr<IReadFunction> read_handler);
    /* KW_SUPPRESS_START:MISRA.FUNC.PARAMS.IDENT: function overloading, each declaration has it's definition */
    void SetRequestHandler(const std::shared_ptr<IWriteFunction> write_handler);
    void SetRequestHandler(const std::shared_ptr<ISeekFunction> lseek_handler);
    void SetRequestHandler(const std::shared_ptr<IOpenFunction> open_handler);
    /* KW_SUPPRESS_END:MISRA.FUNC.PARAMS.IDENT */

    /// @brief check for all registered handlers and register it on the system,
    /// Otherwise let the system proceed with the default handlers.
    ///
    void AttachRegisteredHandlers();

    /// @brief Getter method for POSIX Connect handlers of the resource.
    ///
    /// @return The address of the QNX structure of the connect functions handlers.
    const resmgr_connect_funcs_t& GetResourceConnectFunctions() const;

    /// @brief Getter method for POSIX I/O handlers of the resource.
    ///
    /// @return The address of the QNX structure of the I/O functions handlers.
    const resmgr_io_funcs_t& GetResourceIoFunctions() const;

  private:
    /// @brief The pathname should be identifying the resource to the client and the system.
    ///
    /// This name is linked with resource manager process and the system after resmgr_attach() is done.
    std::string resource_path_;

    /// @brief The permission flag of the resource.
    ///
    /// This flags is similar to any UNIX-based file permissions (ex:777 provide read,write and execute access to uid,
    /// guid and others).
    std::uint32_t resource_flags_;

    /// @brief The resource file type.
    ///
    /// Could be any type defined in <sys/ftype.h>, but most of the cases is going to be _FTYPE_ANY.
    _file_type resource_file_type_;

    /// @brief struct for all resource-based properties.
    ///
    /// originally this was just QNX (iofunc_attr_t structure) containing the properties of the file like change time,
    /// the current permission and so ... ,
    /// but was extended also to include reference to the configured handlers of the POSIX request
    ///
    extended_dev_attr_t resource_attributes_;

    /// @brief I/O request handlers (read,write and lseek ...)
    ///
    /// QNX provide default handlers for all I/O POSIX interfaces,
    /// The resource manager just overwrite the registered handler by the application via AttachRegisteredHandlers().
    resmgr_io_funcs_t io_funcs_;

    /// @brief Connect request handlers (open,close...)
    ///
    /// QNX provide default handlers for all Connect POSIX interfaces,
    /// The resource manager just overwrite the registered handler by the application via AttachRegisteredHandlers().
    resmgr_connect_funcs_t connect_funcs_;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_RESOURCE_H
