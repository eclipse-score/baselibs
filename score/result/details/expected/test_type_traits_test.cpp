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
#include "score/result/details/expected/test_type_traits.h"

#include <gtest/gtest.h>

namespace score::details
{
namespace
{

TEST(ExplicitImplicitConstructibleTest, ChecksWorkCorrectly)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that is_only_explicitly_constructible_v and is_implicitly_constructible_v correctly "
                   "distinguish an explicit-only default constructor from an implicit one.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an explicitly and an implicitly constructible type
    struct Explicit
    {
        explicit Explicit() = default;
    };

    struct Implicit
    {
    };

    // Expect that the type traits work correctly
    static_assert(is_only_explicitly_constructible_v<Explicit>, "Explicit must be only explicitly constructible");
    static_assert(!is_implicitly_constructible_v<Explicit>, "Explicit must not be implicitly constructible");
    static_assert(!is_only_explicitly_constructible_v<Implicit>, "Implicit must not be only explicitly constructible");
    static_assert(is_implicitly_constructible_v<Implicit>, "Implicit must be implicitly constructible");
}

}  // namespace
}  // namespace score::details
