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
#include "score/os/utils/mqueue.h"

#include <fcntl.h>
#include <gtest/gtest.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <exception>
#include <memory>
#include <thread>

namespace score
{
namespace os
{
namespace test
{

namespace
{
#if defined(__QNX__)
#if __QNX__ >= 800
constexpr size_t kQueueNameHash = 18020715410057215184U;
constexpr size_t kNonExistingQueueNameHash = 11493048575142093764U;
#else  // QNX 7.x or earlier
constexpr size_t kQueueNameHash = 18020715410057215184U;
constexpr size_t kNonExistingQueueNameHash = 16659226646718876469U;
#endif
#else  //__linux__
constexpr size_t kQueueNameHash = 3778941452914592862U;
constexpr size_t kNonExistingQueueNameHash = 2445131158773332806U;
#endif
}  // namespace

static const std::string name{"myqueue"};

struct FixtureMQueueShould : ::testing::Test
{
    MQueue queue{name, AccessMode::kCreate};

    void check_mq_permissions()
    {
        auto perm = queue.get_mq_st_mode();

        ASSERT_TRUE((perm.value() & S_IRUSR));
        ASSERT_TRUE((perm.value() & S_IWUSR));
        ASSERT_TRUE((perm.value() & S_IRGRP));
        ASSERT_TRUE((perm.value() & S_IWGRP));
        ASSERT_TRUE((perm.value() & S_IROTH));
        ASSERT_FALSE((perm.value() & S_IWOTH));
        ASSERT_FALSE((perm.value() & S_IXOTH));
    }

    void SetUp() override
    {
        check_mq_permissions();
    }

    void TearDown() override
    {
        queue.unlink();
    }
};

struct FixtureMQueueStringShould : ::testing::Test
{
    std::int32_t m_fd = -1;
    std::string m_name = "/" + name;
    void SetUp() override
    {
        mode_t perm{S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH};

        std::int32_t flags = O_CREAT | O_RDWR;

        struct mq_attr attrib;
        attrib.mq_flags = 0;
        attrib.mq_maxmsg = 10;
        attrib.mq_msgsize = 100;
        attrib.mq_curmsgs = 0;

        m_fd = mq_open(m_name.c_str(), flags, perm, &attrib);
    }

    void TearDown() override
    {
        mq_unlink(m_name.c_str());
    }
};

struct FixtureMQueueMaxMsgSizeShould : ::testing::Test
{
    MQueue queue{name, AccessMode::kCreate, 100};
    std::string msg{""};

    void SetUp() override
    {
        for (std::uint16_t i = 1; i < 99; ++i)
        {
            msg += "1";
        }
    }

    void TearDown() override
    {
        queue.unlink();
    }
};

TEST_F(FixtureMQueueShould, sendACharPointerToOtherProcess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueShould send AChar Pointer To Other Process");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* msg = "020223456"; /* TODO : create tests with different formats of the msg, for better check */
    std::thread test_tr([&msg = msg]() {
        MQueue sender{name, AccessMode::kUse};
        sender.send(msg, 9);
    });

    MQueue receiver{name, AccessMode::kUse};
    char str[10] = {0};
    receiver.receive(str);
    test_tr.join();

    std::string result(str, 9);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.size(), 9);
    EXPECT_EQ(result, std::string(msg, 9));
}

TEST_F(FixtureMQueueShould, sendAStringToOtherProcess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueShould send AString To Other Process");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::string msg{std::to_string(0x01)};
    std::thread test_tr([&msg = msg]() {
        MQueue sender{name, AccessMode::kUse};
        sender.send(msg);
    });

    MQueue receiver{name, AccessMode::kUse};
    std::string str = receiver.receive();
    test_tr.join();
    EXPECT_EQ(str, msg);
}

TEST_F(FixtureMQueueMaxMsgSizeShould, SendALongStringToOtherProzess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueMaxMsgSizeShould Send ALong String To Other Prozess");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::thread test_tr([&msg = msg]() {
        MQueue sender{name, AccessMode::kUse};
        sender.send(msg);
    });

    MQueue receiver{name, AccessMode::kUse};
    std::string str = receiver.receive();
    test_tr.join();

    EXPECT_EQ(str, msg);
}

TEST_F(FixtureMQueueMaxMsgSizeShould, ReopenOnlyWithId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueMaxMsgSizeShould Reopen Only With Id");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    size_t id = queue.get_id();

    std::thread test_tr([&msg = msg]() {
        MQueue sender{name, AccessMode::kUse};
        sender.send(msg);
    });

    MQueue receiver{id};
    std::string str = receiver.receive();
    test_tr.join();
    EXPECT_EQ(str, msg);
}

TEST(MQueue, TryOpenNotExistingMQqueue)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueue Try Open Not Existing MQqueue");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MQueue test{"blah", AccessMode::kUse};
    EXPECT_EQ(test.get_id(), kNonExistingQueueNameHash);
    EXPECT_FALSE(test.unlink().has_value());
}

TEST(MQueue, shouldReturnId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueue should Return Id");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MQueue queue{"some_name", AccessMode::kCreate};
    std::size_t n = queue.get_id();
    EXPECT_EQ(n, kQueueNameHash);
    EXPECT_TRUE(queue.unlink().has_value());
}

TEST(MQueue, ShouldGetEmtpyMessage)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueue Should Get Emtpy Message");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MQueue queue{"some_name", AccessMode::kCreateNonBlocking};
    std::string msg = queue.receive();
    EXPECT_EQ(msg, "");
    queue.unlink();
}

TEST_F(FixtureMQueueShould, timedBlockEmptyQueue)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueShould timed Block Empty Queue");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::pair<std::string, bool> result = queue.timed_receive(std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, "");
    EXPECT_TRUE(result.second);
}

TEST(MQueue, timedNonBlockEmptyQueue)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueue timed Non Block Empty Queue");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MQueue queue{"some_name", AccessMode::kCreateNonBlocking};
    std::pair<std::string, bool> result = queue.timed_receive(std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, "");
    EXPECT_TRUE(result.second);
    queue.unlink();
}

TEST_F(FixtureMQueueShould, timedBlockSendMessage)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueShould timed Block Send Message");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::string msg{"thunder"};
    std::thread test_tr1([&msg = msg]() {
        MQueue sender{name, AccessMode::kUse};
        sender.send(msg);
    });
    test_tr1.join();
    std::pair<std::string, bool> result = queue.timed_receive(std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, "thunder");
    EXPECT_FALSE(result.second);
}

TEST_F(FixtureMQueueStringShould, timedBlockCharArrayMessage)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueStringShould timed Block Char Array Message");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::thread test_tr1([]() {
        char smsg[] = {'0', '2', '0', '2', '2', '3', '4', '5', '6', '\0'};
        MQueue sender{name};
        sender.send(smsg, 9);
    });
    test_tr1.join();
    MQueue receiver{name};
    const auto msg_size = static_cast<std::size_t>(receiver.get_msg_size());
    auto rmsg = std::make_unique<char[]>(msg_size);

    const std::pair<ssize_t, bool> result = receiver.timed_receive(rmsg.get(), std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, 9);
    EXPECT_FALSE(result.second);
    EXPECT_EQ("020223456", std::string(rmsg.get()));
}

TEST_F(FixtureMQueueStringShould, timedBlockDeffectedCharArrayMessage)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueStringShould timed Block Deffected Char Array Message");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::thread test_tr1([]() {
        uint8_t send_arr[9] = {0x00, 0x02, 0x00, 0x02, 0x02, 0x03, 0x04, 0x05, 0x06};
        char* smsg = reinterpret_cast<char*>(send_arr);
        MQueue sender{name};
        sender.send(smsg, 9);
    });
    test_tr1.join();
    char* rmsg = NULL;
    MQueue receiver{name};
    std::pair<ssize_t, bool> result = receiver.timed_receive(rmsg, std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, -1);
    EXPECT_FALSE(result.second);
}

TEST_F(FixtureMQueueStringShould, timedBlockMessage)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FixtureMQueueStringShould timed Block Message");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::thread test_tr1([]() {
        uint8_t send_arr[9] = {0x00, 0x02, 0x00, 0x02, 0x02, 0x03, 0x04, 0x05, 0x06};
        char* smsg = reinterpret_cast<char*>(send_arr);
        MQueue sender{name};
        for (std::uint16_t i = 0; i < 10; ++i)
        {
            sender.timed_send(smsg, 9, std::chrono::milliseconds(100));
        }
    });
    test_tr1.join();
    char* rmsg = NULL;
    MQueue receiver{name};
    std::pair<ssize_t, bool> result = receiver.timed_receive(rmsg, std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, -1);
    EXPECT_FALSE(result.second);
}

}  // namespace test
}  // namespace os
}  // namespace score
