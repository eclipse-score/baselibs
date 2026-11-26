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
#include "score/json/internal/parser/vajson/structure_validator.h"

#include "score/json/error.h"

namespace score::json::vajson::internal
{

auto StructureValidator::AddValue() noexcept -> score::Result<void>
{
    return BeginValue().transform([this]() noexcept {
        value_completed_ = true;
    });
}

auto StructureValidator::AddKey() noexcept -> score::Result<void>
{
    return CheckSeparated()
        .and_then([this]() noexcept {
            return CheckNoPendingKey();
        })
        .and_then([this]() noexcept {
            return CheckInObject();
        })
        .transform([this]() noexcept {
            key_pending_ = true;
        });
}

auto StructureValidator::OpenArray() noexcept -> score::Result<void>
{
    return Open(Container::kArray);
}

auto StructureValidator::OpenObject() noexcept -> score::Result<void>
{
    return Open(Container::kObject);
}

auto StructureValidator::CloseArray() noexcept -> score::Result<void>
{
    return Close(Container::kArray);
}

auto StructureValidator::CloseObject() noexcept -> score::Result<void>
{
    return Close(Container::kObject);
}

auto StructureValidator::AddComma() noexcept -> score::Result<void>
{
    return CheckInContainer()
        .and_then([this]() noexcept {
            return CheckNoPendingKey();
        })
        .transform([this]() noexcept {
            value_completed_ = false;
        });
}

auto StructureValidator::CheckEndOfDocument() const noexcept -> score::Result<void>
{
    score::Result<void> result{};
    if (!open_containers_.empty())
    {
        result = score::MakeUnexpected(
            Error::kParsingError,
            (open_containers_.back() == Container::kArray) ? "Expected closing bracket." : "Expected closing brace.");
    }
    else if (!value_completed_)
    {
        result = score::MakeUnexpected(Error::kParsingError, "Document is empty.");
    }
    else
    {
        // The document consists of exactly one complete value.
    }
    return result;
}

auto StructureValidator::BeginValue() noexcept -> score::Result<void>
{
    return CheckSeparated()
        .and_then([this]() noexcept {
            return CheckKeyPresent();
        })
        .transform([this]() noexcept {
            key_pending_ = false;
        });
}

auto StructureValidator::Open(const Container container) noexcept -> score::Result<void>
{
    return BeginValue().transform([this, container]() noexcept {
        open_containers_.push_back(container);
    });
}

auto StructureValidator::Close(const Container container) noexcept -> score::Result<void>
{
    return CheckClosing(container)
        .and_then([this]() noexcept {
            return CheckNoPendingKey();
        })
        .transform([this]() noexcept {
            open_containers_.pop_back();
            value_completed_ = true;
        });
}

auto StructureValidator::CheckSeparated() const noexcept -> score::Result<void>
{
    score::Result<void> result{};
    if (value_completed_)
    {
        result = score::MakeUnexpected(Error::kParsingError,
                                       open_containers_.empty() ? "Multiple top-level values." : "Missing comma.");
    }
    return result;
}

auto StructureValidator::CheckKeyPresent() const noexcept -> score::Result<void>
{
    score::Result<void> result{};
    if (IsInObject() && (!key_pending_))
    {
        result = score::MakeUnexpected(Error::kParsingError, "Expected a key.");
    }
    return result;
}

auto StructureValidator::CheckNoPendingKey() const noexcept -> score::Result<void>
{
    score::Result<void> result{};
    if (key_pending_)
    {
        result = score::MakeUnexpected(Error::kParsingError, "Expected a value after a key.");
    }
    return result;
}

auto StructureValidator::CheckInObject() const noexcept -> score::Result<void>
{
    score::Result<void> result{};
    if (!IsInObject())
    {
        result = score::MakeUnexpected(Error::kParsingError, "Key outside of an object.");
    }
    return result;
}

auto StructureValidator::CheckInContainer() const noexcept -> score::Result<void>
{
    score::Result<void> result{};
    if (open_containers_.empty())
    {
        result = score::MakeUnexpected(Error::kParsingError, "Comma outside of an array or object.");
    }
    return result;
}

auto StructureValidator::CheckClosing(const Container container) const noexcept -> score::Result<void>
{
    score::Result<void> result{};
    if (open_containers_.empty() || (open_containers_.back() != container))
    {
        result = score::MakeUnexpected(
            Error::kParsingError,
            (container == Container::kArray) ? "Unexpected closing bracket." : "Unexpected closing brace.");
    }
    return result;
}

auto StructureValidator::IsInObject() const noexcept -> bool
{
    return (!open_containers_.empty()) && (open_containers_.back() == Container::kObject);
}

}  // namespace score::json::vajson::internal
