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

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:False positive. */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: return type int used to be compatable with qnx io read function. */
int SimpleReadFunction::operator()(
    resmgr_context_t* ctp, /* KW_SUPPRESS:MISRA.VAR.NEEDS.CONST: edited at _IO_SET_READ_NBYTES */
    io_read_t* msg,        /* KW_SUPPRESS:MISRA.VAR.NEEDS.CONST:*/
                           // follow the qnx interface so it may be edited in  nested qnx function
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:False positive no macro is used */
    RESMGR_OCB_T* ocb)
/* KW_SUPPRESS_END:MISRA.USE.EXPANSION:False positive no macro is used */
{
    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    auto read_verify_result = iofunc_.iofunc_read_verify(ctp, msg, ocb, nullptr);
    if (!read_verify_result.has_value())
    {
        mw::log::LogError() << "error occoured on iofunc_read_verify: " << strerror(read_verify_result.error());
        return read_verify_result.error();
    }
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    auto get_offset_result = getOffset(msg, ocb);
    if (!get_offset_result.has_value())
    {
        mw::log::LogError() << get_offset_result.error().Message();
        return *get_offset_result.error();
    }
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
    /* KW_SUPPRESS_START:AUTOSAR.CAST.CSTYLE:  nbytes of type size_t to be passed to read()*/
    const size_t nbytes = _IO_READ_GET_NBYTES(msg);
    /* KW_SUPPRESS_END:AUTOSAR.CAST.CSTYLE:  nbytes of type size_t to be passed to read()*/
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    auto pre_read_result = preRead(get_offset_result.value(), nbytes);
    if (!pre_read_result.has_value())
    {
        mw::log::LogError() << pre_read_result.error().Message();
        return *pre_read_result.error();
    }
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

    uint64_t out_data = 0;
    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    auto read_result = read(get_offset_result.value(), nbytes, out_data);
    if (!read_result.has_value())
    {
        mw::log::LogError() << read_result.error().Message();
        return *read_result.error();
    }
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

    // Update access time
    if ((nbytes > 0) && (read_result.value() > 0))
    {
        /* KW_SUPPRESS_START:MISRA.CONV.INT.SIGN: Both operands are of type unsigned long */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Both operands are of type unsigned int */
        ocb->attr->attr.flags |= static_cast<std::uint32_t>(IOFUNC_ATTR_ATIME);
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Both operands are of type unsigned int */
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
        /* KW_SUPPRESS_END:MISRA.CONV.INT.SIGN */
    }

    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    auto post_read_result = postRead();
    if (!post_read_result.has_value())
    {
        mw::log::LogError() << post_read_result.error().Message();
        return *post_read_result.error();
    }
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    auto msgwrite_result = resmgr_.resmgr_msgwrite(ctp, &out_data, read_result.value(), 0);
    if (!msgwrite_result.has_value())
    {
        mw::log::LogError() << "error occoured on resmgr_msgwrite: " << msgwrite_result.error().ToString();
        return -1;
    }
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
    /* KW_SUPPRESS_START:MISRA.ASSIGN.SUBEXPR: false positive, no assignvment operator is used */
    /* KW_SUPPRESS_START:MISRA.CVALUE.IMPL.CAST.CPP:False positive: Not an implicit cast. */
    _IO_SET_READ_NBYTES(ctp, static_cast<int>(read_result.value()));
    /* KW_SUPPRESS_END:MISRA.CVALUE.IMPL.CAST.CPP:False positive: Not an implicit cast. */
    /* KW_SUPPRESS_END:MISRA.ASSIGN.SUBEXPR */
    return EOK;
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
}
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: return type int used to be compatable with qnx io read function. */
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:False positive. */

/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST:False positive. */
/* KW_SUPPRESS_START:MISRA.USE.EXPANSION:False positive no macro is used */
score::Result<off_t> SimpleReadFunction::getOffset(const io_read_t* msg, const RESMGR_OCB_T* ocb)
/* KW_SUPPRESS_END:MISRA.USE.EXPANSION:False positive no macro is used */
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
            /* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Needed for cast to void ptr */
            /* KW_SUPPRESS_START:MISRA.PTR.ARITH:Needed to get offset from this location */
            /* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME.UNDERSCORE::False positive. */
            // reinterpret_cast is needed for cast to void ptr
            // pointer arithmetic is needed to get offset from this location
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) justified
            auto xoff = reinterpret_cast<const struct _xtype_offset*>(
                &msg->i + 1);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) justified
            /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME.UNDERSCORE::False positive. */
            /* KW_SUPPRESS_END:MISRA.PTR.ARITH:Needed to get offset from this location */
            /* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:Needed for cast to void ptr */
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
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST:False positive. */

}  // namespace os
}  // namespace score
