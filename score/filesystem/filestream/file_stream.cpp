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
#include "score/filesystem/filestream/file_stream.h"

#include <score/assert.hpp>

score::ResultBlank score::filesystem::FileStream::Close()
{
    auto buf = rdbuf();
    if (buf != nullptr)
    {
        auto stdio_filebuf = dynamic_cast<details::StdioFileBuf*>(buf);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(stdio_filebuf != nullptr,
                               "FileStream must not be used with a streambuf that's not derived from StdioFileBuf");
        return stdio_filebuf->Close();
    }
    else
    {
        return {};
    }
}
