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
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H

#include "score/filesystem/filestream/file_buf.h"

#include "score/result/result.h"

#include <score/utility.hpp>

#include <fstream>
#include <iostream>

namespace score::filesystem
{

// NOLINTNEXTLINE(fuchsia-multiple-inheritance): The multiple inheritance comes from iostream. We can't change that.
class FileStream : public std::iostream
{
  public:
    explicit FileStream(details::StdioFileBuf* filebuf) : std::basic_ios<char>::basic_ios{}, std::iostream{filebuf} {}
    ~FileStream() override = default;
    FileStream(const FileStream&) = delete;
    FileStream& operator=(const FileStream&) = delete;
    FileStream(FileStream&& other) = delete;
    // coverity[autosar_cpp14_a12_8_6_violation] See above.
    FileStream& operator=(FileStream&& other) = delete;

    Result<void> Close();
};

namespace details
{

template <typename Buf>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance): The multiple inheritance comes from iostream. We can't change that.
class FileStreamImpl : public FileStream
{
  public:
    template <typename First,
              std::enable_if_t<!std::is_same_v<score::cpp::remove_cvref_t<First>, FileStreamImpl>, bool> = true,
              typename... Args>
    explicit FileStreamImpl(First&& first, Args&&... remaining)
        : std::basic_ios<char>::basic_ios{},
          FileStream{&buf_},
          buf_{std::forward<First>(first), std::forward<Args>(remaining)...}
    {
    }

    FileStreamImpl(const FileStreamImpl&) = delete;

    FileStreamImpl& operator=(FileStreamImpl&& other) noexcept(false)
    {
        buf_ = std::move(other.buf_);
        return *this;
    }

    ~FileStreamImpl() override
    {
        score::cpp::ignore = Close();
    }

    FileStreamImpl& operator=(const FileStreamImpl&) = delete;
    // There's no ambiguity, as constructor is templated to avoid first element belonging to this class
    // coverity[autosar_cpp14_a13_3_1_violation:FALSE]
    FileStreamImpl(FileStreamImpl&& other) noexcept(false) : FileStream{&buf_}, buf_{std::move(other.buf_)} {}

  private:
    Buf buf_;
};

}  // namespace details

}  // namespace score::filesystem

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_STREAM_H
