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

#include "score/os/stat_impl.h"

#include "gtest/gtest.h"
#include <fcntl.h>

namespace score
{
namespace os
{
namespace test
{

TEST(StatImpl, StatRegularFile)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Stat Regular File");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{"stat_file"};
    const auto fd = ::open(filename, O_CREAT | O_RDWR, 0644);
    EXPECT_NE(fd, -1);
    struct StatBuffer buf;
    bool resolve_symlinks = true;

    const auto result = score::os::Stat::instance().stat(filename, buf, resolve_symlinks);

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(S_ISREG(buf.st_mode));
    ::close(fd);
}

TEST(StatImpl, StatSymbolicLink)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Stat Symbolic Link");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{"stat_file"};
    const auto fd = ::open(filename, O_CREAT | O_RDWR, 0644);
    EXPECT_NE(fd, -1);
    struct StatBuffer buf;
    bool resolve_symlinks = false;
    const auto result = score::os::Stat::instance().stat(filename, buf, resolve_symlinks);

    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(S_ISLNK(buf.st_mode));
    ::close(fd);
}

TEST(StatImpl, StatNonExistentFile)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Stat Non Existent File");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* non_existent_file = "nonexistent/file";
    struct StatBuffer buf;
    bool resolve_symlinks = true;

    const auto result = score::os::Stat::instance().stat(non_existent_file, buf, resolve_symlinks);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kNoSuchFileOrDirectory);
}

TEST(StatImpl, fstatSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl fstat Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{"stat_file"};
    const auto fd = ::open(filename, O_CREAT | O_RDONLY, 0644);
    EXPECT_NE(fd, -1);
    struct StatBuffer buf;
    const auto result = score::os::Stat::instance().fstat(fd, buf);
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(S_ISREG(buf.st_mode));
    ::close(fd);
}

TEST(StatImpl, fstatFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl fstat Failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto fd = -1;
    struct StatBuffer buf;
    EXPECT_FALSE(score::os::Stat::instance().fstat(fd, buf).has_value());
}

TEST(StatImpl, MkdirFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Mkdir Failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* directory = "/hd/src";
    Stat::Mode mode = Stat::Mode::kUnknown;

    const auto result = score::os::Stat::instance().mkdir(directory, mode);
    EXPECT_FALSE(result.has_value());

    struct StatBuffer buf;
    auto stat = score::os::Stat::instance().stat(directory, buf, true);
    EXPECT_FALSE(stat.has_value());
}

TEST(StatImpl, MkdirSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Mkdir Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* directory = "directory";
    Stat::Mode mode = Stat::Mode::kReadWriteExecUser;

    const auto result = score::os::Stat::instance().mkdir(directory, mode);
    EXPECT_TRUE(result.has_value());

    struct StatBuffer buf;
    auto stat = score::os::Stat::instance().stat(directory, buf, true);
    EXPECT_TRUE(stat.has_value());
    EXPECT_TRUE(S_ISDIR(buf.st_mode));
}

TEST(StatImpl, ChmodSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Chmod Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{"stat_test_file"};
    const auto fd = ::open(filename, O_CREAT | O_WRONLY, 0644);
    EXPECT_NE(fd, -1);
    Stat::Mode mode = Stat::Mode::kReadWriteExecUser;

    const auto result = score::os::Stat::instance().chmod(filename, mode);
    EXPECT_TRUE(result.has_value());

    struct stat file_info;
    ::stat(filename, &file_info);
    EXPECT_TRUE(file_info.st_mode & S_IRWXU);
    ::close(fd);
}

TEST(StatImpl, ChmodFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Chmod Failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{""};
    Stat::Mode mode = Stat::Mode::kUnknown;

    const auto result = score::os::Stat::instance().chmod(filename, mode);
    EXPECT_FALSE(result.has_value());
}

TEST(StatImpl, FchmodSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Fchmod Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{"stat_test_file"};
    const auto fd = ::open(filename, O_CREAT | O_WRONLY, 0644);
    EXPECT_NE(fd, -1);

    const auto result = score::os::Stat::instance().fchmod(fd, Stat::Mode::kReadWriteExecUser);
    EXPECT_TRUE(result.has_value());

    struct stat file_info;
    ::stat(filename, &file_info);
    EXPECT_TRUE(file_info.st_mode & S_IRWXU);

    ::close(fd);
}

TEST(StatImpl, FchmodFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Fchmod Failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto fd = -1;

    const auto result = score::os::Stat::instance().fchmod(fd, Stat::Mode::kReadWriteExecUser);
    EXPECT_FALSE(result.has_value());
}

TEST(StatImpl, FchmodatSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Fchmodat Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* file_name = "my_directory";
    int fd = open(file_name, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    EXPECT_NE(fd, -1);
    bool resolve_symlinks = true;

    const auto result =
        score::os::Stat::instance().fchmodat(AT_FDCWD, file_name, Stat::Mode::kReadUser, resolve_symlinks);
    EXPECT_TRUE(result.has_value());
    ::close(fd);
}

TEST(StatImpl, FchmodatErrorNoFollowSymlinks)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Fchmodat Error No Follow Symlinks");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{"stat_test_file"};
    const auto fd = ::open(filename, O_CREAT | O_RDWR, 0644);
    EXPECT_NE(fd, -1);
    bool resolve_symlinks = false;

    const auto result =
        score::os::Stat::instance().fchmodat(fd, filename, Stat::Mode::kReadWriteExecUser, resolve_symlinks);
    EXPECT_FALSE(result.has_value());
    ::close(fd);
}

TEST(StatImpl, DefaultObjectAllocationIsNotNull)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl Default Object Allocation Is Not Null");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto result = score::os::Stat::Default();
    EXPECT_NE(result, nullptr);
}

TEST(StatImpl, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StatImpl PMRDefault Shall Return Impl Instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Stat::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::StatImpl*>(instance.get()));
}

}  // namespace test
}  // namespace os
}  // namespace score
