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

#include "flatbuffers/base.h"

#include <gtest/gtest.h>

#include <regex>
#include <string>

namespace
{

TEST(IdlParserTest, VersionStringIsNonEmptyAndHasDotSeparatedFormat)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "FLATBUFFERS_VERSION() returns a non-empty string in X.Y.Z format");

    const std::string version{flatbuffers::FLATBUFFERS_VERSION()};

    // [0-9]+ one or more decimal digits (major / minor / revision)
    // \.     a literal dot separator
    // Repeated three times and anchored to the full string by std::regex_match.
    EXPECT_TRUE(std::regex_match(version, std::regex{R"([0-9]+\.[0-9]+\.[0-9]+)"}))
        << "Version string '" << version << "' does not match expected #.#.# format";
}

}  // namespace
