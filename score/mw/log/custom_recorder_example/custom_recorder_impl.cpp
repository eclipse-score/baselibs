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
#include "score/mw/log/custom_recorder_example/custom_recorder_impl.h"
#include <iostream>

namespace user
{
namespace specific
{
namespace impl
{
namespace detail
{

score::cpp::optional<score::mw::log::SlotHandle> CustomRecorderImpl::StartRecord(const std::string_view,
                                                                        const score::mw::log::LogLevel) noexcept
{
    SlotHandle slot_handle{};
    return slot_handle;
}

void CustomRecorderImpl::StopRecord(const SlotHandle&) noexcept {}

void CustomRecorderImpl::Log(const SlotHandle&, const bool) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::uint8_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::int8_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::uint16_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::int16_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::uint32_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::int32_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::uint64_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::int64_t) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const float) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const double) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const std::string_view data) noexcept
{
    std::cout << __FUNCTION__ << " " << data << std::endl;
}

void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogHex8) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogHex16) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogHex32) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogHex64) noexcept {}

void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogBin8) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogBin16) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogBin32) noexcept {}
void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogBin64) noexcept {}

void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogRawBuffer) noexcept {}

void CustomRecorderImpl::Log(const SlotHandle&, const score::mw::log::LogSlog2Message) noexcept {}

bool CustomRecorderImpl::IsLogEnabled(const score::mw::log::LogLevel&, const std::string_view) const noexcept
{
    return true;
}

}  // namespace detail
}  // namespace impl
}  // namespace specific
}  // namespace user
