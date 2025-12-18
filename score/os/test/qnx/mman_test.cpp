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
#include "score/os/qnx/mman_impl.h"
#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/neutrino.h>

class MmanTestFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = std::make_unique<score::os::qnx::MmanQnxImpl>();
        // Set I/O privileges for the calling thread
        ThreadCtl(_NTO_TCTL_IO, 0);
    }

    std::unique_ptr<score::os::qnx::MmanQnxImpl> unit_{};
    uintptr_t address_ = 0x60UL;
    const char* shm_name = "/test_mmap";
};

TEST_F(MmanTestFixture, MmapMunmapDeviceIOReturnNoErrorIfPassValidAddress)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mmap Munmap Device Io Return No Error If Pass Valid Address");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto val = unit_->mmap_device_io(1, address_);
    EXPECT_TRUE(val.has_value());

    const auto val_2 = unit_->munmap_device_io(val.value(), 1);
    EXPECT_TRUE(val_2.has_value());
    /* To verify the successfull unmapping */
    EXPECT_NE(val_2.value(), -1);
}

TEST_F(MmanTestFixture, ShmCtlFailsWithInvalidPhysicalAddress)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Ctl Fails With Invalid Physical Address");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto shm_open_result = ::shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, 0000);

    ASSERT_NE(shm_open_result, -1);
    const auto result = unit_->shm_ctl(shm_open_result, SHMCTL_ANON | SHMCTL_TYMEM | SHMCTL_PHYS, -1, 4095);

    EXPECT_FALSE(result.has_value());

    ASSERT_EQ(::close(shm_open_result), 0);
    ASSERT_NE(::shm_unlink(shm_name), -1);
}

TEST_F(MmanTestFixture, ShmCtlSucceeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Ctl Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* name = "/memory";

    auto open_result = ::posix_typed_mem_open(name, O_RDWR, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    ASSERT_NE(open_result, -1);

    const char* shm_name = "/test_mmap1";
    auto shm_open_result = ::shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, 0000);

    ASSERT_NE(shm_open_result, -1);
    const auto result = unit_->shm_ctl(shm_open_result, SHMCTL_ANON | SHMCTL_TYMEM | SHMCTL_PHYS, open_result, 0);

    EXPECT_TRUE(result.has_value());
    ASSERT_EQ(::close(shm_open_result), 0);
}

TEST_F(MmanTestFixture, MemOffsetFailsWhenPassInvalidVirtualAddress)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mem Offset Fails When Pass Invalid Virtual Address");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    off_t physical_addr{};
    const auto result = unit_->mem_offset(nullptr, NOFD, 4095U, &physical_addr, nullptr);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::createFromErrno(EACCES));
}

TEST_F(MmanTestFixture, MemOffsetSucceeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mem Offset Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* name = "/memory";

    const auto open_result = ::posix_typed_mem_open(name, O_RDONLY, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    ASSERT_NE(open_result, -1);

    const auto mmap_result = ::mmap(nullptr, 4095U, PROT_READ, MAP_SHARED, open_result, 0);
    ASSERT_NE(mmap_result, MAP_FAILED);

    off_t physical_addr{};
    const auto result = unit_->mem_offset(mmap_result, NOFD, 4095U, &physical_addr, nullptr);

    EXPECT_TRUE(result.has_value());
}

TEST_F(MmanTestFixture, MemOffset64FailsWhenPassInvalidVirtualAddress)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mem Offset Fails When Pass Invalid Virtual Address");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    off_t physical_addr{};
    const auto result = unit_->mem_offset64(nullptr, NOFD, 4095U, &physical_addr, nullptr);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::createFromErrno(EACCES));
}

TEST_F(MmanTestFixture, MemOffset64Succeeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mem Offset Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* name = "/memory";

    const auto open_result = ::posix_typed_mem_open(name, O_RDONLY, POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    ASSERT_NE(open_result, -1);

    const auto mmap_result = ::mmap(nullptr, 4095U, PROT_READ, MAP_SHARED, open_result, 0);
    ASSERT_NE(mmap_result, MAP_FAILED);

    off_t physical_addr{};
    const auto result = unit_->mem_offset64(mmap_result, NOFD, 4095U, &physical_addr, nullptr);

    EXPECT_TRUE(result.has_value());
}

TEST_F(MmanTestFixture, ShmOpenSucceeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Open Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t oflag = O_CREAT | O_RDWR;
    mode_t mode = 0666;

    const auto result = unit_->shm_open(shm_name, oflag, mode);

    ASSERT_TRUE(result.has_value());
    ::close(result.value());
}

TEST_F(MmanTestFixture, ShmOpenFails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Open Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t oflag = O_RDWR;
    mode_t mode = 0666;
    const auto result = unit_->shm_open("invalid_shm", oflag, mode);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(MmanTestFixture, ShmOpenHandleSucceeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Open Handle Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t fd = ::shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ASSERT_NE(fd, -1);

    pid_t pid = ::getpid();
    std::int32_t flags = 0;
    std::uint32_t options = 0;
    shm_handle_t handle;

    auto create_handle_result = ::shm_create_handle(fd, pid, flags, &handle, options);
    ASSERT_EQ(create_handle_result, 0);

    auto open_handle_result = unit_->shm_open_handle(handle, flags);
    ASSERT_TRUE(open_handle_result.has_value());
    ::close(fd);
}

TEST_F(MmanTestFixture, ShmOpenHandleFailsWithInvalidHandle)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Open Handle Fails With Invalid Handle");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    shm_handle_t invalid_handle = -1;
    std::int32_t flags = O_RDWR;

    const auto result = unit_->shm_open_handle(invalid_handle, flags);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kBadFileDescriptor);
}

TEST_F(MmanTestFixture, ShmCreateHandleSucceeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Create Handle Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ASSERT_NE(fd, -1);

    pid_t pid = getpid();
    std::int32_t flags = 0;
    shm_handle_t handle;
    std::uint32_t options = 0;

    const auto result = unit_->shm_create_handle(fd, pid, flags, &handle, options);

    ASSERT_TRUE(result.has_value());
    ::close(fd);
}

TEST_F(MmanTestFixture, ShmCreateHandleFailsWithBadFileDescritor)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Shm Create Handle Fails With Bad File Descritor");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t invalid_fd = -1;
    pid_t pid = getpid();
    std::int32_t flags = 0;
    shm_handle_t handle;
    std::uint32_t options = 0;

    const auto result = unit_->shm_create_handle(invalid_fd, pid, flags, &handle, options);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kBadFileDescriptor);
}

TEST_F(MmanTestFixture, MmapSucceeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mmap Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t length = 4096;
    const std::int32_t protection = PROT_READ | PROT_WRITE;
    const std::int32_t flags = MAP_PRIVATE | MAP_ANONYMOUS;
    const std::int32_t fd = -1;
    const std::int64_t offset = 0;

    const auto result = unit_->mmap(nullptr, length, protection, flags, fd, offset);

    ASSERT_TRUE(result.has_value());
    ::munmap(result.value(), length);
}

TEST_F(MmanTestFixture, MmapFails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mmap Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t length = 0;
    const std::int32_t protection = PROT_READ | PROT_WRITE;
    const std::int32_t flags = MAP_PRIVATE | MAP_ANONYMOUS;
    const std::int32_t fd = -1;
    const std::int64_t offset = 0;

    const auto result = unit_->mmap(nullptr, length, protection, flags, fd, offset);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(MmanTestFixture, Mmap64Succeeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mmap Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t length = 4096;
    const std::int32_t protection = PROT_READ | PROT_WRITE;
    const std::int32_t flags = MAP_PRIVATE | MAP_ANONYMOUS;
    const std::int32_t fd = -1;
    const std::int64_t offset = 0;

    const auto result = unit_->mmap64(nullptr, length, protection, flags, fd, offset);

    EXPECT_TRUE(result.has_value());
    ::munmap(result.value(), length);
}

TEST_F(MmanTestFixture, Mmap64Fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mmap Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t length = 0;
    const std::int32_t protection = PROT_READ | PROT_WRITE;
    const std::int32_t flags = MAP_PRIVATE | MAP_ANONYMOUS;
    const std::int32_t fd = -1;
    const std::int64_t offset = 0;

    const auto result = unit_->mmap64(nullptr, length, protection, flags, fd, offset);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(MmanTestFixture, MunmapSucceeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Munmap Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t length = 4096;
    const std::int32_t protection = PROT_READ | PROT_WRITE;
    const std::int32_t flags = MAP_PRIVATE | MAP_ANONYMOUS;
    const std::int32_t fd = -1;
    const std::int64_t offset = 0;

    const auto result = unit_->mmap(nullptr, length, protection, flags, fd, offset);
    ASSERT_TRUE(result.has_value());

    const auto unmap_result = unit_->munmap(result.value(), length);

    EXPECT_TRUE(unmap_result.has_value());
}

TEST_F(MmanTestFixture, MunmapFails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Munmap Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::size_t invalid_length = 0;

    const auto unmap_result = unit_->munmap(nullptr, invalid_length);

    EXPECT_FALSE(unmap_result.has_value());
    EXPECT_EQ(unmap_result.error(), score::os::Error::Code::kInvalidArgument);
}
