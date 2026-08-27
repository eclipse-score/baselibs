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
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_FACTORY_FAKE_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_FACTORY_FAKE_H

#include "score/filesystem/filestream/file_factory_mock.h"
#include "score/filesystem/filestream/i_string_stream_collection.h"

#include "score/os/stat.h"

#include <functional>

namespace score::filesystem
{

/// @brief Fake implementation of IFileFactory, to be used for Unit-Tests.
///
/// @details The methods Get() enable manipulation of underlying streams. This factory owns
/// the underlying string streams and thus the underlying character devices. Towards the user we provide another stream
/// that also points towards the same character device.
/// @note This class is already included in StandardFilesystemFake.
/// So there is no need to create this class separately if StandardFilesystemFake object is already created.
class FileFactoryFake : public FileFactoryMock
{
  public:
    explicit FileFactoryFake(IStringStreamCollection& collection);

    /// @brief Returns reference to string stream for specified path.
    ///
    /// @details The method is intended to be used in unit tests.
    /// The specified path should exist in IStringStreamCollection, otherwise the assert error occurs.
    [[nodiscard]] std::stringstream& Get(const Path& path) const;

  private:
    [[nodiscard]] score::Result<std::unique_ptr<std::iostream>> FakeOpenWithMode(const Path& path,
                                                                                 std::ios_base::openmode mode,
                                                                                 os::Stat::Mode create_mode) const;

    [[nodiscard]] score::Result<std::unique_ptr<std::iostream>> FakeOpen(const Path& path,
                                                                         std::ios_base::openmode mode) const
    {
        return FakeOpenWithMode(path, mode, os::Stat::Mode::kNone);
    }
    // The fake implementation of AtomicUpdate is behaving just like Open. This is fine as the behavior is the same
    // in the absence of spontaneous power cuts that skip the sync / rename part.
    [[nodiscard]] Result<std::unique_ptr<FileStream>> FakeAtomicUpdate(
        const Path& path,
        std::ios_base::openmode mode,
        const AtomicUpdateOwnershipFlags ownership_flag = kUseTargetFileUID | kUseTargetFileGID) const;

    std::reference_wrapper<IStringStreamCollection> collection_;
};

}  // namespace score::filesystem

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_FACTORY_FAKE_H
