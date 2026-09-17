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
// this header file is create to be called instead of <sys/iofunc.h> to guarantee the right order then right
// definitation for IOFUNC_ATTR_T and RESMGR_HANDLE_T
#ifndef SCORE_LIB_OS_QNX_IOFUNC_TYPES_H_
#define SCORE_LIB_OS_QNX_IOFUNC_TYPES_H_

// clang-format off
// the order is important to correctly define RESMGR_HANDLE_T, see QNX resmgr_attach() documentation
// coverity[autosar_cpp14_m7_3_1_violation] see comment above
struct extended_dev_attr_t;
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: The suppression is necessary to extend IOFUNC_ATTR_T, RESMGR_HANDLE_T with additional device-specific attributes while
// maintaining compatibility with QNX’s resource management framework.
// coverity[autosar_cpp14_a16_0_1_violation]
#define IOFUNC_ATTR_T struct extended_dev_attr_t
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#define RESMGR_HANDLE_T struct extended_dev_attr_t

// coverity[autosar_cpp14_m16_0_1_violation] see comment above
#include <sys/iofunc.h>
#include <sys/dispatch.h>
// clang-format off
// the order is important to correctly define RESMGR_HANDLE_T, see QNX resmgr_attach() documentation

#endif  // SCORE_LIB_OS_QNX_IOFUNC_TYPES_H_
