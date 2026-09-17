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
#ifndef SCORE_LIB_OS_INOTIFY_IMPL_H
#define SCORE_LIB_OS_INOTIFY_IMPL_H

#include "score/os/inotify.h"

namespace score
{
namespace os
{

class InotifyImpl final : public Inotify
{
  public:
    constexpr InotifyImpl() = default;
    score::cpp::expected<std::int32_t, Error> inotify_init() const noexcept override;

    score::cpp::expected<std::int32_t, Error> inotify_add_watch(const std::int32_t fd,
                                                                const char* const pathname,
                                                                const EventMask mask) const noexcept override;

    score::cpp::expected<std::int32_t, Error> inotify_rm_watch(const std::int32_t fd,
                                                               const std::int32_t wd) const noexcept override;
};

namespace internal
{

std::uint32_t EventMaskToInteger(const Inotify::EventMask event_mask) noexcept;

}  // namespace internal

}  // namespace os
}  // namespace score

#endif
