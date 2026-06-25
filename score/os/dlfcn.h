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

#ifndef SCORE_LIB_OS_DLFCN_H
#define SCORE_LIB_OS_DLFCN_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <cstdint>
#include <cstring>
#include <optional>
#include <string_view>
#include <type_traits>

namespace score
{
namespace os
{

class Dlfcn
{
  public:
    enum class DlOpenFlag : std::uint32_t
    {
        kLazy = 1U,
        kNow = 2U,
        kLocal = 4U,
        kGlobal = 8U,
    };

    /// \brief Opens a shared library.
    ///
    /// @return An opaque handle to the loaded library, or an Error on failure.
    virtual score::cpp::expected<void*, Error> dlopen(const char* const file_name, const DlOpenFlag flags) const noexcept = 0;

    /// \brief Looks up a symbol in a shared library.
    ///
    /// @return The address of the symbol as void*, or an Error if not found.
    virtual score::cpp::expected<void*, Error> dlsym(void* const handle, const char* const symbol_name) const noexcept = 0;
    /// \brief Closes a shared library handle previously returned by dlopen().
    ///
    /// @return score::cpp::expected_blank<Error> - success or an Error on failure.
    virtual score::cpp::expected_blank<Error> dlclose(void* handle) const noexcept = 0;

    /// \brief Returns the last dynamic linker error message, or std::nullopt if none.
    ///
    /// @details After a failed dlopen() or dlsym() call, this method returns the diagnostic
    ///          string produced by the dynamic linker (equivalent to ::dlerror()).
    ///
    ///          The returned string_view is only valid until the next call to dlopen(), dlsym(),
    ///          or dlerror() on the same instance — callers must consume it immediately
    ///          and must not store the view for later use.
    ///
    /// @note This class is not thread-safe. Concurrent calls to dlopen(), dlsym(), or dlerror()
    ///       on the same instance from multiple threads constitute a data race.
    ///
    /// @return A string_view of the error message, or std::nullopt if no error is pending.
    virtual std::optional<std::string_view> dlerror() const noexcept = 0;
    /// \brief Type-safe wrapper around dlsym that returns a typed function pointer.
    ///
    /// @return The resolved function pointer, or an Error if the symbol was not found.
    template <typename FnPtr>
    score::cpp::expected<FnPtr, Error> DlsymAs(void* const handle, const char* const symbol_name) const noexcept
    {
        static_assert(std::is_function<std::remove_pointer_t<FnPtr>>::value, "FnPtr must be a function pointer type");
        static_assert(sizeof(FnPtr) == sizeof(void*), "function pointer and void* must have the same size");

        const auto result = dlsym(handle, symbol_name);
        if (!result.has_value())
        {
            return score::cpp::make_unexpected(result.error());
        }
        FnPtr fn_ptr{};
        std::memcpy(&fn_ptr, &result.value(), sizeof(fn_ptr));
        return fn_ptr;
    }

    virtual ~Dlfcn() = default;
    Dlfcn(const Dlfcn&) = delete;
    Dlfcn& operator=(const Dlfcn&) = delete;
    Dlfcn(Dlfcn&&) = delete;
    Dlfcn& operator=(Dlfcn&&) = delete;

  protected:
    Dlfcn() = default;
};

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Dlfcn::DlOpenFlag> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_DLFCN_H
