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
#include "score/os/glob_impl.h"

#include <score/span.hpp>

namespace score
{
namespace os
{

GlobImpl::GlobImpl() : Glob{}, buffer_{} {}

GlobImpl::~GlobImpl()
{
    if (buffer_.gl_pathv != nullptr)
    {
        ::globfree(&buffer_);
    }
}

// Suppress "AUTOSAR C++14 A12-8-4", The rule states: "Move constructor shall not initialize its class members and base
// classes using copy semantics.
// Justification: This is a wrapper class, underlying object contains address of data (pathnames) which is copied,
// but actual data is not copied, hence there is no performance impact. False positive.
// coverity[autosar_cpp14_a12_8_4_violation]
GlobImpl::GlobImpl(GlobImpl&& other) noexcept : Glob{}, buffer_(other.buffer_)
{
    other.buffer_.gl_pathc = 0U;
    other.buffer_.gl_pathv = nullptr;
}

GlobImpl& GlobImpl::operator=(GlobImpl&& other) noexcept
{
    // Negative case: Not possible to do self assignment test as clang-test checks on CI fails with error:
    // explicitly moving variable of type 'score::os::GlobImpl' to itself.
    if (this != &other)  // LCOV_EXCL_BR_LINE
    {
        if (buffer_.gl_pathv != nullptr)
        {
            ::globfree(&buffer_);
        }
        buffer_ = other.buffer_;

        other.buffer_.gl_pathc = 0U;
        other.buffer_.gl_pathv = nullptr;
    }
    return *this;
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// Rationale: some of the std functions involving string, stl structures may throw bad_alloc, but it is rare scenario
// Exception thrown from noexcept lib will result in terminate.
// Such exception can not be handled by any other means, hence suppressed here to avoid try..catch
// coverity[autosar_cpp14_a15_5_3_violation]
score::cpp::expected<Glob::MatchResult, Error> GlobImpl::Match(const std::string& pattern, const Glob::Flag flags) noexcept
{
    MatchResult match_result{};

    auto glob_result = ::glob(
        pattern.c_str(), static_cast<std::int32_t>(internal::glob_helper::FlagToInteger(flags)), nullptr, &buffer_);
    if (glob_result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromGlobError(glob_result));
    }

    const auto paths_count = buffer_.gl_pathc;
    score::cpp::span<char*> paths(buffer_.gl_pathv, paths_count);

    for (const char* path : paths)
    {
        // Manual code analysis:
        // Negative test case: This is a defensive progamming. According to QNX documentation, the GLOB_DOOFFS flag
        // allows adding `glob_t->gl_offs` `nullptr` pointers to the start of `glob_t->gl_pathv`. However, this cannot
        // be done with the current implementation since `buffer_is` is a private member.
        if (path != nullptr)  // LCOV_EXCL_BR_LINE
        {
            score::cpp::ignore = match_result.paths.emplace_back(path);  // LCOV_EXCL_LINE : tooling issue
        }
        else
        {
            return score::cpp::make_unexpected(Error::createUnspecifiedError());  // LCOV_EXCL_LINE
        }
    }
    match_result.count = match_result.paths.size();

    return match_result;
}

}  // namespace os
}  // namespace score
