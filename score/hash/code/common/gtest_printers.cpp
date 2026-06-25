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

#include "score/hash/code/common/gtest_printers.h"

namespace score::cpp
{

std::ostream& operator<<(std::ostream& out, const score::cpp::span<const std::uint8_t> span)
{
    constexpr std::size_t kMaxCount = 32;

    out << "{";
    auto it = span.begin();
    if (!span.empty())
    {
        out << std::hex << static_cast<int>(*it);
        ++it;
    }

    std::size_t i = 0;
    for (; it != span.end(); ++it)
    {
        out << ", " << std::hex << static_cast<int>(*it);
        if (i == kMaxCount)
        {  // Enough has been printed.
            out << " ...";
            break;
        }
        i++;
    }
    out << "}";
    return out;
}

void PrintTo(const score::cpp::span<const std::uint8_t>& span, std::ostream* os)
{
    *os << span;
}

}  // namespace score::cpp

namespace score
{
namespace hash
{

std::ostream& operator<<(std::ostream& out, const Hash& hash)
{
    return out << hash.GetBytes();
}

void PrintTo(const Hash& hash, std::ostream* os)
{
    PrintTo(hash.GetBytes(), os);
}

}  // namespace hash
}  // namespace score
