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
#include "score/os/mocklib/qnx/mock_iofunc.h"

#include <sys/iomsg.h>

#include <future>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sys/dcmd_all.h>
#include <stdio.h>

namespace
{

constexpr std::uint32_t kFuncNConnect{0U};
constexpr std::uint32_t kFuncNIo{0U};
constexpr std::uint32_t kAttrMode{0U};
constexpr resmgr_connect_funcs_t* kNoConnect{nullptr};
constexpr resmgr_io_funcs_t* kNoIo{nullptr};
constexpr iofunc_attr_t* kNoAttr{nullptr};
constexpr struct _client_info* kNoInfo{nullptr};
constexpr resmgr_context_t* kNoContext{nullptr};
constexpr io_write_t* kNoMsg{nullptr};
constexpr iofunc_mount_t* kNoMountPoint{nullptr};
constexpr io_read_t* kNoReadMsg{nullptr};
constexpr io_devctl_t* kNoDevctlMsg{nullptr};
constexpr iofunc_ocb_t* kNoOcb{nullptr};
constexpr std::int32_t* kNoNonBlock{nullptr};

constexpr std::size_t kNoSize{0};
constexpr std::uint16_t kPrivateMessageTypeFirst{_IO_MAX + 1};
constexpr std::uint16_t kPrivateMessageTypeLast{kPrivateMessageTypeFirst};
constexpr std::uint16_t kPrivateMessageTerminate{kPrivateMessageTypeFirst};

constexpr auto test_path = "/test/iofunc_unit_test_path";

/// This \c instance() call is necessary for providing coverage of the instance method, we can remove it when we remove
/// the instance function from the class
TEST(IoFuncTest, InstanceCall)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Instance Call");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // check whether instance() returns injection
    score::os::IoFuncQnx mock{};
    score::os::IoFunc::set_testing_instance(mock);
    const auto& mocked_unit = score::os::IoFunc::instance();
    EXPECT_EQ(&mocked_unit, &mock);

    // restore instance and should return non-mock object - implementation object
    score::os::IoFunc::restore_instance();
    const auto& impl_unit = score::os::IoFunc::instance();
    EXPECT_NE(&impl_unit, &mock);
}

// Mock test
struct IoFuncMockTest : ::testing::Test
{
    void SetUp() override
    {
        unit_ = &iofuncmock;
    };

    score::os::MockIoFunc iofuncmock;
    score::os::IoFunc* unit_{};
};

TEST_F(IoFuncMockTest, iofunc_attr_init)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Attr Init");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_attr_init);
    unit_->iofunc_attr_init(kNoAttr, kAttrMode, kNoAttr, kNoInfo);
}

TEST_F(IoFuncMockTest, iofunc_func_init)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Func Init");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_func_init);
    unit_->iofunc_func_init(kFuncNConnect, kNoConnect, kFuncNIo, kNoIo);
}

TEST_F(IoFuncMockTest, iofunc_mount_init)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Mount Init");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_mount_init);
    unit_->iofunc_mount_init(kNoMountPoint, kNoSize);
}

TEST_F(IoFuncMockTest, iofunc_close_ocb_default)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Close Ocb Default");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_close_ocb_default);
    unit_->iofunc_close_ocb_default(kNoContext, nullptr, kNoOcb);
}

TEST_F(IoFuncMockTest, iofunc_devctl_default)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Devctl Default");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_devctl_default);
    unit_->iofunc_devctl_default(kNoContext, kNoDevctlMsg, kNoOcb);
}

TEST_F(IoFuncMockTest, iofunc_write_verify)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Write Verify");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_write_verify);
    unit_->iofunc_write_verify(kNoContext, kNoMsg, kNoOcb, kNoNonBlock);
}

TEST_F(IoFuncMockTest, iofunc_read_verify)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Read Verify");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_read_verify);
    unit_->iofunc_read_verify(kNoContext, kNoReadMsg, kNoOcb, kNoNonBlock);
}

TEST_F(IoFuncMockTest, iofunc_lseek_default)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Lseek Default");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(iofuncmock, iofunc_lseek_default);
    unit_->iofunc_lseek_default(nullptr, nullptr, nullptr);
}
// ------------ IoFuncQnx -------------
class IoFuncFixture : public ::testing::Test
{
  protected:
    struct DevCtl
    {
        io_devctl_t msg;    // devctl message header
        std::int32_t data;  // devctl message payload
    };
    void SetUp() override
    {
        unit_ = std::make_unique<score::os::IoFuncQnx>();
    }

    std::unique_ptr<score::os::IoFunc> unit_;
};

TEST_F(IoFuncFixture, iofunc_close_ocb_default_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Close Ocb Default Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    extended_dev_attr_t attr{};
    // close_ocb will call free over ocb
    iofunc_ocb_t* ocb = ::iofunc_ocb_calloc(&ctp, &attr);
    ocb->attr = &attr;
    EXPECT_TRUE(unit_->iofunc_close_ocb_default(&ctp, nullptr, ocb));
}

TEST_F(IoFuncFixture, iofunc_mount_init_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Mount Init Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    iofunc_mount_t mount_point{};
    mount_point.flags = 5U;
    mount_point.size = 5U;

    EXPECT_TRUE(unit_->iofunc_mount_init(&mount_point, sizeof(mount_point)).has_value());
    EXPECT_EQ(mount_point.flags, 0U);
    EXPECT_EQ(mount_point.size, sizeof(mount_point));
}

TEST_F(IoFuncFixture, iofunc_mount_init_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Mount Init Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    iofunc_mount_t mount_point{};
    mount_point.flags = 5U;
    EXPECT_FALSE(unit_->iofunc_mount_init(&mount_point, kNoSize).has_value());
    EXPECT_EQ(mount_point.flags, 5U);
}

TEST_F(IoFuncFixture, iofunc_devctl_default_should_return_resmgr_default_with_unknown_dmcd)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Devctl Default Should Return Resmgr Default With Unknown Dmcd");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    DevCtl ctl{};
    ctl.msg.i.dcmd = 0;  // making it explicit
    iofunc_ocb_t ocb{};

    EXPECT_EQ(unit_->iofunc_devctl_default(&ctp, &ctl.msg, &ocb).value(), _RESMGR_DEFAULT);
}

TEST_F(IoFuncFixture, iofunc_devctl_default_should_return_data_with_dcmd_all_getflags)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Devctl Default Should Return Data With Dcmd All Getflags");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    DevCtl ctl{};
    ctl.msg.i.dcmd = DCMD_ALL_GETFLAGS;
    iofunc_ocb_t ocb{};

    EXPECT_EQ(unit_->iofunc_devctl_default(&ctp, &ctl.msg, &ocb).value(), -1);
    EXPECT_EQ(ctp.iov[0].iov_len, sizeof(DevCtl));
}

TEST_F(IoFuncFixture, iofunc_devctl_default_should_fail_with_dcmd_all_setflags_incomplete_message)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Devctl Default Should Fail With Dcmd All Setflags Incomplete Message");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    ctp.size = 0;  // making it explicit
    DevCtl ctl{};
    ctl.msg.i.dcmd = DCMD_ALL_SETFLAGS;
    ctl.msg.i.nbytes = 0;  // making it explicit
    iofunc_ocb_t ocb{};
    extended_dev_attr_t attr{};
    ocb.attr = &attr;

    EXPECT_FALSE(unit_->iofunc_devctl_default(&ctp, &ctl.msg, &ocb));
}

TEST_F(IoFuncFixture, iofunc_devctl_default_should_succeed_with_dcmd_all_setflags_complete_message)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Devctl Default Should Succeed With Dcmd All Setflags Complete Message");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    ctp.size = sizeof(DevCtl);
    DevCtl ctl{};
    ctl.msg.i.dcmd = DCMD_ALL_SETFLAGS;
    ctl.msg.i.nbytes = sizeof(ctl.data);
    iofunc_ocb_t ocb{};
    extended_dev_attr_t attr{};
    ocb.attr = &attr;

    EXPECT_EQ(unit_->iofunc_devctl_default(&ctp, &ctl.msg, &ocb).value(), EOK);
}

TEST_F(IoFuncFixture, iofunc_write_verifyReturnsErrorIfInvalidCtp)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Write Verify returns Error If Invalid Ctp");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    ctp.rcvid = -1;
    io_write_t msg{};
    iofunc_ocb_t ocb{};
    std::int32_t* nonblock{};

    EXPECT_FALSE(unit_->iofunc_write_verify(&ctp, &msg, &ocb, nonblock));
}

TEST_F(IoFuncFixture, iofunc_read_verifyReturnsErrorIfInvalidCtp)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Read Verify returns Error If Invalid Ctp");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    ctp.rcvid = -1;
    io_read_t msg{};
    iofunc_ocb_t ocb{};
    std::int32_t* nonblock{};

    EXPECT_FALSE(unit_->iofunc_read_verify(&ctp, &msg, &ocb, nonblock));
}

TEST_F(IoFuncFixture, iofunc_lseek_defaultReturnsErrorIfInvalidMsg)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Lseek Default returns Error If Invalid Msg");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    io_lseek_t msg{};
    msg.i.whence = -1;  // not one of SEEK_SET, SEEK_CUR, or SEEK_END
    iofunc_ocb_t ocb{};

    EXPECT_FALSE(unit_->iofunc_lseek_default(&ctp, &msg, &ocb));
}

TEST(IoFuncTest, iofunc_write_verify_fails_for_empty_parameters)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Write Verify Fails For Empty Parameters");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    io_write_t msg{};
    iofunc_ocb_t ocb{};

    auto& iofunc = score::os::IoFunc::instance();
    auto result = iofunc.iofunc_write_verify(&ctp, &msg, &ocb, nullptr);

    EXPECT_FALSE(result.has_value());
}

TEST(IoFuncCreateInstance, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PMR Default Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::IoFunc::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::IoFuncQnx*>(instance.get()));
}

TEST(IoFuncTest, iofunc_client_info_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Client Info Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    struct _client_info* pinfo{nullptr};

    auto& iofunc = score::os::IoFunc::instance();
    const auto client_info_result = iofunc.iofunc_client_info_ext(&ctp, 0, &pinfo);

    EXPECT_FALSE(client_info_result.has_value());
}

TEST(IoFuncTest, iofunc_client_info_free_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc client_info_free success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct _client_info* pinfo{nullptr};

    auto& iofunc = score::os::IoFunc::instance();
    const auto client_info_result = iofunc.iofunc_client_info_ext_free(&pinfo);

    EXPECT_TRUE(client_info_result.has_value());
}

TEST(IoFuncTest, iofunc_check_access_failure_client_info_nullptr)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Check Access Failure Client Info Nullptr");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    iofunc_attr_t attr{};

    auto& iofunc = score::os::IoFunc::instance();
    const auto check_access_result = iofunc.iofunc_check_access(&ctp, &attr, S_ISGID, nullptr);

    EXPECT_FALSE(check_access_result.has_value());
    EXPECT_EQ(check_access_result.error(), ENOSYS);
}

TEST(IoFuncTest, iofunc_check_access_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Check Access Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    iofunc_attr_t attr{};
    struct _client_info info{};

    // Client as a root
    info.cred.euid = 0;

    auto& iofunc = score::os::IoFunc::instance();
    const auto check_access_result = iofunc.iofunc_check_access(&ctp, &attr, S_ISGID, &info);

    EXPECT_TRUE(check_access_result.has_value());
}

TEST(IoFuncTest, iofunc_attr_lock_failure_invalid_mutex)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Attr Lock Failure Invalid Mutex");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    iofunc_attr_t attr{};
    ::pthread_mutex_init(&attr.lock, nullptr);
    ::pthread_mutex_destroy(&attr.lock);

    auto& iofunc = score::os::IoFunc::instance();
    const auto lock_result = iofunc.iofunc_attr_lock(&attr);

    EXPECT_FALSE(lock_result.has_value());
    EXPECT_EQ(lock_result.error(), EINVAL);
}

TEST(IoFuncTest, iofunc_attr_unlock_failure_invalid_mutex)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Attr Unlock Failure Invalid Mutex");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    iofunc_attr_t attr{};
    ::pthread_mutex_init(&attr.lock, nullptr);
    ::pthread_mutex_destroy(&attr.lock);

    auto& iofunc = score::os::IoFunc::instance();
    const auto unlock_result = iofunc.iofunc_attr_unlock(&attr);

    EXPECT_FALSE(unlock_result.has_value());
    EXPECT_EQ(unlock_result.error(), EINVAL);
}

TEST(IoFuncTest, iofunc_attr_lock_unlock_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Attr Lock Unlock Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    iofunc_attr_t attr{};

    auto& iofunc = score::os::IoFunc::instance();
    const auto lock_result = iofunc.iofunc_attr_lock(&attr);
    const auto unlock_result = iofunc.iofunc_attr_unlock(&attr);

    EXPECT_TRUE(lock_result.has_value());
    EXPECT_TRUE(unlock_result.has_value());
}

TEST(IoFuncTest, iofunc_open_failure_both_attr_nullptr)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Open Failure Both Attr Nullptr");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    io_open_t msg{};
    _client_info info{};

    auto& iofunc = score::os::IoFunc::instance();
    const auto open_result = iofunc.iofunc_open(&ctp, &msg, nullptr, nullptr, &info);

    EXPECT_FALSE(open_result.has_value());
    EXPECT_EQ(open_result.error(), EINVAL);
}

TEST(IoFuncTest, iofunc_open_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Open Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    io_open_t msg{};
    iofunc_attr_t attr{};
    _client_info info{};
    // Client as a root
    info.cred.euid = 0;

    auto& iofunc = score::os::IoFunc::instance();
    const auto open_result = iofunc.iofunc_open(&ctp, &msg, &attr, nullptr, &info);

    EXPECT_TRUE(open_result.has_value());
}

TEST(IoFuncTest, iofunc_ocb_attach_failure_invalid_ctp)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Iofunc Ocb Attach Failure Invalid Ctp");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    resmgr_context_t ctp{};
    io_open_t msg{};
    iofunc_ocb_t ocb{};
    iofunc_attr_t attr{};
    resmgr_io_funcs_t io_funcs{};
    ctp.id = -1;

    auto& iofunc = score::os::IoFunc::instance();
    const auto ocb_attach_result = iofunc.iofunc_ocb_attach(&ctp, &msg, &ocb, &attr, &io_funcs);

    EXPECT_FALSE(ocb_attach_result.has_value());
    EXPECT_EQ(ocb_attach_result.error(), ENOENT);
}

}  // namespace
