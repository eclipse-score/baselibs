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
#include "score/os/mocklib/mqueuemock.h"
#include "score/os/mocklib/stat_mock.h"
#include "score/os/utils/mqueue.h"

#include <gtest/gtest.h>
#include <unistd.h>
#include <exception>
#include <thread>

namespace score
{
namespace os
{
namespace test
{

using ::testing::_;
using ::testing::Return;

struct MQueueFixture : ::testing::Test
{
    score::os::MqueueMock mqueue_mock;

    void SetUp() override
    {
        score::os::Mqueue::set_testing_instance(mqueue_mock);
        EXPECT_CALL(mqueue_mock, mq_unlink(_)).WillOnce(Return(score::cpp::expected_blank<Error>{}));
    }
};

TEST_F(MQueueFixture, shouldConfigureNonBlock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueue should configure non block");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite |
                                      score::os::Mqueue::OpenFlag::kNonBlocking;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _)).WillOnce(Return(1));
    MQueue queue{"some_name", AccessMode::kCreateNonBlocking};
}

TEST_F(MQueueFixture, shouldConfigureBlock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueue should configure block");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _)).WillOnce(Return(1));
    MQueue queue{"some_name", AccessMode::kCreate};
}

TEST_F(MQueueFixture, shouldUnlinkDefinedQueue)
{

    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mqueue should Unlink Defined Queue");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MQueue queue{"some_name", AccessMode::kCreate};

    EXPECT_CALL(mqueue_mock, mq_unlink(_)).WillOnce(Return(score::cpp::expected_blank<Error>{}));
    queue.unlink();
}

TEST_F(MQueueFixture, shouldUnlinkUndefinedQueue)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mqueue should Unlink Undefined Queue");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MQueue queue{"some_name", AccessMode::kCreate};

    EXPECT_CALL(mqueue_mock, mq_unlink(_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOENT))));
    const auto val = queue.unlink();
    std::stringstream str;
    str << val.error();
    score::cpp::expected_blank<score::os::Error> err = score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOENT));
    EXPECT_TRUE(val.error() == err.error());
}

TEST_F(MQueueFixture, shouldFaileToUnlink)
{

    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "mqueue should Faile To Unlink");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MQueue queue{"some_name", AccessMode::kCreate};

    EXPECT_CALL(mqueue_mock, mq_unlink(_)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    const auto val = queue.unlink();
    std::stringstream str;
    str << val.error();
    score::cpp::expected_blank<score::os::Error> err = score::cpp::make_unexpected(score::os::Error::createFromErrno());
    EXPECT_TRUE(val.error() == err.error());
}

TEST(MQueue, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueue shall PMR Default Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Mqueue::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::impl::MqueueImpl*>(instance.get()));
}

TEST_F(MQueueFixture, shouldFailToReceive)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Fail To Receive");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MQueue queue{"some_name", AccessMode::kCreate};

    EXPECT_CALL(mqueue_mock, mq_timedreceive(_, _, _, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EMSGSIZE))));
    const auto result = queue.timed_receive(std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, "");
    EXPECT_FALSE(result.second);
}

TEST_F(MQueueFixture, shouldFailToReceiveTwice)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Fail To Receive Twice");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MQueue queue{"some_name", AccessMode::kCreate};

    EXPECT_CALL(mqueue_mock, mq_timedreceive(_, _, _, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINTR))))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EMSGSIZE))));
    const auto result = queue.timed_receive(std::chrono::milliseconds(100));
    EXPECT_EQ(result.first, "");
    EXPECT_FALSE(result.second);
}

TEST_F(MQueueFixture, shouldFailToSend)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Fail To Send");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    uint8_t send_arr[9] = {0x00, 0x02, 0x00, 0x02, 0x02, 0x03, 0x04, 0x05, 0x06};
    char* smsg = reinterpret_cast<char*>(send_arr);

    MQueue queue{"some_name", AccessMode::kCreate};
    EXPECT_CALL(mqueue_mock, mq_timedsend(_, _, _, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EMSGSIZE))));
    const auto result = queue.timed_send(smsg, 9, std::chrono::milliseconds(100));
    EXPECT_EQ(result, -1);
}

TEST_F(MQueueFixture, shouldFailToSendDueInteruptSignal)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Fail To Send Due Interupt Signal");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    uint8_t send_arr[9] = {0x00, 0x02, 0x00, 0x02, 0x02, 0x03, 0x04, 0x05, 0x06};
    char* smsg = reinterpret_cast<char*>(send_arr);

    MQueue queue{"some_name", AccessMode::kCreate};
    EXPECT_CALL(mqueue_mock, mq_timedsend(_, _, _, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINTR))))
        .WillOnce(Return(score::cpp::expected_blank<Error>{}));
    const auto result = queue.timed_send(smsg, 9, std::chrono::milliseconds(100));
    EXPECT_EQ(result, 0);
}

TEST_F(MQueueFixture, shouldConfigurekExistUseOthCreate)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Configurek Exist Use Oth Create");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _)).Times(2).WillRepeatedly(Return(1));
    MQueue queue{"some_name", AccessMode::kCreate};
    MQueue queue2{"some_name", AccessMode::kIfExistUseOthCreate};
}

TEST_F(MQueueFixture, shouldCallSend)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Call Send");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite;
    std::string msg{"msg"};
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _)).WillOnce(Return(1));
    EXPECT_CALL(mqueue_mock, mq_send(_, _, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    MQueue queue{"some_name", AccessMode::kCreate};
    queue.send(msg);
}

TEST_F(MQueueFixture, shouldCallReceive)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Call Receive");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite;
    std::string msg{"msg"};
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _)).WillOnce(Return(1));
    EXPECT_CALL(mqueue_mock, mq_receive(_, _, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    MQueue queue{"some_name", AccessMode::kCreate};
    queue.receive();
}

TEST_F(MQueueFixture, shouldReturnErrorWhenOpenFailed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Return Error When Open Failed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    MQueue queue{"some_name", AccessMode::kCreate};
}

TEST_F(MQueueFixture, shouldReturnErrorWhenSetPermissionsFailed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture should Return Error When Set Permissions Failed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::StatMock stat_mock;
    score::os::Stat::set_testing_instance(stat_mock);
    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _)).WillOnce(Return(1));
    EXPECT_CALL(stat_mock, fchmod(_, _)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    MQueue queue{"some_name", AccessMode::kCreate};
}

TEST_F(MQueueFixture, failOnGetQueuePermissions)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MQueueFixture fail On Get Queue Permissions");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const score::cpp::expected_blank<Error> error = score::cpp::make_unexpected(Error::createFromErrno(5));
    score::os::StatMock stat_mock;
    score::os::Stat::set_testing_instance(stat_mock);
    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kCreate | score::os::Mqueue::OpenFlag::kReadWrite;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags, _, _)).WillOnce(Return(1));
    EXPECT_CALL(stat_mock, fchmod(_, _)).WillOnce(Return(score::cpp::blank{}));
    EXPECT_CALL(stat_mock, fstat(_, _)).WillOnce(Return(error));
    MQueue queue{"some_name", AccessMode::kCreate};

    queue.get_mq_st_mode();
}

TEST(Mqueue, shouldOpenReadAndWrite)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mqueue should Open Read And Write");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::MqueueMock mqueue_mock;
    score::os::Mqueue::set_testing_instance(mqueue_mock);
    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kReadWrite;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags)).WillOnce(Return(1));
    ON_CALL(mqueue_mock, mq_getattr(_, _)).WillByDefault(Return(score::cpp::expected_blank<Error>{}));
    MQueue queue{"some_name", AccessMode::kUse};
}

TEST(Mqueue, failOnGetQueueAttributes)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Mqueue fail On Get Queue Attributes");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::MqueueMock mqueue_mock;
    score::os::Mqueue::set_testing_instance(mqueue_mock);
    score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kReadWrite;
    EXPECT_CALL(mqueue_mock, mq_open(_, flags)).WillOnce(Return(1));
    EXPECT_CALL(mqueue_mock, mq_getattr(_, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    MQueue queue{"some_name", AccessMode::kUse};
}

}  // namespace test
}  // namespace os
}  // namespace score
