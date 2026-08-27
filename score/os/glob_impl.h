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
#ifndef SCORE_LIB_OS_GLOB_IMPL_H
#define SCORE_LIB_OS_GLOB_IMPL_H

#include "score/os/glob.h"

namespace score
{
namespace os
{

class GlobImpl final : public Glob
{
  public:
    GlobImpl();
    ~GlobImpl() override;

    GlobImpl(const GlobImpl&) = delete;
    GlobImpl& operator=(const GlobImpl&) = delete;
    GlobImpl(GlobImpl&&) noexcept;
    GlobImpl& operator=(GlobImpl&&) noexcept;

    score::cpp::expected<Glob::MatchResult, Error> Match(const std::string& pattern,
                                                         const Glob::Flag flags) noexcept override;

  private:
    glob_t buffer_;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_GLOB_IMPL_H
