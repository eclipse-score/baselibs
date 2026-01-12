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
#ifndef SCORE_LIB_FILESYSTEM_ITERATOR_RECURSIVE_DIRECTORY_ITERATOR_H
#define SCORE_LIB_FILESYSTEM_ITERATOR_RECURSIVE_DIRECTORY_ITERATOR_H

#include "score/filesystem/iterator/directory_entry.h"
#include "score/filesystem/iterator/directory_iterator.h"
#include "score/os/errno.h"

#include <optional>
#include <stack>

namespace score
{
namespace filesystem
{

/// @brief RecursiveDirectoryIterator is a LegacyInputIterator that iterates over the DirectoryEntry elements of a
/// directory, and, recursively, over the entries of all subdirectories. The iteration order is unspecified, except that
/// each directory entry is visited only once.
///
/// Symlinks are not followed.
///
/// The special pathnames dot and dot-dot are skipped.
///
/// If the RecursiveDirectoryIterator reports an error or is advanced past the last directory entry of the top-level
/// directory, it becomes equal to the default-constructed iterator, also known as the end iterator. Two end iterators
/// are always equal, dereferencing or incrementing the end iterator is undefined behavior.
///
/// If a file or a directory is deleted or added to the directory tree after the recursive directory iterator has been
/// created, it is unspecified whether the change would be observed through the iterator.
///
/// If the directory structure contains cycles, the end iterator may be unreachable.
class RecursiveDirectoryIterator final  // LCOV_EXCL_BR_LINE false positive
{
  public:
    using value_type = DirectoryEntry;
    using difference_type = std::ptrdiff_t;
    using pointer = const DirectoryEntry*;
    using reference = const DirectoryEntry&;
    using iterator_category = std::input_iterator_tag;

    /// @brief Constructs the end iterator.
    RecursiveDirectoryIterator() noexcept;

    /// @brief Constructs a directory iterator that refers to the first directory entry of a directory identified
    explicit RecursiveDirectoryIterator(const Path&,
                                        const DirectoryOptions directory_options = DirectoryOptions::kNone) noexcept;
    RecursiveDirectoryIterator(const RecursiveDirectoryIterator&) noexcept;
    RecursiveDirectoryIterator(RecursiveDirectoryIterator&&) noexcept;
    RecursiveDirectoryIterator& operator=(const RecursiveDirectoryIterator&) noexcept;
    RecursiveDirectoryIterator& operator=(RecursiveDirectoryIterator&&) noexcept;
    ~RecursiveDirectoryIterator() noexcept;

    /// @brief Accesses the pointed-to DirectoryEntry
    reference operator*() const noexcept;

    /// @brief Accesses the pointed-to DirectoryEntry
    pointer operator->() const noexcept;

    /// @brief Advances the iterator to the next entry. Invalidates all copies of the previous value of *this.
    RecursiveDirectoryIterator& operator++() noexcept;

    /// @brief Checks whether an error occurred while iterating through a directory
    /// @note This is an extension to the C++ std::filesystem version, needed from proper error reporting.
    bool HasValue() const noexcept;

    /// @brief Returns the error that occurred while iterating through a directory
    /// @note This is an extension to the C++ std::filesystem version, needed from proper error reporting.
    score::os::Error Error() const noexcept;

    friend bool operator==(const RecursiveDirectoryIterator& l, const RecursiveDirectoryIterator& r) noexcept;
    friend bool operator!=(const RecursiveDirectoryIterator& l, const RecursiveDirectoryIterator& r) noexcept;

  private:
    /// @brief Helper method for operator++(), handles folders
    /// @param file_type File type
    /// @return True if folder logic is handled and no needed any further op. Otherwise False
    bool OperatorIncFolderHelper(const FileType file_type) noexcept;

    std::shared_ptr<std::stack<DirectoryIterator>> folders_{std::make_shared<std::stack<DirectoryIterator>>()};
    std::shared_ptr<std::optional<score::os::Error>> error_{std::make_shared<std::optional<score::os::Error>>()};
    DirectoryOptions directory_options_{};
    bool follow_directory_symlink_{};
};

/// @brief Enable the use of DirectoryIterator with range-based for loops
RecursiveDirectoryIterator begin(const RecursiveDirectoryIterator&) noexcept;

/// @brief Enable the use of DirectoryIterator with range-based for loops
RecursiveDirectoryIterator end(const RecursiveDirectoryIterator&) noexcept;

bool operator==(const RecursiveDirectoryIterator& l, const RecursiveDirectoryIterator& r) noexcept;
bool operator!=(const RecursiveDirectoryIterator& l, const RecursiveDirectoryIterator& r) noexcept;

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_ITERATOR_RECURSIVE_DIRECTORY_ITERATOR_H
