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
#include "score/os/utils/qnx/resource_manager/tests/stubs/simple_read_function_stub.h"
#include "score/os/utils/qnx/resource_manager/include/error.h"

SimpleReadFunctionStub::SimpleReadFunctionStub(score::os::ResMgr& resmgr, score::os::IoFunc& iofunc, std::string& data)
    : score::os::SimpleReadFunction(resmgr, iofunc), data_(data)
{
}

// we are simulating reading from registers with 8 bytes, register at a time
score::Result<size_t> SimpleReadFunctionStub::read(const off_t offset, const size_t nbytes, uint64_t& result)
{
    // Reject negative offsets up front so that the conversion to size_t below is value-preserving.
    if (offset < 0)
    {
        return score::MakeUnexpected(score::os::ErrorCode::kIllegalSeek);
    }

    if ((static_cast<size_t>(offset) * nbytes) >= data_.size())
    {
        return score::MakeUnexpected(score::os::ErrorCode::kIllegalSeek);
    }

    if (nbytes == 8)
    {
        result = *(reinterpret_cast<const uint64_t*>(data_.data()) + offset);
    }
    else if (nbytes == 4)
    {
        result = *(reinterpret_cast<const uint32_t*>(data_.data()) + offset);
    }
    else
    {
        return score::MakeUnexpected(score::os::ErrorCode::kIllegalSeek);
    }
    return nbytes;
}
