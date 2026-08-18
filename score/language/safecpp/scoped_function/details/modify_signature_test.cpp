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
#include "score/language/safecpp/scoped_function/details/modify_signature.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace score::safecpp::details
{
namespace
{

class ModifySignatureTest : public ::testing::Test
{
};

TEST_F(ModifySignatureTest, RetainsKeyTraitsOfSignature)
{
    static_assert(std::is_same_v<ModifySignatureT<std::int32_t(bool)>, score::cpp::optional<std::int32_t>(bool)>,
                  "Types must equal");
}

TEST_F(ModifySignatureTest, RetainsConst)
{
    static_assert(
        std::is_same_v<ModifySignatureT<std::int32_t(bool) const>, score::cpp::optional<std::int32_t>(bool) const>,
        "Types must equal");
}

TEST_F(ModifySignatureTest, RetainsNoexcept)
{
    static_assert(std::is_same_v<ModifySignatureT<std::int32_t(bool) noexcept>,
                                 score::cpp::optional<std::int32_t>(bool) noexcept>,
                  "Types must equal");
}

TEST_F(ModifySignatureTest, RetainsConstNoexcept)
{
    static_assert(std::is_same_v<ModifySignatureT<std::int32_t(bool) const noexcept>,
                                 score::cpp::optional<std::int32_t>(bool) const noexcept>,
                  "Types must equal");
}

}  // namespace
}  // namespace score::safecpp::details
