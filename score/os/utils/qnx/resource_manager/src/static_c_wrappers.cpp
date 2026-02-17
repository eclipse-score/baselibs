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

#include "score/os/utils/qnx/resource_manager/include/static_c_wrappers.h"

/// @brief static C wrapper for read() request handler
///
/// As QNX system just accept c signature (pointer to c function) to register the handler,
/// This function has the intention to provide this signature and also wrap the context to the
/// the configured handler by the application if there (the handler is not nullptr).
/// in case there is no handler registered and the reference is nullptr,
/// this would call the default handler
/* KW_SUPPRESS_START:MISRA.NS.GLOBAL: Passed to QNX C Library */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN: False positive not relevant. */
int io_read_handler(resmgr_context_t* ctp, io_read_t* msg, iofunc_ocb_t* ocb)
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN: False positive not relevant. */
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_END:MISRA.NS.GLOBAL: Passed to QNX C Library */
{
    // The dynamic_cast is needed here to adapt the configured/overriden handler to be executed.
    const std::shared_ptr<score::os::IReadFunction> handler_reference = ocb->attr->handlers.read_handler;

    // check if the handler is registered
    if (handler_reference == nullptr)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
        return ENOSYS;  // Can't handle operation - return error.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
    }
    else
    {
        return (*handler_reference)(ctp, msg, ocb);
    }
}

/// @brief static C wrapper for write() request handler
///
/// As QNX system just accept c signature (pointer to c function) to register the handler,
/// This function has the intention to provide this signature and also wrap the context to the
/// the configured handler by the application if there (the handler is not nullptr).
/// in case there is no handler registered and the reference is nullptr,
/// this would call the default handler.
/* KW_SUPPRESS_START:MISRA.NS.GLOBAL: Passed to QNX C Library */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN: False positive not relevant. */
int io_write_handler(resmgr_context_t* ctp, io_write_t* msg, iofunc_ocb_t* ocb)
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN: False positive not relevant. */
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_END:MISRA.NS.GLOBAL: Passed to QNX C Library */
{
    // The dynamic_cast is needed here to adapt the configured/overriden handler to be executed.
    const std::shared_ptr<score::os::IWriteFunction> handler_reference = ocb->attr->handlers.write_handler;

    // check if the handler is registered
    if (handler_reference == nullptr)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
        return ENOSYS;  // Can't handle operation - return error.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
    }
    else
    {
        return (*handler_reference)(ctp, msg, ocb);
    }
}

/// @brief static C wrapper for lseek() request handler
///
/// As QNX system just accept c signature (pointer to c function) to register the handler,
/// This function has the intention to provide this signature and also wrap the context to the
/// the configured handler by the application if there (the handler is not nullptr).
/// in case there is no handler registered and the reference is nullptr,
/// this would call the default handler.
/* KW_SUPPRESS_START:MISRA.NS.GLOBAL: Passed to QNX C Library */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN: False positive not relevant. */
int io_lseek_handler(resmgr_context_t* ctp, io_lseek_t* msg, iofunc_ocb_t* ocb)
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN: False positive not relevant. */
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_END:MISRA.NS.GLOBAL: Passed to QNX C Library */
{
    // The dynamic_cast is needed here to adapt the configured/overriden handler to be executed.
    const std::shared_ptr<score::os::ISeekFunction> handler_reference = ocb->attr->handlers.lseek_handler;

    // check if the handler is registered
    if (handler_reference == nullptr)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
        return ENOSYS;  // Can't handle operation - return error.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
    }
    else
    {
        return (*handler_reference)(ctp, msg, ocb);
    }
}

/// @brief static C wrapper for open() request handler
///
/// As QNX system just accept c signature (pointer to c function) to register the handler,
/// This function has the intention to provide this signature and also wrap the context to the
/// the configured handler by the application if there (the handler is not nullptr).
/// in case there is no handler registered and the reference is nullptr,
/// this would call the default handler.
/* KW_SUPPRESS_START:MISRA.NS.GLOBAL: Passed to QNX C Library */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN: False positive not relevant. */
int connect_open_handler(resmgr_context_t* ctp, io_open_t* msg, extended_dev_attr_t* attr, void* extra)
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN: False positive not relevant. */
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST: Should fit QNX library signature */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_END:MISRA.NS.GLOBAL: Passed to QNX C Library */
{
    // The dynamic_cast is needed here to adapt the configured/overriden handler to be executed.
    const std::shared_ptr<score::os::IOpenFunction> handler_reference = attr->handlers.open_handler;
    // check if the handler is registered
    if (handler_reference == nullptr)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Library macros */
        return ENOSYS;  // Can't handle operation - return error.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Library macros */
    }
    else
    {
        return (*handler_reference)(ctp, msg, attr, extra);
    }
}
