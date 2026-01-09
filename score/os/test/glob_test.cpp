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
#include "score/os/glob.h"
#include "score/os/glob_impl.h"

#include "gtest/gtest.h"

#include <glob.h>
#include <tuple>

namespace score
{
namespace os
{
namespace test
{

class FlagToIntegerTests : public ::testing::TestWithParam<std::tuple<Glob::Flag, int>>
{
};

TEST_P(FlagToIntegerTests, ConvertFlag)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FlagToIntegerTests Convert Flag");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto flag = std::get<0>(GetParam());
    auto expected = std::get<1>(GetParam());
    auto result = internal::glob_helper::FlagToInteger(flag);
    EXPECT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(ConvertFlags,
                         FlagToIntegerTests,
                         ::testing::Values(std::make_tuple(Glob::Flag::kAppend, GLOB_APPEND),
                                           std::make_tuple(Glob::Flag::kDooFFs, GLOB_DOOFFS),
                                           std::make_tuple(Glob::Flag::kErr, GLOB_ERR),
                                           std::make_tuple(Glob::Flag::kMark, GLOB_MARK),
                                           std::make_tuple(Glob::Flag::kNoCheck, GLOB_NOCHECK),
                                           std::make_tuple(Glob::Flag::kNoEscape, GLOB_NOESCAPE),
                                           std::make_tuple(Glob::Flag::kNoSort, GLOB_NOSORT),
                                           std::make_tuple(Glob::Flag::kPeriod, GLOB_PERIOD),
                                           std::make_tuple(Glob::Flag::kMagChar, GLOB_MAGCHAR),
                                           std::make_tuple(Glob::Flag::kAltDirFunc, GLOB_ALTDIRFUNC),
                                           std::make_tuple(Glob::Flag::kBrace, GLOB_BRACE),
                                           std::make_tuple(Glob::Flag::kNoMagic, GLOB_NOMAGIC),
                                           std::make_tuple(Glob::Flag::kTilde, GLOB_TILDE)));

#if defined(__linux__)
INSTANTIATE_TEST_SUITE_P(ConvertLinuxSpecificFlags,
                         FlagToIntegerTests,
                         ::testing::Values(std::make_tuple(Glob::Flag::kTildeCheck, GLOB_TILDE_CHECK),
                                           std::make_tuple(Glob::Flag::kOnlyDir, GLOB_ONLYDIR)));
#else
INSTANTIATE_TEST_SUITE_P(ConvertNonLinuxSpecificFlags,
                         FlagToIntegerTests,
                         ::testing::Values(std::make_tuple(Glob::Flag::kNoDotDir, GLOB_NO_DOTDIRS),
                                           std::make_tuple(Glob::Flag::kLimit, GLOB_LIMIT)));
#endif

TEST(FlagToIntegerTests, MultipleFlagsConversion)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FlagToIntegerTests Multiple Flags Conversion");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto combinedFlags = Glob::Flag::kAppend | Glob::Flag::kNoCheck | Glob::Flag::kPeriod;
    Glob::FlagType expected = GLOB_APPEND | GLOB_NOCHECK | GLOB_PERIOD;
    EXPECT_EQ(internal::glob_helper::FlagToInteger(combinedFlags), expected);

    combinedFlags = Glob::Flag::kAltDirFunc | Glob::Flag::kNoMagic | Glob::Flag::kBrace;
    expected = GLOB_ALTDIRFUNC | GLOB_NOMAGIC | GLOB_BRACE;
    EXPECT_EQ(internal::glob_helper::FlagToInteger(combinedFlags), expected);

#ifdef __linux__
    combinedFlags = Glob::Flag::kTildeCheck | Glob::Flag::kOnlyDir;
    expected = GLOB_TILDE_CHECK | GLOB_ONLYDIR;
    EXPECT_EQ(internal::glob_helper::FlagToInteger(combinedFlags), expected);
#else
    combinedFlags = Glob::Flag::kNoDotDir | Glob::Flag::kLimit;
    expected = GLOB_NO_DOTDIRS | GLOB_LIMIT;
    EXPECT_EQ(internal::glob_helper::FlagToInteger(combinedFlags), expected);
#endif
}

}  // namespace test
}  // namespace os
}  // namespace score
