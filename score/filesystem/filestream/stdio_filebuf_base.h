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
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_STDIO_FILEBUF_BASE_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_STDIO_FILEBUF_BASE_H

#include <iosfwd>
#include <iostream>

// The usage of the pre-processor here is imperative to differentiate between
// GCC's libstdc++ and LLVM's libstdcxx.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GLIBCXX__)  // We're using GCC's libstdc++

#include <ext/stdio_filebuf.h>
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#define FILE_FACTORY_GLIBCXX

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#elif defined(_LIBCPP_VERSION)  // We're using libstdcxx from LLVM
#include <fstream>
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#define FILE_FACTORY_LLVMLIBCXX
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#else
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#error Unknown standard C++ library variant
// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#endif

namespace score::filesystem::details
{

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#ifdef FILE_FACTORY_GLIBCXX

using StdioFilebufBase = __gnu_cxx::stdio_filebuf<char>;

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#elif defined(FILE_FACTORY_LLVMLIBCXX)

// False positive, the ifdef guards guarantee that only one version is defined.
// coverity[autosar_cpp14_m3_2_3_violation]
class StdioFilebufBase : public std::filebuf
{
  public:
    StdioFilebufBase(int fd, std::ios::openmode mode) : std::filebuf{}, file_handle_{fd}
    {
        // Use a non-standard extension to associate the file descriptor with the filebuf
        // coverity[autosar_cpp14_a0_1_2_violation]
        __open(fd, mode);  // NOLINT(score-qnx-banned-builtin): This is not a builtin but an extension we need to use
    }
    ~StdioFilebufBase() override = default;
    StdioFilebufBase(const StdioFilebufBase&) = delete;
    StdioFilebufBase& operator=(const StdioFilebufBase&) = delete;
    // Move constructor/operator of the base class are not marked noexcept.
    // Adding noexcept here would violate LSP and base class compatibility.
    // Need to be compatible with the GCC counterpart.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFilebufBase(StdioFilebufBase&&) = default;  // NOLINT(performance-noexcept-move-constructor): see above
    // Need to be compatible with the GCC counterpart.
    // coverity[autosar_cpp14_a12_8_6_violation]
    StdioFilebufBase& operator=(StdioFilebufBase&&) =  // NOLINT(performance-noexcept-move-constructor): see above
        default;

    int fd() const noexcept
    {
        return file_handle_;
    }

  private:
    int file_handle_;
};

// coverity[autosar_cpp14_a16_0_1_violation] See rationale at beginning of this file
#endif  // FILE_FACTORY_LLVMLIBCXX

}  // namespace score::filesystem::details

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_STDIO_FILEBUF_BASE_H
