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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_TYPES
#define SCORE_ANALYSIS_TRACING_COMMON_TYPES

#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"
#include "score/memory/shared/shared_memory_resource.h"
#include "score/result/result.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

namespace score
{
namespace analysis
{
namespace tracing
{

namespace alignment
{
// False positive, value is used as a default argument in Allocate()
// coverity[autosar_cpp14_a0_1_1_violation]
constexpr auto kBlockSize = 64U;
}  // namespace alignment

// False positive, value is used as a the size for chunk lists
// coverity[autosar_cpp14_a0_1_1_violation]
constexpr std::uint8_t kMaxChunksPerOneTraceRequest = 10U;

// Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain
// instances of non-volatile variables being given values that are not subsequently used."
// The variable kNumberOfElements is declared and given value that is used in multiple files
// like object_factory.cpp and trace_job_processor_factory.cpp (false positive)
// coverity[autosar_cpp14_a0_1_1_violation: False]
constexpr std::uint16_t kNumberOfElements = 500U;
// Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain
// instances of non-volatile variables being given values that are not subsequently used."
// The variable kRingBufferSharedMemorySize is declared and given value that is used in multiple files
// like shm_ring_buffer.cpp (false positive)
// coverity[autosar_cpp14_a0_1_1_violation: False]
constexpr std::size_t kRingBufferSharedMemorySize = 102400U;

/// @brief Type used to store Trace client Id
using TraceClientId = std::uint8_t;

/// @brief Type used to store Trace Context Id used to distinguish traced data
using TraceContextId = std::uint32_t;

/// @brief Type used to store Global Trace Context Id do distinguish traced data from different clients
struct GlobalTraceContextId
{
    TraceClientId client_id_;
    TraceContextId context_id_;
};

struct TmdStatistics
{
    std::size_t tmd_total;
    std::size_t tmd_max;
    std::size_t tmd_average;
    // Suppress "AUTOSAR C++14 A9-6-1" rule finding. This rule states: "Data types used for interfacing with
    // hardware or conforming to communication protocols shall be trivial, standard-layout and only contain members
    // of types with defined sizes.".
    // client_pid is a system-defined identifier used for OS process handling inside the same ECU (IPC).
    // No external communication interface is provided.
    // coverity[autosar_cpp14_a9_6_1_violation]
    pid_t client_pid;
    // Suppress "AUTOSAR C++14 A9-6-1" rule finding. This rule states: "Data types used for interfacing with
    // hardware or conforming to communication protocols shall be trivial, standard-layout and only contain members
    // of types with defined sizes.".
    // tmd_alloc_rate is used for internal calculations only inside the same ECU, not interfacing with external
    // hardware or communication protocols.
    // coverity[autosar_cpp14_a9_6_1_violation]
    float tmd_alloc_rate;
};

inline bool operator==(const GlobalTraceContextId& lhs, const GlobalTraceContextId& rhs) noexcept
{
    return (lhs.client_id_ == rhs.client_id_) && (lhs.context_id_ == rhs.context_id_);
}

inline bool operator<(const GlobalTraceContextId& lhs, const GlobalTraceContextId& rhs) noexcept
{
    return (lhs.client_id_ == rhs.client_id_) ? (lhs.context_id_ < rhs.context_id_) : (lhs.client_id_ < rhs.client_id_);
}

enum class BindingType : std::uint8_t
{
    kFirst = 0,
    kLoLa = kFirst,
    kVector = 1,
    kVectorZeroCopy = 2,
    kUndefined = 3,
};

/// @brief Type used to store pointer to ManagedMemoryResource
using ResourcePointer = std::shared_ptr<score::memory::shared::ManagedMemoryResource>;

/// @brief Type used to store pointer to SharedMemoryResource
using SharedResourcePointer = std::shared_ptr<score::memory::shared::ISharedMemoryResource>;

/// Type used to store handle to shared-memory region, used to optimize data transfers
using ShmObjectHandle = std::int32_t;

/// Type used to store "scoid" (server connection id), which is unique for each connection to server's channel in QNX
/// message passing
using ClientScoid = std::int32_t;

/// Type used to store client's "pid" (process id)
using ClientPid = std::uint32_t;

// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
static constexpr ShmObjectHandle kInvalidSharedObjectIndex{-1};

// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
static constexpr std::size_t kApplicationIdentifierLength{255U};
/// Type used to store the max size for application identifier it's choosen based on
/// max allowed file name in QNX which is 255 chars.
using AppIdType = std::array<char, kApplicationIdentifierLength>;

// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
static constexpr std::size_t kIpcProtocolMetaInfoApplicationIdentifierLength{8U};
/// Type used to store the max size for application identifier it's choosen based on
/// "ad_make_40ms_asil_b_low_level_perception" which is 40 chars + 10 chars if a new client would have a newername
using IpcProtocolMetaInfoAppIdType = std::array<char, kIpcProtocolMetaInfoApplicationIdentifierLength>;

/// We use a ScopedFunction so that the tracing runtime controls the scope during which the callback can
/// be called.
using TraceDoneCallBackType = safecpp::MoveOnlyScopedFunction<void(TraceContextId)>;

/// @brief Type used to return from RegisterClient method
using RegisterClientResult = score::Result<TraceClientId>;

/// @brief Type used to return from RegisterShmObject method
using RegisterSharedMemoryObjectResult = score::Result<ShmObjectHandle>;

/// @brief Type used to return from RegisterTraceDoneCB method
using RegisterTraceDoneCallBackResult = score::Result<Blank>;

/// @brief Type used to return from Trace method
using TraceResult = score::Result<Blank>;

/// @brief Flags to be used when opening client-side SHM objects
// No harm to do bitwise operations on these values m5_0_21
// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
//  coverity[autosar_cpp14_m5_0_21_violation]
static constexpr std::int32_t kSharedMemoryObjectOpenModes{S_IRUSR | S_IRGRP};

/// @brief Flags to be used when opening client-side SHM objects
// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
static constexpr std::int32_t kSharedMemoryObjectOpenFlags{O_RDONLY};

/// @brief Flags to be used when opening client-side SHM for ShmRingBuffer
// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
static constexpr std::int32_t kSharedMemoryRingBufferObjectOpenFlags{O_RDWR};

/// @brief Flags to be used when opening client-side SHM for ShmRingBuffer
// No harm to do bitwise operations on these values m5_0_21
// Suppress "AUTOSAR C++14 A0-1-1" rule finds: "A project shall not contain instances of non-volatile variables
// being given values that are not subsequently used"
// False positive, variable is used.
// Suppress "autosar_cpp14_m5_0_21_violation" rule finding. This rule states: "Bitwise operators
// shall only be applied to operands of unsigned underlying type."
// QNX mode permission macros (S_IRUSR, S_IWUSR, etc.) are defined as signed integers by the standard.
// Only bitwise OR operations are performed - no right shifts that could cause implementation-defined behavior.
// clang-format off
// coverity[autosar_cpp14_m5_0_21_violation]
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
static constexpr std::uint32_t kSharedMemoryRingBufferObjectOpenModes{static_cast<std::uint32_t>(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)};
// clang-format on

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif /* SCORE_ANALYSIS_TRACING_COMMON_TYPES */
