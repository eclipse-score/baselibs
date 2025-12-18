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
#include "score/os/dirent_impl.h"
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{
class DirentTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        char tmp_dir_template[] = "dirent_test.XXXXXX";
        char* result = ::mkdtemp(tmp_dir_template);
        ASSERT_NE(result, nullptr);
        temp_dir_ = result;
    }

    void TearDown() override
    {
        ASSERT_EQ(::rmdir(temp_dir_.c_str()), 0);
    }

    std::string temp_dir_;
    struct dirent** namelist_;
    ::score::os::DirentImpl unit{};
};

TEST_F(DirentTest, ScanPositiveTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Scan shall return success for scan dir");

    auto result = unit.scandir(temp_dir_.c_str(), &namelist_, nullptr, nullptr);
    EXPECT_TRUE(result.has_value());
    int n = result.value();
    while (n > 0)
    {
        n--;
        ::free(namelist_[n]);
    }
    ::free(namelist_);
}

TEST_F(DirentTest, ScanNegativeTest)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Scan shall return error for invalid path");

    auto proc_path = "invalid/path";
    auto result = unit.scandir(proc_path, &namelist_, nullptr, nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(DirentTest, OpenDirSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Open dir shall return success for valid dir");

    auto result = unit.opendir(temp_dir_.c_str());
    EXPECT_TRUE(result.has_value());
    DIR* dir_ptr = result.value();

    ::closedir(dir_ptr);
}

TEST_F(DirentTest, OpenDirFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Open dir shall return error for invalid dir");

    auto result = unit.opendir("invalid/path");
    EXPECT_FALSE(result.has_value());
}

TEST_F(DirentTest, ReadDirSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Read dir shall return success for valid dir");

    DIR* dir_ptr = ::opendir(temp_dir_.c_str());
    ASSERT_NE(dir_ptr, nullptr);
    ::closedir(dir_ptr);

    std::string sub_dir1 = temp_dir_ + "/subdir1";
    ASSERT_EQ(::mkdir(sub_dir1.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH), 0);

    score::cpp::expected<struct dirent*, score::os::Error> dirent_result;
    dir_ptr = ::opendir(temp_dir_.c_str());
    ASSERT_NE(dir_ptr, nullptr);

    while ((dirent_result = unit.readdir(dir_ptr)).has_value())
    {
        auto entry = dirent_result.value();
        if (strcmp(entry->d_name, "subdir1") == 0)
        {
            break;
        }
    }

    EXPECT_TRUE(dirent_result.has_value());
    ::rmdir(sub_dir1.c_str());
    ::closedir(dir_ptr);
}

TEST_F(DirentTest, ReadDirEnd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Read dir shall return success for valid dir");

    DIR* dir_ptr = ::opendir(temp_dir_.c_str());
    ASSERT_NE(dir_ptr, nullptr);

    while (unit.readdir(dir_ptr).has_value())
    {
    }

    auto dirent_result = unit.readdir(dir_ptr);
    EXPECT_FALSE(dirent_result.has_value());

    ::closedir(dir_ptr);
}

TEST_F(DirentTest, CloseDirSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Close dir shall return success for valid dir");

    DIR* dir_ptr = ::opendir(temp_dir_.c_str());
    ASSERT_NE(dir_ptr, nullptr);

    auto result = unit.closedir(dir_ptr);
    EXPECT_TRUE(result.has_value());
}

TEST(Dirent, get_instance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    RecordProperty("Description", "Dirent shall provide instance functionality");

    EXPECT_NO_FATAL_FAILURE(Dirent::instance());
}

}  // namespace
}  // namespace os
}  // namespace score
