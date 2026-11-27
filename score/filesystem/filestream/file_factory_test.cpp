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
#include "score/filesystem/filestream/file_factory.h"
#include "score/filesystem/factory/filesystem_factory.h"
#include "score/filesystem/file_utils/file_test_utils.h"
#include "score/filesystem/iterator/directory_iterator.h"
#include "score/os/mocklib/fcntl_mock.h"
#include "score/os/mocklib/stat_mock.h"
#include "score/os/mocklib/unistdmock.h"
#include "score/os/stat.h"

#include <fcntl.h>
#include <gtest/gtest.h>

#include <ftw.h>
#include <sys/stat.h>

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace score::filesystem
{
namespace
{

using namespace ::testing;

class FileFactoryTest : public ::testing::Test
{
  public:
    FileFactoryTest() : unit_{}, test_tmpdir_{}, remove_temp_{false}
    {
        InitializeTempDir();
    }

    ~FileFactoryTest() override
    {
        RemoveTempContent();
    }

  protected:
    void InitializeTempDir()
    {
        SCOPED_TRACE("Setup of temporary directory");
        auto temp_dir_result = FileTestUtils::GetTempDirectory();
        ASSERT_TRUE(temp_dir_result.has_value());
        test_tmpdir_ = temp_dir_result.value();
    }

    static int RemoveDirentry(const char* fname, const struct stat*, int, struct FTW*)
    {
        std::remove(fname);
        return 0;
    }

    void RemoveTempContent() const
    {
        if (remove_temp_)
        {
            ::nftw(test_tmpdir_.Native().c_str(), RemoveDirentry, 64, FTW_DEPTH);
        }
    }

    FileFactory unit_;
    Path test_tmpdir_;
    bool remove_temp_;
};

class FileFactoryTestWithFcntlMock : public FileFactoryTest
{
  protected:
    os::MockGuard<os::FcntlMock> fcntl_;
};

class FileFactoryTestWithStatMock : public FileFactoryTest
{
  protected:
    os::MockGuard<os::StatMock> stat_;
    os::MockGuard<os::UnistdMock> unistd_;
};

TEST_F(FileFactoryTest, OpensFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description", "Check that opening a file for output succeeds and allows writing data.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto file = unit_.Open("/tmp/some_file", std::ios_base::out);

    ASSERT_TRUE(file.has_value());

    *file.value() << "Foo";

    ::unlink("/tmp/some_file");
}

TEST_F(FileFactoryTest, OpensFileForAppending)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description", "Check that opening a file in append mode succeeds and allows writing data.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto file = unit_.Open("/tmp/some_file", std::ios_base::out | std::ios_base::app);

    ASSERT_TRUE(file.has_value());

    *file.value() << "Foo";

    ::unlink("/tmp/some_file");
}

TEST_F(FileFactoryTest, OpensNonExistingFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description", "Check that opening a missing file for input returns an open-file-stream error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto file = unit_.Open("/tmp/some_file_not_existing", std::ios_base::in);

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTest, OpenForAtomicUpdate)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic update replaces target file content after the update stream closes.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;

    // Given a file with some random content
    auto file_result = unit_.Open(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    std::unique_ptr<std::iostream> file = *std::move(file_result);
    (*file) << "Test";
    file.reset();

    // When updating the file with the atomic update
    file_result = unit_.AtomicUpdate(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    (*file) << "Narf";
    file.reset();

    // Then the file contains the content that just had been written
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    std::string text;
    (*file) >> text;
    EXPECT_EQ(text, "Narf");
}

TEST_F(FileFactoryTest, AtomicUpdateInvalidMode)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description", "Check that atomic update with input-only mode is rejected as not implemented.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;
    auto file_result = unit_.AtomicUpdate(test_filename, std::ios::in);

    ASSERT_FALSE(file_result.has_value());
    EXPECT_EQ(file_result.error(), filesystem::ErrorCode::kNotImplemented);
}

TEST_F(FileFactoryTest, OldContentVisibleBeforeAtomicObjectGetsDeleted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that the old file content stays visible until the atomic-update stream is released, then "
                   "changes to the new content.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static constexpr auto kTestFileName = "keep_content_before_drop";
    Path test_filename = test_tmpdir_ / kTestFileName;

    // Given a file with some random content
    auto file_result = unit_.Open(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file_result.has_value());
    std::unique_ptr<std::iostream> file = *std::move(file_result);
    (*file) << "Test";
    file.reset();

    // When updating the file without dropping the object
    auto update_result = unit_.AtomicUpdate(test_filename, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(update_result.has_value());
    auto update_file = *std::move(update_result);
    (*update_file) << "Narf";

    // Then the file should still contain the previous content

    // Now read from the "old" file by regularly opening it
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    std::string text;
    (*file) >> text;
    EXPECT_EQ(text, "Test");
    text.clear();
    file.reset();

    // When finally dropping the atomic update file
    update_file.reset();

    // Then the file content shall change
    file_result = unit_.Open(test_filename, std::ios::in);
    ASSERT_TRUE(file_result.has_value());
    file = *std::move(file_result);
    (*file) >> text;
    EXPECT_EQ(text, "Narf");
}

class FileFactoryFailedAtomicUpdateTest : public FileFactoryTest
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(filesystem_.standard->CreateDirectory(directory_).has_value());
        auto file = unit_.Open(target_, std::ios::out | std::ios::trunc);
        ASSERT_TRUE(file.has_value());
        (**file) << kOldContent;
    }

    void TearDown() override
    {
        EXPECT_TRUE(filesystem_.standard->RemoveAll(directory_).has_value());
    }

    std::string ReadTarget()
    {
        auto file = unit_.Open(target_, std::ios::in);
        std::string text;
        if (file.has_value())
        {
            (**file) >> text;
        }
        return text;
    }

    std::ptrdiff_t CountDirectoryEntries() const
    {
        return std::distance(DirectoryIterator{directory_}, DirectoryIterator{});
    }

    static constexpr std::string_view kOldContent{"Test"};
    Filesystem filesystem_{FilesystemFactory{}.CreateInstance()};
    Path directory_{test_tmpdir_ / "failed_atomic_update"};
    Path target_{directory_ / "target"};
};

TEST_F(FileFactoryFailedAtomicUpdateTest, CloseKeepsTargetUnchanged)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that closing an atomic update whose stream has failed returns an error, keeps the target "
                   "unchanged and removes the temporary file.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "error-guessing");

    auto update = unit_.AtomicUpdate(target_, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(update.has_value());
    (**update) << "Narf";
    (*update)->setstate(std::ios::failbit);

    auto result = (*update)->Close();

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCloseFailed);
    EXPECT_EQ(ReadTarget(), kOldContent);
    EXPECT_EQ(CountDirectoryEntries(), 1);
}

TEST_F(FileFactoryFailedAtomicUpdateTest, DestructionKeepsTargetUnchanged)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that destroying an atomic update whose stream has failed keeps the target unchanged and "
                   "removes the temporary file.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "error-guessing");

    auto update = unit_.AtomicUpdate(target_, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(update.has_value());
    (**update) << "Narf";
    (*update)->setstate(std::ios::badbit);

    update->reset();

    EXPECT_EQ(ReadTarget(), kOldContent);
    EXPECT_EQ(CountDirectoryEntries(), 1);
}

TEST_F(FileFactoryTest, ErrorOnFailingAtomicUpdate)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic update creation fails with not implemented when the target path is a "
                   "directory.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto fs = FilesystemFactory{}.CreateInstance();

    static constexpr auto kFaultyTargetFile{"file_in_disguise"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;
    ASSERT_TRUE(fs.standard->CreateDirectory(test_dir).has_value());

    auto stream = unit_.AtomicUpdate(test_dir, std::ios::out | std::ios::trunc);
    ASSERT_FALSE(stream.has_value());
    EXPECT_EQ(stream.error(), filesystem::ErrorCode::kNotImplemented);

    EXPECT_TRUE(fs.standard->RemoveAll(test_dir).has_value());
}

TEST_F(FileFactoryTest, AtomicUpdateWithEmptyPath)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description", "Check that atomic update with an empty path fails to open the file stream.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto fs = FilesystemFactory{}.CreateInstance();

    static constexpr auto kFaultyTargetFile{""};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;
    ASSERT_TRUE(fs.standard->CreateDirectory(test_dir).has_value());

    auto stream = unit_.AtomicUpdate(test_dir, std::ios::out | std::ios::trunc);
    ASSERT_FALSE(stream.has_value());
    EXPECT_EQ(stream.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithFcntlMock, OpenFileHandleFailedOnOpen)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that opening a file handle returns an open-file-stream error when os::open fails.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    constexpr os::Stat::Mode kDefaultMode = os::Stat::Mode::kReadUser | os::Stat::Mode::kWriteUser |
                                            os::Stat::Mode::kReadGroup | os::Stat::Mode::kWriteGroup |
                                            os::Stat::Mode::kReadOthers | os::Stat::Mode::kWriteOthers;

    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = details::OpenFileHandle(test_tmpdir_, std::ios_base::out, kDefaultMode);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithFcntlMock, OpenFileHandleFailedOnOpen_AppendMode)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that append-mode file-handle opening returns an open-file-stream error when os::open "
                   "fails.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    constexpr os::Stat::Mode kDefaultMode = os::Stat::Mode::kReadUser | os::Stat::Mode::kWriteUser |
                                            os::Stat::Mode::kReadGroup | os::Stat::Mode::kWriteGroup |
                                            os::Stat::Mode::kReadOthers | os::Stat::Mode::kWriteOthers;

    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = details::OpenFileHandle(test_tmpdir_, std::ios_base::app, kDefaultMode);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithFcntlMock, AtomicUpdateFileHandleFailedOnOpen)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic-update creation returns an open-file-stream error when opening the temporary "
                   "file fails.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    EXPECT_CALL(*fcntl_, open(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = unit_.AtomicUpdate(test_dir, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithStatMock, AtomicUpdateFileHandleFailedOnStat)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic-update stream creation succeeds when retrieving the target file status fails.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    EXPECT_CALL(*stat_, stat(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = unit_.AtomicUpdate(test_tmpdir_, std::ios_base::out);
    ASSERT_TRUE(result.has_value());
}

TEST_F(FileFactoryTestWithStatMock, OpenForAtomicUpdateWithOwnershipChange)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic-update creation succeeds and applies ownership when status metadata requests a "
                   "change.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static constexpr auto kTestFileName = "not_existing_yet";
    Path test_filename = test_tmpdir_ / kTestFileName;

    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG | S_IWUSR};
        buffer.st_uid = 1234;
        buffer.st_gid = 5678;
        return score::cpp::expected_blank<os::Error>{};
    };
    ON_CALL(*stat_, stat(_, _, _)).WillByDefault(Invoke(set_mode));

    // access(W_OK) confirms write permission
    EXPECT_CALL(*unistd_, access(_, os::Unistd::AccessMode::kWrite))
        .WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    // Expecting a call to chown
    EXPECT_CALL(*unistd_, chown(_, _, _)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));

    // When updating the file with the atomic update with ownership change
    const auto file_result = unit_.AtomicUpdate(test_filename, std::ios::out);
    ASSERT_TRUE(file_result.has_value());
}

TEST_F(FileFactoryTestWithStatMock, AtomicUpdateFileHandleFailedOnChown)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic-update creation fails with a permission-setting error when chown fails.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG | S_IWUSR};
        buffer.st_uid = 1234;
        buffer.st_gid = 5678;
        return score::cpp::expected_blank<os::Error>{};
    };
    EXPECT_CALL(*stat_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    EXPECT_CALL(*unistd_, access(_, os::Unistd::AccessMode::kWrite))
        .WillOnce(Return(score::cpp::expected_blank<os::Error>{}));
    EXPECT_CALL(*unistd_, chown(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    static constexpr auto kFaultyTargetFile{"path"};
    Path test_dir = test_tmpdir_ / kFaultyTargetFile;

    auto result = unit_.AtomicUpdate(test_tmpdir_, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kCouldNotSetPermissions);
}

TEST_F(FileFactoryTestWithStatMock, AtomicUpdateFailsWhenWriteAccessDenied)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic-update creation fails with write-permission-denied when write access is not "
                   "granted.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // stat returns a regular file
    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG | S_IRUSR | S_IRGRP | S_IROTH};
        buffer.st_uid = static_cast<decltype(buffer.st_uid)>(::getuid());
        buffer.st_gid = static_cast<decltype(buffer.st_gid)>(::getgid());
        return score::cpp::expected_blank<os::Error>{};
    };
    EXPECT_CALL(*stat_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    // access(W_OK) reports no write permission
    EXPECT_CALL(*unistd_, access(_, os::Unistd::AccessMode::kWrite))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    static constexpr auto kTestFileName{"readonly_file"};
    Path test_filename = test_tmpdir_ / kTestFileName;

    auto result = unit_.AtomicUpdate(test_filename, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kWritePermissionDenied);
}

TEST_F(FileFactoryTestWithStatMock, AtomicUpdateSucceedsWhenWriteAccessGranted)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic-update creation succeeds when write access is granted for a regular file.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // stat returns a regular file
    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFREG | S_IWUSR};
        buffer.st_uid = static_cast<decltype(buffer.st_uid)>(::getuid());
        buffer.st_gid = static_cast<decltype(buffer.st_gid)>(::getgid());
        return score::cpp::expected_blank<os::Error>{};
    };
    EXPECT_CALL(*stat_, stat(_, _, _)).WillOnce(Invoke(set_mode));
    // access(W_OK) confirms write permission
    EXPECT_CALL(*unistd_, access(_, os::Unistd::AccessMode::kWrite))
        .WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    static constexpr auto kTestFileName{"writable_file"};
    Path test_filename = test_tmpdir_ / kTestFileName;

    auto result = unit_.AtomicUpdate(test_filename, std::ios_base::out);
    ASSERT_TRUE(result.has_value());
}

TEST_F(FileFactoryTestWithStatMock, AtomicUpdateFailsOnNonRegularFile)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that atomic-update creation rejects non-regular targets with a not-implemented error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // stat returns a non-regular file (e.g. a directory)
    auto set_mode = [](auto, auto& buffer, auto) {
        buffer.st_mode = mode_t{S_IFDIR};
        buffer.st_uid = static_cast<decltype(buffer.st_uid)>(::getuid());
        buffer.st_gid = static_cast<decltype(buffer.st_gid)>(::getgid());
        return score::cpp::expected_blank<os::Error>{};
    };
    EXPECT_CALL(*stat_, stat(_, _, _)).WillOnce(Invoke(set_mode));

    static constexpr auto kTestFileName{"non_regular_file"};
    Path test_filename = test_tmpdir_ / kTestFileName;

    auto result = unit_.AtomicUpdate(test_filename, std::ios_base::out);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), filesystem::ErrorCode::kNotImplemented);
}

TEST(FileFactoryHelpersTest, ComposeTempFilenameUsesAllParameters)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_io");
    RecordProperty("Description",
                   "Check that temporary filename composition includes the base name, thread-ID hash, and timestamp.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const auto result = details::ComposeTempFilename("foo", 42, 32);
    EXPECT_EQ(result, ".foo-000042-00000032");
}

constexpr auto kRegularFileOpenRequirement{"comp_req__filesystem__regular_file_open"};

Path CreateRegularFile(const Path& directory, const std::string& content)
{
    const Path path{directory / "regular_file"};
    std::ofstream file{path.Native()};
    file << content;
    return path;
}

TEST_F(FileFactoryTest, OpenRegularFileReadsRegularFile)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that a regular file is opened for reading and its content can be read.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const Path path{CreateRegularFile(test_tmpdir_, "Foo")};

    auto file = unit_.OpenRegularFile(path, std::ios::in);

    ASSERT_TRUE(file.has_value());
    const std::string content{std::istreambuf_iterator<char>{*file.value()}, std::istreambuf_iterator<char>{}};
    EXPECT_EQ(content, "Foo");
    std::remove(path.CStr());
}

TEST_F(FileFactoryTest, OpenRegularFileCreatesAndWritesRegularFile)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that a new regular file is created and written when opened for writing.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const Path path{test_tmpdir_ / "new_file"};
    std::remove(path.CStr());

    auto file = unit_.OpenRegularFile(path, std::ios::out | std::ios::trunc);
    ASSERT_TRUE(file.has_value());
    *file.value() << "Foo";
    file.value().reset();

    std::ifstream written{path.Native()};
    const std::string content{std::istreambuf_iterator<char>{written}, std::istreambuf_iterator<char>{}};
    EXPECT_EQ(content, "Foo");
    std::remove(path.CStr());
}

TEST_F(FileFactoryTest, OpenRegularFileRejectsFifoForReadingWithoutBlocking)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that opening a FIFO without a writer for reading is rejected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    const Path path{test_tmpdir_ / "fifo"};
    std::remove(path.CStr());
    ASSERT_EQ(::mkfifo(path.CStr(), S_IRUSR | S_IWUSR), 0);

    const auto file = unit_.OpenRegularFile(path, std::ios::in);
    std::remove(path.CStr());

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), ErrorCode::kNotARegularFile);
}

TEST_F(FileFactoryTest, OpenRegularFileRejectsFifoForWritingWithoutBlocking)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that opening a FIFO without a reader for writing is rejected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    const Path path{test_tmpdir_ / "fifo"};
    std::remove(path.CStr());
    ASSERT_EQ(::mkfifo(path.CStr(), S_IRUSR | S_IWUSR), 0);

    const auto file = unit_.OpenRegularFile(path, std::ios::out);
    std::remove(path.CStr());

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), ErrorCode::kNotARegularFile);
}

TEST_F(FileFactoryTest, OpenRegularFileRejectsDirectory)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that a directory is rejected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto file = unit_.OpenRegularFile(test_tmpdir_, std::ios::in);

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), ErrorCode::kNotARegularFile);
}

TEST_F(FileFactoryTest, OpenRegularFileFailsForMissingFile)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that opening a missing file for reading fails.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");

    const auto file = unit_.OpenRegularFile(test_tmpdir_ / "missing_file", std::ios::in);

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(FileFactoryTestWithStatMock, OpenRegularFileClosesFileWhenStatusIsUnavailable)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that the file is closed and an error returned when fstat fails.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "error-guessing");
    const Path path{CreateRegularFile(test_tmpdir_, "Foo")};
    EXPECT_CALL(*stat_, fstat(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));
    EXPECT_CALL(*unistd_, close(_)).WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    const auto file = unit_.OpenRegularFile(path, std::ios::in);
    std::remove(path.CStr());

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), ErrorCode::kCouldNotRetrieveStatus);
}

class OpenRegularFileWithFcntlMock : public FileFactoryTestWithFcntlMock
{
  protected:
    void SetUp() override
    {
        path_ = CreateRegularFile(test_tmpdir_, "Foo");
        // NOLINTNEXTLINE(score-banned-function): The mocked open returns a real descriptor.
        file_descriptor_ = ::open(path_.CStr(), O_RDONLY);
        ASSERT_GE(file_descriptor_, 0);
        EXPECT_CALL(*fcntl_, open(_, os::Fcntl::Open::kReadOnly | os::Fcntl::Open::kNonBlocking, _))
            .WillOnce(Return(file_descriptor_));
    }

    void TearDown() override
    {
        std::remove(path_.CStr());
    }

    Path path_{};
    std::int32_t file_descriptor_{-1};
};

TEST_F(OpenRegularFileWithFcntlMock, RestoresBlockingIo)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that the non-blocking flag is cleared after the file type check.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    EXPECT_CALL(*fcntl_, fcntl(file_descriptor_, os::Fcntl::Command::kFileGetStatusFlags))
        .WillOnce(Return(score::cpp::expected<os::Fcntl::Open, os::Error>{os::Fcntl::Open::kReadOnly |
                                                                          os::Fcntl::Open::kNonBlocking}));
    EXPECT_CALL(*fcntl_, fcntl(file_descriptor_, os::Fcntl::Command::kFileSetStatusFlags, os::Fcntl::Open::kReadOnly))
        .WillOnce(Return(score::cpp::expected_blank<os::Error>{}));

    const auto file = unit_.OpenRegularFile(path_, std::ios::in);

    EXPECT_TRUE(file.has_value());
}

TEST_F(OpenRegularFileWithFcntlMock, FailsWhenStatusFlagsCannotBeRead)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that an error is returned when the file status flags cannot be read.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "error-guessing");
    EXPECT_CALL(*fcntl_, fcntl(file_descriptor_, os::Fcntl::Command::kFileGetStatusFlags))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    const auto file = unit_.OpenRegularFile(path_, std::ios::in);

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), ErrorCode::kCouldNotOpenFileStream);
}

TEST_F(OpenRegularFileWithFcntlMock, FailsWhenBlockingIoCannotBeRestored)
{
    RecordProperty("PartiallyVerifies", kRegularFileOpenRequirement);
    RecordProperty("Description", "Check that an error is returned when the non-blocking flag cannot be cleared.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "error-guessing");
    EXPECT_CALL(*fcntl_, fcntl(file_descriptor_, os::Fcntl::Command::kFileGetStatusFlags))
        .WillOnce(Return(score::cpp::expected<os::Fcntl::Open, os::Error>{os::Fcntl::Open::kReadOnly}));
    EXPECT_CALL(*fcntl_, fcntl(file_descriptor_, os::Fcntl::Command::kFileSetStatusFlags, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createUnspecifiedError())));

    const auto file = unit_.OpenRegularFile(path_, std::ios::in);

    ASSERT_FALSE(file.has_value());
    EXPECT_EQ(file.error(), ErrorCode::kCouldNotOpenFileStream);
}

}  // namespace
}  // namespace score::filesystem
