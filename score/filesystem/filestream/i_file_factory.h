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
#ifndef SCORE_LIB_FILESYSTEM_I_FILE_FACTORY_H
#define SCORE_LIB_FILESYSTEM_I_FILE_FACTORY_H

#include "score/filesystem/error.h"
#include "score/filesystem/filestream/file_stream.h"
#include "score/filesystem/path.h"

#include "score/os/ObjectSeam.h"

#include <ios>
#include <istream>
#include <memory>

namespace score
{
namespace filesystem
{

using AtomicUpdateOwnershipFlags = std::uint32_t;

inline constexpr AtomicUpdateOwnershipFlags kUseTargetFileUID =
    static_cast<AtomicUpdateOwnershipFlags>(static_cast<AtomicUpdateOwnershipFlags>(1U) << 0U);
inline constexpr AtomicUpdateOwnershipFlags kUseTargetFileGID =
    static_cast<AtomicUpdateOwnershipFlags>(static_cast<AtomicUpdateOwnershipFlags>(1U) << 1U);
inline constexpr AtomicUpdateOwnershipFlags kUseCurrentProcessUID =
    static_cast<AtomicUpdateOwnershipFlags>(static_cast<AtomicUpdateOwnershipFlags>(1U) << 2U);
inline constexpr AtomicUpdateOwnershipFlags kUseCurrentProcessGID =
    static_cast<AtomicUpdateOwnershipFlags>(static_cast<AtomicUpdateOwnershipFlags>(1U) << 3U);

/// @brief Abstracts the way of how to create input / out operations towards files
///
/// @details In general the idea is to use this factory to create respective /// @brief Then it is possible to inject
/// mocks (aka FileFactoryFake) to avoid file streams in testing environments.
class IFileFactory : public os::ObjectSeam<IFileFactory>
{
  public:
    /// @brief Access method for migration purpose to dependency injection
    static IFileFactory& instance() noexcept;

    /// @brief Opens a respective file stream under the provided path
    virtual score::Result<std::unique_ptr<std::iostream>> Open(const Path&, const std::ios_base::openmode mode) = 0;

    /// Open a (possibly existing) file for atomic updating its contents, creating it if it does not exist.
    ///
    /// If the file already exists, its contents get replaced by the new data when the returned object goes out of
    /// scope, or it's `Close` method gets called. This happens atomically by creating a temporary file that receives
    /// the data during the write phase. Once the returned object goes out of scope or is closed, the temporary file's
    /// contents get synced to disc and the file gets renamed to the target file name. This way, you will always
    /// either see the previous content (if any), or the newly written complete data.
    ///
    /// @param path Path to the file that is going to be updated.
    /// @param mode The open mode for the file. Currently, only writing and truncating are supported.
    /// @return On success, a pointer to a FileStream object, an error otherwise.
    // as intended, we don't enforce users to specify ownership flags unless needed
    // defaults for override and base function are the same thus static binding is safe
    // NOLINTNEXTLINE(google-default-arguments) : see above
    [[nodiscard]] virtual Result<std::unique_ptr<FileStream>> AtomicUpdate(
        const Path& path,
        const std::ios_base::openmode mode,
        const AtomicUpdateOwnershipFlags ownership_flag = kUseTargetFileUID | kUseTargetFileGID) = 0;

    /// @brief Destructor
    virtual ~IFileFactory() noexcept;

    /// @brief Default Constructor
    IFileFactory() noexcept;

  protected:
    /// @brief Copy Constructor
    IFileFactory(const IFileFactory&) noexcept;

    /// @brief Copy Assignment Operator
    IFileFactory& operator=(const IFileFactory&) & noexcept;

    /// @brief Move Constructor
    IFileFactory(IFileFactory&&) noexcept;

    /// @brief Move Assignment Operator
    IFileFactory& operator=(IFileFactory&&) & noexcept;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_I_FILE_FACTORY_H
