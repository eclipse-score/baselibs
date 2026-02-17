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

#ifndef SCORE_LIB_OS_QNX_RESOURCE_MANAGER_STATIC_WRAPPERS_H
#define SCORE_LIB_OS_QNX_RESOURCE_MANAGER_STATIC_WRAPPERS_H

#include "score/os/qnx/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @brief the C static handler of read() request that
/// should be registered to the QNX system during setup the resource manager.
///
/// @param ctp A pointer to a resmgr_context_t structure that the resource-manager
/// library uses to pass context information between functions.
/// @param msg A pointer to the io_read_t structure that contains the message that
/// the resource manager received.
/// @param ocb A pointer to the iofunc_ocb_t structure for the Open Control Block that was
/// created when the client opened the resource.
///
/// @return the status of the read request.
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
int io_read_handler(resmgr_context_t* ctp, io_read_t* msg, iofunc_ocb_t* ocb);
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */

/// @brief the C static handler of write() request that
/// should be registered to the QNX system during setup the resource manager.
///
/// @param ctp A pointer to a resmgr_context_t structure that the resource-manager
/// library uses to pass context information between functions.
/// @param msg A pointer to the io_write_t structure that contains the message that
/// the resource manager received.
/// @param ocb A pointer to the iofunc_ocb_t structure for the Open Control Block that was
/// created when the client opened the resource.
///
/// @return the status of the write request.
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
int io_write_handler(resmgr_context_t* ctp, io_write_t* msg, iofunc_ocb_t* ocb);
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */

/// @brief the C static handler of lseek() request that
/// should be registered to the QNX system during setup the resource manager.
///
/// @param ctp A pointer to a resmgr_context_t structure that the resource-manager
/// library uses to pass context information between functions.
/// @param msg A pointer to the io_lseek_t structure that contains the message that
/// the resource manager received.
/// @param ocb A pointer to the iofunc_ocb_t structure for the Open Control Block that was
/// created when the client opened the resource.
///
/// @return the status of the lseek request.
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
int io_lseek_handler(resmgr_context_t* ctp, io_lseek_t* msg, iofunc_ocb_t* ocb);
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */

/// @brief the C static handler of open() request that
/// should be registered to the QNX system during setup the resource manager.
///
/// @param ctp A pointer to a resmgr_context_t structure that the resource-manager
/// library uses to pass context information between functions.
/// @param msg A pointer to the io_open_t structure that contains the message that
/// the resource manager received.
/// @param attr A pointer to the iofunc_attr structure for the handle function attribute that was
/// created when the client opened the resource.
/// @param extra Extra information from the library. If you're calling iofunc_open_default()
/// from a resource manager's open() function, simply pass the extra argument that's passed to open().
///
/// @return the status of the open request.
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */
/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
int connect_open_handler(resmgr_context_t* ctp, io_open_t* msg, extended_dev_attr_t* attr, void* extra);
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR: False positive not relevant. */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Should fit QNX library signature */

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SCORE_LIB_OS_QNX_RESOURCE_MANAGER_STATIC_WRAPPERS_H
