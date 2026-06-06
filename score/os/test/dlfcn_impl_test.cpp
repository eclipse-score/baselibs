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

#include "score/os/dlfcn_impl.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace score::os
{
namespace
{

using StrlenFn = std::size_t (*)(const char*);

TEST(DlfcnImpl, DlopenSucceedsWithNullFilename)
{
    RecordProperty("Description", "DlfcnImpl::dlopen succeeds when called with nullptr (global symbol table)");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(result.has_value()) << "Failed to dlopen nullptr";
    EXPECT_NE(result.value(), nullptr);
    EXPECT_TRUE(dlfcn.dlclose(result.value()).has_value());
}

TEST(DlfcnImpl, DlopenReturnsErrorForNonexistentLibrary)
{
    RecordProperty("Description", "DlfcnImpl::dlopen returns an error for a nonexistent library");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto result{dlfcn.dlopen("/no/such/library_xyz.so", Dlfcn::DlOpenFlag::kNow)};
    EXPECT_FALSE(result.has_value());
}

TEST(DlfcnImpl, DlopenSetsLastDlErrorOnFailure)
{
    RecordProperty("Description", "DlfcnImpl::dlerror returns a non-null string after a failed dlopen");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto result{dlfcn.dlopen("/no/such/library_xyz.so", Dlfcn::DlOpenFlag::kNow)};
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(dlfcn.dlerror().has_value());
}

TEST(DlfcnImpl, DlopenClearsDlErrorOnSuccess)
{
    RecordProperty("Description", "DlfcnImpl::dlerror returns nullptr after a successful dlopen");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(dlfcn.dlerror().has_value());
    EXPECT_TRUE(dlfcn.dlclose(result.value()).has_value());
}

TEST(DlfcnImpl, DlsymResolvesKnownSymbol)
{
    RecordProperty("Description", "DlfcnImpl::dlsym resolves a known symbol from the global symbol table");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());

    const auto symbol_result{dlfcn.dlsym(handle_result.value(), "strlen")};  // strlen guaranteed by libc
    ASSERT_TRUE(symbol_result.has_value());
    EXPECT_NE(symbol_result.value(), nullptr);
    EXPECT_TRUE(dlfcn.dlclose(handle_result.value()).has_value());
}

TEST(DlfcnImpl, DlsymReturnsErrorForUnknownSymbol)
{
    RecordProperty("Description", "DlfcnImpl::dlsym returns an error for an unknown symbol");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());

    const auto symbol_result{dlfcn.dlsym(handle_result.value(), "no_such_symbol_xyz_42")};
    EXPECT_FALSE(symbol_result.has_value());
    EXPECT_TRUE(dlfcn.dlclose(handle_result.value()).has_value());
}

TEST(DlfcnImpl, DlsymSetsLastDlErrorOnFailure)
{
    RecordProperty("Description", "DlfcnImpl::dlerror returns a non-null string after a failed dlsym");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());

    const auto symbol_result{dlfcn.dlsym(handle_result.value(), "no_such_symbol_xyz_42")};
    ASSERT_FALSE(symbol_result.has_value());
    EXPECT_TRUE(dlfcn.dlerror().has_value());
    EXPECT_TRUE(dlfcn.dlclose(handle_result.value()).has_value());
}

TEST(DlfcnImpl, DlsymClearsDlErrorOnSuccess)
{
    RecordProperty("Description", "DlfcnImpl::dlerror returns nullptr after a successful dlsym");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());

    const auto symbol_result{dlfcn.dlsym(handle_result.value(), "strlen")};  // strlen guaranteed by libc
    ASSERT_TRUE(symbol_result.has_value());
    EXPECT_FALSE(dlfcn.dlerror().has_value());
    EXPECT_TRUE(dlfcn.dlclose(handle_result.value()).has_value());
}

TEST(DlfcnImpl, DlopenWithCombinedFlags)
{
    RecordProperty("Description", "DlfcnImpl::dlopen works with combined kNow | kLocal flags");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kNow | Dlfcn::DlOpenFlag::kLocal)};
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), nullptr);
    EXPECT_TRUE(dlfcn.dlclose(result.value()).has_value());
}

TEST(DlfcnImpl, DlopenWithGlobalFlag)
{
    RecordProperty("Description", "DlfcnImpl::dlopen works with the kGlobal flag");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kNow | Dlfcn::DlOpenFlag::kGlobal)};
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), nullptr);
    EXPECT_TRUE(dlfcn.dlclose(result.value()).has_value());
}

TEST(DlfcnImpl, DlsymAsResolvesKnownFunctionSymbol)
{
    RecordProperty("Description", "DlsymAs resolves a known symbol and returns a typed function pointer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());

    const auto fn_result{dlfcn.DlsymAs<StrlenFn>(handle_result.value(), "strlen")};  // strlen guaranteed by libc
    ASSERT_TRUE(fn_result.has_value());
    EXPECT_NE(fn_result.value(), nullptr);
    EXPECT_EQ(fn_result.value()("hello"), 5U);
    EXPECT_TRUE(dlfcn.dlclose(handle_result.value()).has_value());
}

TEST(DlfcnImpl, DlsymAsReturnsErrorForUnknownSymbol)
{
    RecordProperty("Description", "DlsymAs returns an error when the symbol does not exist");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());

    const auto fn_result{dlfcn.DlsymAs<StrlenFn>(handle_result.value(), "no_such_symbol_xyz_42")};
    EXPECT_FALSE(fn_result.has_value());
    EXPECT_TRUE(dlfcn.dlclose(handle_result.value()).has_value());
}

TEST(DlfcnImpl, DlcloseSucceedsWithValidHandle)
{
    RecordProperty("Description", "DlfcnImpl::dlclose succeeds when given a valid handle from dlopen");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());

    const auto close_result{dlfcn.dlclose(handle_result.value())};
    EXPECT_TRUE(close_result.has_value());
}

TEST(DlfcnImpl, DlcloseClearsDlErrorOnSuccess)
{
    RecordProperty("Description", "DlfcnImpl::dlerror returns nullopt after a successful dlclose");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    const DlfcnImpl dlfcn{};
    static_cast<void>(dlfcn.dlopen("/no/such/library_xyz.so", Dlfcn::DlOpenFlag::kNow));
    ASSERT_TRUE(dlfcn.dlerror().has_value());

    const auto handle_result{dlfcn.dlopen(nullptr, Dlfcn::DlOpenFlag::kLazy)};
    ASSERT_TRUE(handle_result.has_value());
    const auto close_result{dlfcn.dlclose(handle_result.value())};
    ASSERT_TRUE(close_result.has_value());
    EXPECT_FALSE(dlfcn.dlerror().has_value());
}

}  // namespace
}  // namespace score::os
