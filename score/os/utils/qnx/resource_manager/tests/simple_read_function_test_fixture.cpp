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
#include "score/os/utils/qnx/resource_manager/tests/simple_read_function_test_fixture.h"

constexpr std::size_t kNoSize{0};

SimpleReadFunctionTestFixture::SimpleReadFunctionTestFixture()
    : read_data{"data 1 &testing2"},
      to_exit{false},
      simple_read_function_stub_(SimpleReadFunctionStub(resmgr, iofunc, read_data))
{
    InitResmgrStructures();
}

// no failures could be diagnosed here
void SimpleReadFunctionTestFixture::InitResmgrStructures() noexcept
{
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 1024;

    // pre-configure resmgr callback data
    iofunc.iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.read = io_read;
    io_funcs.lseek = io_seek;

    constexpr mode_t attrMode{S_IFNAM | 0444};

    // pre-configure resmgr access rights data
    iofunc.iofunc_attr_init(&attr.attr, attrMode, nullptr, nullptr);
}

std::int32_t SimpleReadFunctionTestFixture::io_read(resmgr_context_t* ctp, io_read_t* msg, RESMGR_OCB_T* ocb) noexcept
{
    auto& fixture = *static_cast<SimpleReadFunctionTestFixture*>(ctp->extra->data);
    return fixture.simple_read_function_stub_(ctp, msg, ocb);
}

// seek call handler for RM
std::int32_t SimpleReadFunctionTestFixture::io_seek(resmgr_context_t* ctp, io_lseek_t* msg, iofunc_ocb_t* ocb) noexcept
{
    auto& fixture = *static_cast<SimpleReadFunctionTestFixture*>(ctp->extra->data);
    const auto result = fixture.iofunc.iofunc_lseek_default(ctp, msg, ocb);

    if (result.has_value())
    {
        return result.value();
    }
    else
    {
        return static_cast<int32_t>(result.error());
    }
}

// private message handler is called from resmgr framework, but doesn't rely on it as much as IO handlers do
// It may need to do some things manually, but that gives it and its clients higher flexibility
std::int32_t SimpleReadFunctionTestFixture::private_message_handler(message_context_t* const ctp,
                                                                    const std::int32_t /*code*/,
                                                                    const std::uint32_t /*flags*/,
                                                                    void* const /*handle*/) noexcept
{
    score::os::ChannelImpl channel;
    // we only accept private requests from ourselves. Testing manually, as resmgr won't do it for us
    const pid_t their_pid = ctp->info.pid;
    const pid_t our_pid = ::getpid();
    if (their_pid != our_pid)
    {
        // Unblock the sender with an error reply. Resmgr won't be doing this for us.
        channel.MsgError(ctp->rcvid, EACCES);
        return EOK;
    }

    // we use private messages as the means of self-termination
    // extract our test fixture and mark the termination status
    static_assert(std::is_same<message_context_t, resmgr_context_t>::value, "message_context_t format");
    auto& fixture = *static_cast<SimpleReadFunctionTestFixture*>(ctp->extra->data);
    fixture.to_exit = true;

    // Unblock the sender with our normal reply. Resmgr won't be doing this for us, too.
    channel.MsgReply(ctp->rcvid, EOK, nullptr, kNoSize);
    return EOK;
}

// a single iteration of the service thread loop. Returns false for termination request
score::cpp::expected<bool, std::int32_t> SimpleReadFunctionTestFixture::NextServiceRequest(
    dispatch_context_t* ctp) noexcept
{
    score::os::DispatchImpl dispatch;
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
    // extract our test fixture, update the statistics
    auto& fixture = *static_cast<SimpleReadFunctionTestFixture*>(ctp->resmgr_context.extra->data);
    return !fixture.to_exit;
}
