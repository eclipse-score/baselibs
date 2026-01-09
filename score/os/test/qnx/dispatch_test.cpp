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
#include "score/os/mocklib/qnx/mock_dispatch.h"
#include "score/os/qnx/dispatch_impl.h"

#include "score/os/qnx/channel.h"
#include "score/os/qnx/iofunc.h"
#include "score/os/qnx/procmgr.h"
#include "score/os/qnx/timer_impl.h"

#include <poll.h>
#include <sys/iomsg.h>

#include <future>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

constexpr std::int32_t kInvalidId{-1};
constexpr std::int32_t kOpenFlags{0};
constexpr std::uint32_t kAttachFlags{0U};
constexpr std::uint32_t kDetachFlags{0U};
constexpr dispatch_t* kNoDispatch{nullptr};
constexpr dispatch_context_t* kNoDispatchContext{nullptr};
constexpr name_attach_t* kNoAttach{nullptr};
constexpr resmgr_attr_t* kNoAttr{nullptr};
constexpr resmgr_connect_funcs_t* kNoConnectFuncs{nullptr};
constexpr resmgr_io_funcs_t* kNoIoFuncs{nullptr};
constexpr RESMGR_HANDLE_T* kNoHandle{nullptr};
constexpr std::int32_t* kNoNonBlock{nullptr};
constexpr resmgr_context_t* kNoResmgrContext{nullptr};
constexpr void* kNoMsg{nullptr};
constexpr void* kNoReply{nullptr};
constexpr std::size_t kNoSize{0};
constexpr std::size_t kNoOffset{0};
constexpr std::uint16_t kPrivateMessageTypeFirst{_IO_MAX + 1};
constexpr std::uint16_t kPrivateMessageTypeLast{kPrivateMessageTypeFirst};
constexpr std::uint16_t kPrivateMessageTerminate{kPrivateMessageTypeFirst};
constexpr _message_attr* noAttr{nullptr};
constexpr void* noHandle{nullptr};

// Mock test

struct DispatchMockTest : ::testing::Test
{
    void SetUp() override
    {
        score::os::Dispatch::set_testing_instance(dispatchmock);
    };
    void TearDown() override
    {
        score::os::Dispatch::restore_instance();
    };

    score::os::MockDispatch dispatchmock;
};

TEST_F(DispatchMockTest, name_attach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Attach");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, name_attach);
    score::os::Dispatch::instance().name_attach(kNoDispatch, "path", kAttachFlags);
}

TEST_F(DispatchMockTest, name_detach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Detach");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, name_detach);
    score::os::Dispatch::instance().name_detach(kNoAttach, kDetachFlags);
}

TEST_F(DispatchMockTest, name_open)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Open");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, name_open);
    score::os::Dispatch::instance().name_open("path", kOpenFlags);
}

TEST_F(DispatchMockTest, name_close)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Close");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, name_close);
    score::os::Dispatch::instance().name_close(kInvalidId);
}

TEST_F(DispatchMockTest, dispatch_create)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Create");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_create);
    score::os::Dispatch::instance().dispatch_create();
}

TEST_F(DispatchMockTest, dispatch_create_channel)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Create Channel");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_create_channel);
    score::os::Dispatch::instance().dispatch_create_channel(kInvalidId, DISPATCH_FLAG_NOLOCK);
}

TEST_F(DispatchMockTest, dispatch_destroy)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Destroy");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_destroy);
    score::os::Dispatch::instance().dispatch_destroy(kNoDispatch);
}

TEST_F(DispatchMockTest, dispatch_context_alloc)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Context Alloc");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_context_alloc);
    score::os::Dispatch::instance().dispatch_context_alloc(kNoDispatch);
}

TEST_F(DispatchMockTest, dispatch_context_free)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Context Free");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_context_free);
    score::os::Dispatch::instance().dispatch_context_free(kNoDispatchContext);
}

TEST_F(DispatchMockTest, dispatch_block)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Block");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_block);
    score::os::Dispatch::instance().dispatch_block(kNoDispatchContext);
}

TEST_F(DispatchMockTest, dispatch_unblock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Unblock");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_unblock);
    score::os::Dispatch::instance().dispatch_unblock(kNoDispatchContext);
}

TEST_F(DispatchMockTest, dispatch_handler)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Handler");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, dispatch_handler);
    score::os::Dispatch::instance().dispatch_handler(kNoDispatchContext);
}

TEST_F(DispatchMockTest, resmgr_attach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Resmgr Attach");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, resmgr_attach);
    score::os::Dispatch::instance().resmgr_attach(
        kNoDispatch, kNoAttr, "/invalid_path", _FTYPE_ANY, kOpenFlags, kNoConnectFuncs, kNoIoFuncs, kNoHandle);
}

TEST_F(DispatchMockTest, resmgr_detach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Resmgr Detach");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, resmgr_detach);
    score::os::Dispatch::instance().resmgr_detach(kNoDispatch, kInvalidId, _RESMGR_DETACH_ALL);
}

TEST_F(DispatchMockTest, resmgr_msgget)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Resmgr Msgget");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, resmgr_msgget);
    score::os::Dispatch::instance().resmgr_msgget(kNoResmgrContext, kNoMsg, kNoSize, kNoOffset);
}

TEST_F(DispatchMockTest, message_connect)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Connect");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_CALL(dispatchmock, message_connect);
    score::os::Dispatch::instance().message_connect(kNoDispatch, MSG_FLAG_SIDE_CHANNEL);
}

TEST_F(DispatchMockTest, message_attach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Attach");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::int32_t (*const noHandler)(
        message_context_t* ctp, std::int32_t code, std::uint32_t flags, void* handle) noexcept {nullptr};
    EXPECT_CALL(dispatchmock, message_attach);
    score::os::Dispatch::instance().message_attach(
        kNoDispatch, noAttr, kPrivateMessageTypeFirst, kPrivateMessageTypeLast, noHandler, noHandle);
}

// Tests of the real stuff

TEST(DispatchTest, name_attach_to_invalid_path_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Attach To Invalid Path Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto& dispatch = score::os::Dispatch::instance();

    EXPECT_FALSE(dispatch.name_attach(kNoDispatch, "/invalid_path", kAttachFlags));
}

TEST(DispatchTest, name_detach_from_invalid_id_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Detach From Invalid Id Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto& dispatch = score::os::Dispatch::instance();

    // name_detach() frees the attach memory even when fails; allocate to prevent crash
    name_attach_t* fake_attach = static_cast<name_attach_t*>(std::calloc(1, sizeof(name_attach_t)));
    fake_attach->mntid = kInvalidId;
    // disable destroying fake_attach->dpp, otherwise it will crash
    EXPECT_FALSE(dispatch.name_detach(fake_attach, NAME_FLAG_DETACH_SAVEDPP));
}

TEST(DispatchTest, name_open_invalid_path_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Open Invalid Path Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto& dispatch = score::os::Dispatch::instance();

    EXPECT_FALSE(dispatch.name_open("/invalid_path", kOpenFlags));
}

TEST(DispatchTest, name_close_invalid_id_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Close Invalid Id Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto& dispatch = score::os::Dispatch::instance();

    EXPECT_FALSE(dispatch.name_close(kInvalidId));
}

TEST(DispatchTest, dispatch_handler_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Handler Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto& dispatch = score::os::Dispatch::instance();

    auto result = dispatch.dispatch_handler(nullptr);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), -1);
}

TEST(DispatchTest, CheckServerHappyFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check Server Happy Flow");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto& dispatch = score::os::Dispatch::instance();

    auto attach = dispatch.name_attach(kNoDispatch, "valid_test_path", kAttachFlags);
    EXPECT_TRUE(attach);
    EXPECT_TRUE(dispatch.name_detach(attach.value(), kDetachFlags));

    // expected path for the client is tested in channel_test.cpp
}

struct DispatchResourceManagerTest : ::testing::Test, extended_dev_attr_t
{
    // statistics
    std::size_t total_write_num{};
    std::size_t total_write_size{};

    // exit flag
    bool to_exit{};

    // resmgr-specific user-provided structures with long lifetimes
    resmgr_attr_t resmgr_attr{};
    resmgr_connect_funcs_t connect_funcs{};
    resmgr_io_funcs_t io_funcs{};
    iofunc_ocb_t ocb{};  // only a single connection is supported in the test
    iofunc_notify_t notify[3];

    void SetUp() override
    {
        total_write_num = 0;
        total_write_size = 0;
        to_exit = false;

        InitResmgrStructures();
    };

    // no failures could be diagnosed here
    void InitResmgrStructures() noexcept
    {
        auto& iofunc = score::os::IoFunc::instance();

        resmgr_attr.nparts_max = 1;
        resmgr_attr.msg_max_size = 1024;

        // pre-configure resmgr callback data
        iofunc.iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
        connect_funcs.open = &io_open;
        io_funcs.notify = &io_notify;
        io_funcs.write = &io_write;
        io_funcs.close_ocb = &io_close_ocb;

        IOFUNC_NOTIFY_INIT(notify);

        constexpr mode_t attrMode{S_IFNAM | 0660};
        constexpr iofunc_attr_t* noAttr{nullptr};
        constexpr _client_info* noClientInfo{nullptr};

        // pre-configure resmgr access rights data
        // (attr member is inherited from extended_dev_attr_t base class)
        iofunc.iofunc_attr_init(&attr, attrMode, noAttr, noClientInfo);
    }

    score::cpp::expected<bool, std::int32_t> NextServiceRequest(dispatch_context_t* ctp) noexcept;

    // resmgr POSIX support callbacks
    static std::int32_t io_open(resmgr_context_t* const ctp,
                                io_open_t* const msg,
                                RESMGR_HANDLE_T* const handle,
                                void* const extra) noexcept;

    static std::int32_t io_close_ocb(resmgr_context_t* const ctp,
                                     void* const reserved,
                                     RESMGR_OCB_T* const ocb) noexcept;

    static std::int32_t io_write(resmgr_context_t* ctp, io_write_t* msg, RESMGR_OCB_T* ocb) noexcept;

    static std::int32_t io_notify(resmgr_context_t* const ctp,
                                  io_notify_t* const msg,
                                  RESMGR_OCB_T* const ocb) noexcept;

    // resmgr private message callback
    static std::int32_t private_message_handler(message_context_t* ctp,
                                                std::int32_t code,
                                                std::uint32_t flags,
                                                void* handle) noexcept;
};

std::int32_t DispatchResourceManagerTest::io_open(resmgr_context_t* const ctp,
                                                  io_open_t* const msg,
                                                  RESMGR_HANDLE_T* const handle,
                                                  void* const /*extra*/) noexcept
{
    auto& iofunc = score::os::IoFunc::instance();

    // handle is a pointer to extended_dev_attr_t base class
    auto& fixture = *static_cast<DispatchResourceManagerTest*>(handle);
    auto& attr = fixture.attr;
    auto& ocb = fixture.ocb;

    iofunc.iofunc_attr_lock(&attr);

    _client_info* pinfo{nullptr};
    score::cpp::expected_blank<std::int32_t> status = iofunc.iofunc_client_info_ext(ctp, 0, &pinfo);
    if (!status.has_value())
    {
        iofunc.iofunc_attr_unlock(&attr);
        return status.error();
    }

    status = iofunc.iofunc_open(ctp, msg, &attr, 0, pinfo);
    if (!status.has_value())
    {
        iofunc.iofunc_attr_unlock(&attr);
        return status.error();
    }

    status = iofunc.iofunc_ocb_attach(ctp, msg, &ocb, &attr, 0);
    if (!status.has_value())
    {
        iofunc.iofunc_attr_unlock(&attr);
        return status.error();
    }

    iofunc.iofunc_attr_unlock(&attr);
    return EOK;
}

std::int32_t DispatchResourceManagerTest::io_close_ocb(resmgr_context_t* const ctp,
                                                       void* const /*reserved*/,
                                                       RESMGR_OCB_T* const ocb) noexcept
{
    auto& iofunc = score::os::IoFunc::instance();

    auto& fixture = *static_cast<DispatchResourceManagerTest*>(ocb->attr);
    auto& attr = fixture.attr;
    auto& notify = fixture.notify;

    iofunc.iofunc_notify_trigger_strict(ctp, notify, INT_MAX, IOFUNC_NOTIFY_INPUT);
    iofunc.iofunc_notify_trigger_strict(ctp, notify, INT_MAX, IOFUNC_NOTIFY_OUTPUT);
    iofunc.iofunc_notify_trigger_strict(ctp, notify, INT_MAX, IOFUNC_NOTIFY_OBAND);

    iofunc.iofunc_notify_remove(ctp, notify);

    iofunc.iofunc_attr_lock(&attr);
    score::cpp::ignore = iofunc.iofunc_ocb_detach(ctp, ocb);
    iofunc.iofunc_attr_unlock(&attr);
    return EOK;
}

std::int32_t DispatchResourceManagerTest::io_write(resmgr_context_t* ctp, io_write_t* msg, RESMGR_OCB_T* ocb) noexcept
{
    auto& dispatch = score::os::Dispatch::instance();
    auto& iofunc = score::os::IoFunc::instance();

    // check if the write operation is allowed
    auto result = iofunc.iofunc_write_verify(ctp, msg, ocb, nullptr);
    if (!result.has_value())
    {
        return result.error();
    }

    // check if we are requested to do just a plain write
    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
    {
        return ENOSYS;
    }

    // get the number of bytes we were asked to write, check that there are enough bytes in the message
    const std::size_t nbytes = _IO_WRITE_GET_NBYTES(msg);
    const std::size_t nbytes_max = static_cast<std::size_t>(ctp->info.srcmsglen) - ctp->offset - sizeof(io_write_t);
    if (nbytes > nbytes_max)
    {
        return EBADMSG;
    }

    // do some actual data transfer from the message, just in case
    constexpr std::size_t bufsize = 64;
    int8_t buf[bufsize];
    if (!dispatch.resmgr_msgget(ctp, &buf[0], std::min(nbytes, bufsize), sizeof(msg->i)).has_value())
    {
        return EBADMSG;
    }

    // extract our test fixture, update the statistics
    auto& fixture = *static_cast<DispatchResourceManagerTest*>(ocb->attr);
    ++fixture.total_write_num;
    fixture.total_write_size += nbytes;

    // mark that we have consumed all the bytes
    _IO_SET_WRITE_NBYTES(ctp, nbytes);

    // tell the clients that we are able to take more data
    // (redundant in our "always ready" case, but provides code coverage for iofunc_notify_trigger)
    auto& notify = fixture.notify;
    iofunc.iofunc_notify_trigger(notify, 1, IOFUNC_NOTIFY_OUTPUT);

    // tell the framework that everything was OK
    return EOK;
}

std::int32_t DispatchResourceManagerTest::io_notify(resmgr_context_t* const ctp,
                                                    io_notify_t* const msg,
                                                    RESMGR_OCB_T* const ocb) noexcept
{
    auto& iofunc = score::os::IoFunc::instance();

    auto& fixture = *static_cast<DispatchResourceManagerTest*>(ocb->attr);
    auto& notify = fixture.notify;

    // 'trig' will tell iofunc_notify() which conditions are currently satisfied.
    const std::int32_t trig = _NOTIFY_COND_OUTPUT;  // clients can always give us data
    return iofunc.iofunc_notify(ctp, msg, notify, trig, NULL, NULL);
}

// private message handler is called from resmgr framework, but doesn't rely on it as much as IO handlers do
// It may need to do some things manually, but that gives it and its clients higher flexibility
std::int32_t DispatchResourceManagerTest::private_message_handler(message_context_t* const ctp,
                                                                  const std::int32_t /*code*/,
                                                                  const std::uint32_t /*flags*/,
                                                                  void* const handle) noexcept
{
    auto& channel = score::os::Channel::instance();

    // we only accept private requests from ourselves. Testing manually, as resmgr won't do it for us
    const pid_t their_pid = ctp->info.pid;
    const pid_t our_pid = ::getpid();
    if (their_pid != our_pid)
    {
        // Unblock the sender with an error reply. Resmgr won't be doing this for us.
        channel.MsgError(ctp->rcvid, EACCES);
        return EOK;
    }

    // extract our test fixture, raise to_exit flag
    auto& fixture = *static_cast<DispatchResourceManagerTest*>(handle);
    fixture.to_exit = true;

    // Unblock the sender with our normal reply. Resmgr won't be doing this for us, too.
    channel.MsgReply(ctp->rcvid, EOK, kNoReply, kNoSize);
    return EOK;
}

// a single iteration of the service thread loop. returns false for termination request
score::cpp::expected<bool, std::int32_t> DispatchResourceManagerTest::NextServiceRequest(dispatch_context_t* ctp) noexcept
{
    auto& dispatch = score::os::Dispatch::instance();
    const auto block_result = dispatch.dispatch_block(ctp);
    if (!block_result)
    {
        return score::cpp::make_unexpected(-1);
    }
    const auto handler_result = dispatch.dispatch_handler(ctp);
    if (!handler_result)
    {
        return score::cpp::make_unexpected(handler_result.error());
    }
    return !to_exit;
}

constexpr auto test_path = "/test/resmgr_unit_test_path";

TEST_F(DispatchResourceManagerTest, CheckResourceManagerHappyFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check Resource Manager Happy Flow");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto& dispatch = score::os::Dispatch::instance();

    // in order to reduce locking overhead, we explicitly disable locking on message handler list access
    const auto dpp_expected = dispatch.dispatch_create_channel(-1, DISPATCH_FLAG_NOLOCK);
    ASSERT_TRUE(dpp_expected);

    // dispatch handle (dpp). A pointer to an opaque structure that describes the service channel
    // (channel id, access rights and callbacks)
    dispatch_t* const dispatch_pointer = dpp_expected.value();

    // _RESMGR_FLAG_SELF flag is required to allow client connections from the same process
    // (beware of potential deadlocks)
    const auto id_expected = dispatch.resmgr_attach(
        dispatch_pointer, &resmgr_attr, test_path, _FTYPE_ANY, _RESMGR_FLAG_SELF, &connect_funcs, &io_funcs, this);
    ASSERT_TRUE(id_expected);
    const std::int32_t id = id_expected.value();

    // attach a private message handler to process service termination messages
    constexpr _message_attr* noAttr{nullptr};
    // constexpr void* noHandle{nullptr};
    EXPECT_TRUE(dispatch.message_attach(
        dispatch_pointer, noAttr, kPrivateMessageTypeFirst, kPrivateMessageTypeLast, &private_message_handler, this));

    // after this call, we won't be able to manipulate the message handler list (due to DISPATCH_FLAG_NOLOCK)
    const auto ctp_expected = dispatch.dispatch_context_alloc(dispatch_pointer);
    ASSERT_TRUE(ctp_expected);

    // context handle (ctp). A pointer to a defined structure that describes the current state of the dispatch thread
    // (dispatch handle, message data, client data, our user-specified data pointer)
    // There can be several such contexts per a single dispatch handle if thread pools are employed.
    dispatch_context_t* const context_pointer = ctp_expected.value();
    // Pay attention to a slight difference between dispatch_context_t and resmgr_context_t.
    // dispatch_context_t is a union of several contexts. In our case, it contains resmgr_context_t.

    // test our assumption that we actually don't need to store id and dpp separately for resmgr_detach() later
    EXPECT_EQ(context_pointer->resmgr_context.id, id);
    EXPECT_EQ(context_pointer->resmgr_context.dpp, dispatch_pointer);

    // create a client connection for private messages. This connection does not need a full-blown resmgr protocol.
    // In particular, it can be used to send service terminate messages and then be closed without errors.
    // On the other hand, posix calls won't work with this connection.
    const auto coid_expected = dispatch.message_connect(dispatch_pointer, MSG_FLAG_SIDE_CHANNEL);
    ASSERT_TRUE(coid_expected);
    const std::int32_t side_channel_coid = coid_expected.value();

    // launch the service listen/reply loop in a separate thread
    // the thread is supposed to finish after an _IO_MSG request request
    auto future = std::async(std::launch::async, [this, context_pointer]() noexcept {
        score::cpp::expected<bool, std::int32_t> result{};
        do
        {
            result = NextServiceRequest(context_pointer);
        } while (result.has_value() && result.value());
        return result;
    });

    // now, create a client and write to the service using standard POSIX calls. Check for expected results.
    const std::int32_t fd = ::open(test_path, O_RDWR);
    ASSERT_TRUE(fd != -1);
    constexpr std::size_t bufsize = 8;
    int8_t buf[bufsize];
    ::memset(&buf[0], 0, bufsize);
    EXPECT_EQ(::write(fd, buf, bufsize), bufsize);
    EXPECT_EQ(::write(fd, buf, bufsize), bufsize);
    EXPECT_EQ(::write(fd, buf, bufsize), bufsize);
    EXPECT_EQ(::write(fd, buf, 0), 0);  // empty write request will still be handled

    pollfd poll_fd{fd, POLLOUT, 0};
    EXPECT_EQ(::poll(&poll_fd, 1, 0), 1);  // our server is always ready to take data
    EXPECT_EQ(poll_fd.revents, POLLOUT);

    // send the service terminate message
    auto& channel = score::os::Channel::instance();
    const std::uint16_t msg{kPrivateMessageTerminate};
    const auto result = channel.MsgSend(side_channel_coid, &msg, sizeof(msg), kNoReply, kNoSize);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), EOK);

    // the service thread has terminated. Wait for it.
    future.wait();
    EXPECT_TRUE(future.get().has_value());

    // now, we can close the private connection. _IO_CLOSE won't be sent, so we will succeed even with no one handling
    // the service loop. As the service channel handle is not closed yet, ::close() would deadlock here.
    EXPECT_TRUE(channel.ConnectDetach(side_channel_coid));

    // close the service channel and free all the associated service structures
    // Now, _IO_CLOSE will be sent even without the service loop.
    EXPECT_TRUE(dispatch.resmgr_detach(dispatch_pointer, id, _RESMGR_DETACH_CLOSE));
    EXPECT_TRUE(dispatch.dispatch_destroy(dispatch_pointer));
    dispatch.dispatch_context_free(context_pointer);

    // channel is closed from the server side. As a resmgr client, we shall fail but not hang here
    // (if we tried to do it before we closed the channel, there would be a deadlock)
    EXPECT_EQ(::write(fd, buf, 0), -1);
    EXPECT_EQ(::close(fd), -1);

    // check that the write statistics was as expected
    EXPECT_EQ(total_write_num, 4);
    EXPECT_EQ(total_write_size, 3 * bufsize);
}

constexpr std::int32_t kTimerPulseCode = _PULSE_CODE_MINAVAIL;

// client-side dispatch shall be able to provide a functional equivalent to poll()
// so, timeouts and file descriptor select events shall be handled.
// In this test, we arm a timer, then in the timer event callback we write into a pipe,
// then we receive the select event from another side of the pipe.
struct DispatchClientTest : ::testing::Test
{
    std::unique_ptr<score::os::qnx::Timer> timer_;
    std::array<std::int32_t, 2> pipe_fds_;
    bool to_exit_;
    bool pulse_received_;
    bool select_received_;

    dispatch_t* dispatch_pointer_;
    dispatch_context_t* context_pointer_;
    std::int32_t side_channel_coid_;
    timer_t timer_id_;

    void SetUp() override
    {
        timer_ = std::make_unique<score::os::qnx::TimerImpl>();

        ::pipe(pipe_fds_.data());
        to_exit_ = false;
        pulse_received_ = false;
        select_received_ = false;
    };

    void TearDown() override
    {
        ::close(pipe_fds_[0]);
        ::close(pipe_fds_[1]);
    }

    void CreateDispatchChannel() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        const auto dpp_expected = dispatch.dispatch_create_channel(-1, 0);
        ASSERT_TRUE(dpp_expected);
        dispatch_pointer_ = dpp_expected.value();
    }

    void DestroyDispatchChannel() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        ASSERT_TRUE(dispatch.dispatch_destroy(dispatch_pointer_));
    }

    void AllocateDispatchContext() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        const auto ctp_expected = dispatch.dispatch_context_alloc(dispatch_pointer_);
        ASSERT_TRUE(ctp_expected);
        context_pointer_ = ctp_expected.value();
    }

    void FreeDispatchContext() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        dispatch.dispatch_context_free(context_pointer_);  // void
    }

    void AttachTimer() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        auto& timer = *timer_;
        const auto coid_expected = dispatch.message_connect(dispatch_pointer_, MSG_FLAG_SIDE_CHANNEL);
        ASSERT_TRUE(coid_expected);
        side_channel_coid_ = coid_expected.value();

        struct sigevent event;
        event.sigev_notify = SIGEV_PULSE;
        event.sigev_coid = side_channel_coid_;
        event.sigev_priority = SIGEV_PULSE_PRIO_INHERIT;
        event.sigev_code = kTimerPulseCode;
        event.sigev_value.sival_int = 0;
        const auto timer_id_expected = timer.TimerCreate(CLOCK_MONOTONIC, &event);
        ASSERT_TRUE(timer_id_expected);
        timer_id_ = timer_id_expected.value();
    }

    void DetachTimer() noexcept
    {
        auto& channel = score::os::Channel::instance();
        auto& timer = *timer_;
        ASSERT_TRUE(timer.TimerDestroy(timer_id_));
        ASSERT_TRUE(channel.ConnectDetach(side_channel_coid_));
    }

    void ArmTimer(std::uint64_t timeout_nsec) noexcept
    {
        auto& timer = *timer_;
        struct _itimer itimer;
        itimer.nsec = timeout_nsec;
        itimer.interval_nsec = 0;
        ASSERT_TRUE(timer.TimerSettime(timer_id_, 0, &itimer, NULL));
    }

    void AttachPulse() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        ASSERT_TRUE(dispatch.pulse_attach(dispatch_pointer_, 0, kTimerPulseCode, &pulse_func, this));
    }

    void DetachPulse() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        ASSERT_TRUE(dispatch.pulse_detach(dispatch_pointer_, kTimerPulseCode, 0));
    }

    void AttachSelect() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        ASSERT_TRUE(dispatch.select_attach(
            dispatch_pointer_, nullptr, pipe_fds_[0], SELECT_FLAG_READ | SELECT_FLAG_REARM, &select_func, this));
    }

    void DetachSelect() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        ASSERT_TRUE(dispatch.select_detach(dispatch_pointer_, pipe_fds_[0]));
    }

    void RunDispatchLoop() noexcept
    {
        auto& dispatch = score::os::Dispatch::instance();
        while (!to_exit_)
        {
            if (dispatch.dispatch_block(context_pointer_))
            {
                score::cpp::ignore = dispatch.dispatch_handler(context_pointer_);
            }
        }
    }

    std::int32_t OnPulse() noexcept
    {
        EXPECT_FALSE(pulse_received_);
        EXPECT_FALSE(select_received_);

        pulse_received_ = true;
        const std::uint8_t pipe_event{0};
        ::write(pipe_fds_[1], &pipe_event, sizeof(pipe_event));
        return 0;
    }

    std::int32_t OnSelect() noexcept
    {
        EXPECT_TRUE(pulse_received_);
        EXPECT_FALSE(select_received_);

        std::uint8_t pipe_event;
        ::read(pipe_fds_[0], &pipe_event, sizeof(pipe_event));
        select_received_ = true;
        to_exit_ = true;
        return 0;
    }

    static std::int32_t select_func(select_context_t* /*ctp*/,
                                    std::int32_t /*fd*/,
                                    std::uint32_t /*flags*/,
                                    void* const handle) noexcept
    {
        return static_cast<DispatchClientTest*>(handle)->OnSelect();
    }

    static std::int32_t pulse_func(message_context_t* /*ctp*/,
                                   std::int32_t /*code*/,
                                   std::uint32_t /*flags*/,
                                   void* handle) noexcept
    {
        return static_cast<DispatchClientTest*>(handle)->OnPulse();
    }
};

TEST_F(DispatchClientTest, DispatchClientHappyFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Client Happy Flow");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // prepare the client setup
    CreateDispatchChannel();
    AttachTimer();
    AttachPulse();
    AttachSelect();
    AllocateDispatchContext();

    ArmTimer(10000000);  // 10 ms

    RunDispatchLoop();

    EXPECT_TRUE(pulse_received_);
    EXPECT_TRUE(select_received_);

    // cleanup
    DetachTimer();
    DetachPulse();
    DetachSelect();
    DestroyDispatchChannel();
    FreeDispatchContext();
}

// shall be the last one in the tests, as it disables the abilities for the process
TEST(DispatchTestFinal, resmgr_attach_without_privileges_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Resmgr Attach Without Privileges Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto dpp = score::os::Dispatch::instance().dispatch_create();
    ASSERT_TRUE(dpp.has_value());

    // drop privileges:
    score::os::ProcMgr::instance().procmgr_ability(
        0, PROCMGR_ADN_ROOT | PROCMGR_AOP_DENY | PROCMGR_AOP_LOCK | PROCMGR_AID_EOL);

    const auto id = score::os::Dispatch::instance().resmgr_attach(
        dpp.value(), kNoAttr, test_path, _FTYPE_ANY, kOpenFlags, kNoConnectFuncs, kNoIoFuncs, kNoHandle);
    ASSERT_EQ(id.error(), score::os::Error::Code::kOperationNotPermitted);
}

TEST(DispatchCreateInstance, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PMR Default Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Dispatch::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::DispatchImpl*>(instance.get()));
}

}  // namespace
