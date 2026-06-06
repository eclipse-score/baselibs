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

#include "score/os/dlfcn_impl.h"

#include <dlfcn.h>
#include <cstring>
#include <optional>
#include <string_view>
#include <type_traits>

namespace score::os
{
namespace
{

[[nodiscard]] std::int32_t DlOpenFlagsToNative(const Dlfcn::DlOpenFlag flags) noexcept
{
    std::uint32_t native_flags{0U};

    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kLazy) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(RTLD_LAZY);
    }
    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kNow) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(RTLD_NOW);
    }
    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kLocal) != 0U)
    {
        // The |= is intentional for portability.
        native_flags |= static_cast<std::uint32_t>(RTLD_LOCAL);
    }
    if (static_cast<std::underlying_type_t<Dlfcn::DlOpenFlag>>(flags & Dlfcn::DlOpenFlag::kGlobal) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(RTLD_GLOBAL);
    }

    return static_cast<std::int32_t>(native_flags);
}

}  // namespace

score::cpp::expected<void*, Error> DlfcnImpl::dlopen(const char* const file_name, const DlOpenFlag flags) const noexcept
{
    void* const handle{::dlopen(file_name, DlOpenFlagsToNative(flags))};
    if (handle == nullptr)
    {
        static_cast<void>(CacheDlError());
        return score::cpp::make_unexpected(Error::createUnspecifiedError());
    }
    has_dl_error_ = false;
    return handle;
}

score::cpp::expected<void*, Error> DlfcnImpl::dlsym(void* const handle, const char* const symbol_name) const noexcept
{
    // POSIX mandates: clear dlerror(), call dlsym(), then check dlerror() to detect failure
    // (dlsym may legitimately return nullptr for a valid symbol).
    static_cast<void>(::dlerror());
    void* const symbol{::dlsym(handle, symbol_name)};
    if (CacheDlError())
    {
        return score::cpp::make_unexpected(Error::createUnspecifiedError());
    }
    return symbol;
}

score::cpp::expected_blank<Error> DlfcnImpl::dlclose(void* const handle) const noexcept
{
    if (::dlclose(handle) != 0)
    {
        // LCOV_EXCL_START (no safe way to make ::dlclose fail without undefined behavior)
        static_cast<void>(CacheDlError());
        return score::cpp::make_unexpected(Error::createUnspecifiedError());
        // LCOV_EXCL_STOP
    }
    has_dl_error_ = false;
    return {};
}

std::optional<std::string_view> DlfcnImpl::dlerror() const noexcept
{
    if (has_dl_error_)
    {
        return std::string_view{last_dl_error_.data()};
    }
    return std::nullopt;
}

bool DlfcnImpl::CacheDlError() const noexcept
{
    const char* const dl_error{::dlerror()};
    if (dl_error != nullptr)
    {
        std::strncpy(last_dl_error_.data(), dl_error, kDlErrorBufSize - 1U);
        last_dl_error_[kDlErrorBufSize - 1U] = '\0';
        has_dl_error_ = true;
        return true;
    }
    has_dl_error_ = false;
    return false;
}

}  // namespace score::os
