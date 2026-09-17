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
#ifndef SCORE_LIB_OS_GRP_IMPL_H
#define SCORE_LIB_OS_GRP_IMPL_H

#include "score/os/grp.h"

#include <grp.h>

#include <mutex>

namespace score
{
namespace os
{

class GrpImpl final : public Grp
{
  public:
    constexpr GrpImpl() = default;
    score::cpp::expected<GroupBuffer, Error> getgrnam(const std::string& group) const noexcept override;

  private:
    static GroupBuffer GroupToGroupBuffer(const struct group& native_group) noexcept;

    // The getgrent(), getgrgid(), and getgrnam() functions share the same static buffer.
    // https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/g/getgrnam.html
    mutable std::mutex gr_buffer_mutex_{};
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_GRP_IMPL_H
