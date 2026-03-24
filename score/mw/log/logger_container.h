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
#ifndef SCORE_MW_LOG_LOGGER_CONTAINER_H
#define SCORE_MW_LOG_LOGGER_CONTAINER_H

#include "score/mw/log/detail/wait_free_stack/wait_free_stack.h"
#include "score/mw/log/logger.h"
#include "score/mw/log/slot_handle.h"
#include <score/static_vector.hpp>

namespace score
{
namespace mw
{
namespace log
{
class LoggerContainer final
{
  public:
    explicit LoggerContainer();

    Logger& GetLogger(const std::string_view context) noexcept;

    size_t GetCapacity() const noexcept;

    Logger& GetDefaultLogger() noexcept;

  private:
    Logger& InsertNewLogger(const std::string_view context) noexcept;

    score::cpp::optional<std::reference_wrapper<Logger>> FindExistingLogger(const std::string_view context) noexcept;
    detail::WaitFreeStack<Logger, concurrency::atomic::AtomicIndirectorReal> stack_;
    Logger default_logger_;
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_LOGGER_CONTAINER_H
