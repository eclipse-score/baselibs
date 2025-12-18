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
#include "score/os/mqueue.h"

#include "score/os/utils/test/lib/random_string.h"
#include <gtest/gtest.h>
#include <future>
#include <thread>

namespace score
{
namespace os
{
namespace
{

using ::testing::Test;
constexpr auto kReceiveMsgMaxSize{64};
constexpr mqd_t kInvalidMqd = -1;
class MqueueTest : public Test
{
  protected:
    void TearDown() override
    {
        mqueue_.mq_unlink(m_name_.c_str());
    }

    const std::uint16_t random_len_str_{31U};
    std::string m_name_{random_string(random_len_str_)};
    impl::MqueueImpl mqueue_;
};

TEST_F(MqueueTest, mq_open_succeeds)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_open_succeeds");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Mqueue::OpenFlag flags{Mqueue::OpenFlag::kCreate};
    Mqueue::ModeFlag perm{Mqueue::ModeFlag::kReadUser};
    const auto ret_open = mqueue_.mq_open(m_name_.c_str(), flags, perm, nullptr);
    ASSERT_TRUE(ret_open.has_value());
    mqueue_.mq_close(ret_open.value());

    const auto ret_open2 = mqueue_.mq_open(m_name_.c_str(), Mqueue::OpenFlag::kReadOnly);
    ASSERT_TRUE(ret_open2.has_value());
    mqueue_.mq_close(ret_open2.value());
    ASSERT_TRUE(mqueue_.mq_unlink(m_name_.c_str()).has_value());
}

TEST_F(MqueueTest, mq_open_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_open_failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Mqueue::OpenFlag flags{Mqueue::OpenFlag::kExclusive};
    Mqueue::ModeFlag perm{Mqueue::ModeFlag::kReadGroup};
    ASSERT_FALSE(mqueue_.mq_open(m_name_.c_str(), flags, perm, nullptr).has_value());
}

TEST_F(MqueueTest, mq_open_failure_without_create_flag)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_open_failure_without_create_flag");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Mqueue::OpenFlag flags{Mqueue::OpenFlag::kReadOnly};
    ASSERT_FALSE(mqueue_.mq_open(m_name_.c_str(), flags).has_value());
}

TEST_F(MqueueTest, mq_unlink_fails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_unlink_fails");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ASSERT_FALSE(mqueue_.mq_unlink(m_name_.c_str()).has_value());
}

TEST_F(MqueueTest, mq_send_and_timedreceive_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_send_and_timedreceive_success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::string message = "Test Message";
    const size_t message_length = message.size() + 1;
    using ModeFlag = Mqueue::ModeFlag;

    std::promise<void> mq_promise;
    std::future<void> mq_future = mq_promise.get_future();

    std::thread reader_thread([this, message, &mq_promise]() {
        mq_attr attr;
        attr.mq_maxmsg = 3;
        attr.mq_msgsize = 32;
        std::uint32_t msg_prio{1};

        const auto mqd = mqueue_.mq_open(m_name_.c_str(),
                                         Mqueue::OpenFlag::kCreate | Mqueue::OpenFlag::kReadWrite,
                                         ModeFlag::kReadUser | ModeFlag::kWriteUser | ModeFlag::kExecUser,
                                         &attr);
        ASSERT_TRUE(mqd.has_value());
        mq_promise.set_value();

        std::array<char, kReceiveMsgMaxSize> received_message;
        struct timespec timeout;
        ::clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 4;
        timeout.tv_nsec = 0;

        auto receive_result =
            mqueue_.mq_timedreceive(mqd.value(), received_message.data(), kReceiveMsgMaxSize, &msg_prio, &timeout);
        EXPECT_TRUE(receive_result.has_value());
        EXPECT_GT(receive_result.value(), 0);
        mqueue_.mq_close(mqd.value());
    });

    mq_future.wait();  // wait for the mqueue to be created by the reader_thread

    const auto mqd = mqueue_.mq_open(m_name_.c_str(), Mqueue::OpenFlag::kReadWrite);
    ASSERT_TRUE(mqd.has_value());
    std::uint32_t send_msg_prio{1};

    EXPECT_TRUE(mqueue_.mq_send(mqd.value(), message.c_str(), message_length, send_msg_prio).has_value());

    reader_thread.join();
    mqueue_.mq_close(mqd.value());
    ASSERT_TRUE(mqueue_.mq_unlink(m_name_.c_str()).has_value());
}

TEST_F(MqueueTest, mq_send_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_send_failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::string message = "Test Message";
    const size_t message_length = message.size() + 1;
    using ModeFlag = Mqueue::ModeFlag;

    mq_attr attr;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = 4;

    const auto mqd = mqueue_.mq_open(m_name_.c_str(),
                                     Mqueue::OpenFlag::kCreate | Mqueue::OpenFlag::kReadOnly,
                                     ModeFlag::kReadUser | ModeFlag::kWriteUser | ModeFlag::kExecUser,
                                     &attr);

    ASSERT_TRUE(mqd.has_value());
    std::uint32_t send_msg_prio{1};

    EXPECT_FALSE(mqueue_.mq_send(mqd.value(), message.c_str(), message_length, send_msg_prio).has_value());

    mqueue_.mq_close(mqd.value());
    ASSERT_TRUE(mqueue_.mq_unlink(m_name_.c_str()).has_value());
}

TEST_F(MqueueTest, mq_timedsend_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_timedsend_failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::string message = "Test Message";
    const size_t message_length = message.size() + 1;
    using ModeFlag = Mqueue::ModeFlag;

    mq_attr attr;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = 4;

    const auto mqd = mqueue_.mq_open(m_name_.c_str(),
                                     Mqueue::OpenFlag::kCreate | Mqueue::OpenFlag::kReadWrite,
                                     ModeFlag::kReadUser | ModeFlag::kWriteUser | ModeFlag::kExecUser,
                                     &attr);
    ASSERT_TRUE(mqd.has_value());
    std::uint32_t send_msg_prio{1};

    struct timespec timeout;
    ::clock_gettime(CLOCK_REALTIME, &timeout);

    EXPECT_FALSE(
        mqueue_.mq_timedsend(mqd.value(), message.c_str(), message_length, send_msg_prio, &timeout).has_value());

    mqueue_.mq_close(mqd.value());
}

TEST_F(MqueueTest, mq_timedreceive_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_timedreceive_failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    mq_attr attr;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = 4;
    std::uint32_t msg_prio{1};
    using ModeFlag = Mqueue::ModeFlag;

    const auto mqd = mqueue_.mq_open(m_name_.c_str(),
                                     Mqueue::OpenFlag::kCreate | Mqueue::OpenFlag::kReadWrite,
                                     ModeFlag::kReadUser | ModeFlag::kWriteUser | ModeFlag::kExecUser,
                                     &attr);
    ASSERT_TRUE(mqd.has_value());

    std::array<char, kReceiveMsgMaxSize> received_message;
    struct timespec timeout;
    ::clock_gettime(CLOCK_REALTIME, &timeout);

    auto receive_result =
        mqueue_.mq_timedreceive(mqd.value(), received_message.data(), kReceiveMsgMaxSize, &msg_prio, &timeout);
    EXPECT_FALSE(receive_result.has_value());
    mqueue_.mq_close(mqd.value());
}

TEST_F(MqueueTest, mq_timedsend_and_mq_receive_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_timedsend_and_mq_receive_success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::string message = "Test Message";
    const size_t message_length = message.size() + 1;
    using ModeFlag = Mqueue::ModeFlag;

    std::promise<void> mq_promise;
    std::future<void> mq_future = mq_promise.get_future();

    std::thread reader_thread([this, message, &mq_promise]() {
        mq_attr attr;
        attr.mq_maxmsg = 1;
        attr.mq_msgsize = 32;
        std::uint32_t msg_prio{1};

        const auto mqd = mqueue_.mq_open(m_name_.c_str(),
                                         Mqueue::OpenFlag::kCreate | Mqueue::OpenFlag::kReadOnly,
                                         ModeFlag::kReadUser | ModeFlag::kWriteUser | ModeFlag::kExecUser,
                                         &attr);
        ASSERT_TRUE(mqd.has_value());
        mq_promise.set_value();

        std::array<char, kReceiveMsgMaxSize> received_message;
        auto receive_result = mqueue_.mq_receive(mqd.value(), received_message.data(), kReceiveMsgMaxSize, &msg_prio);

        EXPECT_TRUE(receive_result.has_value());
        EXPECT_GT(receive_result.value(), 0);

        EXPECT_EQ(msg_prio, 1);
        EXPECT_STREQ(received_message.data(), message.c_str());

        mqueue_.mq_close(mqd.value());
    });

    mq_future.wait();  // wait for the mqueue to be created by the reader_thread

    const auto mqd = mqueue_.mq_open(m_name_.c_str(), Mqueue::OpenFlag::kReadWrite);
    ASSERT_TRUE(mqd.has_value());
    const std::uint32_t send_msg_prio{1};
    struct timespec timeout;
    ::clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 4;

    ASSERT_TRUE(
        mqueue_.mq_timedsend(mqd.value(), message.c_str(), message_length, send_msg_prio, &timeout).has_value());

    reader_thread.join();
    mqueue_.mq_close(mqd.value());
    ASSERT_TRUE(mqueue_.mq_unlink(m_name_.c_str()).has_value());
}

TEST_F(MqueueTest, mq_receive_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_receive_failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    char buffer[100];
    std::uint32_t msg_prio;

    auto result = mqueue_.mq_receive(kInvalidMqd, buffer, sizeof(buffer), &msg_prio);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MqueueTest, mq_close_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_close_success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto mqd =
        mqueue_.mq_open(m_name_.c_str(),
                        Mqueue::OpenFlag::kCreate,
                        Mqueue::ModeFlag::kExecGroup | Mqueue::ModeFlag::kWriteUser | Mqueue::ModeFlag::kReadUser,
                        nullptr);
    ASSERT_TRUE(mqd.has_value());

    auto result = mqueue_.mq_close(mqd.value());
    EXPECT_TRUE(result.has_value());
}

TEST_F(MqueueTest, mq_close_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_close_failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    auto result = mqueue_.mq_close(kInvalidMqd);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MqueueTest, mq_getattr_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_getattr_success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    mq_attr attr;
    attr.mq_maxmsg = 3;
    attr.mq_msgsize = 1024;

    auto mqd = mqueue_.mq_open(m_name_.c_str(), Mqueue::OpenFlag::kCreate, Mqueue::ModeFlag::kWriteGroup, &attr);
    ASSERT_TRUE(mqd.has_value());

    mq_attr get_attr;
    auto result = mqueue_.mq_getattr(mqd.value(), get_attr);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(get_attr.mq_maxmsg, attr.mq_maxmsg);
    EXPECT_EQ(get_attr.mq_msgsize, attr.mq_msgsize);
    mqueue_.mq_close(mqd.value());
}

TEST_F(MqueueTest, mq_getattr_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest mq_getattr_failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    mq_attr attr;
    auto result = mqueue_.mq_getattr(kInvalidMqd, attr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(MqueueTest, modeflag_to_nativeflag)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MqueueTest modeflag_to_nativeflag");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    using OpenFlag = Mqueue::OpenFlag;
    Mqueue::OpenFlag flags{OpenFlag::kWriteOnly | OpenFlag::kCreate | OpenFlag::kReadWrite | OpenFlag::kNonBlocking |
                           OpenFlag::kCloseOnExec};
    Mqueue::ModeFlag perm{Mqueue::ModeFlag::kExecOthers | Mqueue::ModeFlag::kWriteOthers |
                          Mqueue::ModeFlag::kReadOthers};
    const auto ret_open = mqueue_.mq_open(m_name_.c_str(), flags, perm, nullptr);
    ASSERT_TRUE(ret_open.has_value());
    mqueue_.mq_close(ret_open.value());
}

TEST(SchedTest, get_instance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedTest get_instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_NO_FATAL_FAILURE(Mqueue::instance());
}

}  // namespace
}  // namespace os
}  // namespace score
