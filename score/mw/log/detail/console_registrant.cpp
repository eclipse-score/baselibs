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
#include "score/mw/log/backend_table.h"
#include "score/mw/log/detail/text_recorder/console_recorder_factory.h"

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

std::unique_ptr<Recorder> CreateConsoleRecorder(const Configuration& config, score::cpp::pmr::memory_resource* memory_resource)
{
    ConsoleRecorderFactory factory;
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
Deviation from Rule M0-1-3:
- A project shall not contain unused variables.
Deviation from Rule M0-1-9:
- There shall be no dead code.
Justification:
- The variable IS used via its constructor's side effect during static initialization.
  BackendRegistrant's constructor registers the CreateConsoleRecorder function pointer into
  gBackendCreators[] at program startup. The variable itself doesn't need to be referenced
  elsewhere - its purpose is fulfilled by the constructor's execution. This is an intentional
  static registration pattern.
*/
// coverity[autosar_cpp14_a3_3_2_violation] See above
// coverity[autosar_cpp14_m0_1_3_violation] See above
// coverity[autosar_cpp14_m0_1_9_violation] See above
const BackendRegistrant kConsoleRegistrant{LogMode::kConsole, &CreateConsoleRecorder};

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
