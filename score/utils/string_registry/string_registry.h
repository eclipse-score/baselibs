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

#ifndef SCORE_UTILS_STRING_REGISTRY_STRING_REGISTRY_H
#define SCORE_UTILS_STRING_REGISTRY_STRING_REGISTRY_H

#include <forward_list>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

#include "score/utils/string_registry/registry_string_view.h"

namespace score::utils::string_registry
{

/// @brief Tag to select the string-literal overload of Insert
struct LiteralTag
{
};

///
/// @class StringRegistry
///
/// @brief A central registry for strings to avoid copying and to facilitate cheap comparison.
///
class StringRegistry final
{
  public:
    ///
    /// @brief Insert a new StringView literal into the string registry.
    ///
    /// @param[in]      new_string        String to be added to the registry.
    /// @param[in]      (no name)         Used only to select StringView literal version of the
    /// method.
    ///
    /// @return String_view of the string in registry and boolean denoting whether the string was
    ///         newly added (true) or was present already (false).
    ///
    std::pair<RegistryStringView, bool> Insert(std::string_view new_string,
                                               LiteralTag /*is_static_string_literal*/) noexcept;

    ///
    /// @brief Insert a new StringView into the string registry.
    ///
    /// @param[in]      new_string        String to be added to the registry.
    ///
    /// @return String_view of the string in registry and boolean denoting whether the string was
    ///         newly added (true) or was present already (false).
    ///
    // NOLINTNEXTLINE(bugprone-exception-escape): If exception is thrown it shall be considered as
    // fatal error and std::terminate is desired behavior.
    std::pair<RegistryStringView, bool> Insert(std::string_view new_string) noexcept;

    ///
    /// @brief Insert a new std::string into the string registry.
    ///
    /// @param[in]      new_string        String to be added to the registry.
    ///
    /// @return String_view of the string in registry and boolean denoting whether the string was
    ///         newly added (true) or was present already (false).
    ///
    std::pair<RegistryStringView, bool> Insert(std::string&& new_string) noexcept;

  private:
    std::unordered_set<std::string_view>
        registered_strings_;                            ///< All registered string views, used for deduplication lookups
    std::forward_list<std::string> dynamic_allocated_;  ///< Backing storage for heap-allocated strings
    std::mutex mutex_;                                  ///< Guards concurrent access to the registry
};

}  // namespace score::utils::string_registry

#endif  // SCORE_UTILS_STRING_REGISTRY_STRING_REGISTRY_H
