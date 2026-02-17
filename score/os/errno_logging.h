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

#ifndef SCORE_LIB_OS_ERRNO_LOGGING_H
#define SCORE_LIB_OS_ERRNO_LOGGING_H

#include "score/os/errno_decl.h"

namespace score
{

namespace mw
{
namespace log
{

class LogStream;

}  // namespace log
}  // namespace mw

namespace os
{
/// \brief Global overload of a LogStream operator to enable translation from error to human readable representation)
score::mw::log::LogStream& operator<<(score::mw::log::LogStream& log_stream, const score::os::Error& error) noexcept;

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_ERRNO_LOGGING_H
