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
#include "score/filesystem/iterator/recursive_directory_iterator.h"

namespace score
{
namespace filesystem
{

// This constructor should be = default but gcc 9 and lower version has a bug that if you default it
// considers the constructor as deleted because one of the members is not noexcept
// For more information see:
// broken_link_j/Ticket-148878?focusedId=16079234&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-16079234
// NOLINTBEGIN(modernize-use-equals-default): See above
// false-positive: can't create delegate due to defaulted constructors and can't mix init list with in-class init
// coverity[autosar_cpp14_a12_1_5_violation]
RecursiveDirectoryIterator::RecursiveDirectoryIterator() noexcept {}
// NOLINTEND(modernize-use-equals-default)

RecursiveDirectoryIterator::RecursiveDirectoryIterator(const RecursiveDirectoryIterator&) noexcept = default;
RecursiveDirectoryIterator::RecursiveDirectoryIterator(RecursiveDirectoryIterator&&) noexcept = default;
RecursiveDirectoryIterator& RecursiveDirectoryIterator::operator=(const RecursiveDirectoryIterator&) noexcept = default;
RecursiveDirectoryIterator& RecursiveDirectoryIterator::operator=(RecursiveDirectoryIterator&&) noexcept = default;
RecursiveDirectoryIterator::~RecursiveDirectoryIterator() noexcept = default;

// false-positive: can't create delegate due to defaulted constructors and can't mix init list with in-class init
// coverity[autosar_cpp14_a12_1_5_violation]
RecursiveDirectoryIterator::RecursiveDirectoryIterator(const Path& path,
                                                       const DirectoryOptions directory_options) noexcept
{
    directory_options_ = directory_options;
    if (directory_options == DirectoryOptions::kFollowDirectorySymlink)
    {
        follow_directory_symlink_ = true;
    }
    else
    {
        follow_directory_symlink_ = false;
    }

    score::cpp::ignore = folders_->emplace(DirectoryIterator{path, directory_options});
    if (!folders_->top().HasValue())
    {
        *error_ = folders_->top().Error();
        *folders_ = std::stack<DirectoryIterator>{};
    }
    else if (folders_->top() == end(DirectoryIterator{}))
    {
        *folders_ = std::stack<DirectoryIterator>{};
    }
    else
    {
        // No action required
    }
}

RecursiveDirectoryIterator::reference RecursiveDirectoryIterator::operator*() const noexcept
{
    // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!folders_->empty());
    // LCOV_EXCL_BR_STOP
    return *folders_->top();
}

RecursiveDirectoryIterator::pointer RecursiveDirectoryIterator::operator->() const noexcept
{
    return &operator*();
}

bool RecursiveDirectoryIterator::OperatorIncFolderHelper(const FileType file_type) noexcept
{
    bool is_folder{file_type == FileType::kDirectory};
    bool is_not_symlink_folder = (is_folder && (!follow_directory_symlink_));

    if (is_not_symlink_folder)
    {
        const auto top_symlink_status = folders_->top()->SymlinkStatus();
        if (!top_symlink_status.has_value())
        {
            // @todo Once we have cleaned up the mess with lib/result and lib/os/error, we can forward here the correct
            // error
            *error_ = score::os::Error::createFromErrno(EACCES); /* KW_SUPPRESS:MISRA.USE.EXPANSION: caused by EACCES */
            *folders_ = std::stack<DirectoryIterator>{};
            return true;
        }
        const bool is_symlink = (top_symlink_status->Type() == FileType::kSymlink);
        is_folder = !is_symlink;
    }

    auto& top = folders_->top();
    if (is_folder)
    {
        const auto& path = top->GetPath();
        score::cpp::ignore = folders_->emplace(DirectoryIterator{path, directory_options_});
    }
    ++top;

    return false;
}

RecursiveDirectoryIterator& RecursiveDirectoryIterator::operator++() noexcept
{
    if (folders_->empty())
    {
        return *this;
    }
    const auto top_status = folders_->top()->Status();
    if (!top_status.has_value())
    {
        // @todo Once we have cleaned up the mess with lib/result and lib/os/error, we can forward here the correct
        // error
        *error_ = score::os::Error::createFromErrno(EACCES); /* KW_SUPPRESS:MISRA.USE.EXPANSION: caused by EACCES */
        *folders_ = std::stack<DirectoryIterator>{};
        return *this;
    }

    if (OperatorIncFolderHelper(top_status->Type()))
    {
        return *this;  // folder already handled, return current status
    }

    auto endDirectoryIterator = end(DirectoryIterator{});
    while (!folders_->empty())
    {
        if (folders_->top() != endDirectoryIterator)
        {
            break;
        }
        if (!folders_->top().HasValue())
        {
            break;
        }
        folders_->pop();
    }
    if ((!folders_->empty()) && (!folders_->top().HasValue()))
    {
        *error_ = folders_->top().Error();
        *folders_ = std::stack<DirectoryIterator>{};
    }
    return *this;
}

bool RecursiveDirectoryIterator::HasValue() const noexcept
{
    return !error_->has_value();
}

// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate() function shall
// not be called implicitly". The API value() must be compatible with the standard hence cannot be not noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
score::os::Error RecursiveDirectoryIterator::Error() const noexcept
{
    return error_->value();
}

bool operator==(const RecursiveDirectoryIterator& l, const RecursiveDirectoryIterator& r) noexcept
{
    return *l.folders_ == *r.folders_;
}

bool operator!=(const RecursiveDirectoryIterator& l, const RecursiveDirectoryIterator& r) noexcept
{
    return !(l == r);
}

RecursiveDirectoryIterator begin(const RecursiveDirectoryIterator& iterator) noexcept
{
    return iterator;
}

RecursiveDirectoryIterator end(const RecursiveDirectoryIterator& iterator) noexcept
{
    score::cpp::ignore = iterator;
    return RecursiveDirectoryIterator{};
}

}  // namespace filesystem
}  // namespace score
