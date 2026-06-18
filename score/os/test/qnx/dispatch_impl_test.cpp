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
#define THREAD_POOL_PARAM_T dispatch_context_t

#include "score/os/qnx/dispatch_impl.h"
#include "score/os/qnx/dispatch.h"

#include <gtest/gtest.h>

#include <future>

#include <sys/neutrino.h>
#include <sys/resmgr.h>
#include <sys/siginfo.h>

namespace
{

struct MessageData
{
    short type;
    struct sigevent event;
};

#ifndef __RCVID_T_SIZE
using rcvid_t = int;
#endif

static constexpr char kServerPath[]{"dispatch_impl_test_server_path"};

class DispatchImplFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = std::make_unique<score::os::DispatchImpl>();
    }
    void TearDown() override {}

    score::cpp::expected<name_attach_t*, score::os::Error> Attach()
    {
        constexpr std::uint32_t attach_flags{0U};
        constexpr dispatch_t* no_dispatch{nullptr};

        return unit_->name_attach(no_dispatch, kServerPath, attach_flags);
    }

    score::cpp::expected_blank<score::os::Error> Detach(name_attach_t* const attach)
    {
        return unit_->name_detach(attach, kDetachFlags);
    }

    static constexpr rcvid_t kInvalidId{-1};
    static constexpr std::int32_t kOpenFlags{0};
    static constexpr std::uint32_t kDetachFlags{0U};
    static constexpr std::size_t kNoBytes{0U};

    std::unique_ptr<score::os::Dispatch> unit_;
};

TEST_F(DispatchImplFixture, NameOpenCloseFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Name Open Close Flow");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const char* const path = "tmp";
    constexpr std::uint32_t flags{0U};
    constexpr dispatch_t* no_dispatch{nullptr};

    auto attach = unit_->name_attach(no_dispatch, path, flags);
    ASSERT_TRUE(attach);

    auto fd = unit_->name_open(path, flags);
    ASSERT_TRUE(fd);

    EXPECT_TRUE(unit_->name_close(fd.value()));
    EXPECT_TRUE(unit_->name_detach(attach.value(), flags));
}

TEST_F(DispatchImplFixture, dispatch_unblock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Dispatch Unblock");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    // attach message, otherwise dispatch_context_alloc will fail
    constexpr std::uint16_t kPrivateMessageTypeFirst{_IO_MAX + 1};
    constexpr std::uint16_t kPrivateMessageTypeLast{kPrivateMessageTypeFirst};
    constexpr _message_attr* no_attr{nullptr};
    constexpr void* no_handle{nullptr};
    EXPECT_TRUE(unit_->message_attach(
        dpp.value(), no_attr, kPrivateMessageTypeFirst, kPrivateMessageTypeLast, nullptr, no_handle));

    auto ctp = unit_->dispatch_context_alloc(dpp.value());
    ASSERT_TRUE(ctp) << ctp.error().ToString();

    unit_->dispatch_unblock(ctp.value());

    // clean up
    unit_->dispatch_context_free(ctp.value());
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, resmgr_detachReturnsErrorIfPassInvalidId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Resmgr Detach returns Error If Pass Invalid Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr rcvid_t invalid_id = -1;
    constexpr auto flags = 0;
    EXPECT_FALSE(unit_->resmgr_detach(dpp.value(), invalid_id, flags));

    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, thread_pool_create_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Thread Pool Create Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    thread_pool_attr_t pool_attr{};
    auto pool = unit_->thread_pool_create(&pool_attr, POOL_FLAG_EXIT_SELF);
    ASSERT_TRUE(pool.has_value());
}

TEST_F(DispatchImplFixture, select_attach_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Select Attach Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr select_attr_t* no_attr{nullptr};
    constexpr std::int32_t no_fd{-1};
    constexpr void* no_handle{nullptr};
    EXPECT_TRUE(unit_->select_attach(dpp.value(), no_attr, no_fd, SELECT_FLAG_SRVEXCEPT, nullptr, no_handle));

    auto ctp = unit_->dispatch_context_alloc(dpp.value());
    ASSERT_TRUE(ctp);

    // clean up
    unit_->dispatch_context_free(ctp.value());
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, select_attach_frozen_context_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Select Attach Frozen Context Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    constexpr std::int32_t no_chid{-1};
    auto dpp = unit_->dispatch_create_channel(no_chid, DISPATCH_FLAG_NOLOCK);
    ASSERT_TRUE(dpp);

    // pre-attach some (even invalid) fd, otherwise failing select_attach corrupts memory inside QNX code
    constexpr select_attr_t* no_attr{nullptr};
    constexpr std::int32_t no_fd{-1};
    constexpr void* no_handle{nullptr};
    EXPECT_TRUE(unit_->select_attach(dpp.value(), no_attr, no_fd, SELECT_FLAG_READ, nullptr, no_handle));

    // after this call, we won't be able to manipulate the message handler list (due to DISPATCH_FLAG_NOLOCK)
    auto ctp = unit_->dispatch_context_alloc(dpp.value());
    ASSERT_TRUE(ctp);

    auto ret = unit_->select_attach(dpp.value(), no_attr, no_fd, SELECT_FLAG_SRVEXCEPT, nullptr, no_handle);
    ASSERT_FALSE(ret);
    // The error is documented to be EINVAL, but the actual error returned is EBUSY

    // clean up
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
    unit_->dispatch_context_free(ctp.value());
}

TEST_F(DispatchImplFixture, select_detach_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Select Detach Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr select_attr_t* no_attr{nullptr};
    constexpr std::int32_t no_fd{-1};
    constexpr void* no_handle{nullptr};
    EXPECT_TRUE(unit_->select_attach(dpp.value(), no_attr, no_fd, SELECT_FLAG_SRVEXCEPT, nullptr, no_handle));

    auto ctp = unit_->dispatch_context_alloc(dpp.value());
    ASSERT_TRUE(ctp);

    EXPECT_TRUE(unit_->select_detach(dpp.value(), no_fd));

    // clean up
    unit_->dispatch_context_free(ctp.value());
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, select_detach_not_attached_fd_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Select Detach Not Attached Fd Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr std::int32_t no_fd{-1};
    auto ret = unit_->select_detach(dpp.value(), no_fd);
    ASSERT_FALSE(ret);
    EXPECT_EQ(ret.error(), score::os::Error::Code::kInvalidArgument);

    // clean up
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, pulse_attach_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Pulse Attach Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr std::int32_t no_flags{0};
    constexpr std::int32_t pulse_code{_PULSE_CODE_MINAVAIL};
    constexpr void* no_handle{nullptr};
    EXPECT_TRUE(unit_->pulse_attach(dpp.value(), no_flags, pulse_code, nullptr, no_handle));

    auto ctp = unit_->dispatch_context_alloc(dpp.value());
    ASSERT_TRUE(ctp);

    // clean up
    unit_->dispatch_context_free(ctp.value());
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, pulse_attach_frozen_context_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Pulse Attach Frozen Context Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    constexpr std::int32_t no_chid{-1};
    auto dpp = unit_->dispatch_create_channel(no_chid, DISPATCH_FLAG_NOLOCK);
    ASSERT_TRUE(dpp);

    // attach message, otherwise dispatch_context_alloc will fail
    constexpr std::uint16_t kPrivateMessageTypeFirst{_IO_MAX + 1};
    constexpr std::uint16_t kPrivateMessageTypeLast{kPrivateMessageTypeFirst};
    constexpr _message_attr* no_attr{nullptr};
    constexpr void* no_handle{nullptr};
    EXPECT_TRUE(unit_->message_attach(
        dpp.value(), no_attr, kPrivateMessageTypeFirst, kPrivateMessageTypeLast, nullptr, no_handle));

    // after this call, we won't be able to manipulate the message handler list (due to DISPATCH_FLAG_NOLOCK)
    auto ctp = unit_->dispatch_context_alloc(dpp.value());
    ASSERT_TRUE(ctp);

    constexpr std::int32_t no_flags{0};
    constexpr std::int32_t pulse_code{_PULSE_CODE_MINAVAIL};
    auto ret = unit_->pulse_attach(dpp.value(), no_flags, pulse_code, nullptr, no_handle);
    ASSERT_FALSE(ret);
    EXPECT_EQ(ret.error(), score::os::Error::Code::kInvalidArgument);

    // clean up
    unit_->dispatch_context_free(ctp.value());
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, pulse_detach_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Pulse Detach Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr std::int32_t no_flags{0};
    constexpr std::int32_t pulse_code{_PULSE_CODE_MINAVAIL};
    constexpr void* no_handle{nullptr};
    EXPECT_TRUE(unit_->pulse_attach(dpp.value(), no_flags, pulse_code, nullptr, no_handle));

    auto ctp = unit_->dispatch_context_alloc(dpp.value());
    ASSERT_TRUE(ctp);

    EXPECT_TRUE(unit_->pulse_detach(dpp.value(), pulse_code, no_flags));

    // clean up
    unit_->dispatch_context_free(ctp.value());
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, pulse_detach_not_attached_code_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Pulse Detach Not Attached Code Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto dpp = unit_->dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr std::int32_t no_flags{0};
    constexpr std::int32_t pulse_code{_PULSE_CODE_MINAVAIL};
    auto ret = unit_->pulse_detach(dpp.value(), pulse_code, no_flags);
    ASSERT_FALSE(ret);
    EXPECT_EQ(ret.error(), score::os::Error::Code::kInvalidArgument);

    // clean up
    EXPECT_TRUE(unit_->dispatch_destroy(dpp.value()));
}

TEST_F(DispatchImplFixture, msg_deliver_event_returns_error_if_invalid_rcvid)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Msg Deliver Event returns error if invalid rcvid is passed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    constexpr sigevent* no_event{nullptr};

    const auto result = unit_->msg_deliver_event(kInvalidId, no_event);

    EXPECT_FALSE(result.has_value());
}

TEST_F(DispatchImplFixture, msg_deliver_event_flow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Msg Deliver Event flow");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    constexpr auto kPulseCode{_PULSE_CODE_MINAVAIL + 6};

    auto attach = Attach();
    ASSERT_TRUE(attach);

    auto client = unit_->name_open(kServerPath, kOpenFlags);
    ASSERT_TRUE(client);

    const auto coid = client.value();
    const auto chid = attach.value()->chid;

    auto future = std::async(std::launch::async, [coid, chid, kPulseCode]() noexcept {
        MessageData msg{};
        _pulse pulse{};

        SIGEV_PULSE_INIT(&msg.event, coid, SIGEV_PULSE_PRIO_INHERIT, kPulseCode, 0);

        if (::MsgRegisterEvent(&msg.event, coid) == -1)
        {
            return -1;
        }

        if (::MsgSend(coid, &msg, sizeof(msg), nullptr, 0) == -1)
        {
            return -1;
        }

        if (::MsgReceivePulse(chid, &pulse, sizeof(pulse), nullptr) == -1)
        {
            return -1;
        }

        if (::MsgUnregisterEvent(&msg.event) == -1)
        {
            return -1;
        }

        return static_cast<int>(pulse.code);
    });

    union
    {
        MessageData mine;
        struct _pulse pulse;
    } msg{};

    const auto rcvid = ::MsgReceive(chid, &msg, sizeof(msg), nullptr);
    ASSERT_NE(rcvid, -1);

    const auto deliver_result = unit_->msg_deliver_event(rcvid, &msg.mine.event);
    EXPECT_TRUE(deliver_result.has_value()) << deliver_result.error().ToString();
    EXPECT_EQ(::MsgReply(rcvid, EOK, nullptr, kNoBytes), EOK);

    EXPECT_EQ(future.get(), kPulseCode);

    EXPECT_TRUE(unit_->name_close(coid));
    EXPECT_TRUE(Detach(attach.value()));
}

TEST_F(DispatchImplFixture, thread_pool_start_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Thread Pool Start Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    thread_pool_attr_t pool_attr{};
    auto dpp = ::dispatch_create();
    ASSERT_TRUE(dpp);

    constexpr std::uint16_t kMinBlockedThreads{2U};
    constexpr std::uint16_t kNewThreadsIncrement{1U};
    constexpr std::uint16_t kMaxBlockedThreads{4U};
    constexpr std::uint16_t kMaxThreads{8U};

    pool_attr.handle = dpp;
    pool_attr.context_alloc = &dispatch_context_alloc;
    pool_attr.block_func = &dispatch_block;
    pool_attr.unblock_func = &dispatch_unblock;
    pool_attr.handler_func = &dispatch_handler;
    pool_attr.context_free = &dispatch_context_free;
    pool_attr.error_func = [](unsigned flags, int err_value) {
        return;
    };
    pool_attr.lo_water = kMinBlockedThreads;
    pool_attr.hi_water = kMaxBlockedThreads;
    pool_attr.increment = kNewThreadsIncrement;
    pool_attr.maximum = kMaxThreads;

    auto pool = ::thread_pool_create(&pool_attr, POOL_FLAG_CALL_HANDLE_ERRF);
    ASSERT_NE(pool, nullptr);

    const auto thread_pool_start_result = unit_->thread_pool_start(pool);
    EXPECT_TRUE(thread_pool_start_result.has_value());
    ASSERT_EQ(thread_pool_start_result.value(), EOK);

    EXPECT_EQ(thread_pool_destroy(pool), 0);

    // clean up
    EXPECT_TRUE(unit_->dispatch_destroy(dpp));
}

}  // namespace
