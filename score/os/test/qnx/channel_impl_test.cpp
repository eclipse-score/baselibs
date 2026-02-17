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
#include "score/os/qnx/channel_impl.h"
#include "score/os/qnx/dispatch_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>

#include <sys/neutrino.h>  // signals
#include <sys/siginfo.h>   // events

namespace
{

// ------------- score::os::ChannelImpl -----------

struct MessageData
{
    short type;
    struct sigevent event;
};

static constexpr char kServerPath[]{"test_server_path"};

class ChannelImplFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        dispatch_ = std::make_unique<score::os::DispatchImpl>();
        unit_ = std::make_unique<score::os::ChannelImpl>();
    }

    void TearDown() override {}

    /// \brief Attach to channel by predefined path
    score::cpp::expected<name_attach_t*, score::os::Error> Attach()
    {
        constexpr std::uint32_t attach_flags{0U};
        constexpr dispatch_t* no_dispatch{nullptr};

        return dispatch_->name_attach(no_dispatch, kServerPath, attach_flags);
    }

    /// \brief Detach from channel
    score::cpp::expected_blank<score::os::Error> Detach(name_attach_t* const attach)
    {
        return dispatch_->name_detach(attach, kDetachFlags);
    }

    static constexpr std::int32_t kInvalidId{-1};
    static constexpr std::int64_t kStatus{42L};
    static constexpr std::int32_t kOpenFlags{0};
    static constexpr std::int32_t kAttachFlags{0};
    static constexpr std::uint32_t kAttachIndex{0U};
    static constexpr std::uint32_t kAttachId{0U};
    static constexpr pid_t kInvalidPid{0};
    static constexpr std::uint32_t kDetachFlags{0U};
    static constexpr void* kNoMsg{nullptr};
    static constexpr void* kNoReply{nullptr};
    static constexpr std::size_t kNoBytes{0U};
    static constexpr _msg_info* kNoInfo{nullptr};

    std::unique_ptr<score::os::Dispatch> dispatch_;
    std::unique_ptr<score::os::Channel> unit_;
};

TEST_F(ChannelImplFixture, MsgReceiveReturnsErrorIfInvalidChId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message receive returns Error If Invalid Ch Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto result = unit_->MsgReceive(kInvalidId, kNoMsg, kNoBytes, kNoInfo);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgReceivevReturnsErrorIfInvalidChId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message receivev returns Error If Invalid Ch Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto riov_size = 8;
    iov_t riov[riov_size]{};

    const auto result = unit_->MsgReceivev(kInvalidId, riov, riov_size, kNoInfo);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgReceivePulseReturnsErrorIfInvalidChId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message receive Pulse returns Error If Invalid Ch Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto result = unit_->MsgReceivePulse(kInvalidId, kNoMsg, kNoBytes, kNoInfo);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgReplyReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message reply returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto buff_size = 255;
    char buff[buff_size];

    const auto result = unit_->MsgReply(kInvalidId, kStatus, buff, buff_size);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgReplyvReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message reply returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto riov_size = 8;
    iov_t riov[riov_size]{};

    const auto result = unit_->MsgReplyv(kInvalidId, kStatus, riov, riov_size);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgErrorReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Error returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr std::int32_t error = EOK;
    const auto result = unit_->MsgError(kInvalidId, error);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgSendReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message send returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MessageData data{};
    const auto result = unit_->MsgSend(kInvalidId, &data, sizeof(data), kNoReply, kNoBytes);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgSendvReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Sendv returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto iov_size = 8;
    iov_t siov[iov_size]{};
    iov_t riov[iov_size]{};

    const auto result = unit_->MsgSendv(kInvalidId, siov, iov_size, riov, iov_size);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, SetIovFillsPredefinedMsgData)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Set Iov Fills Predefined Msg Data");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    iov_t msg{};
    MessageData data{};
    auto* data_ptr = static_cast<void*>(&data);

    unit_->SetIov(&msg, data_ptr, sizeof(data));

    EXPECT_EQ(msg.iov_len, sizeof(data));
    EXPECT_EQ(msg.iov_base, data_ptr);
}

TEST_F(ChannelImplFixture, SetIovConstFillsPredefinedMsgData)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Set Iov Const Fills Predefined Msg Data");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    iov_t msg{};
    MessageData data{};
    const auto* data_ptr = static_cast<void*>(&data);

    unit_->SetIovConst(&msg, data_ptr, sizeof(data));

    EXPECT_EQ(msg.iov_len, sizeof(data));
    EXPECT_EQ(msg.iov_base_const, data_ptr);
}

TEST_F(ChannelImplFixture, MsgSendPulseReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message send Pulse returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr std::int32_t calling_thread_priority{-1};
    constexpr std::int32_t code{0};
    constexpr std::int32_t value{0};

    const auto result = unit_->MsgSendPulse(kInvalidId, calling_thread_priority, code, value);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgSendPulsePtrReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message send Pulse Ptr returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr std::int32_t calling_thread_priority{-1};
    constexpr std::int32_t code{0};
    std::int32_t value{0};

    const auto result = unit_->MsgSendPulsePtr(kInvalidId, calling_thread_priority, code, &value);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgDeliverEventReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Deliver Event returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr sigevent* no_event{nullptr};
    const auto result = unit_->MsgDeliverEvent(kInvalidId, no_event);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, ConnectClientInfoReturnsErrorIfNonExistingCoid)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Client Info returns Error If Non Existing Coid");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr int32_t non_existing_coid{std::numeric_limits<int32_t>::min()};
    constexpr _client_info* no_client_info{nullptr};
    constexpr std::int32_t ngroups{0};

    const auto result = unit_->ConnectClientInfo(non_existing_coid, no_client_info, ngroups);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, ConnectServerInfoReturnsErrorIfNonExistingPid)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Client Info returns Error If Non Existing Coid");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr int32_t invalid_pid{-1};  // kInvalidPid value 0 means current process here
    constexpr int32_t non_existing_scoid{std::numeric_limits<int32_t>::min()};
    constexpr _server_info* no_server_info{nullptr};

    const auto result = unit_->ConnectServerInfo(invalid_pid, non_existing_scoid, no_server_info);

    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, ConnectAttachReturnsErrorIfInvalidInput)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Attach returns Error If Invalid Input");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto result = unit_->ConnectAttach(kAttachId, kInvalidPid, kInvalidId, kAttachIndex, kAttachFlags);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, ConnectDetachReturnsErrorIfInvalidRcvId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Detach returns Error If Invalid Rcv Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto result = unit_->ConnectDetach(kInvalidId);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, ConnectAttachAndDetatchFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Attach And Detatch Flow");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto chid = ChannelCreate(kOpenFlags);
    const auto coid = this->unit_->ConnectAttach(kAttachId, kAttachId, chid, kAttachIndex, kAttachFlags);
    EXPECT_TRUE(coid.has_value());
    const auto result = this->unit_->ConnectDetach(coid.value());
    EXPECT_TRUE(result.has_value());
}

TEST_F(ChannelImplFixture, ConnectAttachAndMsgRegisterUnregisterEventFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Attach And Msg Register Event Flow");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto chid = ChannelCreate(kOpenFlags);
    const auto coid = this->unit_->ConnectAttach(kAttachId, kAttachId, chid, kAttachIndex, kAttachFlags);
    EXPECT_TRUE(coid.has_value());
    struct sigevent event;
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGUSR1;
    event.sigev_value.sival_ptr = NULL;
    auto result = this->unit_->MsgRegisterEvent(&event, coid.value());
    EXPECT_TRUE(result.has_value());
    result = this->unit_->MsgUnregisterEvent(&event);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgRegisterEventReturnsError)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MsgRegisterEvent returns error if called with invalid connection id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr sigevent* no_event{nullptr};
    const auto result = this->unit_->MsgRegisterEvent(no_event, kInvalidId);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgUnregisterEventReturnsError)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MsgRegisterEvent returns error if called with invalid connection id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct sigevent event{};
    SIGEV_SIGNAL_INIT(&event, SIGUSR2);
    const auto result = this->unit_->MsgUnregisterEvent(&event);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ChannelImplFixture, MsgDeliverEventFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Deliver Event Flow");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // This test sends MsgDeliverEvent from main thread to client.
    // Sequence call:
    /* Main thread                      Client thread:
           MsgReceive - blocked             ...
                                            MsgRegisterEvent
                                            MsgSend - blocked
           MsgReceive - unblocked
           MsgDeliverEvent
           MsgReply
                                            MsgSend - unblocked
                                            ConnectServerInfo - connection still exists
                                            MsgReceivePulse
                                            MsgUnregisterEvent
    */
    constexpr auto MY_PULSE_CODE{_PULSE_CODE_MINAVAIL + 5};

    auto attach = Attach();
    ASSERT_TRUE(attach);

    auto client = dispatch_->name_open(kServerPath, kOpenFlags);
    EXPECT_TRUE(client);

    const auto coid = client.value();
    const auto chid = attach.value()->chid;

    // client thread get notified by main thread receiving event
    auto future = std::async(std::launch::async, [this, coid, chid, MY_PULSE_CODE]() noexcept {
        MessageData msg{};
        _pulse pulse{};

        SIGEV_PULSE_INIT(&msg.event, coid, SIGEV_PULSE_PRIO_INHERIT, MY_PULSE_CODE, 0);

        if (!this->unit_->MsgRegisterEvent(&msg.event, coid).has_value())
        {
            std::cerr << "error occurs when call MsgRegisterEvent()" << std::endl;
            return -1;
        }

        // give the pulse event we initialized above to the server for later delivery
        if (this->unit_->MsgSend(coid, &msg, sizeof(msg), nullptr, 0).has_value() == false)
        {
            std::cerr << "error occurs when call MsgSend()" << std::endl;
            return -1;
        }

        // check that the connection still exists
        auto coid_expected = this->unit_->ConnectServerInfo(0, coid, nullptr);
        if (!coid_expected.has_value() || (coid_expected.value() != coid))
        {
            std::cerr << "error occurs when call ConnectServerInfo()" << std::endl;
            return -1;
        }

        // wait for the pulse from the server
        const auto rcvid = this->unit_->MsgReceivePulse(chid, &pulse, sizeof(pulse), nullptr);
        if (!rcvid.has_value())
        {
            std::cerr << "error occurs when call MsgReceivePulse()" << std::endl;
            return -1;
        }

        if (!this->unit_->MsgUnregisterEvent(&msg.event).has_value())
        {
            std::cerr << "error occurs when call MsgUnregisterEvent()" << std::endl;
            return -1;
        }

        // must be equal MY_PULSE_CODE, checking in main thread
        return static_cast<int>(pulse.code);
    });  // end client

    union
    {
        struct MessageData mine;
        struct _pulse pulse;
    } msg;

    // wait until client thread will be ready to receive any messages.
    // Client is blocked by MsgSend
    auto rcvid = unit_->MsgReceive(chid, &msg, sizeof(msg), nullptr);
    EXPECT_TRUE(rcvid);
    // meanwhile client is still blocked, deliver an event to it
    auto dlv_result = unit_->MsgDeliverEvent(rcvid.value(), &msg.mine.event);
    EXPECT_TRUE(dlv_result) << dlv_result.error().ToString();
    // unblock MsgSend client call
    const auto rep_result = unit_->MsgReply(rcvid.value(), EOK, nullptr, kNoBytes);
    EXPECT_TRUE(rep_result) << rep_result.error().ToString();
    // wait for completion of client thread
    future.wait();
    EXPECT_EQ(future.get(), MY_PULSE_CODE);

    EXPECT_TRUE(Detach(attach.value()));
}

TEST_F(ChannelImplFixture, MessageFlow_SendReceiveReply)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Flow: Send receive Reply");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    /*
        MsgSendv is called from main thread and gets blocked until client reads the message by MsgReceivev and reply
        with MsgReplyv
        Sequence call:
            Main thread                 Client thread
                ...                         MsgReceivev - blocked
                MsgSendv - blocked          ...
                                            MsgReceivev - unblocked
                                            MsgReplyv
                MsgSendv - unblocked
    */
    auto attach = Attach();
    ASSERT_TRUE(attach);

    auto client = dispatch_->name_open(kServerPath, kOpenFlags);
    EXPECT_TRUE(client);

    // receive a message in a separate thread
    auto future = std::async(std::launch::async, [this, chid = attach.value()->chid]() noexcept {
        iov_t riov{};
        const auto rcvid = this->unit_->MsgReceivev(chid, &riov, 1, kNoInfo);
        if (rcvid.has_value() == false)
        {
            std::cerr << "error occurs when call MsgReceivev(): " << rcvid.error().ToString();
            return false;
        }
        const auto result = this->unit_->MsgReplyv(rcvid.value(), EOK, nullptr, kNoBytes);
        return result.has_value();
    });

    iov_t siov{};
    iov_t riov{};
    const auto result = unit_->MsgSendv(client.value(), &siov, 1, &riov, 1);
    EXPECT_TRUE(result.has_value());

    future.wait();
    EXPECT_TRUE(future.get());

    EXPECT_TRUE(Detach(attach.value()));
}

}  // namespace
