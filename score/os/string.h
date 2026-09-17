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
#ifndef SCORE_LIB_OS_STRING_H
#define SCORE_LIB_OS_STRING_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include <cstddef>
#include <cstdint>

namespace score
{
namespace os
{

class String : public ObjectSeam<String>
{
  public:
    static String& instance() noexcept;

    virtual void* memcpy(void* const dest, const void* const src, const std::size_t n) const noexcept = 0;

    virtual char* strerror(const std::int32_t errnum) const noexcept = 0;

    virtual void* memset(void* const dest, const std::int32_t c, const std::size_t n) const noexcept = 0;

    virtual ~String() = default;

  protected:
    String() = default;
    String(const String&) = default;
    String(String&&) = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_STRING_H
