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
#ifndef SCORE_LIB_DATETIME_CONVERTER_TIME_CONVERSION_H
#define SCORE_LIB_DATETIME_CONVERTER_TIME_CONVERSION_H

#include <time.h>
#include <chrono>

namespace score
{
namespace common
{

template <typename Duration>
timespec duration_to_timespec(Duration dur)
{
    timespec ts{};
    ts.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
    ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(dur % std::chrono::seconds(1)).count();
    return ts;
}

timespec timeout_in_timespec(const std::chrono::milliseconds timeout,
                             const std::chrono::time_point<std::chrono::system_clock> current_time);

}  // namespace common
}  // namespace score

#endif  // SCORE_LIB_DATETIME_CONVERTER_TIME_CONVERSION_H
