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
#include "custom_recorder_factory_impl.h"
#include "custom_recorder_impl.h"

namespace user
{
namespace specific
{
namespace impl
{
namespace detail
{

CustomRecorderFactoryImpl::CustomRecorderFactoryImpl(score::cpp::pmr::unique_ptr<score::os::Fcntl>)
    : LogRecorderFactory<CustomRecorderFactoryImpl>()
{
}

std::unique_ptr<score::mw::log::Recorder> CustomRecorderFactoryImpl::CreateConcreteLogRecorder(
    const score::mw::log::detail::Configuration&,
    score::cpp::pmr::memory_resource*)
{
    return std::make_unique<CustomRecorderImpl>();
}

}  // namespace detail
}  // namespace impl
}  // namespace specific
}  // namespace user
