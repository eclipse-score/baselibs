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
#include "score/os/utils/qnx/resource_manager/tests/mocks/mock_simple_read_function.h"
#include "score/os/utils/qnx/resource_manager/tests/simple_read_function_test_fixture.h"

#include "score/os/mocklib/qnx/mock_iofunc.h"
#include "score/os/mocklib/qnx/mock_resmgr.h"

#include "score/os/utils/qnx/resource_manager/include/error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <future>

namespace
{
using ::testing::Return;

constexpr std::size_t kNoSize{0};
constexpr std::uint16_t kPrivateMessageTypeFirst{_IO_MAX + 1};
constexpr std::uint16_t kPrivateMessageTypeLast{kPrivateMessageTypeFirst};
constexpr std::uint16_t kPrivateMessageTerminate{kPrivateMessageTypeFirst};

constexpr auto test_path = "/test/resmgr_unit_test_path";

// Mock test

struct SimpleReadFunctionTest : ::testing::Test
{
    SimpleReadFunctionTest() : simple_read_function_mock(score::os::MockSimpleReadFunction(resmgr_mock, iofunc_mock)) {};

    score::os::MockResMgr resmgr_mock;
    score::os::MockIoFunc iofunc_mock;
    score::os::MockSimpleReadFunction simple_read_function_mock;
};

TEST_F(SimpleReadFunctionTest, getOffset_null_msg)
{
    io_read_t* msg{nullptr};
    RESMGR_OCB_T ocb;
    memset(&ocb, 0, sizeof(ocb));

    auto result = score::os::SimpleReadFunction::getOffset(msg, &ocb);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(*result.error(), ESPIPE);
}

TEST_F(SimpleReadFunctionTest, getOffset_null_ocb)
{
    io_read_t msg;
    RESMGR_OCB_T* ocb{nullptr};
    memset(&msg, 0, sizeof(msg));

    auto result = score::os::SimpleReadFunction::getOffset(&msg, ocb);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(*result.error(), ESPIPE);
}

TEST_F(SimpleReadFunctionTest, getOffset_null_msg_ocb)
{
    io_read_t* msg{nullptr};
    RESMGR_OCB_T* ocb{nullptr};

    auto result = score::os::SimpleReadFunction::getOffset(msg, ocb);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(*result.error(), ESPIPE);
}

TEST_F(SimpleReadFunctionTest, getOffset_io_type_offset)
{
    io_read_t msg;
    RESMGR_OCB_T ocb;
    off_t expected_off = 0xf1;

    memset(&msg, 0, sizeof(msg));
    memset(&ocb, 0, sizeof(ocb));
    msg.i.xtype = _IO_XTYPE_OFFSET;
    struct _xtype_offset* xoff = (struct _xtype_offset*)(&msg.i + 1);
    xoff->offset = expected_off;

    auto offset_result = score::os::SimpleReadFunction::getOffset(&msg, &ocb);
    ASSERT_TRUE(offset_result);
    ASSERT_EQ(offset_result.value(), expected_off);
}

TEST_F(SimpleReadFunctionTest, getOffset_io_type_none)
{
    io_read_t msg;
    RESMGR_OCB_T ocb;
    off_t expected_off = 0xf1;

    memset(&msg, 0, sizeof(msg));
    memset(&ocb, 0, sizeof(ocb));
    msg.i.xtype = _IO_XTYPE_NONE;
    ocb.offset = expected_off;

    auto offset_result = score::os::SimpleReadFunction::getOffset(&msg, &ocb);
    ASSERT_TRUE(offset_result);
    ASSERT_EQ(offset_result.value(), expected_off);
}

TEST_F(SimpleReadFunctionTest, getOffset_invalid_io_type)
{
    io_read_t msg;
    RESMGR_OCB_T ocb;

    memset(&msg, 0, sizeof(msg));
    memset(&ocb, 0, sizeof(ocb));
    // set invalid type
    msg.i.xtype = ~_IO_XTYPE_OFFSET;

    auto result = score::os::SimpleReadFunction::getOffset(&msg, &ocb);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(*result.error(), ENOSYS);
}

TEST_F(SimpleReadFunctionTest, func_operator_returns_invalid_system_call_due_to_verify_read)
{
    EXPECT_CALL(iofunc_mock, iofunc_read_verify).WillOnce(Return(score::cpp::make_unexpected(EPERM)));
    // return error due to iofunc_read_verify catches error
    ASSERT_EQ(simple_read_function_mock(nullptr, nullptr, nullptr), EPERM);
}

TEST_F(SimpleReadFunctionTest, func_operator_returns_invalid_seeking_due_to_offset_fail)
{

    EXPECT_CALL(iofunc_mock, iofunc_read_verify).WillOnce(Return(score::cpp::blank()));
    // return error due to getoffset catches ocb and msg are nullptr
    ASSERT_EQ(simple_read_function_mock(nullptr, nullptr, nullptr), ESPIPE);
}

TEST_F(SimpleReadFunctionTest, func_operator_return_error_due_to_preRead)
{
    EXPECT_CALL(iofunc_mock, iofunc_read_verify).WillOnce(Return(score::cpp::blank()));
    EXPECT_CALL(simple_read_function_mock, preRead)
        .WillOnce(Return(score::MakeUnexpected(score::os::ErrorCode::kInvalidArgument)));

    io_read_t msg;
    RESMGR_OCB_T ocb;
    IOFUNC_ATTR_T attr;
    off_t offset = 412;

    // init the msg and ocb by valid expected data
    memset(&msg, 0, sizeof(msg));
    memset(&ocb, 0, sizeof(ocb));
    memset(&attr.attr, 0, sizeof(attr));
    ocb.attr = &attr;

    msg.i.xtype = _IO_XTYPE_OFFSET;
    struct _xtype_offset* xoff = (struct _xtype_offset*)(&msg.i + 1);
    xoff->offset = offset;

    auto res = simple_read_function_mock(nullptr, &msg, &ocb);

    // return error due to preRead
    ASSERT_EQ(res, EINVAL);
}

TEST_F(SimpleReadFunctionTest, func_operator_return_error_due_to_read)
{
    EXPECT_CALL(iofunc_mock, iofunc_read_verify).WillOnce(Return(score::cpp::blank()));
    EXPECT_CALL(simple_read_function_mock, preRead).WillOnce(Return(score::Result<void>()));
    EXPECT_CALL(simple_read_function_mock, read)
        .WillOnce(Return(score::MakeUnexpected(score::os::ErrorCode::kInvalidArgument)));

    io_read_t msg;
    RESMGR_OCB_T ocb;
    IOFUNC_ATTR_T attr;
    off_t offset = 412;

    // init the msg and ocb by valid expected data
    memset(&msg, 0, sizeof(msg));
    memset(&ocb, 0, sizeof(ocb));
    memset(&attr.attr, 0, sizeof(attr));
    ocb.attr = &attr;

    msg.i.xtype = _IO_XTYPE_OFFSET;
    struct _xtype_offset* xoff = (struct _xtype_offset*)(&msg.i + 1);
    xoff->offset = offset;

    auto res = simple_read_function_mock(nullptr, &msg, &ocb);

    // return error due to read
    ASSERT_EQ(res, EINVAL);
}

TEST_F(SimpleReadFunctionTest, func_operator_return_error_due_to_post_read)
{
    EXPECT_CALL(iofunc_mock, iofunc_read_verify).WillOnce(Return(score::cpp::blank()));
    EXPECT_CALL(simple_read_function_mock, preRead).WillOnce(Return(score::Result<void>()));
    EXPECT_CALL(simple_read_function_mock, read).WillOnce(Return(0));
    EXPECT_CALL(simple_read_function_mock, postRead)
        .WillOnce(Return(score::MakeUnexpected(score::os::ErrorCode::kInvalidArgument)));

    io_read_t msg;
    RESMGR_OCB_T ocb;
    IOFUNC_ATTR_T attr;
    off_t offset = 412;

    // init the msg and ocb by valid expected data
    memset(&msg, 0, sizeof(msg));
    memset(&ocb, 0, sizeof(ocb));
    memset(&attr.attr, 0, sizeof(attr));
    ocb.attr = &attr;

    msg.i.xtype = _IO_XTYPE_OFFSET;
    struct _xtype_offset* xoff = (struct _xtype_offset*)(&msg.i + 1);
    xoff->offset = offset;

    auto res = simple_read_function_mock(nullptr, &msg, &ocb);

    // return error due to preRead
    ASSERT_EQ(res, EINVAL);
}

TEST_F(SimpleReadFunctionTest, func_operator_returns_error_due_to_resmgr_msgwrite)
{
    EXPECT_CALL(iofunc_mock, iofunc_read_verify).WillOnce(Return(score::cpp::blank()));
    EXPECT_CALL(simple_read_function_mock, preRead).WillOnce(Return(score::Result<void>()));
    EXPECT_CALL(simple_read_function_mock, read).WillOnce(Return(8));
    EXPECT_CALL(simple_read_function_mock, postRead).WillOnce(Return(score::Result<void>()));
    EXPECT_CALL(resmgr_mock, resmgr_msgwrite)
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EPERM))));
    // simulate the error is occured so set errno with an error
    score::os::seterrno(EPERM);

    io_read_t msg;
    RESMGR_OCB_T ocb;
    IOFUNC_ATTR_T attr;
    off_t offset = 412;

    // init the msg and ocb by valid expected data
    memset(&msg, 0, sizeof(msg));
    memset(&ocb, 0, sizeof(ocb));
    memset(&attr.attr, 0, sizeof(attr));
    ocb.attr = &attr;

    msg.i.xtype = _IO_XTYPE_OFFSET;
    struct _xtype_offset* xoff = (struct _xtype_offset*)(&msg.i + 1);
    xoff->offset = offset;

    // return error due to resmgr_msgwrite catches an error so errno code will be returned reported
    ASSERT_EQ(simple_read_function_mock(nullptr, &msg, &ocb), -1);
}

TEST_F(SimpleReadFunctionTestFixture, CheckResourceManagerHappyFlow)
{
    // in order to reduce locking overhead, we explicitly disable locking on message handler list access
    const auto dpp_expected = dispatch.dispatch_create_channel(-1, DISPATCH_FLAG_NOLOCK);
    ASSERT_TRUE(dpp_expected);

    // dispatch handle (dpp). A pointer to an opaque structure that describes the service channel
    // (channel id, access rights and callbacks)
    dispatch_t* const dispatch_pointer = dpp_expected.value();

    // _RESMGR_FLAG_SELF flag is required to allow client connections from the same process
    // (beware of potential deadlocks)
    const auto id_expected = dispatch.resmgr_attach(
        dispatch_pointer, &resmgr_attr, test_path, _FTYPE_ANY, _RESMGR_FLAG_SELF, &connect_funcs, &io_funcs, &attr);
    ASSERT_TRUE(id_expected);
    const std::int32_t id = id_expected.value();

    // attach a private message handler to process service termination messages
    EXPECT_TRUE(dispatch.message_attach(dispatch_pointer,
                                        nullptr,
                                        kPrivateMessageTypeFirst,
                                        kPrivateMessageTypeLast,
                                        &private_message_handler,
                                        nullptr));

    // after this call, we won't be able to manipulate the message handler list (due to DISPATCH_FLAG_NOLOCK)
    const auto ctp_expected = dispatch.dispatch_context_alloc(dispatch_pointer);
    ASSERT_TRUE(ctp_expected);

    // context handle (ctp). A pointer to a defined structure that describes the current state of the dispatch thread
    // (dispatch handle, message data, client data, our user-specified data pointer)
    // There can be several such contexts per a single dispatch handle if thread pools are employed.
    dispatch_context_t* const context_pointer = ctp_expected.value();
    // Pay attention to a slight difference between dispatch_context_t and resmgr_context_t.
    // dispatch_context_t is a union of several contexts. In our case, it contains resmgr_context_t.

    // a tricky, but documented (since QNX 7.0) way to pass user-defined data to resmgr callbacks
    context_pointer->resmgr_context.extra->data = this;

    // create a client connection for private messages. This connection does not need a full-blown resmgr protocol.
    // In particular, it can be used to send service terminate messages and then be closed without errors.
    // On the other hand, posix calls won't work with this connection.
    const auto coid_expected = dispatch.message_connect(dispatch_pointer, MSG_FLAG_SIDE_CHANNEL);
    ASSERT_TRUE(coid_expected);
    const std::int32_t side_channel_coid = coid_expected.value();

    // launch the service listen/reply loop in a separate thread
    // the thread is supposed to finish after an _IO_MSG request request
    auto future = std::async(std::launch::async, [context_pointer]() noexcept {
        score::cpp::expected<bool, std::int32_t> result{};
        do
        {
            result = NextServiceRequest(context_pointer);
        } while (result.has_value() && result.value());
        return result;
    });

    // now, create a client and read from the service using standard POSIX calls. Check for expected results.
    const std::int32_t fd = ::open(test_path, O_RDONLY);
    ASSERT_TRUE(fd != -1);
    // here we simulating reading from 64bit registers.
    const std::size_t reg_size = sizeof(uint64_t);
    constexpr std::size_t n = 1;
    char buf[reg_size + 1];
    ::memset(&buf[0], 0, reg_size + 1);

    auto res = lseek(fd, n, SEEK_SET);
    ASSERT_NE(res, -1);
    EXPECT_EQ(::read(fd, buf, reg_size), reg_size);
    // we assert that lseek and read functions move the curser by n weighted by reg_size inside the testing string and
    // finally get the data from this string in another word it simulates that seek function moves the curser to the
    // n-th register
    EXPECT_TRUE(strcmp(buf, read_data.c_str() + (reg_size * n)) == 0);

    // now, let's repeat it with readblock to check for combined seek and read operation
    ::memset(&buf[0], 0, reg_size + 1);
    EXPECT_EQ(::readblock(fd, 1, n, reg_size, buf), reg_size);
    EXPECT_TRUE(strcmp(buf, read_data.c_str() + (reg_size * n)) == 0);

    // send the service terminate message
    const std::uint16_t msg{kPrivateMessageTerminate};
    const auto result = channel.MsgSend(side_channel_coid, &msg, sizeof(msg), nullptr, kNoSize);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), EOK);

    // the service thread has terminated. Wait for it.
    future.wait();
    EXPECT_TRUE(future.get().has_value());

    // now, we can close the private connection. _IO_CLOSE won't be sent, so we will succeed even with no one handling
    // the service loop. As the service channel handle is not closed yet, ::close() would deadlock here.
    EXPECT_TRUE(channel.ConnectDetach(side_channel_coid));

    // close the service channel and free all the associated service structures
    EXPECT_TRUE(dispatch.resmgr_detach(dispatch_pointer, id, _RESMGR_DETACH_CLOSE));
    EXPECT_TRUE(dispatch.dispatch_destroy(dispatch_pointer));
    dispatch.dispatch_context_free(context_pointer);
}

}  // namespace
