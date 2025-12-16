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
#include "score/os/qnx/neutrino_impl.h"
#include <score/jthread.hpp>
#include <condition_variable>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using Neutrino = score::os::qnx::Neutrino;

struct NeutrinoTest : public ::testing::Test
{
    void SetUp() override
    {
        server_name_ = std::string("test_") + ::testing::UnitTest::GetInstance()->current_test_info()->name();
        test_completed_ = false;
        StartWatchdog(test_completed_, kMaxTimeoutMilliseconds);
        neutrino_ = std::make_unique<score::os::qnx::NeutrinoImpl>();
        dispatch_ = std::make_unique<score::os::DispatchImpl>();
        channel_ = std::make_unique<score::os::ChannelImpl>();

        memset(sample_request, 0, kMinMessageSize);
        memset(sample_response, 0, kMinMessageSize);

        memcpy(sample_request, kSampleRequestMessage.c_str(), kSampleRequestMessage.size());
        memcpy(sample_response, kSampleReponseMessage.c_str(), kSampleReponseMessage.size());
    }

    void TearDown() override
    {
        test_completed_ = true;
        if (watchdog_.joinable())
        {
            watchdog_.join();
        }
        memset(sample_request, 0, kMinMessageSize);
        memset(sample_response, 0, kMinMessageSize);
    }

    score::cpp::expected<std::tuple<std::int32_t, name_attach_t*, dispatch_t*>, score::os::Error> CreateNamedServer(
        const Neutrino::ChannelFlag flags = Neutrino::ChannelFlag::kPrivate,
        const std::string& server_name = std::string());
    score::cpp::expected_blank<score::os::Error> DestroyNamedServer(name_attach_t* name_attach, dispatch_t* dispatch);
    score::cpp::expected<std::int32_t, score::os::Error> CreateNamedServerClient(const std::string& server_name = std::string());
    score::cpp::expected_blank<score::os::Error> DestroyClient(const std::int32_t channel_id);
    score::cpp::expected<std::int32_t, score::os::Error> SetTimerTimeout(const Neutrino::ClockType clock_type,
                                                                const Neutrino::TimerTimeoutFlag flags,
                                                                const std::chrono::nanoseconds timeout);
    score::cpp::expected_blank<score::os::Error> SendMessage(
        const std::int32_t connection_id,
        const iov_t& request,
        iov_t& response,
        std::optional<
            std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
            timeout = std::nullopt);
    score::cpp::expected<std::int32_t, score::os::Error> ReceiveMessage(
        const std::int32_t channel_id,
        iov_t& request,
        std::optional<
            std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
            timeout = std::nullopt,
        std::optional<_msg_info> message_info = std::nullopt);
    score::cpp::expected_blank<score::os::Error> ReplyMessage(const std::int32_t receive_message_id, const iov_t& response);
    void StartWatchdog(std::atomic<bool>& test_done, int max_timeout_ms);

    std::string server_name_{};
    static constexpr auto kMinMessageSize = sizeof(_pulse);
    static constexpr std::uint32_t kMaxTimeoutMilliseconds{500};
    static constexpr std::uint32_t kDefaultTimerTimeoutMilliseconds{25};
    static constexpr std::uint8_t kClientCreationTrials{10};

    char sample_request[kMinMessageSize];
    char sample_response[kMinMessageSize];
    const std::string kSampleRequestMessage{"SampleRequest"};
    const std::string kSampleReponseMessage{"SampleResponse"};

    std::unique_ptr<score::os::qnx::Neutrino> neutrino_;
    std::unique_ptr<score::os::Dispatch> dispatch_;
    std::unique_ptr<score::os::Channel> channel_;
    std::mutex name_open_mutex_;
    std::mutex name_close_mutex_;

    std::atomic<bool> test_completed_{false};
    std::thread watchdog_;
};

void NeutrinoTest::StartWatchdog(std::atomic<bool>& test_done, int max_timeout_ms)
{
    watchdog_ = std::thread([&test_done, max_timeout_ms]() {
        const auto check_interval_ms = 10U;
        int waited_ms = 0;
        while (!test_done.load() && (waited_ms < max_timeout_ms))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms));
            waited_ms += check_interval_ms;
        }

        if (!test_done.load())
        {
            ADD_FAILURE() << "[Watchdog] Test timed out after " << waited_ms
                          << " ms: " << ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name()
                          << "." << ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::exit(EXIT_FAILURE);
        }
    });
}

score::cpp::expected_blank<score::os::Error> NeutrinoTest::DestroyNamedServer(name_attach_t* name_attach, dispatch_t* dispatch)
{
    const auto name_detach = dispatch_->name_detach(name_attach, NAME_FLAG_DETACH_SAVEDPP);
    if (!name_detach.has_value())
    {
        return score::cpp::make_unexpected(name_detach.error());
    }

    const auto dispatch_destroy = dispatch_->dispatch_destroy(dispatch);
    if (!dispatch_destroy.has_value())
    {
        return score::cpp::make_unexpected(dispatch_destroy.error());
    }
    return {};
}

score::cpp::expected<std::tuple<std::int32_t, name_attach_t*, dispatch_t*>, score::os::Error> NeutrinoTest::CreateNamedServer(
    const Neutrino::ChannelFlag flags,
    const std::string& server_name)
{
    const auto channel_id = neutrino_->ChannelCreate(flags);
    if (!channel_id.has_value())
    {
        return score::cpp::make_unexpected(channel_id.error());
    }

    const auto dispatch_channel = dispatch_->dispatch_create_channel(channel_id.value(), 0);
    if (!dispatch_channel.has_value())
    {
        return score::cpp::make_unexpected(dispatch_channel.error());
    }

    const auto name_attach = dispatch_->name_attach(
        dispatch_channel.value(), server_name.empty() ? server_name_.c_str() : server_name.c_str(), 0);
    if (!name_attach.has_value())
    {
        return score::cpp::make_unexpected(name_attach.error());
    }
    return std::make_tuple(channel_id.value(), name_attach.value(), dispatch_channel.value());
}

score::cpp::expected<std::int32_t, score::os::Error> NeutrinoTest::CreateNamedServerClient(const std::string& server_name)
{
    std::lock_guard<std::mutex> lock{name_open_mutex_};
    score::cpp::expected<std::int32_t, score::os::Error> result{};
    for (std::uint32_t trial = 0U; trial < kClientCreationTrials; ++trial)
    {
        result = dispatch_->name_open(server_name.empty() ? server_name_.c_str() : server_name.c_str(), 0);
        if (result.has_value())
        {
            return result;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{10});
    }
    return result;
}

score::cpp::expected_blank<score::os::Error> NeutrinoTest::DestroyClient(const std::int32_t connection_id)
{
    std::lock_guard<std::mutex> lock{name_close_mutex_};
    return dispatch_->name_close(connection_id);
}

score::cpp::expected<std::int32_t, score::os::Error> NeutrinoTest::SetTimerTimeout(const Neutrino::ClockType clock_type,
                                                                          const Neutrino::TimerTimeoutFlag flags,
                                                                          const std::chrono::nanoseconds timeout)
{
    return neutrino_->TimerTimeout(clock_type, flags, nullptr, timeout);
}

score::cpp::expected_blank<score::os::Error> NeutrinoTest::SendMessage(
    const std::int32_t connection_id,
    const iov_t& request,
    iov_t& response,
    std::optional<
        std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
        timeout)
{
    if (timeout.has_value())
    {
        const auto clock_type = std::get<0>(timeout.value());
        const auto timeout_flags = std::get<1>(timeout.value());
        const auto timeout_nanoseconds = std::get<2>(timeout.value());
        const auto set_timer_timeout = SetTimerTimeout(clock_type, timeout_flags, timeout_nanoseconds);
        if (!set_timer_timeout.has_value())
        {
            return score::cpp::make_unexpected(set_timer_timeout.error());
        }
    }

    const auto send_message = channel_->MsgSendv(connection_id, &request, 1, &response, 1);
    if (!send_message.has_value())
    {
        return score::cpp::make_unexpected(send_message.error());
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> NeutrinoTest::ReceiveMessage(
    const std::int32_t channel_id,
    iov_t& request,
    std::optional<
        std::tuple<const Neutrino::ClockType, const Neutrino::TimerTimeoutFlag, const std::chrono::nanoseconds>>
        timeout,
    std::optional<_msg_info> message_info)
{
    if (timeout.has_value())
    {
        const auto clock_type = std::get<0>(timeout.value());
        const auto timeout_flags = std::get<1>(timeout.value());
        const auto timeout_nanoseconds = std::get<2>(timeout.value());
        const auto set_timer_timeout = SetTimerTimeout(clock_type, timeout_flags, timeout_nanoseconds);
        if (!set_timer_timeout.has_value())
        {
            return score::cpp::make_unexpected(set_timer_timeout.error());
        }
    }

    const auto receive_message =
        channel_->MsgReceivev(channel_id, &request, 1, message_info.has_value() ? &message_info.value() : nullptr);
    if (!receive_message.has_value())
    {
        return score::cpp::make_unexpected(receive_message.error());
    }

    return receive_message.value();
}

score::cpp::expected_blank<score::os::Error> NeutrinoTest::ReplyMessage(const std::int32_t receive_message_id,
                                                               const iov_t& reply)
{
    return channel_->MsgReplyv(receive_message_id, 0, &reply, 1);
}

TEST_F(NeutrinoTest, TimerTimeoutCalledOnMessageSend)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Called On Message Send");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer();
            EXPECT_TRUE(create_server.has_value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_to_connect = true;
            }
            client_server_sync.notify_one();

            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, &response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kSend | Neutrino::TimerTimeoutFlag::kReply};
            // Send a message which is never received

            const auto send_message = SendMessage(
                create_client.value(),
                request,
                response,
                std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
            EXPECT_FALSE(send_message.has_value());
            EXPECT_EQ(send_message.error(), score::os::Error::Code::kKernelTimeout);

            const auto destroy_client = DestroyClient(create_client.value());
            EXPECT_TRUE(destroy_client.has_value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_for_destruction = true;
            }
            client_server_sync.notify_one();
        });
}

TEST_F(NeutrinoTest, TimerTimeoutCalledOnMessageSend_1)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Called On Message Send");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server =
                CreateNamedServer(Neutrino::ChannelFlag::kPrivate | Neutrino::ChannelFlag::kDisconnect);
            EXPECT_TRUE(create_server.has_value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_to_connect = true;
            }
            client_server_sync.notify_one();

            iov_t response;

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            bool first_request = true;
            while (true)
            {
                const auto message_receive = ReceiveMessage(
                    channel_id, request, std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{10}));
                if (!message_receive.has_value())
                {
                    EXPECT_EQ(message_receive.error(), score::os::Error::Code::kKernelTimeout);
                    // you don't reply if you are timed out
                    continue;
                }
                else
                {
                    if (message_receive.value() != 0)
                    {
                        EXPECT_STREQ(request_message, sample_request);
                    }
                    else
                    {
                        const auto pulse = *reinterpret_cast<_pulse*>(request_message);
                        EXPECT_EQ(pulse.code, _PULSE_CODE_DISCONNECT);
                        // we need to destroy the internal server-client connection id mapping
                        // keep in mind that the client coid (from MsgReceive) != scoid from this pulse
                        const auto destroy_client = DestroyClient(pulse.scoid);
                        EXPECT_TRUE(destroy_client.has_value());
                        // all the clients are tied to the same process and we receive one pulse when the last client
                        // from a process has disconnected, as stated by the QNX documentation
                        break;
                    }
                }

                if (first_request)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds + 10});
                    char response_message[kMinMessageSize] = "SampleRespons0";
                    channel_->SetIovConst(&response, response_message, kMinMessageSize);
                }
                else
                {
                    // SampleResponse
                    char response_message[kMinMessageSize];
                    memcpy(response_message, kSampleReponseMessage.c_str(), kSampleReponseMessage.size());
                    channel_->SetIovConst(&response, response_message, kMinMessageSize);
                }
                const auto message_reply = ReplyMessage(message_receive.value(), response);
                if (first_request)
                {
                    EXPECT_FALSE(message_reply.has_value());
                    // we should fail to reply, the sender was timed out
                    EXPECT_EQ(message_reply.error(), score::os::Error::Code::kNoSuchProcess);
                    first_request = false;
                }
                else
                {
                    EXPECT_TRUE(message_reply.has_value());
                }
            }

            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, &response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kSend | Neutrino::TimerTimeoutFlag::kReply};

            // Send a message which had an error due to timeout
            auto send_message = SendMessage(
                create_client.value(),
                request,
                response,
                std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
            EXPECT_FALSE(send_message.has_value());
            EXPECT_EQ(send_message.error(), score::os::Error::Code::kKernelTimeout);

            // Send the message once again
            send_message = SendMessage(
                create_client.value(),
                request,
                response,
                std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));

            // Now it should succeed
            EXPECT_TRUE(send_message.has_value());
            // We should receive the response specific to the current request, not the older one
            EXPECT_STREQ(response_message, kSampleReponseMessage.c_str());

            const auto destroy_client = DestroyClient(create_client.value());
            EXPECT_TRUE(destroy_client.has_value());
        });
}

TEST_F(NeutrinoTest, TimerTimeoutCalledOnMessageReply)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Called On Message Reply");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer();
            EXPECT_TRUE(create_server.has_value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_to_connect = true;
            }
            client_server_sync.notify_one();

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);
            const auto message_receive = ReceiveMessage(channel_id, request);
            EXPECT_TRUE(message_receive.has_value());
            EXPECT_STREQ(request_message, sample_request);

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, &response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kSend | Neutrino::TimerTimeoutFlag::kReply};
            // Send a message which is received but no one replies to it

            const auto send_message = SendMessage(
                create_client.value(),
                request,
                response,
                std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
            EXPECT_FALSE(send_message.has_value());
            EXPECT_EQ(send_message.error(), score::os::Error::Code::kKernelTimeout);

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_for_destruction = true;
            }
            client_server_sync.notify_one();

            const auto destroy_client = DestroyClient(create_client.value());
            EXPECT_TRUE(destroy_client.has_value());
        });
}

TEST_F(NeutrinoTest, TimerTimeoutOnMessageReceive)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout On Message Receive");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto create_server = CreateNamedServer();
    EXPECT_TRUE(create_server.has_value());

    const auto clock_type{Neutrino::ClockType::kRealtime};
    const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
    char request_message[kMinMessageSize];
    iov_t request;
    channel_->SetIov(&request, request_message, kMinMessageSize);

    const auto message_receive = ReceiveMessage(
        std::get<0>(create_server.value()),
        request,
        std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
    EXPECT_FALSE(message_receive.has_value());
    EXPECT_EQ(message_receive.error(), score::os::Error::Code::kKernelTimeout);

    const auto destroy_named_server =
        DestroyNamedServer(std::get<1>(create_server.value()), std::get<2>(create_server.value()));
    EXPECT_TRUE(destroy_named_server.has_value());
}

TEST_F(NeutrinoTest, TimerTimeoutNeverCalledWrongFlag)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Never Called Wrong Flag");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer();
            EXPECT_TRUE(create_server.has_value());

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_to_connect = true;
            }
            client_server_sync.notify_one();

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);
            const auto message_receive = ReceiveMessage(channel_id, request);
            EXPECT_TRUE(message_receive.has_value());
            EXPECT_STREQ(request_message, sample_request);

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, &response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            // here we should expect either kSend or kReply, as the client
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
            // Send a message which is received but no one replies to it and we never unblock due to wrong timeout flag
            const auto send_message = SendMessage(
                create_client.value(),
                request,
                response,
                std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
        });

    // Wait a bit to show that the client does not timeout by itself
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // force the client thread to shut down as it's never unblocked
    pthread_cancel(client_thread.native_handle());

    // shutdown the server explicitly
    is_ready_for_destruction = true;
    client_server_sync.notify_one();
}

TEST_F(NeutrinoTest, TimerTimeoutNeverCalledChannelUnblockFlag)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Never Called Channel Unblock Flag");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    bool is_ready_for_destruction = false;

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            const auto create_server = CreateNamedServer(Neutrino::ChannelFlag::kUnblock);
            EXPECT_TRUE(create_server.has_value());

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_to_connect = true;
            }
            client_server_sync.notify_one();

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);
            const auto message_receive = ReceiveMessage(channel_id, request);
            EXPECT_TRUE(message_receive.has_value());
            EXPECT_STREQ(request_message, sample_request);

            std::this_thread::sleep_for(std::chrono::milliseconds(kDefaultTimerTimeoutMilliseconds));
            iov_t response;
            channel_->SetIovConst(&response, sample_response, kMinMessageSize);
            const auto message_reply = ReplyMessage(message_receive.value(), response);
            EXPECT_TRUE(message_reply.has_value());

            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_for_destruction]() {
                    return is_ready_for_destruction;
                });
            }
            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex, &is_ready_for_destruction]() {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());

            // We are blocked here until we receive a response, no matter the timeout we set
            // The main problem is that if the server sets kUnblock, clients won't even respond to SIGKILL or
            // SIGTERM We simulate that, in the end, server responds after some time...
            iov_t request;
            channel_->SetIovConst(&request, sample_request, kMinMessageSize);
            char response_message[kMinMessageSize];
            iov_t response;
            channel_->SetIov(&response, response_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReply | Neutrino::TimerTimeoutFlag::kSend};
            const auto before_sending = std::chrono::high_resolution_clock::now();
            const auto send_message = SendMessage(
                create_client.value(),
                request,
                response,
                std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
            const auto after_receiving = std::chrono::high_resolution_clock::now();
            EXPECT_LT(std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}, after_receiving - before_sending);
            EXPECT_TRUE(send_message.has_value());
            const auto destroy_client = DestroyClient(create_client.value());
            EXPECT_TRUE(destroy_client.has_value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_for_destruction = true;
            }
            client_server_sync.notify_one();
        });
}

TEST_F(NeutrinoTest, TestServerPulseOnClientShutdown)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Test Server Pulse On Client Shutdown");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect{false};

    auto server_thread = score::cpp::jthread(
        [this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex](const score::cpp::stop_token&) {
            // get notified for every client which disconnects
            const auto create_server =
                CreateNamedServer(Neutrino::ChannelFlag::kPrivate | Neutrino::ChannelFlag::kDisconnect);
            EXPECT_TRUE(create_server.has_value());

            auto channel_id = std::get<0>(create_server.value());
            auto name_attach = std::get<1>(create_server.value());
            auto dispatch = std::get<2>(create_server.value());

            {
                std::lock_guard<std::mutex> lock(client_server_sync_mutex);
                is_ready_to_connect = true;
            }
            client_server_sync.notify_one();

            iov_t response;
            channel_->SetIovConst(&response, sample_response, kMinMessageSize);

            char request_message[kMinMessageSize];
            iov_t request;
            channel_->SetIov(&request, request_message, kMinMessageSize);

            const auto clock_type{Neutrino::ClockType::kRealtime};
            const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
            while (true)
            {
                const auto message_receive = ReceiveMessage(
                    channel_id, request, std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{10}));
                if (!message_receive.has_value())
                {
                    EXPECT_EQ(message_receive.error(), score::os::Error::Code::kKernelTimeout);
                    // you don't reply if you are timed out
                    continue;
                }
                else
                {
                    if (message_receive.value() != 0)
                    {
                        EXPECT_STREQ(request_message, sample_request);
                    }
                    else
                    {
                        const auto pulse = *reinterpret_cast<_pulse*>(request_message);
                        EXPECT_EQ(pulse.code, _PULSE_CODE_DISCONNECT);
                        // we need to destroy the internal server-client connection id mapping
                        // keep in mind that the client coid (from MsgReceive) != scoid from this pulse
                        const auto destroy_client = DestroyClient(pulse.scoid);
                        EXPECT_TRUE(destroy_client.has_value());
                        // all the clients are tied to the same process and we receive one pulse when the last client
                        // from a process has disconnected, as stated by the QNX documentation
                        break;
                    }
                }
                const auto message_reply = ReplyMessage(message_receive.value(), response);
                EXPECT_TRUE(message_reply.has_value());
            }

            const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
            EXPECT_TRUE(destroy_named_server.has_value());
        });

    auto client_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
        {
            std::unique_lock<std::mutex> lock(client_server_sync_mutex);
            client_server_sync.wait(lock, [&is_ready_to_connect]() {
                return is_ready_to_connect;
            });
        }

        const auto create_client = CreateNamedServerClient();
        EXPECT_TRUE(create_client.has_value());

        iov_t request;
        channel_->SetIovConst(&request, sample_request, kMinMessageSize);
        char response_message[kMinMessageSize];
        iov_t response;
        channel_->SetIov(&response, response_message, kMinMessageSize);

        const auto clock_type{Neutrino::ClockType::kRealtime};
        const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReply | Neutrino::TimerTimeoutFlag::kSend};
        const auto send_message = SendMessage(
            create_client.value(),
            request,
            response,
            std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
        EXPECT_TRUE(send_message.has_value());
        EXPECT_STREQ(response_message, sample_response);

        const auto destroy_client = DestroyClient(create_client.value());
        EXPECT_TRUE(destroy_client.has_value());
    });
}

TEST_F(NeutrinoTest, TestSendErrorOnServerDeath)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Test send Error On Server Death");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;

    auto server_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
        const auto create_server = CreateNamedServer();
        EXPECT_TRUE(create_server.has_value());

        {
            std::lock_guard<std::mutex> lock(client_server_sync_mutex);
            is_ready_to_connect = true;
        }
        client_server_sync.notify_one();

        auto name_attach = std::get<1>(create_server.value());
        auto dispatch = std::get<2>(create_server.value());

        std::this_thread::sleep_for(std::chrono::milliseconds{50});

        const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
        EXPECT_TRUE(destroy_named_server.has_value());
    });

    auto client_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
        {
            std::unique_lock<std::mutex> lock(client_server_sync_mutex);
            client_server_sync.wait(lock, [&is_ready_to_connect]() {
                return is_ready_to_connect;
            });
        }

        const auto create_client = CreateNamedServerClient();
        EXPECT_TRUE(create_client.has_value());

        iov_t request;
        channel_->SetIovConst(&request, sample_request, kMinMessageSize);
        char response_message[kMinMessageSize];
        iov_t response;
        channel_->SetIov(&response, response_message, kMinMessageSize);

        const auto clock_type{Neutrino::ClockType::kRealtime};
        const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReply | Neutrino::TimerTimeoutFlag::kSend};
        const auto timeout = std::chrono::milliseconds{200};

        const auto before_sending = std::chrono::high_resolution_clock::now();
        const auto send_message =
            SendMessage(create_client.value(), request, response, std::make_tuple(clock_type, timeout_flag, timeout));
        const auto after_receiving = std::chrono::high_resolution_clock::now();
        EXPECT_GT(timeout, after_receiving - before_sending);
        EXPECT_FALSE(send_message.has_value());
        EXPECT_EQ(send_message.error(), score::os::Error::Code::kBadFileDescriptor);

        const auto destroy_client = DestroyClient(create_client.value());
        EXPECT_TRUE(destroy_client.has_value());
    });
}

TEST_F(NeutrinoTest, TestClientPulseOnServerDeath)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Test Client Pulse On Server Death");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::mutex client_server_sync_mutex;
    std::condition_variable client_server_sync;
    bool is_ready_to_connect = false;
    std::int32_t client_connection_id = 0;

    auto server_thread = score::cpp::jthread([this, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex]() {
        const auto create_server = CreateNamedServer(Neutrino::ChannelFlag::kPrivate);
        EXPECT_TRUE(create_server.has_value());

        {
            std::lock_guard<std::mutex> lock(client_server_sync_mutex);
            is_ready_to_connect = true;
        }
        client_server_sync.notify_one();

        auto name_attach = std::get<1>(create_server.value());
        auto dispatch = std::get<2>(create_server.value());

        std::this_thread::sleep_for(std::chrono::milliseconds{50});
        const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
        EXPECT_TRUE(destroy_named_server.has_value());
    });

    auto client_thread = score::cpp::jthread(
        [this, &client_connection_id, &is_ready_to_connect, &client_server_sync, &client_server_sync_mutex](
            const score::cpp::stop_token& stop_token) {
            {
                std::unique_lock<std::mutex> lock(client_server_sync_mutex);
                client_server_sync.wait(lock, [&is_ready_to_connect]() {
                    return is_ready_to_connect;
                });
            }

            const auto create_client = CreateNamedServerClient();
            EXPECT_TRUE(create_client.has_value());
            client_connection_id = create_client.value();

            while (!stop_token.stop_requested())
            {
                // just doing our job as a good client here
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
            }
        });

    auto client_stop_source = client_thread.get_stop_source();
    auto observer_thread = score::cpp::jthread([this,
                                         &client_connection_id,
                                         &client_stop_source,
                                         &is_ready_to_connect,
                                         &client_server_sync,
                                         &client_server_sync_mutex]() {
        const auto observe_server = CreateNamedServer(Neutrino::ChannelFlag::kConnectionIdDisconnect, "observe_server");

        const auto channel_id = std::get<0>(observe_server.value());
        auto name_attach = std::get<1>(observe_server.value());
        auto dispatch = std::get<2>(observe_server.value());

        char request_message[kMinMessageSize];
        iov_t request;
        channel_->SetIov(&request, request_message, kMinMessageSize);

        const auto clock_type{Neutrino::ClockType::kRealtime};
        const auto timeout_flag{Neutrino::TimerTimeoutFlag::kReceive};
        while (true)
        {
            // TODO: Use a pulse here
            const auto message_receive = ReceiveMessage(
                channel_id,
                request,
                std::make_tuple(clock_type, timeout_flag, std::chrono::milliseconds{kDefaultTimerTimeoutMilliseconds}));
            if (!message_receive.has_value())
            {
                EXPECT_EQ(message_receive.error(), score::os::Error::Code::kKernelTimeout);
                continue;
            }
            else
            {
                if (message_receive.value() != 0)
                {
                    // ignore anything which is not a pulse
                    continue;
                }
                else
                {
                    const auto pulse = *reinterpret_cast<_pulse*>(request_message);
                    EXPECT_EQ(pulse.code, _PULSE_CODE_COIDDEATH);
                    // we will receive a pulse with the value of the client channel id
                    // which was connected to the destroyed server
                    EXPECT_EQ(client_connection_id, pulse.value.sival_int);
                    const auto destroy_client = DestroyClient(pulse.value.sival_int);
                    EXPECT_TRUE(destroy_client.has_value());
                    break;
                }
            }
        }

        client_stop_source.request_stop();
        const auto destroy_named_server = DestroyNamedServer(name_attach, dispatch);
        EXPECT_TRUE(destroy_named_server.has_value());
    });
}
