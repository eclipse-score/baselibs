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
#include "score/os/mocklib/qnx/mock_channel.h"
#include "score/os/qnx/channel_impl.h"
#include "score/os/qnx/dispatch.h"

#include <future>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

constexpr std::int32_t invalid_id{-1};
constexpr std::int32_t kAttachFlags{0};
constexpr std::uint32_t kAttachIndex{0U};
constexpr std::uint32_t kAttachId{0U};
constexpr pid_t kInvalidPid{0};
// scoid{-1} is reserved for the current calling process
constexpr std::int32_t invalid_scoid{0};

constexpr std::int32_t calling_thread_priority{-1};
constexpr std::int32_t code{0};
constexpr std::int32_t value{0};
constexpr std::int32_t code_ptr{1};
static_assert(sizeof(void*) == sizeof(std::uint64_t), "64-bit 'pointer' pulses");
constexpr std::uint64_t value_uintptr{0x123456789ABCDEF0UL};
/* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET: "Pointer" argument is just a 64-bit integral value, see [TicketOld-81106] */
void* const value_ptr{reinterpret_cast<void*>(value_uintptr)};
/* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET: "Pointer" argument is just a 64-bit integral value, see [TicketOld-81106] */

constexpr std::int32_t error{EACCES};
constexpr std::int64_t status{42L};
constexpr void* no_msg{nullptr};
constexpr void* no_reply{nullptr};
constexpr std::size_t no_bytes{0U};
constexpr _msg_info* no_info{nullptr};
constexpr _client_info* no_client_info{nullptr};
constexpr std::int32_t ngroups{0};
constexpr sigevent* no_event{nullptr};

// Mock test

struct ChannelMockTest : ::testing::Test
{
    void SetUp() override
    {
        score::os::Channel::set_testing_instance(channelmock);
    };
    void TearDown() override
    {
        score::os::Channel::restore_instance();
    };

    score::os::MockChannel channelmock;
};

TEST_F(ChannelMockTest, MsgReceive)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Receive");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgReceive);
    score::os::Channel::instance().MsgReceive(invalid_id, no_msg, no_bytes, no_info);
}

TEST_F(ChannelMockTest, MsgReceivePulse)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message receive Pulse");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgReceivePulse);
    score::os::Channel::instance().MsgReceivePulse(invalid_id, no_msg, no_bytes, no_info);
}

TEST_F(ChannelMockTest, MsgReply)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Reply");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgReply);
    score::os::Channel::instance().MsgReply(invalid_id, status, no_reply, no_bytes);
}

TEST_F(ChannelMockTest, MsgError)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Error");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgError);
    score::os::Channel::instance().MsgError(invalid_id, error);
}

TEST_F(ChannelMockTest, MsgSend)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Send");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgSend);
    score::os::Channel::instance().MsgSend(invalid_id, no_msg, no_bytes, no_reply, no_bytes);
}

TEST_F(ChannelMockTest, MsgSendPulse)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message send Pulse");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgSendPulse);
    score::os::Channel::instance().MsgSendPulse(invalid_id, calling_thread_priority, code, value);
}

TEST_F(ChannelMockTest, MsgSendPulsePtr)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message send Pulse Ptr");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgSendPulsePtr);
    score::os::Channel::instance().MsgSendPulsePtr(invalid_id, calling_thread_priority, code_ptr, value_ptr);
}

TEST_F(ChannelMockTest, MsgDeliverEvent)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Message Deliver Event");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgDeliverEvent);
    score::os::Channel::instance().MsgDeliverEvent(invalid_id, no_event);
}

TEST_F(ChannelMockTest, ConnectClientInfo)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Client Info");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, ConnectClientInfo);
    score::os::Channel::instance().ConnectClientInfo(invalid_scoid, no_client_info, ngroups);
}

TEST_F(ChannelMockTest, ConnectAttach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Attach");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, ConnectAttach);
    score::os::Channel::instance().ConnectAttach(kAttachId, kInvalidPid, invalid_id, kAttachIndex, kAttachFlags);
}

TEST_F(ChannelMockTest, ConnectDetach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Connect Detach");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, ConnectDetach);
    score::os::Channel::instance().ConnectDetach(invalid_id);
}

TEST_F(ChannelMockTest, MsgRegisterEvent)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Msg Register Event");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(channelmock, MsgRegisterEvent);
    score::os::Channel::instance().MsgRegisterEvent(no_event, invalid_id);
}

// Tests of the real stuff

TEST(ChannelTest, CoverUnhappyPaths)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Cover Unhappy Paths");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto& channel = score::os::Channel::instance();

    EXPECT_FALSE(channel.MsgReceive(invalid_id, no_msg, no_bytes, no_info));
    EXPECT_FALSE(channel.MsgReceivePulse(invalid_id, no_msg, no_bytes, no_info));
    EXPECT_FALSE(channel.MsgReply(invalid_id, status, no_reply, no_bytes));
    EXPECT_FALSE(channel.MsgError(invalid_id, error));
    EXPECT_FALSE(channel.MsgSend(invalid_id, no_msg, no_bytes, no_reply, no_bytes));
    EXPECT_FALSE(channel.MsgSendPulse(invalid_id, calling_thread_priority, code, value));
    EXPECT_FALSE(channel.MsgSendPulsePtr(invalid_id, calling_thread_priority, code, value_ptr));
    EXPECT_FALSE(channel.MsgDeliverEvent(invalid_id, no_event));
    EXPECT_FALSE(channel.ConnectAttach(kAttachId, kInvalidPid, invalid_id, kAttachIndex, kAttachFlags));
    EXPECT_FALSE(channel.ConnectDetach(invalid_id));

    EXPECT_FALSE(channel.ConnectClientInfo(invalid_scoid, no_client_info, ngroups));
}

namespace
{

union message_data
{
    _pulse pulse;
    struct message_t
    {
        uint16_t type;
        uint16_t subtype;
        uint8_t data;
    } message;
};

bool got_message{false};
bool got_pulse{false};
bool got_pulse_ptr{false};

// service listen/reply loop body
score::cpp::expected<bool, score::os::Error> NextServiceRequest(const name_attach_t* const attach)
{
    auto& channel = score::os::Channel::instance();
    message_data msg;
    _msg_info info;

    const auto rcv = channel.MsgReceive(attach->chid, &msg, sizeof(msg), &info);
    if (!rcv.has_value())
    {
        return score::cpp::make_unexpected(rcv.error());
    }

    const int rcvid = rcv.value();
    const bool is_pulse = rcvid == 0;

    if (is_pulse)
    {
        switch (msg.pulse.code)
        {
            case _PULSE_CODE_DISCONNECT:
            {
                const auto result = channel.ConnectDetach(msg.pulse.scoid);
                if (!result.has_value())
                {
                    return score::cpp::make_unexpected(result.error());
                }
                // successfully finish the loop after our (single) client disconnects
                return false;
            }
            case _PULSE_CODE_UNBLOCK:
                return true;
            case code:
            case code_ptr:
            {
                _client_info cinfo;
                const auto result = channel.ConnectClientInfo(msg.pulse.scoid, &cinfo, ngroups);
                if (!result.has_value())
                {
                    return score::cpp::make_unexpected(result.error());
                }
                if (msg.pulse.code == code)
                {
                    if (got_pulse || msg.pulse.value.sival_int != value)
                    {
                        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
                    }
                    got_pulse = true;
                }
                else if (msg.pulse.code == code_ptr)
                {
                    /* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET: ...[TicketOld-81106] */
                    const auto uintptr = reinterpret_cast<uint64_t>(msg.pulse.value.sival_ptr);
                    /* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET: ...[TicketOld-81106] */

                    if (got_pulse_ptr || uintptr != value_uintptr)
                    {
                        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
                    }
                    got_pulse_ptr = true;
                }
                return true;
            }
            default:
                if (msg.pulse.code >= 0)
                {
                    return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
                }
                return true;
        }
    }

    // stuff for handling name_attach-related messages
    if (msg.message.type == _IO_CONNECT)
    {
        const auto result = channel.MsgReply(rcvid, EOK, no_reply, no_bytes);
        if (!result.has_value())
        {
            return score::cpp::make_unexpected(result.error());
        }
        return true;
    }
    if (msg.message.type > _IO_BASE && msg.message.type <= _IO_MAX)
    {
        const auto result = channel.MsgError(rcvid, ENOSYS);
        if (!result.has_value())
        {
            return score::cpp::make_unexpected(result.error());
        }
        return true;
    }

    // handling our messages
    if (got_message || msg.message.type != 0 || msg.message.data != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }

    _client_info cinfo;
    auto result = channel.ConnectClientInfo(info.scoid, &cinfo, ngroups);
    if (!result.has_value())
    {
        return score::cpp::make_unexpected(result.error());
    }

    result = channel.MsgReply(rcvid, status, no_reply, no_bytes);
    if (!result.has_value())
    {
        return score::cpp::make_unexpected(result.error());
    }
    got_message = true;
    return true;
}

}  // namespace

TEST(ChannelTest, CheckHappyFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check Happy Flow");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto& channel = score::os::Channel::instance();
    auto& dispatch = score::os::Dispatch::instance();

    constexpr std::int32_t open_flags{0};
    constexpr std::uint32_t attach_flags{0U};
    constexpr std::uint32_t detach_flags{0U};
    constexpr dispatch_t* no_dispatch{nullptr};

    auto attach = dispatch.name_attach(no_dispatch, "test_server_path", attach_flags);
    EXPECT_TRUE(attach);

    // launch the service listen/reply loop in a separate thread
    auto future = std::async(std::launch::async, [attach]() noexcept {
        score::cpp::expected<bool, score::os::Error> result;
        do
        {
            result = NextServiceRequest(attach.value());
        } while (result.has_value() && result.value());
        return result;
    });

    auto client = dispatch.name_open("test_server_path", open_flags);
    EXPECT_TRUE(client);
    EXPECT_TRUE(channel.MsgSendPulse(client.value(), calling_thread_priority, code, value));
    EXPECT_TRUE(channel.MsgSendPulsePtr(client.value(), calling_thread_priority, code_ptr, value_ptr));

    message_data data{};
    EXPECT_EQ(channel.MsgSend(client.value(), &data, sizeof(data), no_reply, no_bytes).value(), status);

    // trigger reply with MsgError()
    data.message.type = _IO_BASE + 1;
    EXPECT_EQ(channel.MsgSend(client.value(), &data, sizeof(data), no_reply, no_bytes).error(),
              score::os::Error::createFromErrno(ENOSYS));

    // MsgSend() is synchronous and has the same priority as (asynchronous) pulses before it,
    // so, after we get a reply, the pulses have been received as well. We can disconnect the client
    EXPECT_TRUE(dispatch.name_close(client.value()));

    // wait till the service reply loop finishes
    future.wait();
    EXPECT_TRUE(future.get().has_value());
    EXPECT_TRUE(got_message && got_pulse && got_pulse_ptr);
    EXPECT_TRUE(dispatch.name_detach(attach.value(), detach_flags));
}

TEST(ChannelCreateInstance, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PMR Default Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Channel::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::ChannelImpl*>(instance.get()));
}

}  // namespace
