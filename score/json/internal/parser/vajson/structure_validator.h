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
/// @file
/// @brief  Checks the structure of a JSON document.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_VALIDATOR_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_VALIDATOR_H

#include <cstdint>
#include <vector>

#include "score/result/result.h"

namespace score::json::vajson::internal
{

/// @brief           Checks the order of the tokens of a JSON document
/// @details         The parser reports each token in document order.
///                  A method returns score::json::vajson::JsonErrc::kInvalidJson if its token is not allowed at the
///                  current position.
///                  Commas are lenient: an array or object may contain leading, repeated, and trailing commas.
///                  Not thread-safe.
class StructureValidator final
{
  public:
    /// @brief           Accepts a null, boolean, number, or string value
    /// @return          The empty Result, or an error if no value is allowed at the current position.
    [[nodiscard]] auto AddValue() noexcept -> score::Result<void>;

    /// @brief           Accepts an object key
    /// @return          The empty Result, or an error if no key is allowed at the current position.
    [[nodiscard]] auto AddKey() noexcept -> score::Result<void>;

    /// @brief           Accepts the start of an array
    /// @return          The empty Result, or an error if no value is allowed at the current position.
    [[nodiscard]] auto OpenArray() noexcept -> score::Result<void>;

    /// @brief           Accepts the start of an object
    /// @return          The empty Result, or an error if no value is allowed at the current position.
    [[nodiscard]] auto OpenObject() noexcept -> score::Result<void>;

    /// @brief           Accepts the end of an array
    /// @return          The empty Result, or an error if the innermost open container is no array or a key lacks
    ///                  its value.
    [[nodiscard]] auto CloseArray() noexcept -> score::Result<void>;

    /// @brief           Accepts the end of an object
    /// @return          The empty Result, or an error if the innermost open container is no object or a key lacks
    ///                  its value.
    [[nodiscard]] auto CloseObject() noexcept -> score::Result<void>;

    /// @brief           Accepts a comma
    /// @return          The empty Result, or an error if the comma is outside of a container or follows a key.
    [[nodiscard]] auto AddComma() noexcept -> score::Result<void>;

    /// @brief           Checks that the document may end at the current position
    /// @return          The empty Result if the document consists of exactly one complete value, or an error.
    [[nodiscard]] auto CheckEndOfDocument() const noexcept -> score::Result<void>;

  private:
    /// @brief           Kind of an open container
    enum class Container : std::uint8_t
    {
        kArray,
        kObject,
    };

    /// @brief           Accepts the start of any value and consumes the pending key
    [[nodiscard]] auto BeginValue() noexcept -> score::Result<void>;
    /// @brief           Accepts the start of a container
    [[nodiscard]] auto Open(Container container) noexcept -> score::Result<void>;
    /// @brief           Accepts the end of a container
    [[nodiscard]] auto Close(Container container) noexcept -> score::Result<void>;

    /// @brief           Rule: a completed value is followed by a comma inside a container, or by the end of document
    [[nodiscard]] auto CheckSeparated() const noexcept -> score::Result<void>;
    /// @brief           Rule: a value inside an object follows a key
    [[nodiscard]] auto CheckKeyPresent() const noexcept -> score::Result<void>;
    /// @brief           Rule: a key is followed by a value
    [[nodiscard]] auto CheckNoPendingKey() const noexcept -> score::Result<void>;
    /// @brief           Rule: a key is inside an object
    [[nodiscard]] auto CheckInObject() const noexcept -> score::Result<void>;
    /// @brief           Rule: a comma is inside a container
    [[nodiscard]] auto CheckInContainer() const noexcept -> score::Result<void>;
    /// @brief           Rule: a closing bracket or brace matches the innermost open container
    [[nodiscard]] auto CheckClosing(Container container) const noexcept -> score::Result<void>;
    /// @brief           Returns true if the innermost open container is an object
    [[nodiscard]] auto IsInObject() const noexcept -> bool;

    /// @brief           The open arrays and objects, the innermost one last
    std::vector<Container> open_containers_{};
    /// @brief           True if the innermost object has a key whose value has not started yet
    bool key_pending_{false};
    /// @brief           True if a value has been completed and no comma has followed yet
    bool value_completed_{false};
};

}  // namespace score::json::vajson::internal

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_VALIDATOR_H
