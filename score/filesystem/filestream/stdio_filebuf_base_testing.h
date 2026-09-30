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
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_STDIO_FILEBUF_BASE_TESTING_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_STDIO_FILEBUF_BASE_TESTING_H

#include <iosfwd>
#include <iostream>

// The usage is pre-processor here is imperative to differentiate between
// the production version and the testing version, that is mockable.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(STDIO_FILEBUF_BASE_TESTING)

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef FILE_FACTORY_GLIBCXX
#include <ext/stdio_filebuf.h>

using RealStdioFilebufBase = __gnu_cxx::stdio_filebuf<char>;

class StdioFilebufBase : public RealStdioFilebufBase
// coverity[autosar_cpp14_a16_0_1_violation]
#else
// coverity[autosar_cpp14_a16_0_1_violation]
class StdioFilebufBase : public std::filebuf
#endif
{
  public:
    StdioFilebufBase(int fd, std::ios::openmode) : file_handle_{fd} {}
    ~StdioFilebufBase() override = default;
    StdioFilebufBase(const StdioFilebufBase&) = delete;
    StdioFilebufBase& operator=(const StdioFilebufBase&) = delete;
    StdioFilebufBase(StdioFilebufBase&& other) noexcept : file_handle_{other.file_handle_}
    {
        other.file_handle_ = -1;
    }

    StdioFilebufBase& operator=(StdioFilebufBase&& other) noexcept
    {
        if (this != &other)
        {
            file_handle_ = other.file_handle_;
            other.file_handle_ = -1;
        }
        return *this;
    }

    // coverity[autosar_cpp14_m11_0_1_violation] False positive, mock declaration
    MOCK_METHOD(StdioFilebufBase*, close, (), ());
    // coverity[autosar_cpp14_m11_0_1_violation] False positive, mock declaration
    MOCK_METHOD(int, sync, (), (override));
    // coverity[autosar_cpp14_m11_0_1_violation] False positive, mock declaration
    MOCK_METHOD(int, is_open, (), ());
    // coverity[autosar_cpp14_m11_0_1_violation] False positive, mock declaration
    MOCK_METHOD(int, open, (const char*, std::ios::openmode), ());
    // coverity[autosar_cpp14_m11_0_1_violation] False positive, mock declaration
    MOCK_METHOD(std::streambuf*, rdbuf, (), (const));

    int fd() const noexcept
    {
        return file_handle_;
    }

  private:
    int file_handle_;
};

}  // namespace score::filesystem::details

#endif  // STDIO_FILEBUF_BASE_TESTING
#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_STDIO_FILEBUF_BASE_TESTING_H
