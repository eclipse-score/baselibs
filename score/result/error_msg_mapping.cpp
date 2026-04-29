/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#include "score/result/error_msg_mapping.h"

extern "C" void LibResultErrorDomainGetMessageForErrorCode(const score::result::ErrorDomain& domain,
                                                           score::result::ErrorCode code,
                                                           std::string_view& result) noexcept
{
    result = domain.MessageFor(code);
}
