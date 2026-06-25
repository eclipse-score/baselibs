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
#include "score/mw/log/log_stream.h"

// Suppress "AUTOSAR C++14 A13-2-2", The rule states: "A binary arithmetic operator and a bitwise operator shall return
// a “prvalue”." The code with '<<' is not a left shift operator but an overload for logging the respective types. code
// analysis tools tend to assume otherwise hence a false positive.
// coverity[autosar_cpp14_a13_2_2_violation]
score::mw::log::LogStream& score::mw::log::operator<<(score::mw::log::LogStream& log_stream,
                                                  const score::result::Error& error) noexcept
{
    log_stream << "Error ";
    log_stream << error.Message();
    log_stream << " occurred";
    if (not error.UserMessage().empty())
    {
        log_stream << " with message ";
        log_stream << error.UserMessage();
    }
    return log_stream;
}

// coverity[autosar_cpp14_a13_2_2_violation]
score::mw::log::LogStream&& score::mw::log::operator<<(score::mw::log::LogStream&& log_stream,
                                                   const score::result::Error& error) noexcept
{
    // Suppress "AUTOSAR C++14 A18-9-2" rule findings. This rule stated: "Forwarding values to other functions shall be
    // done via: (1) std::move if the value is an rvalue reference, (2) std::forward if the value is forwarding
    // reference". False positive, the tool is flagging the use of << as a forwarding operation, but this is the stream
    // insertion operator.
    // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
    log_stream << "Error ";
    // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
    log_stream << error.Message();
    // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
    log_stream << " occurred";
    if (not error.UserMessage().empty())
    {
        // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
        log_stream << " with message ";
        // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
        log_stream << error.UserMessage();
    }
    return std::move(log_stream);
}
