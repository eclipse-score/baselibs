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
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#if !defined(STDIO_FILEBUF_BASE_TESTING)
#include "score/filesystem/filestream/stdio_filebuf_base.h"
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#else
#include "score/filesystem/filestream/stdio_filebuf_base_testing.h"
#endif  // STDIO_FILEBUF_BASE_TESTING

#include "score/filesystem/path.h"

#include "score/result/result.h"

#include <iosfwd>
#include <iostream>

namespace score::filesystem::details
{
class StdioFileBuf : public StdioFilebufBase
{
  public:
    using StdioFilebufBase::StdioFilebufBase;
    StdioFileBuf(const StdioFileBuf&) = delete;
    StdioFileBuf& operator=(const StdioFileBuf&) = delete;
    ~StdioFileBuf() override = default;
    // Move constructor/operator of the base class are not marked noexcept.
    // Adding noexcept here would violate LSP and base class compatibility.
    // Need to be public as the class is used directly and moving is needed.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFileBuf& operator=(StdioFileBuf&&) = default;  // NOLINT(performance-noexcept-move-constructor): see above
    // Need to be public as the class is used directly and moving is needed.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFileBuf(StdioFileBuf&&) = default;  // NOLINT(performance-noexcept-move-constructor): see above

    virtual ResultBlank Close();
};

class AtomicFileBuf : public StdioFileBuf
{
  public:
    AtomicFileBuf(int fd, std::ios::openmode mode, Path from_path, Path to_path);
    // Move constructor/operator of the base class are not marked noexcept.
    // Adding noexcept here would violate LSP and base class compatibility.
    AtomicFileBuf& operator=(AtomicFileBuf&&) = default;  // NOLINT(performance-noexcept-move-constructor): see above
    AtomicFileBuf(AtomicFileBuf&&) = default;             // NOLINT(performance-noexcept-move-constructor): see above
    AtomicFileBuf(const AtomicFileBuf&) = delete;
    AtomicFileBuf& operator=(const AtomicFileBuf&) = delete;
    ~AtomicFileBuf() override;

    ResultBlank Close() override;

  private:
    Path from_path_;
    Path to_path_;
};

}  // namespace score::filesystem::details

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_FILE_BUF_H
