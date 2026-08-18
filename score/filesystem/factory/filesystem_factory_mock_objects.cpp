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
#include "score/filesystem/factory/filesystem_factory_mock_objects.h"

namespace score
{
namespace filesystem
{

using ::testing::Invoke;

FilesystemFactoryMockObjects::FilesystemFactoryMockObjects()
{
    score::cpp::ignore = ON_CALL(*this, CreateInstance())
                             .WillByDefault(Invoke(this, &FilesystemFactoryMockObjects::MockObjectsCreateInstance));
}

Filesystem FilesystemFactoryMockObjects::MockObjectsCreateInstance() const noexcept
{
    return {standard_, utils_, streams_};
}

StandardFilesystemMock& FilesystemFactoryMockObjects::GetStandard() const noexcept
{
    return *standard_;
}
FileUtilsMock& FilesystemFactoryMockObjects::GetUtils() const noexcept
{
    return *utils_;
}
FileFactoryMock& FilesystemFactoryMockObjects::GetStreams() const noexcept
{
    return *streams_;
}

}  // namespace filesystem
}  // namespace score
