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
#ifndef SCORE_MW_LOG_EXAMPLE_CUSTOM_RECORDER_FACTORY_IMPL_H
#define SCORE_MW_LOG_EXAMPLE_CUSTOM_RECORDER_FACTORY_IMPL_H

#include "score/mw/log/detail/log_recorder_factory.hpp"

namespace user
{
namespace specific
{
namespace impl
{
namespace detail
{
class CustomRecorderFactoryImpl : public score::mw::log::detail::LogRecorderFactory<CustomRecorderFactoryImpl>
{
  public:
    CustomRecorderFactoryImpl() = default;
    explicit CustomRecorderFactoryImpl(score::cpp::pmr::unique_ptr<score::os::Fcntl> fcntl_instance);
    std::unique_ptr<score::mw::log::Recorder> CreateConcreteLogRecorder(const score::mw::log::detail::Configuration& config,
                                                                      score::cpp::pmr::memory_resource* memory_resource);
};

}  // namespace detail
}  // namespace impl
}  // namespace specific
}  // namespace user

#endif  // SCORE_MW_LOG_EXAMPLE_CUSTOM_RECORDER_FACTORY_IMPL_H
