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

#ifndef SCORE_LIB_OS_QNX_TYPES_H
#define SCORE_LIB_OS_QNX_TYPES_H

/* KW_SUPPRESS_START:MISRA.INCL.INSIDE */
/* Inclusion order is important to provide the right system defintion with the */
/* library */
// clang-format off
// the order is important to correctly define RESMGR_HANDLE_T, see QNX resmgr_attach() documentation
#include "score/os/qnx/types/iofunc_types.h"
// clang-format on
#include <memory>

#include "score/os/qnx/types/i_open_function.h"
#include "score/os/qnx/types/i_read_function.h"
#include "score/os/qnx/types/i_seek_function.h"
#include "score/os/qnx/types/i_write_function.h"
/* KW_SUPPRESS_END:MISRA.INCL.INSIDE */
/* Inclusion order is important to provide the right system defintion with the */
/* library */
namespace score
{
namespace os
{

/// @brief Resource Manager Configuration (General for all resources to be managed).
///
/// Responsibilities
/// - defining the generic configuration of the resource manager.
///
struct ResourceManagerConfig
{
    /// @brief Flags that affect the behavior of the resource manager interface.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint32_t flags;

    /// @brief The number of components to allocate for the IOV array.
    ///
    /// If you specify 0, the resource manager library bumps the value to the minimum usable by the library itself.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint32_t nparts_max;

    /// @brief The minimum amount of room to reserve for receiving a message that's allocated in resmgr_context_alloc().
    ///
    /// If the value is too low, or you specify it as 0, resmgr_attach() picks a value that's usable.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::uint32_t msg_max_size;
};

/// @brief structure includes pointer to the I/O and connect handlers
///
/// Responsibilities
/// - should be filled with the right handler of the i/o and connect function
///   to be registered in the system during the setup phase.
///
struct ResMgrHandlersSpec
{
    /// @brief reference to the configured handler for Open() request.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<IOpenFunction> open_handler;

    /// @brief reference to the configured handler for Read() request.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<IReadFunction> read_handler;

    /// @brief reference to the configured handler for Write() request.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<IWriteFunction> write_handler;

    /// @brief reference to the configured handler for lseek() request.
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<ISeekFunction> lseek_handler;
};

}  // namespace os
}  // namespace score

/// @brief qnx device attributes
///
/// Responsibilities
/// - extention to the QNX type to include references to the i/O handler.
///
/* KW_SUPPRESS_START:MISRA.NS.GLOBAL: Passed to QNX C Library */
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init) attr passed to os by pointer to be initialize
// coverity[autosar_cpp14_m7_3_1_violation] see comment above
struct extended_dev_attr_t
{
    /// @brief The resource file properties (change time, permissions ....)
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    iofunc_attr_t attr{};

    /// @brief reference to the all configured I/O and Connect handlers for this resource
    // No harm to define it as public
    // coverity[autosar_cpp14_m11_0_1_violation]
    score::os::ResMgrHandlersSpec handlers;
};
// NOLINTEND(cppcoreguidelines-pro-type-member-init)
/* KW_SUPPRESS_END:MISRA.NS.GLOBAL: Passed to QNX C Library */

#endif  // SCORE_LIB_OS_QNX_TYPES_H
