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

#include "score/utils/string_registry/string_registry.h"

namespace score::utils::string_registry
{

// This Insert should be used for inserting compile time string literals into the registry.
std::pair<RegistryStringView, bool> StringRegistry::Insert(const std::string_view new_string,
                                                           LiteralTag /*is_static_string_literal*/) noexcept
{
    const std::lock_guard<std::mutex> locked{mutex_};

    // emplace will already search for previously existing entry
    const auto registered_string = registered_strings_.emplace(new_string);
    return std::pair<RegistryStringView, bool>{RegistryStringView{*registered_string.first}, registered_string.second};
}

std::pair<RegistryStringView, bool>
// NOLINTNEXTLINE(bugprone-exception-escape): All exceptions are either handled or left unhandled as
// a design decision.
StringRegistry::Insert(const std::string_view new_string) noexcept
{
    const std::lock_guard<std::mutex> locked{mutex_};

    const auto iter = registered_strings_.find(new_string);
    if (iter == registered_strings_.end())
    {
        auto& inserted_string = dynamic_allocated_.emplace_front(new_string);
        const auto registered_string = registered_strings_.insert(inserted_string);
        return std::pair<RegistryStringView, bool>{RegistryStringView{*registered_string.first},
                                                   registered_string.second};
    }

    return std::pair<RegistryStringView, bool>{RegistryStringView{*iter}, false};
}

std::pair<RegistryStringView, bool> StringRegistry::Insert(std::string&& new_string) noexcept
{
    const std::lock_guard<std::mutex> locked{mutex_};

    const auto iter = registered_strings_.find(new_string);
    if (iter == registered_strings_.end())
    {
        auto& inserted_string = dynamic_allocated_.emplace_front(std::move(new_string));
        const auto registered_string = registered_strings_.insert(inserted_string);
        return std::pair<RegistryStringView, bool>{RegistryStringView{*registered_string.first},
                                                   registered_string.second};
    }

    return std::pair<RegistryStringView, bool>{RegistryStringView{*iter}, false};
}

}  // namespace score::utils::string_registry
