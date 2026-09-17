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
#ifndef SCORE_LIB_OS_DIRENT_IMPL_H
#define SCORE_LIB_OS_DIRENT_IMPL_H

#include "score/os/dirent.h"

#include "score/expected.hpp"

namespace score
{
namespace os
{

// Suppress "AUTOSAR C++14 M3-2-3" rule finding: "A type, object or function that is used in multiple translation units
// shall be declared in one and only one file.".
// This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class DirentImpl final : public Dirent
{
  public:
    constexpr DirentImpl() = default;
    score::cpp::expected<DIR*, score::os::Error> opendir(const char* const name) const noexcept override;

    score::cpp::expected<struct dirent*, score::os::Error> readdir(DIR* const dirp) const noexcept override;

    // Rationale: violation is happening out of our code domain due to QNX - ::scandir, no harm to our code
    // coverity[autosar_cpp14_a5_0_3_violation] see above
    score::cpp::expected<std::int32_t, score::os::Error> scandir(
        const char* const dirp,
        struct dirent*** const namelist,
        std::int32_t (*const filter)(const struct dirent*),
        std::int32_t (*const compar)(const struct dirent**, const struct dirent**)) const noexcept override;

    score::cpp::expected_blank<score::os::Error> closedir(DIR* const dirp) const noexcept override;
};
}  // namespace os
}  // namespace score

#endif
