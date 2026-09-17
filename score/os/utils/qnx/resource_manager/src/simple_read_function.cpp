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
#include "score/os/utils/qnx/resource_manager/include/simple_read_function.h"

#include "score/os/utils/qnx/resource_manager/include/error.h"

#include "score/mw/log/logging.h"

namespace score
{
namespace os
{

SimpleReadFunction::SimpleReadFunction(ResMgr& resmgr, IoFunc& iofunc) : resmgr_(resmgr), iofunc_(iofunc) {}

int SimpleReadFunction::operator()(resmgr_context_t* ctp,
                                   io_read_t* msg,
                                   // follow the qnx interface so it may be edited in  nested qnx function
                                   RESMGR_OCB_T* ocb)
{
    auto read_verify_result = iofunc_.iofunc_read_verify(ctp, msg, ocb, nullptr);
    if (!read_verify_result.has_value())
    {
        mw::log::LogError() << "error occoured on iofunc_read_verify: " << strerror(read_verify_result.error());
        return read_verify_result.error();
    }

    auto get_offset_result = getOffset(msg, ocb);
    if (!get_offset_result.has_value())
    {
        mw::log::LogError() << get_offset_result.error().Message();
        return *get_offset_result.error();
    }

    const size_t nbytes = _IO_READ_GET_NBYTES(msg);
    auto pre_read_result = preRead(get_offset_result.value(), nbytes);
    if (!pre_read_result.has_value())
    {
        mw::log::LogError() << pre_read_result.error().Message();
        return *pre_read_result.error();
    }

    uint64_t out_data = 0;
    auto read_result = read(get_offset_result.value(), nbytes, out_data);
    if (!read_result.has_value())
    {
        mw::log::LogError() << read_result.error().Message();
        return *read_result.error();
    }

    // Update access time
    if ((nbytes > 0) && (read_result.value() > 0))
    {
        ocb->attr->attr.flags |= static_cast<std::uint32_t>(IOFUNC_ATTR_ATIME);
    }

    auto post_read_result = postRead();
    if (!post_read_result.has_value())
    {
        mw::log::LogError() << post_read_result.error().Message();
        return *post_read_result.error();
    }

    auto msgwrite_result = resmgr_.resmgr_msgwrite(ctp, &out_data, read_result.value(), 0);
    if (!msgwrite_result.has_value())
    {
        mw::log::LogError() << "error occoured on resmgr_msgwrite: " << msgwrite_result.error().ToString();
        return -1;
    }

    _IO_SET_READ_NBYTES(ctp, static_cast<int>(read_result.value()));
    return EOK;
}

score::Result<off_t> SimpleReadFunction::getOffset(const io_read_t* msg, const RESMGR_OCB_T* ocb)
{
    if ((msg == nullptr) || (ocb == nullptr))
    {
        return score::MakeUnexpected(ErrorCode::kIllegalSeek, "get offset error because 'msg' and 'ocb' is null");
    }

    score::Result<off_t> result;
    switch (msg->i.xtype & _IO_XTYPE_MASK)
    {
        case _IO_XTYPE_OFFSET:
        {
            // reinterpret_cast is needed for cast to void ptr
            // pointer arithmetic is needed to get offset from this location
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) justified
            auto xoff = reinterpret_cast<const struct _xtype_offset*>(
                &msg->i + 1);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) justified
            result = xoff->offset;
            break;
        }

        case _IO_XTYPE_NONE:
            result = ocb->offset;
            break;

        default:
            result = score::MakeUnexpected(ErrorCode::kUnsupportedFunction,
                                           "get offset error because of unknown offset type");
            break;
    }

    return result;
}

}  // namespace os
}  // namespace score
