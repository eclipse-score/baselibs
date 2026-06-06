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

#include "score/mw/log/plugin_api.h"

#include "gtest/gtest.h"

#include <cstdint>

extern "C" MwLogAbiVersion MwLogVersion(void)
{
    return MwLogAbiVersion{MW_LOG_ABI_VERSION_MAJOR, MW_LOG_ABI_VERSION_MINOR};
}

extern "C" void MwLogRegisterBackends(void) noexcept
{
    // No-op for API contract tests.
}

namespace
{

TEST(MwLogPluginApiTest, AbiVersionMacrosAreDefined)
{
    RecordProperty("Description", "MW_LOG_ABI_VERSION_MAJOR and MW_LOG_ABI_VERSION_MINOR shall be defined ");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    // The macros must be usable in a constexpr context.
    constexpr uint16_t kMajor = MW_LOG_ABI_VERSION_MAJOR;
    constexpr uint16_t kMinor = MW_LOG_ABI_VERSION_MINOR;
    EXPECT_EQ(kMajor, 1U);
    EXPECT_EQ(kMinor, 0U);
}

TEST(MwLogPluginApiTest, AbiVersionStructLayout)
{
    RecordProperty("Description", "MwLogAbiVersion shall contain major and minor fields of uint16_t");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    MwLogAbiVersion version{};
    version.major = 1U;
    version.minor = 2U;
    EXPECT_EQ(version.major, 1U);
    EXPECT_EQ(version.minor, 2U);

    static_assert(sizeof(MwLogAbiVersion) == 4U, "MwLogAbiVersion shall be 4 bytes");
}

TEST(MwLogPluginApiTest, CompatibilityCheckExactMatch)
{
    RecordProperty("Description", "A plugin with the same major and minor shall be compatible");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    const MwLogAbiVersion plugin_version{MW_LOG_ABI_VERSION_MAJOR, MW_LOG_ABI_VERSION_MINOR};
    EXPECT_NE(MwLogIsVersionCompatible(plugin_version), 0);
}

TEST(MwLogPluginApiTest, CompatibilityCheckLowerMinorIsCompatible)
{
    RecordProperty("Description", "A plugin with same major but lower minor shall be compatible.");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    const MwLogAbiVersion plugin_version{MW_LOG_ABI_VERSION_MAJOR, 0U};
    EXPECT_NE(MwLogIsVersionCompatible(plugin_version), 0);
}

TEST(MwLogPluginApiTest, CompatibilityCheckHigherMinorIsIncompatible)
{
    RecordProperty("Description", "A plugin with same major but higher minor shall be incompatible.");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    const MwLogAbiVersion plugin_version{MW_LOG_ABI_VERSION_MAJOR,
                                         static_cast<uint16_t>(MW_LOG_ABI_VERSION_MINOR + 1U)};
    EXPECT_EQ(MwLogIsVersionCompatible(plugin_version), 0);
}

TEST(MwLogPluginApiTest, CompatibilityCheckDifferentMajorIsIncompatible)
{
    RecordProperty("Description", "A plugin with a different major version shall be incompatible.");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    const MwLogAbiVersion plugin_version_higher{static_cast<uint16_t>(MW_LOG_ABI_VERSION_MAJOR + 1U),
                                                MW_LOG_ABI_VERSION_MINOR};
    EXPECT_EQ(MwLogIsVersionCompatible(plugin_version_higher), 0);

    if (MW_LOG_ABI_VERSION_MAJOR > 1U)
    {
        const MwLogAbiVersion plugin_version_lower{static_cast<uint16_t>(MW_LOG_ABI_VERSION_MAJOR - 1U),
                                                   MW_LOG_ABI_VERSION_MINOR};
        EXPECT_EQ(MwLogIsVersionCompatible(plugin_version_lower), 0);
    }
}

TEST(MwLogPluginApiTest, CompatibilityCheckZeroMajorIsIncompatible)
{
    RecordProperty("Description", "A plugin with major=0 shall be incompatible.");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    const MwLogAbiVersion plugin_version{0U, MW_LOG_ABI_VERSION_MINOR};
    EXPECT_EQ(MwLogIsVersionCompatible(plugin_version), 0);
}

TEST(MwLogPluginApiTest, MwLogVersionIsCallableAsFunctionPointer)
{
    RecordProperty("Description",
                   "MwLogVersion() shall be usable as a C function pointer type "
                   "compatible with dlsym() cast.");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-1");

    using VersionFn = MwLogAbiVersion (*)();
    VersionFn fn = &MwLogVersion;
    EXPECT_NE(fn, nullptr);
}

TEST(MwLogPluginApiTest, MwLogRegisterBackendsIsCallableAsFunctionPointer)
{
    RecordProperty("Description",
                   "MwLogRegisterBackends() shall be usable as a C function pointer "
                   "type compatible with dlsym() casting (REQ-DD-1).");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-1");
    using RegisterFn = void (*)();
    RegisterFn fn = &MwLogRegisterBackends;
    EXPECT_NE(fn, nullptr);
}

TEST(MwLogPluginApiTest, MwLogVersionReturnsAbiVersion)
{
    RecordProperty("Description", "MwLogVersion() shall return {MW_LOG_ABI_VERSION_MAJOR, MW_LOG_ABI_VERSION_MINOR}.");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    const MwLogAbiVersion version = MwLogVersion();
    EXPECT_EQ(version.major, MW_LOG_ABI_VERSION_MAJOR);
    EXPECT_EQ(version.minor, MW_LOG_ABI_VERSION_MINOR);
}

TEST(MwLogPluginApiTest, MwLogVersionViaFunctionPointerMatchesDirectCall)
{
    RecordProperty("Description",
                   "MwLogVersion() called via function pointer (simulating dlsym()) "
                   "shall return the same value as a direct call (REQ-DD-2).");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    using VersionFn = MwLogAbiVersion (*)();
    VersionFn fn = &MwLogVersion;

    const MwLogAbiVersion via_ptr = fn();
    const MwLogAbiVersion direct = MwLogVersion();
    EXPECT_EQ(via_ptr.major, direct.major);
    EXPECT_EQ(via_ptr.minor, direct.minor);
    EXPECT_EQ(via_ptr.major, MW_LOG_ABI_VERSION_MAJOR);
    EXPECT_EQ(via_ptr.minor, MW_LOG_ABI_VERSION_MINOR);
}

TEST(MwLogPluginApiTest, MwLogVersionResultIsCompatibleWithHost)
{
    RecordProperty("Description",
                   "MwLogVersion() from a correctly built plugin shall pass "
                   "MwLogIsVersionCompatible() (REQ-DD-2).");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-2");

    const MwLogAbiVersion version = MwLogVersion();
    EXPECT_NE(MwLogIsVersionCompatible(version), 0);
}

TEST(MwLogPluginApiTest, MwLogRegisterBackendsIsCallableWithoutCrash)
{
    RecordProperty("Description", "MwLogRegisterBackends() shall be callable without an exception.");
    RecordProperty("TestType", "requirements-based"); // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis"); // requirements
    RecordProperty("ParentRequirement", "REQ-DD-1");

    // Must not crash or throw.
    MwLogRegisterBackends();
    SUCCEED();
}

}  // namespace
