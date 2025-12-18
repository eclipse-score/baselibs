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
#ifndef SCORE_MW_LOG_CUSTOM_RECORDER_H_2
#define SCORE_MW_LOG_CUSTOM_RECORDER_H_2

#include "score/mw/log/custom_recorder_example/custom_recorder_factory_impl.h"
#include "score/mw/log/custom_recorder_example/custom_recorder_impl.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

// the user should provide these two alias based on their implementation
using CustomRecorder = user::specific::impl::detail::CustomRecorderImpl;
using CustomRecorderFactory = user::specific::impl::detail::CustomRecorderFactoryImpl;

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CUSTOM_RECORDER_H
