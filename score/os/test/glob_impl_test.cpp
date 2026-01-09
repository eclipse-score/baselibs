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
#include "score/os/errno.h"
#include "score/os/glob.h"

#include "gtest/gtest.h"

#include <fcntl.h>
#include <cstdint>
#include <vector>

namespace score
{
namespace os
{
namespace test
{

class GlobImplTest : public ::testing::Test
{
  public:
    void SetUp() noexcept override
    {
        for (auto& file : test_files_)
        {
            int fd = ::open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            ASSERT_NE(fd, -1) << "Failed to create test file: " << file;
            ::close(fd);
        }

        glob_instance_ = Glob::Default();
    }

    void TearDown() noexcept override
    {
        for (auto& file : test_files_)
        {
            std::remove(file);
        }
    }

    void CheckMatchedFiles(const Glob::MatchResult& result, const std::vector<std::string>& expected_files)
    {
        ASSERT_EQ(result.count, expected_files.size())
            << "Should match exactly " << expected_files.size() << "test files.";

        for (const auto& file : expected_files)
        {
            EXPECT_TRUE(std::find(result.paths.begin(), result.paths.end(), file) != result.paths.end());
        }
    }

  protected:
    const char* test_files_[5] = {"testfile1.txt",
                                  "testfile2.txt",
                                  "testfile3.txt",
                                  "testfile1.docx",
                                  "testfile2.docx"};

    std::unique_ptr<Glob> glob_instance_;
};

TEST_F(GlobImplTest, MatchExistingFiles)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Match Existing Files");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto result = glob_instance_->Match("testfile*.txt", Glob::Flag::kNoSort);
    ASSERT_TRUE(result.has_value());

    std::vector<std::string> expected_files = {"testfile1.txt", "testfile2.txt", "testfile3.txt"};

    CheckMatchedFiles(result.value(), expected_files);
}

TEST_F(GlobImplTest, MatchTerminatesOnErrorWithGlobErrFlag)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Match Terminates On Error With Glob Err Flag");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto result = glob_instance_->Match("restricted_dir/*", Glob::Flag::kErr);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::createFromGlobError(GLOB_ABORTED));
}

TEST_F(GlobImplTest, MatchCombiningAppendAndSortFlags)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Match Combining Append And Sort Flags");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto first_result = glob_instance_->Match("*.txt", Glob::Flag::kNoSort);
    ASSERT_TRUE(first_result.has_value());

    auto combinedFlags = Glob::Flag::kAppend | Glob::Flag::kNoSort;
    auto second_result = glob_instance_->Match("*.docx", combinedFlags);
    ASSERT_TRUE(second_result.has_value());

    std::vector<std::string> expected_files = {
        "testfile1.txt", "testfile2.txt", "testfile3.txt", "testfile1.docx", "testfile2.docx"};

    CheckMatchedFiles(second_result.value(), expected_files);
}

TEST_F(GlobImplTest, MatchNoMatchFound)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Match No Match Found");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto result = glob_instance_->Match("nonexistentfilepattern.*", Glob::Flag::kNoSort);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), score::os::Error::Code::kGlobNoMatch);
}

TEST_F(GlobImplTest, MoveAssignmentFreesCurrentData)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Move Assignment Frees Current Data");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    GlobImpl glob1{};
    GlobImpl glob2{};

    auto match_result1 = glob1.Match("*.txt", Glob::Flag::kNoSort);
    ASSERT_TRUE(match_result1.has_value());

    auto match_result2 = glob2.Match("*.docx", Glob::Flag::kNoSort);
    ASSERT_TRUE(match_result2.has_value());

    for (auto& file : test_files_)
    {
        std::remove(file);
    }

    const char* new_file{"testfile4.txt"};
    auto fd = ::open(new_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_NE(fd, -1) << "Failed to create test file: " << new_file;
    ::close(fd);

    glob2 = std::move(glob1);

    match_result2 = glob2.Match("*.txt", Glob::Flag::kAppend);
    ASSERT_TRUE(match_result2.has_value());

    std::vector<std::string> expected_files = {"testfile1.txt", "testfile2.txt", "testfile3.txt", "testfile4.txt"};

    CheckMatchedFiles(match_result2.value(), expected_files);
    std::remove(new_file);
}

TEST_F(GlobImplTest, MoveAssignmentToNewInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Move Assignment To New Instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    GlobImpl glob1{};
    GlobImpl glob2{};

    auto match_result1 = glob1.Match("*.txt", Glob::Flag::kNoSort);
    ASSERT_TRUE(match_result1.has_value());

    for (auto& file : test_files_)
    {
        std::remove(file);
    }

    glob2 = std::move(glob1);

    const char* new_file{"testfile4.txt"};
    auto fd = ::open(new_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_NE(fd, -1) << "Failed to create test file: " << new_file;
    ::close(fd);

    auto match_result2 = glob2.Match("*.txt", Glob::Flag::kAppend);
    ASSERT_TRUE(match_result2.has_value());
    EXPECT_EQ(match_result2->count, 4U) << "Should match exactly four test files.";

    std::remove(new_file);
}

TEST_F(GlobImplTest, MoveConstructor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Move Constructor");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    GlobImpl glob1{};

    auto match_result1 = glob1.Match("*.txt", Glob::Flag::kNoSort);
    ASSERT_TRUE(match_result1.has_value());

    for (auto& file : test_files_)
    {
        std::remove(file);
    }

    const char* new_file{"testfile4.txt"};
    auto fd = ::open(new_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_NE(fd, -1) << "Failed to create test file: " << new_file;
    ::close(fd);

    GlobImpl glob2{std::move(glob1)};

    const auto match_result2 = glob2.Match("*.txt", Glob::Flag::kAppend);
    ASSERT_TRUE(match_result2.has_value());

    std::vector<std::string> expected_files = {"testfile1.txt", "testfile2.txt", "testfile3.txt", "testfile4.txt"};

    CheckMatchedFiles(match_result2.value(), expected_files);

    match_result1 = glob1.Match("*.txt", Glob::Flag::kAppend);
    ASSERT_TRUE(match_result1.has_value());
    ASSERT_EQ(match_result1->count, 1U);
    ASSERT_EQ(match_result1->paths[0], new_file);

    std::remove(new_file);
}

TEST_F(GlobImplTest, DefaultWithMemoryResource)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GlobImplTest Default With Memory Resource");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto glob = Glob::Default(score::cpp::pmr::get_default_resource());

    auto match_result = glob->Match("*.txt", Glob::Flag::kDooFFs);
    ASSERT_TRUE(match_result.has_value());
    EXPECT_EQ(match_result->count, 3U) << "Should match exactly three test files.";
}

}  // namespace test
}  // namespace os
}  // namespace score
