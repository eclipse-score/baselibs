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
#include "score/os/statvfs_impl.h"

#include <gtest/gtest.h>

#include <fstream>

namespace score
{
namespace os
{
namespace
{

TEST(StatvfsTest, StatvfsSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatvfsTest Statvfs Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    char tmp_dir_template[] = "statvfs_test.XXXXXX";
    char* result = ::mkdtemp(tmp_dir_template);
    ASSERT_NE(result, nullptr);

    std::string temp_dir_ = result;
    struct statvfs buf{};
    std::string temp_file = temp_dir_ + "/testfile";

    std::ofstream file(temp_file);
    file << "Test content";
    file.close();

    auto statvfs_result = Statvfs::instance().statvfs(temp_file.c_str(), &buf);
    ASSERT_TRUE(statvfs_result.has_value()) << "statvfs failed";
    EXPECT_GT(buf.f_files, 0);
    EXPECT_GT(buf.f_namemax, 0);

    ::unlink(temp_file.c_str());

    int rmdir_result = ::rmdir(temp_dir_.c_str());
    ASSERT_EQ(rmdir_result, 0) << "Failed to remove directory: " << strerror(errno);
}

TEST(StatvfsTest, StatvfsFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatvfsTest Statvfs Failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    struct statvfs buf;
    std::string invalid_path = "/invalid/path";

    auto result = Statvfs::instance().statvfs(invalid_path.c_str(), &buf);
    EXPECT_FALSE(result.has_value());
}

}  // namespace
}  // namespace os
}  // namespace score
