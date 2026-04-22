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
#include "score/filesystem/iterator/directory_iterator.h"

#include "score/os/dirent.h"

namespace score
{
namespace filesystem
{
namespace
{

/// @brief As per POSIX specification, readdir() returns a nullptr and sets the error according to errno, if the end of
/// the directory has been reached. In order for us to be sure, we have to set a specific errno first, before doing any
/// OS operation.
constexpr auto END_OF_DIRECTORY = 0;
}  // namespace

DirectoryIterator::DirectoryIterator(const DirectoryIterator&) noexcept = default;
DirectoryIterator::DirectoryIterator(DirectoryIterator&&) noexcept = default;
// Different LCOV behavior: covered on Linux but not on QNX8, excluded from coverage for QNX8
DirectoryIterator& DirectoryIterator::operator=(const DirectoryIterator&) noexcept = default;  // LCOV_EXCL_LINE
DirectoryIterator& DirectoryIterator::operator=(DirectoryIterator&&) noexcept = default;
DirectoryIterator::~DirectoryIterator() noexcept = default;

DirectoryIterator::Directory::Directory(const Path& path) noexcept : path_{path}
{
    // The reason for banning is, because it's error-prone to use. One should use abstractions e.g. provided by
    // the C++ standard library. Since this library exactly is such abstraction, we can use the OS function.
    // NOLINTNEXTLINE(score-banned-function): See above
    posix_directory_ = os::Dirent::instance().opendir(path.CStr());
}

DirectoryIterator::Directory::~Directory() noexcept
{
    if (posix_directory_.has_value())
    {
        // The reason for banning is, because it's error-prone to use. One should use abstractions e.g. provided by
        // the C++ standard library. Since this library exactly is such abstraction, we can use the OS function.
        // NOLINTNEXTLINE(score-banned-function): See above
        score::cpp::ignore = os::Dirent::instance().closedir(posix_directory_.value());
    }
}

DirectoryIterator::Directory& DirectoryIterator::Directory::operator++()
{
    bool do_one_more_iteration{false};  // do-while loop helps avoid recursion (Ticket-48731)
    do
    {
        do_one_more_iteration = false;
        if (posix_directory_.has_value())
        {
            // LCOV_EXCL_BR_START no obvious branches here to cover by unit test
            // seterrno - exceptions?
            // posix_directory_.value() should return value since posix_directory_.has_value() == true
            // coverity[autosar_cpp14_m0_3_2_violation]: indirectly validated through os::Dirent
            score::os::seterrno(END_OF_DIRECTORY);
            // The reason for banning is, because it's error-prone to use. One should use abstractions e.g. provided by
            // the C++ standard library. Since this library exactly is such abstraction, we can use the OS function.
            // NOLINTNEXTLINE(score-banned-function): See above
            const auto directory_entry = os::Dirent::instance().readdir(posix_directory_.value());
            // LCOV_EXCL_BR_STOP
            if (directory_entry.has_value())
            {
                // LCOV_EXCL_BR_START no obvious branches here to cover by unit test
                const auto& d_name = directory_entry.value()->d_name;
                // LCOV_EXCL_BR_STOP
                // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay) valid usage
                const bool is_dot = (d_name == std::string{"."});
                // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay) valid usage
                const bool is_dot_dot = (d_name == std::string{".."});
                if (is_dot || is_dot_dot)
                {
                    do_one_more_iteration = true;
                }
                else
                {
                    current_entry_ = DirectoryEntry{path_ / d_name};
                }
            }
            else
            {
                // LCOV_EXCL_BR_START no obvious branches here to cover by unit test
                // directory_entry.has_value() == false, directory_entry.error() should return error
                const auto directory_entry_error = directory_entry.error();
                // LCOV_EXCL_BR_STOP
                if (directory_entry_error == score::os::Error::createFromErrno(END_OF_DIRECTORY))
                {
                    current_entry_ = DirectoryEntry{};
                }
                else
                {
                    SetError(directory_entry_error);
                }
            }
        }
        else
        {
            SetError(posix_directory_.error());
        }
    } while (do_one_more_iteration);
    return *this;
}

const DirectoryEntry& DirectoryIterator::Directory::CurrentEntry() const noexcept
{
    return current_entry_;
}

bool DirectoryIterator::Directory::HasError() const noexcept
{
    return error_.has_value();
}

// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate() function shall
// not be called implicitly". The API value() must be compatible with the standard hence cannot be not noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
score::os::Error DirectoryIterator::Directory::GetError() const noexcept
{
    return error_.value();
}

void DirectoryIterator::Directory::SetError(score::os::Error error) noexcept
{
    score::cpp::ignore = error_.emplace(error);
    current_entry_ = DirectoryEntry{};
}

DirectoryIterator::DirectoryIterator() noexcept = default;

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
DirectoryIterator::DirectoryIterator(const Path& path, const DirectoryOptions directory_options) noexcept
    : directory_{std::make_shared<DirectoryIterator::Directory>(path)}
{
    operator++();
    score::cpp::ignore = directory_options;
}

DirectoryIterator::reference DirectoryIterator::operator*() const noexcept
{
    // LCOV_EXCL_BR_START no obvious branches here to cover by unit test
    // can't be compile time inited
    // coverity[autosar_cpp14_a3_3_2_violation]
    static const DirectoryIterator::value_type empty_entry{};
    // LCOV_EXCL_BR_STOP
    return directory_.get() != nullptr ? directory_->CurrentEntry() : empty_entry;
}

DirectoryIterator::pointer DirectoryIterator::operator->() const noexcept
{
    return &operator*();
}

// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate() function shall
// not be called implicitly". Since directory_.get() is checked before calling ++(*directory_)
// This is false positive.
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
DirectoryIterator& DirectoryIterator::operator++() noexcept
{
    if (directory_.get() != nullptr)
    {
        ++(*directory_);
    }
    return *this;
}

bool DirectoryIterator::HasValue() const noexcept
{
    return directory_.get() != nullptr ? (!directory_->HasError()) : true;
}

score::os::Error DirectoryIterator::Error() const noexcept
{
    return directory_.get() != nullptr ? directory_->GetError() : score::os::Error::createFromErrno(END_OF_DIRECTORY);
}

bool operator==(const DirectoryIterator& l, const DirectoryIterator& r) noexcept
{
    return *l == *r;
}

bool operator!=(const DirectoryIterator& l, const DirectoryIterator& r) noexcept
{
    return !(l == r);
}

DirectoryIterator begin(const DirectoryIterator& iterator) noexcept
{
    return iterator;
}

DirectoryIterator end(const DirectoryIterator& iterator) noexcept
{
    score::cpp::ignore = iterator;
    return DirectoryIterator{};
}

}  // namespace filesystem
}  // namespace score
