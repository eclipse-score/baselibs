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
#include "score/os/errno_logging.h"
#include "score/mw/log/log_stream.h"
#include "score/os/errno.h"
#include <string_view>

// Suppress "AUTOSAR C++14 A3-2-2" rule finding: "A binary arithmetic operator and a bitwise
// operator shall return a “prvalue”." Here, stream input operator is implemented,
// which needs to return reference to self to support cascading. Here '<<' is not a left shift operator but an overload
// for logging the respective types. code analysis tools tend to assume otherwise hence a false positive.
// coverity[autosar_cpp14_a13_2_2_violation]
score::mw::log::LogStream& score::os::operator<<(score::mw::log::LogStream& log_stream,
                                                 const score::os::Error& error) noexcept
{
    log_stream << "An OS error has occurred with error code: ";

    auto error_string = error.ToStringContainer(error);
    log_stream << std::string_view{error_string.data()};

    return log_stream;
}
