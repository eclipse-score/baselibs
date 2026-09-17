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
#ifndef SCORE_LIB_OS_DIRENT_H
#define SCORE_LIB_OS_DIRENT_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <dirent.h>  // remove after transition
#include <sys/types.h>
#include <cstdint>

namespace score
{
namespace os
{

/// \brief OS-independent abstraction of https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dirent.h.html
class Dirent : public ObjectSeam<Dirent>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Dirent& instance() noexcept;

    virtual score::cpp::expected<DIR*, score::os::Error> opendir(const char* const name) const noexcept = 0;
    virtual score::cpp::expected<struct dirent*, score::os::Error> readdir(DIR* const dirp) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> scandir(
        const char* const dirp,
        struct dirent*** const namelist,
        std::int32_t (*const filter)(const struct dirent*),
        std::int32_t (*const compar)(const struct dirent**, const struct dirent**)) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> closedir(DIR* const dirp) const noexcept = 0;

    virtual ~Dirent() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Dirent(const Dirent&) = delete;
    Dirent& operator=(const Dirent&) = delete;
    Dirent(Dirent&& other) = delete;
    Dirent& operator=(Dirent&& other) = delete;

  protected:
    Dirent() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_DIRENT_H
