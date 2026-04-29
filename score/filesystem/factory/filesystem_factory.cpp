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
#include "score/filesystem/factory/filesystem_factory.h"

#include "score/filesystem/details/standard_filesystem.h"
#include "score/filesystem/file_utils/file_utils.h"
#include "score/filesystem/filestream/file_factory.h"

namespace score
{
namespace filesystem
{
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
Filesystem FilesystemFactory::CreateInstance() const noexcept
{
    auto standard = std::make_shared<StandardFilesystem>();
    auto streams = std::make_shared<FileFactory>();
    auto utils = std::make_shared<FileUtils>(*standard, *streams);
    return {std::move(standard), std::move(utils), std::move(streams)};
}

}  // namespace filesystem
}  // namespace score
