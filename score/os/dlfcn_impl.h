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

#ifndef SCORE_LIB_OS_DLFCN_IMPL_H
#define SCORE_LIB_OS_DLFCN_IMPL_H

#include "score/os/dlfcn.h"

#include <array>
#include <cstdint>

namespace score
{
namespace os
{

class DlfcnImpl final : public Dlfcn
{
  public:
    DlfcnImpl() = default;

    score::cpp::expected<void*, Error> dlopen(const char* const file_name, const DlOpenFlag flags) const noexcept override;
    score::cpp::expected<void*, Error> dlsym(void* const handle, const char* const symbol_name) const noexcept override;
    score::cpp::expected_blank<Error> dlclose(void* handle) const noexcept override;
    std::optional<std::string_view> dlerror() const noexcept override;

  private:
    static constexpr std::size_t kDlErrorBufSize{256U};

    [[nodiscard]] bool CacheDlError() const noexcept;

    mutable std::array<char, kDlErrorBufSize> last_dl_error_{};
    mutable bool has_dl_error_{false};
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_DLFCN_IMPL_H
