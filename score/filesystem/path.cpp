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
#include "score/filesystem/path.h"

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <list>
#include <sstream>
#include <string_view>

namespace score
{
namespace filesystem
{

namespace
{
void RemovePotentialTrailingDirectorySeparator(std::list<std::string>& new_parts) noexcept
{
    if (new_parts.size() >= 2U)
    {
        auto current_part = new_parts.rbegin();
        if (current_part->empty())
        {
            ++current_part;
            // LCOV_EXCL_BR_START caused by exception, covered in Path.PathsAreLexicallyNormalized
            if ((*current_part == Path::dotdot) || (*current_part == Path::dot))
            // LCOV_EXCL_BR_STOP
            {
                new_parts.pop_back();
            }
        }
    }
}

void RemovePotentialMoveUpsAndSeparatorsAfterRoot(std::list<std::string>& new_parts) noexcept
{
    if ((!new_parts.front().empty()) && (new_parts.front().front() == Path::preferred_separator))
    {
        auto current_part = std::next(new_parts.begin());
        while (current_part != new_parts.end())
        {
            if (*current_part == Path::dotdot)
            {
                const auto previous_part = current_part;
                current_part++;
                score::cpp::ignore = new_parts.erase(previous_part);
            }
            else
            {
                break;
            }
        }
    }
}

void RemovePotentialFileNamesFollowedByMoveUpsAndSeparator(std::list<std::string>& new_parts) noexcept
{
    bool do_replace_dotdot = true;

    while (do_replace_dotdot)
    {
        do_replace_dotdot = false;
        if (new_parts.empty())
        {
            break;
        }

        auto previous_part = new_parts.begin();
        for (auto current_part = std::next(new_parts.begin()); current_part != new_parts.end(); ++current_part)
        {
            if (((*current_part == Path::dotdot) && (*previous_part != Path::dotdot)) &&
                (previous_part->front() != Path::preferred_separator))
            {
                score::cpp::ignore = new_parts.erase(current_part);
                score::cpp::ignore = new_parts.erase(previous_part);
                do_replace_dotdot = true;
                break;
            }
            previous_part = current_part;
        }
    }
}

std::string CreatePathStringFromParts(std::list<std::string>& new_parts) noexcept
{
    std::string normalized_path{};
    bool is_separator_required = false;
    for (auto current_part = new_parts.begin(); current_part != new_parts.end(); ++current_part)
    {
        if (is_separator_required)
        {
            normalized_path += std::string{Path::preferred_separator};
        }
        normalized_path += *current_part;
        if ((!current_part->empty()) && (current_part->front() != Path::preferred_separator))
        {
            is_separator_required = true;
        }
    }
    return normalized_path;
}

}  // namespace

Path::Path() noexcept = default;
// 1. Path::Parse method is called conditionally in Path constructor.
//    In Path::Parse itself all Path instances created with (parse == false) => no recursion.
// 2. This constructor should be = default but gcc 9 and lower version has a bug that if you default it
//    considers the constructor as deleted because one of the members is not noexcept
//    For more information see:
//    broken_link_j/Ticket-148878?focusedId=16079234&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-16079234
// NOLINTNEXTLINE(misc-no-recursion,modernize-use-equals-default): See above
Path::Path(const Path& p) noexcept : native_path_{p.native_path_}, parts_{p.parts_} {}
Path::Path(Path&& p) noexcept : native_path_{std::move(p.native_path_)}, parts_{std::move(p.parts_)} {}

// Path::Parse method is called conditionally in Path constructor.
// In Path::Parse itself all Path instances created with (parse == false) => no recursion.
// coverity[autosar_cpp14_a6_2_1_violation] false-positive; can't be replaced with =default because of the self guard
Path& Path::operator=(const Path& p) noexcept  // NOLINT(misc-no-recursion): See above coverity
{
    if (this == &p)
    {
        return *this;
    }
    native_path_ = p.native_path_;
    parts_ = p.parts_;  // LCOV_EXCL_BR_LINE caused by exception
    return *this;
}

// coverity[autosar_cpp14_a6_2_1_violation] false-positive; can't be replaced with =default because of the self guard
Path& Path::operator=(Path&& p) noexcept
{
    if (this == &p)
    {
        return *this;
    }
    native_path_ = std::move(p.native_path_);
    parts_ = std::move(p.parts_);
    return *this;
}

Path::~Path() noexcept = default;

Path::Path(const string_type& user_path, const Format format) noexcept : Path{string_type{user_path}, true}
{
    score::cpp::ignore = format;
}
Path::Path(string_type&& user_path, const Format format) noexcept : Path{std::move(user_path), true}
{
    score::cpp::ignore = format;
}

// Path::Parse method is called conditionally in Path constructor.
// In Path::Parse itself all Path instances created with (parse == false) => no recursion.
// NOLINTNEXTLINE(misc-no-recursion): See above
Path::Path(Path::string_type&& path, const bool do_parsing) noexcept : native_path_{std::move(path)}, parts_{}
{
    if (do_parsing)
    {
        Parse(native_path_);
    }
}

Path& Path::operator/=(const Path& to_append) noexcept
{
    // If the user tries to append an absolute path, overwrite the current path.
    // As defined by the standard: https://en.cppreference.com/w/cpp/filesystem/path/append
    // LCOV_EXCL_BR_START caused by exception + RootName() is always empty
    if ((Empty() || to_append.IsAbsolute()) ||
        ((to_append.HasRootName() && (to_append.RootName().Native() != RootName().Native()))))
    // LCOV_EXCL_BR_STOP
    {
        native_path_ = to_append.native_path_;
    }
    else
    {
        if (native_path_.back() != '/') /* KW_SUPPRESS:MISRA.CHAR.NOT_CHARACTER: Is a character */
        {
            score::cpp::ignore = native_path_.append(std::string{preferred_separator});
        }
        score::cpp::ignore = native_path_.append(to_append.native_path_);
    }
    Parse(native_path_);
    return *this;
}

const Path::value_type* Path::CStr() const noexcept
{
    return native_path_.c_str();
}

const Path::string_type& Path::Native() const noexcept
{
    return native_path_;
}

// In accordance with the C++ Standard
// coverity[autosar_cpp14_a13_5_2_violation]
Path::operator string_type() const noexcept
{
    return native_path_;
}

Path Path::LexicallyNormal() const noexcept
{
    constexpr std::string_view dot_string_view = ".";

    // Here we implement the normalization algorithm specified in: https://en.cppreference.com/w/cpp/filesystem/path
    // 1. If the path is empty, stop (normal form of an empty path is an empty path)
    if (parts_.empty())  // means no directory-separators
    {
        return native_path_;
    }

    // 2. Replace each directory-separator (which may consist of multiple slashes) with a single
    // path::preferred_separator.
    // Comment: Nothing to do. Directory-separator == path::preferred_separator == '/'.

    // 3. Replace each slash character in the root-name with path::preferred_separator.
    // Comment: N/A for POSIX

    // 4. Remove each dot and any immediately following directory-separator.
    std::list<std::string> new_parts;
    score::cpp::ignore = std::copy_if(
        parts_.cbegin(), parts_.cend(), std::back_inserter(new_parts), [&dot_string_view](const auto& part) {
            return part.Native() != dot_string_view;
        });
    if (new_parts.empty())
    {
        return ".";
    }

    // 5. Remove each non-dot-dot filename immediately followed by a directory-separator and a dot-dot, along with any
    // immediately following directory-separator.
    RemovePotentialFileNamesFollowedByMoveUpsAndSeparator(new_parts);
    if (new_parts.empty())
    {
        return ".";
    }

    // 6. If there is root-directory, remove all dot-dots and any directory-separators immediately following them.
    RemovePotentialMoveUpsAndSeparatorsAfterRoot(new_parts);

    // 7. If the last filename is dot-dot, remove any trailing directory-separator.
    RemovePotentialTrailingDirectorySeparator(new_parts);

    // 8. If the path is empty, add a dot (normal form of ./ is .)
    // Comment: Already done in previous steps.

    // Create part from parts
    std::string normalized_path = CreatePathStringFromParts(new_parts);

    if (normalized_path.empty())
    {
        return ".";
    }

    AddPreferredSeperatorIfNeeded(new_parts, normalized_path);

    return normalized_path;
}

void Path::AddPreferredSeperatorIfNeeded(std::list<std::string>& new_parts, std::string& normalized_path) const noexcept
{
    const bool last_was_dot = ((parts_.back().Native() == dot) || (parts_.back().Native() == dotdot));
    // LCOV_EXCL_BR_START caused by exception, covered in Path.PathsAreLexicallyNormalized
    if ((last_was_dot && (new_parts.back() != dotdot)) &&
        ((new_parts.size() != 1U) || (native_path_.front() != preferred_separator)))
    // LCOV_EXCL_BR_STOP
    {
        normalized_path += preferred_separator;
    }
}

Path Path::RootName() const noexcept
{
    // In POSIX, we don't have a root-name
    return Path{};
}

Path Path::RootDirectory() const noexcept
{
    // In POSIX, the root directory is always `/` if it is absolute
    if (IsAbsolute())
    {
        return Path{'/'};
    }
    return Path{};
}

Path Path::RootPath() const noexcept
{
    // In POSIX, the root-path equals the root directory.
    return RootDirectory();
}

Path Path::RelativePath() const noexcept
{
    if (IsAbsolute())
    {
        string_type relative_path = Native();
        score::cpp::ignore = relative_path.erase(0U, RootPath().Native().size());
        return relative_path;
    }

    return *this;
}

Path Path::ParentPath() const noexcept
{
    if (Empty())
    {
        return Path{};
    }
    string_type parent_path = Native();
    if (parent_path.find_first_not_of(preferred_separator) == string_type::npos)
    {
        return *this;
    }
    auto position_of_last_path_separator = parent_path.find_last_of(preferred_separator);
    if (position_of_last_path_separator == 0U)
    {
        return Path{preferred_separator};
    }
    if (position_of_last_path_separator == string_type::npos)
    {
        return Path{};
    }
    while (parent_path[position_of_last_path_separator - 1U] == preferred_separator)
    {
        --position_of_last_path_separator;
    }
    score::cpp::ignore = parent_path.erase(position_of_last_path_separator, string_type::npos);
    return parent_path;
}

Path Path::Filename() const noexcept
{
    const auto position_of_filename = FilenamePosition();
    if (position_of_filename == string_type::npos)
    {
        return Path{};
    }
    if (position_of_filename == 0U)
    {
        return Native();
    }
    string_type filename = Native();
    score::cpp::ignore = filename.erase(0U, position_of_filename);
    return filename;
}

std::size_t Path::FilenamePosition() const noexcept
{
    if (Empty())
    {
        return string_type::npos;
    }
    const std::size_t position_of_last_separator = Native().find_last_of(preferred_separator);
    std::size_t position_of_filename = 0U;
    if (position_of_last_separator != string_type::npos)
    {
        position_of_filename = position_of_last_separator + 1U;
    }
    if (position_of_filename == Native().size())
    {
        position_of_filename = string_type::npos;
    }
    return position_of_filename;
}

std::size_t Path::ExtensionPosition(const std::size_t position_of_filename) const noexcept
{
    if (position_of_filename == string_type::npos)
    {
        return string_type::npos;
    }
    const string_type filename = Native().substr(position_of_filename);
    if ((filename == ".") || (filename == ".."))
    {
        return string_type::npos;
    }
    std::size_t position_of_extension_separator = filename.substr(1U).find_last_of('.');
    if (position_of_extension_separator == string_type::npos)
    {
        return string_type::npos;
    }
    position_of_extension_separator += position_of_filename + 1U;
    return position_of_extension_separator;
}

std::size_t Path::ExtensionPosition() const noexcept
{
    return ExtensionPosition(FilenamePosition());
}

Path Path::Extension() const noexcept
{
    const std::size_t position_of_extension_separator = ExtensionPosition();
    if (position_of_extension_separator == string_type::npos)
    {
        return Path{};
    }
    return Native().substr(position_of_extension_separator);
}

Path Path::Stem() const noexcept
{
    const std::size_t position_of_filename = FilenamePosition();

    if (position_of_filename == string_type::npos)
    {
        return {};
    }

    const std::size_t position_of_extension_separator = ExtensionPosition(position_of_filename);
    if (position_of_extension_separator == string_type::npos)
    {
        return Native().substr(position_of_filename);
    }
    return Native().substr(position_of_filename, position_of_extension_separator - position_of_filename);
}

Path& Path::ReplaceExtension(const Path& replacement) noexcept
{
    const std::size_t position_of_extension_separator = ExtensionPosition();
    if (position_of_extension_separator != string_type::npos)
    {
        score::cpp::ignore = native_path_.erase(position_of_extension_separator);
    }

    string_type extension = replacement.Native();
    if ((!extension.empty()) && (extension.at(0U) != '.'))
    {
        native_path_ += ".";
    }

    native_path_ += extension;

    Parse(native_path_);

    return *this;
}

Path& Path::RemoveFilename() noexcept
{
    const std::size_t filename_position = FilenamePosition();
    if (filename_position != string_type::npos)
    {
        score::cpp::ignore = native_path_.erase(filename_position);
        Parse(native_path_);
    }
    return *this;
}

bool Path::Empty() const noexcept
{
    return native_path_.empty();
}

bool Path::HasRootPath() const noexcept
{
    return !RootPath().Empty();
}
bool Path::HasRootName() const noexcept
{
    return !RootName().Empty();
}
bool Path::HasRootDirectory() const noexcept
{
    return !RootDirectory().Empty();
}
bool Path::HasRelativePath() const noexcept
{
    return Native() != RootPath().Native();
}
bool Path::HasParentPath() const noexcept
{
    return !ParentPath().Empty();
}

bool Path::HasFilename() const noexcept
{
    return !Filename().Empty();
}

bool Path::HasExtension() const noexcept
{
    return !Extension().Empty();
}

bool Path::IsAbsolute() const noexcept
{
    if (!native_path_.empty())
    {
        return native_path_.front() == preferred_separator;
    }

    return false;
}

bool Path::IsRelative() const noexcept
{
    return !IsAbsolute();
}

bool operator==(const Path& lhs, const Path& rhs) noexcept
{
    return lhs.Native() == rhs.Native();
}

bool operator!=(const Path& lhs, const Path& rhs) noexcept
{
    return lhs.Native() != rhs.Native();
}

Path operator/(const Path& lhs, const Path& rhs) noexcept
{
    Path appended = lhs;
    appended /= rhs;
    return appended;
}

bool operator<(const Path& lhs, const Path& rhs) noexcept
{
    return lhs.Native() < rhs.Native();
}

// Path::Parse method is called conditionally in Path constructor.
// In Path::Parse itself all Path instances created with (parse == false) => no recursion.
// NOLINTNEXTLINE(misc-no-recursion): See above
void Path::Parse(const string_type& path) noexcept
{
    parts_.clear();
    if (path.empty())
    {
        return;
    }

    const auto starts_with_separator = (path.front() == preferred_separator);
    if (starts_with_separator)
    {
        score::cpp::ignore = parts_.emplace_back(Path{string_type{preferred_separator}, false});
    }

    std::stringstream stringstream_path{path};
    std::string path_part{};
    AddPathPartToParts(stringstream_path, path_part);
    // At this point the parts_ array should contain the parts of the path separated by the preferred_separator,
    // or should contain at least by the preferred_separator itself.
    // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(parts_.size() != 0U, "At this point the parts_ array should contain at least one element.");
    // LCOV_EXCL_BR_STOP

    const auto is_multipart = (parts_.size() > 1U);
    const auto ends_with_separator = (path.back() == preferred_separator);
    // LCOV_EXCL_BR_START Tooling issue. Line coverage proofs that both branches and therefore all decisions are covered
    if (ends_with_separator && (is_multipart || !starts_with_separator))
    // LCOV_EXCL_BR_STOP
    {
        score::cpp::ignore = parts_.emplace_back(Path{"", false});
    }
    if (parts_.size() == 1U)
    {
        parts_.clear();
    }
}

// Path::AddPathPartToParts method is called conditionally in Path constructor.
// In Path::AddPathPartToParts itself all Path instances created with (parse == false) => no recursion.
// NOLINTNEXTLINE(misc-no-recursion) See above
void Path::AddPathPartToParts(std::stringstream& stringstream_path, std::string& path_part) noexcept
{
    while (!std::getline(stringstream_path, path_part, preferred_separator).fail())
    {
        if (!path_part.empty())
        {
            score::cpp::ignore = parts_.emplace_back(Path{std::move(path_part), false});
        }
    }
}

Path::iterator Path::begin() const noexcept
{
    if (!parts_.empty())
    {
        return iterator{*this, parts_.begin()};
    }
    if (native_path_.empty())
    {
        return iterator{};
    }
    return iterator{*this, false};
}

Path::iterator Path::end() const noexcept
{
    if (!parts_.empty())
    {
        return iterator{*this, parts_.end()};
    }
    if (native_path_.empty())
    {
        return iterator{};
    }
    return iterator{*this, true};
}

// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate() function shall
// not be called implicitly". Since path_.has_value() is checked before calling path_.value(),
// std::bad_optional_access should never be thrown. This is false positive.
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
bool Path::iterator::equals(const iterator& r) const noexcept
{
    if (path_.has_value() != r.path_.has_value())
    {
        return false;
    }
    if (!path_.has_value())
    {
        return true;
    }
    if ((&path_.value().get()) != (&r.path_.value().get()))
    {
        return false;
    }
    if (path_.value().get().parts_.empty())
    {
        return is_at_end_ == r.is_at_end_;
    }
    return cur_ == r.cur_;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Path::iterator& Path::iterator::operator++() noexcept
{
    // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
        path_.has_value(),
        "The path should contain value when incrementing an iterator. Probably the iterator is not inizialized.");
    // LCOV_EXCL_BR_STOP
    if (path_.value().get().parts_.empty())
    {
        // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
            !is_at_end_,
            "The increment cannot be applied because the iterator already points to the end (path with "
            "one part only).");
        // LCOV_EXCL_BR_STOP
        is_at_end_ = true;
    }
    else
    {
        // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
            cur_ != path_.value().get().parts_.end(),
            "The increment cannot be applied because the iterator already points to the end (multiple parts path).");
        // LCOV_EXCL_BR_STOP
        ++cur_;
    }
    return *this;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Path::iterator& Path::iterator::operator--() noexcept
{
    // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
        path_.has_value(),
        "The path should contain value when decrementing an iterator. Probably the iterator is not inizialized.");
    // LCOV_EXCL_BR_STOP
    if (path_.value().get().parts_.empty())
    {
        // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
            is_at_end_,
            "The decrement cannot be applied because the iterator already points to the first element "
            "(path with one part only).");
        // LCOV_EXCL_BR_STOP
        is_at_end_ = false;
    }
    else
    {
        // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
            cur_ != path_.value().get().parts_.begin(),
            "The decrement cannot be applied because the iterator already points to the first element "
            "(multiple parts path).");
        // LCOV_EXCL_BR_STOP
        --cur_;
    }
    return *this;
}

Path::iterator Path::iterator::operator++(int) noexcept
{
    const auto tmp = *this;
    ++*this;
    return tmp;
}

Path::iterator Path::iterator::operator--(int) noexcept
{
    const auto tmp = *this;
    --*this;
    return tmp;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Path::iterator::reference Path::iterator::operator*() const noexcept
{
    // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
        path_.has_value(),
        "The path should contain value when dereferencing an iterator. Probably the iterator is not inizialized.");
    // LCOV_EXCL_BR_STOP
    if (path_.value().get().parts_.empty())
    {
        // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(!is_at_end_, "The end()-iterator should not be dereferenced (path with one part only).");
        return *path_;
        // LCOV_EXCL_BR_STOP
    }
    // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(cur_ != path_.value().get().parts_.end(),
                           "The end()-iterator should not be dereferenced (multiple parts path).");
    // LCOV_EXCL_BR_STOP
    return *cur_;
}

bool operator==(const Path::iterator& l, const Path::iterator& r) noexcept
{
    return l.equals(r);
}

bool operator!=(const Path::iterator& l, const Path::iterator& r) noexcept
{
    return !l.equals(r);
}

// This constructor should be = default but gcc 9 and lower version has a bug that if you default it
// considers the constructor as deleted because one of the members is not noexcept
// For more information see:
// broken_link_j/Ticket-148878?focusedId=16079234&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-16079234
// NOLINTBEGIN(modernize-use-equals-default): See above
// Suppress "AUTOSAR C++14 A12-7-1", The rule states: "If the behavior of a user-defined special member function
// is identical to implicitly defined special member function, then it shall be defined "=default" or be left
// undefined". See above for the suppression justification.
// coverity[autosar_cpp14_a12_7_1_violation]
Path::iterator::iterator(const Path::iterator& it) noexcept : path_{it.path_}, cur_{it.cur_}, is_at_end_{it.is_at_end_}
// NOLINTEND(modernize-use-equals-default): See above
{
}

Path::iterator::pointer Path::iterator::operator->() const noexcept
{
    return &operator*();
}

Path::iterator::iterator(const Path& path, const std::vector<Path>::const_iterator cur) noexcept
    : path_{path}, cur_{cur}, is_at_end_{}
{
}

// coverity[autosar_cpp14_a6_2_1_violation] false-positive data members are not changed;
Path::iterator& Path::iterator::operator=(const Path::iterator& it) noexcept
{
    if (this == &it)
    {
        return *this;
    }
    path_ = it.path_;
    cur_ = it.cur_;
    is_at_end_ = it.is_at_end_;
    return *this;
}

// The & ref-qualifier ensures that the move assignment operator can only be called on actual objects (lvalues), not on
// temporary objects (rvalues), which makes the code safer and more predictable.
Path::iterator& Path::iterator::operator=(Path::iterator&& it) & noexcept
{
    if (this == &it)
    {
        return *this;
    }
    path_ = std::move(it.path_);
    cur_ = std::move(it.cur_);
    is_at_end_ = std::move(it.is_at_end_);
    return *this;
}

Path::iterator::iterator(const Path& path, const bool is_at_end) noexcept : path_{path}, cur_{}, is_at_end_{is_at_end}
{
}

}  // namespace filesystem
}  // namespace score
