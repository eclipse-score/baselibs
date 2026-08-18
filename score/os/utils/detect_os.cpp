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
#include "score/os/utils/detect_os.h"

#include <score/optional.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <cctype>

namespace
{
bool IsOs(const std::string os);
const std::string osLinux{"linux"};
const std::string osQnx{"qnx"};

bool IsOs(const std::string os)
{
    bool returnValue{false};
    /* Branching is due to hidden exception handling */
    score::cpp::optional<score::os::SystemInfo> sysInfo =
        score::os::Uname::instance().GetUname(); /* LCOV_EXCL_BR_LINE */

    if (sysInfo.has_value())
    {
        /* Branching is due to hidden exception handling */
        /* LCOV_EXCL_BR_START */
        const auto info = sysInfo.value();
        std::string sysName{info.sysname};
        /* LCOV_EXCL_BR_STOP */
        score::cpp::ignore = std::transform(sysName.begin(),
                                            sysName.end(),
                                            sysName.begin(),
                                            /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC,MISRA.CONV.INT.SIGN: */
                                            /* Unsigned char is used for char data handling */
                                            [](const unsigned char c) {
                                                return std::tolower(c);
                                            });
        /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC,MISRA.CONV.INT.SIGN: */
        const std::size_t found = sysName.find(os);
        returnValue = found != std::string::npos;
    }
    return returnValue;
}
}  // namespace

bool score::os::IsLinux()
{
    /* Branching is due to hidden exception handling */
    return IsOs(osLinux) && (!IsOs(osQnx)); /* LCOV_EXCL_BR_LINE */
}

bool score::os::IsQnx()
{
    /* Branching is due to hidden exception handling */
    return IsOs(osQnx) && (!IsOs(osLinux)); /* LCOV_EXCL_BR_LINE */
}
