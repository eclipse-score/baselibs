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
#ifndef SCORE_MW_LOG_DETAIL_COMMON_RECORDER_FACTORY_H
#define SCORE_MW_LOG_DETAIL_COMMON_RECORDER_FACTORY_H

#include "score/os/fcntl_impl.h"
#include "score/mw/log/configuration/target_config_reader.h"
#include "score/mw/log/irecorder_factory.h"
#include "score/mw/log/recorder.h"

#include <memory>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
/*
Deviation from Rule: M3-2-3
- A type, object or function that is used in multiple translation units shall be declared in one and only one file
Justification:
- class RecorderFactory only defined once
*/
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class RecorderFactory : public IRecorderFactory
{
  public:
    RecorderFactory() noexcept = default;

    std::unique_ptr<Recorder> CreateFromConfiguration(
        score::cpp::pmr::memory_resource* memory_resource) const noexcept override;
    std::unique_ptr<Recorder> CreateFromConfiguration(const std::unique_ptr<const ITargetConfigReader> config_reader,
                                                      score::cpp::pmr::memory_resource* memory_resource) const noexcept;

    std::unique_ptr<Recorder> CreateWithConsoleLoggingOnly(
        score::cpp::pmr::memory_resource* memory_resource) const noexcept override;

    std::unique_ptr<Recorder> CreateStub() const noexcept override;

    std::unique_ptr<Recorder> CreateRecorderFromLogMode(
        const LogMode& log_mode,
        const Configuration& config,
        score::cpp::pmr::unique_ptr<score::os::Fcntl> fcntl_instance =
            score::os::FcntlImpl::Default(score::cpp::pmr::get_default_resource()),
        score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource()) const noexcept;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_COMMON_RECORDER_FACTORY_H
