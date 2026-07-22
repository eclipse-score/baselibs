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

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <variant>

using TestVariant = std::variant<const std::int32_t*, std::int8_t, std::unique_ptr<std::string>>;

namespace
{
template <typename T>
bool create_test_variant(TestVariant* variant, std::uint32_t size, T&& initial_value)
{
    if (size == sizeof(TestVariant))
    {
        new (variant) TestVariant{std::forward<T>(initial_value)};
        return true;
    }
    else
    {
        return false;
    }
}

constexpr std::int32_t kIntData[]{7, 3, 5, 3, 0};

}  // namespace

extern "C" bool create_test_variant_i8(TestVariant* variant, std::uint32_t size)
{
    return create_test_variant(variant, size, std::int8_t{15});
}

extern "C" bool create_test_variant_i32_ptr(TestVariant* variant, std::uint32_t size)
{
    return create_test_variant(variant, size, &kIntData[0]);
}

extern "C" void delete_test_variant(TestVariant* variant)
{
    std::destroy_at(variant);
}

extern "C" std::uint64_t create_test_variant_ptr_to_str(TestVariant* variant, std::uint32_t size)
{
    if (size == sizeof(TestVariant))
    {
        new (variant) TestVariant{std::make_unique<std::string>("Hello, World!")};
        return reinterpret_cast<std::byte*>(&std::get<2>(*variant)) - reinterpret_cast<std::byte*>(variant);
    }
    else
    {
        return std::numeric_limits<std::uint64_t>::max();
    }
}
