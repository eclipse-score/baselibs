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
#ifndef MW_LOG_DETAILS_SVP_TIME_H_
#define MW_LOG_DETAILS_SVP_TIME_H_

#include <cstdint>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

struct SVPTime
{
    std::uint32_t timestamp;
    std::uint32_t sec;
    std::int32_t ms;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // MW_LOG_DETAILS_SVP_TIME_H_
