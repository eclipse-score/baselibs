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

#ifndef SCORE_LIB_OS_UTILS_PATH_H
#define SCORE_LIB_OS_UTILS_PATH_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/utils/path.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <memory>
#include <string>

namespace score
{
namespace os
{

class Path : public ObjectSeam<Path>
{
  public:
    static Path& instance() noexcept;

    /// \brief Creates a new instance of the production implementation.
    /// \details This is to enable the usage of OSAL without the Singleton instance(). Especially library code
    /// should avoid the Singleton instance() method as this would interfere the unit tests of user code that also uses
    /// the instance().
    static std::unique_ptr<Path> Default() noexcept;

    static score::cpp::pmr::unique_ptr<Path> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    virtual std::string get_base_name(const std::string& path) const noexcept = 0;

    /// @brief Returns the parent directory name of `path`.
    virtual std::string get_parent_dir(const std::string& path) const noexcept = 0;

    /// @brief Returns the absolute path to the executable
    virtual score::cpp::expected<std::string, score::os::Error> get_exec_path() const noexcept = 0;

    virtual ~Path() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Path(const Path&) = delete;
    Path& operator=(const Path&) = delete;
    Path(Path&& other) = delete;
    Path& operator=(Path&& other) = delete;

  protected:
    Path() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_PATH_H
