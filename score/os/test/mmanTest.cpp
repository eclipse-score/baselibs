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
#include "score/os/mman.h"

#include "gtest/gtest.h"

#include <fcntl.h>
#include <sys/stat.h>

namespace score
{
namespace os
{
namespace test
{

TEST(mmap, MapAndUnmap)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Map And Unmap");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto filename{"mmap_file"};
    constexpr auto offset{0};
    std::vector<Mman::Protection> type_flag_vector = {
        Mman::Protection::kRead, Mman::Protection::kExec, Mman::Protection::kNoCache};
    for (const auto& tflag : type_flag_vector)
    {
        const auto fd = ::open(filename, O_CREAT | O_RDWR, 0644);
        EXPECT_NE(fd, -1);

        const auto data{"1234567890"};
        const auto bytes_written = ::write(fd, data, strlen(data));
        EXPECT_NE(bytes_written, -1);
        const auto size{static_cast<std::uint64_t>(bytes_written)};

        const auto result = score::os::Mman::instance().mmap(
            nullptr, size, tflag | Mman::Protection::kWrite, Mman::Map::kShared, fd, offset);

        EXPECT_TRUE(result.has_value());
        EXPECT_STREQ(static_cast<char*>(result.value()), data);
        EXPECT_TRUE(score::os::Mman::instance().munmap(result.value(), size).has_value());
        close(fd);
        unlink(filename);
    }
}

TEST(mmap, MapFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Map Failure");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto offset{0};
    constexpr auto size{0};
    const std::int32_t fd = -1;
    const auto result =
        score::os::Mman::instance().mmap(nullptr,
                                       size,
                                       score::os::Mman::Protection::kRead,
                                       score::os::Mman::Map::kPrivate | Mman::Map::kPhys | Mman::Map::kFixed,
                                       fd,
                                       offset);

    EXPECT_FALSE(result.has_value());
}

TEST(mmap, UnmapFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Unmap Failure");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    void* invalid_address = reinterpret_cast<void*>(0xDEADBEEF);
    std::size_t size{0};
    const auto unmap_ret = score::os::Mman::instance().munmap(invalid_address, size);
    EXPECT_FALSE(unmap_ret.has_value());
    EXPECT_EQ(unmap_ret.error(), Error::Code::kInvalidArgument);
}

TEST(mmap, OpenAndCloseSharedMemory)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Open And Close Shared Memory");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* name = "/test_mmap";
    Fcntl::Open oflag = Fcntl::Open::kCreate | Fcntl::Open::kReadWrite;
    Stat::Mode mode = Stat::Mode::kReadWriteExecUser;

    auto fd = score::os::Mman::instance().shm_open(name, oflag, mode);
    ASSERT_TRUE(fd.has_value());

    ASSERT_EQ(close(fd.value()), 0);
    ASSERT_TRUE(score::os::Mman::instance().shm_unlink(name).has_value());
}

TEST(mmap, ShmOpenNonExistingFile)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Shm Open Non Existing File");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* name = "";
    Fcntl::Open oflag = Fcntl::Open::kCreate | Fcntl::Open::kReadOnly | Fcntl::Open::kExclusive;
    Stat::Mode mode = Stat::Mode::kNone;

    auto result = score::os::Mman::instance().shm_open(name, oflag, mode);
    EXPECT_FALSE(result.has_value());
}

TEST(mmap, UnlinkNonExistentSharedMemory)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Unlink Non Existent Shared Memory");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = score::os::Mman::instance().shm_unlink("");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kNoSuchFileOrDirectory);
}

#if defined(__EXT_POSIX1_200112)
TEST(mmap, OpenInvalidTypedMemory)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Open Invalid Typed Memory");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* name = "";
    Fcntl::Open oflag = Fcntl::Open::kReadWrite;
    std::vector<Mman::PosixTypedMem> type_flag_vector = {
        Mman::PosixTypedMem::kAllocate, Mman::PosixTypedMem::kAllocateContig, Mman::PosixTypedMem::kMapAllocatable};
    for (const auto& tflag : type_flag_vector)
    {
        auto open_result = score::os::Mman::instance().posix_typed_mem_open(name, oflag, tflag);
        EXPECT_FALSE(open_result.has_value());
    }
}

TEST(mmap, GetInfoInvalidFD)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Get Info Invalid FD");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::int32_t invalid_fd = -1;
    struct posix_typed_mem_info info;

    auto info_result = score::os::Mman::instance().posix_typed_mem_get_info(invalid_fd, &info);

    EXPECT_FALSE(info_result.has_value());
    EXPECT_EQ(info_result.error(), Error::Code::kNoSuchFileOrDirectory);
}

TEST(mmap, OpenTypedMemory)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Open Typed Memory");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* name = "/memory";
    Fcntl::Open oflag = Fcntl::Open::kReadOnly;
    Mman::PosixTypedMem tflag = Mman::PosixTypedMem::kAllocateContig;

    auto open_result = score::os::Mman::instance().posix_typed_mem_open(name, oflag, tflag);
    ASSERT_TRUE(open_result.has_value());

    auto fd = open_result.value();
    struct posix_typed_mem_info info;
    info.posix_tmi_length = 0;
    ASSERT_EQ(::posix_typed_mem_get_info(fd, &info), 0);
    EXPECT_GT(info.posix_tmi_length, 0);

    ASSERT_EQ(close(fd), 0);
}

TEST(mmap, InfoTypedMemory)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Info Typed Memory");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* name = "/memory";

    auto fd = ::posix_typed_mem_open(name, O_RDONLY, POSIX_TYPED_MEM_ALLOCATE);
    ASSERT_NE(fd, -1);

    struct posix_typed_mem_info info;
    info.posix_tmi_length = 0;

    auto info_result = score::os::Mman::instance().posix_typed_mem_get_info(fd, &info);
    EXPECT_TRUE(info_result.has_value());

    EXPECT_GT(info.posix_tmi_length, 0);
    ASSERT_EQ(close(fd), 0);
}
#endif

TEST(mmap, DefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mmap Default Shall Return Impl Instance");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto default_instance = score::os::Mman::Default();
    ASSERT_TRUE(default_instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::internal::MmanImpl*>(default_instance.get()));
}

TEST(MmanTest, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MmanTest PMRDefault Shall Return Impl Instance");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Mman::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::internal::MmanImpl*>(instance.get()));
}

}  // namespace test
}  // namespace os
}  // namespace score
