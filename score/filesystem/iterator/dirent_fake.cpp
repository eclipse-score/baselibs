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
#include "score/filesystem/iterator/dirent_fake.h"

#include "score/filesystem/standard_filesystem_fake.h"

#include <score/assert.hpp>

namespace score
{
namespace filesystem
{

DirentFake::DirentFake(StandardFilesystemFake& filesystem)
    : score::os::MockDirent{}, filesystem_{filesystem}, dir_streams_{}, next_id_{1}
{
    ON_CALL(*this, opendir(::testing::_)).WillByDefault(::testing::Invoke(this, &DirentFake::FakeOpendir));
    ON_CALL(*this, readdir(::testing::_)).WillByDefault(::testing::Invoke(this, &DirentFake::FakeReaddir));
    EXPECT_CALL(*this, scandir(::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    ON_CALL(*this, closedir(::testing::_)).WillByDefault(::testing::Invoke(this, &DirentFake::FakeClosedir));
}

score::cpp::expected<DIR*, score::os::Error> DirentFake::FakeOpendir(const char* name) noexcept
{
    const Path path = name;
    const auto exists_result = filesystem_.Exists(path);
    if ((!exists_result.has_value()) || (!exists_result.value()))
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOENT));
    }
    const auto status_result = filesystem_.Status(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT(status_result.has_value());
    if (status_result.value().Type() != FileType::kDirectory)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOTDIR));
    }
    const auto filelist_result = filesystem_.GetFileList(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT(filelist_result.has_value());
    DIR* id = reinterpret_cast<DIR*>(next_id_);
    ++next_id_;
    auto& dir_stream = dir_streams_[id];
    dir_stream.path_ = filesystem_.WeaklyCanonical(path).value();
    dir_stream.filelist_ = filelist_result.value();
    dir_stream.filelist_.emplace_front("..");
    dir_stream.filelist_.emplace_front(".");
    dir_stream.current_ = dir_stream.filelist_.begin();
    return id;
}

score::cpp::expected<struct dirent*, score::os::Error> DirentFake::FakeReaddir(DIR* dirp) noexcept
{
    auto dir_stream_iterator = dir_streams_.find(dirp);
    if (dir_stream_iterator == dir_streams_.end())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EBADF));
    }
    auto& dir_stream = dir_stream_iterator->second;
    if (dir_stream.current_ == dir_stream.filelist_.end())
    {
        // according to score::os::DirentImpl in case of end of directory
        // the unexpected error from errno should be returned instead of nullptr
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    const std::string& name = *dir_stream.current_;
    ++dir_stream.current_;
    // resize buffer for dirent
    const std::size_t name_size = strnlen(name.c_str(), static_cast<std::size_t>(NAME_MAX) + 1U);
    SCORE_LANGUAGE_FUTURECPP_ASSERT(name_size <= static_cast<std::size_t>(NAME_MAX));
    const std::size_t dirent_buffer_size =
        std::max(sizeof(dirent), (sizeof(dirent) - sizeof(dirent::d_name) + name_size + 1U));
    dir_stream.dirent_buffer_.resize(dirent_buffer_size, 0);
    dirent& dir_entry = *reinterpret_cast<dirent*>(&dir_stream.dirent_buffer_[0]);
    // copy name
    score::cpp::ignore = strncpy(dir_entry.d_name, name.c_str(), name_size);
    dir_entry.d_name[name_size] = '\0';
    // set file type (Skipped - see note)
    // Note: dirent::d_type is not supported in QNX and is not used in aas/lib/filesystem/iterator
    return &dir_entry;
}

score::cpp::expected_blank<score::os::Error> DirentFake::FakeClosedir(DIR* dirp) noexcept
{
    const auto dir_stream_iterator = dir_streams_.find(dirp);
    if (dir_stream_iterator == dir_streams_.end())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EBADF));
    }
    else
    {
        score::cpp::ignore = dir_streams_.erase(dir_stream_iterator);
    }
    return {};
}

}  // namespace filesystem
}  // namespace score
