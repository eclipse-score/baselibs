/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#include "score/mw/log/backend_table.h"
#include "score/mw/log/detail/slog/slog_recorder_factory.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

std::unique_ptr<Recorder> CreateSlogRecorder(const Configuration& config, score::cpp::pmr::memory_resource* memory_resource)
{
    SlogRecorderFactory factory;
    return factory.CreateLogRecorder(config, memory_resource);
}

/*
Deviation from Rule A3-3-2:
- Static and thread-local objects shall be constant-initialized.
Justification:
- BackendRegistrant constructor executes during dynamic initialization to write a function
  pointer into gBackendCreators[]. The target array is constant-initialized (zero-init
  at load time), so it is valid before this constructor runs. The registrant struct itself
  is trivially destructible. This follows the established pattern used by Runtime::Instance().
*/
// coverity[autosar_cpp14_a3_3_2_violation]
const BackendRegistrant kSlogRegistrant{LogMode::kSystem, &CreateSlogRecorder};

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
