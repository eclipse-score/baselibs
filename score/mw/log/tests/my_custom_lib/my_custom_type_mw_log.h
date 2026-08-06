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
#ifndef SCORE_MW_LOG_TEST_MY_CUSTOM_LIB_MY_CUSTOM_TYPE_MW_LOG_H
#define SCORE_MW_LOG_TEST_MY_CUSTOM_LIB_MY_CUSTOM_TYPE_MW_LOG_H

#include "score/mw/log/log_stream.h"
#include "score/mw/log/tests/my_custom_lib/my_custom_type.h"

namespace my
{
namespace custom
{
namespace type
{

score::mw::log::LogStream& operator<<(score::mw::log::LogStream& log_stream,
                                    const my::custom::type::MyCustomType& my_custom_type) noexcept;

}  // namespace type
}  // namespace custom
}  // namespace my

#endif  // SCORE_MW_LOG_TEST_MY_CUSTOM_LIB_MY_CUSTOM_TYPE_MW_LOG_H
