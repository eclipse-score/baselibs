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

#ifndef SCORE_UTILS_STRING_REGISTRY_REGISTRY_STRING_VIEW_H
#define SCORE_UTILS_STRING_REGISTRY_REGISTRY_STRING_VIEW_H

#include <ostream>
#include <string_view>

namespace score::utils::string_registry
{

///
/// @class RegistryStringView
///
/// @brief RegistryStringView is an unmodifiable view of a string held in StringRegistry.
///        For performance reasons the comparison between instances of RegistryStringView is done
///        using simple pointer equality check and a check against other String_views is done by
///        character comparison.
///        It is the programmer's responsibility to ensure that RegistryStringView does not
///        outlive the StringRegistry it is referring to.
///
class RegistryStringView final
{
    friend class StringRegistry;

  public:
    using ConstPointer = std::string_view::const_pointer;
    using ConstIterator = std::string_view::const_iterator;
    using DifferenceType = std::string_view::difference_type;
    using SizeType = std::string_view::size_type;
    // npos is a constant representing the largest possible value of SizeType, because of
    // signed-to-unsigned implicit conversion. It is used as an "end of string" or "not found"
    // indicator. More details: https://en.cppreference.com/w/cpp/string/basic_string/npos
    static constexpr SizeType kNpos{static_cast<SizeType>(-1)};

    ///
    /// @brief Copy constructor (default)
    ///
    constexpr RegistryStringView(const RegistryStringView& other) noexcept = default;

    ///
    /// @brief Move constructor (default)
    ///
    constexpr RegistryStringView(RegistryStringView&& other) noexcept = default;

    ///
    /// @brief Destructor (default)
    ///
    ~RegistryStringView() noexcept = default;

    ///
    /// @brief Copy assignment operator (default)
    ///
    constexpr RegistryStringView& operator=(const RegistryStringView& view) & noexcept = default;

    ///
    /// @brief Move assignment operator (default)
    ///
    constexpr RegistryStringView& operator=(RegistryStringView&& view) & noexcept = default;

    ///
    /// @brief Get string data
    ///
    [[nodiscard]] constexpr ConstPointer Data() const noexcept
    {
        return string_view_.data();
    }

    ///
    /// @brief Get string length
    ///
    [[nodiscard]] constexpr SizeType Length() const noexcept
    {
        return string_view_.length();
    }

    ///
    /// @brief Get string length
    ///
    [[nodiscard]] constexpr SizeType Size() const noexcept
    {
        return string_view_.size();
    }

    ///
    /// @brief Get whether the string is empty
    ///
    [[nodiscard]] constexpr bool Empty() const noexcept
    {
        return string_view_.empty();
    }

    ///
    /// @brief Const iterator to the beginning
    ///
    [[nodiscard]] constexpr ConstIterator Begin() const noexcept
    {
        return Data();
    }

    ///
    /// @brief Const iterator to the beginning
    ///
    [[nodiscard]] constexpr ConstIterator CBegin() const noexcept
    {
        return Data();
    }

    ///
    /// @brief Iterator to end
    ///
    [[nodiscard]] ConstIterator End() const noexcept
    {
        return std::next(Data(), static_cast<DifferenceType>(Length()));
    }

    ///
    /// @brief Const iterator to the end
    ///
    [[nodiscard]] ConstIterator CEnd() const noexcept
    {
        return std::next(Data(), static_cast<DifferenceType>(Length()));
    }

    ///
    /// @brief Get the underlying std::string_view
    ///
    [[nodiscard]] constexpr std::string_view StringView() const noexcept
    {
        return string_view_;
    }

  private:
    std::string_view string_view_;  ///< Underlying view pointing into StringRegistry storage

    explicit constexpr RegistryStringView(std::string_view view) : string_view_{view} {}
};

///
/// @brief operator==
///
constexpr bool operator==(RegistryStringView lhs, RegistryStringView rhs) noexcept
{
    return (lhs.Data() == rhs.Data()) && (lhs.Length() == rhs.Length());
}

///
/// @brief operator!=
///
constexpr bool operator!=(RegistryStringView lhs, RegistryStringView rhs) noexcept
{
    return !(lhs == rhs);
}

///
/// @brief operator<
///
constexpr bool operator<(RegistryStringView lhs, RegistryStringView rhs) noexcept
{
    return lhs.StringView() < rhs.StringView();
}

///
/// @brief operator<=
///
constexpr bool operator<=(RegistryStringView lhs, RegistryStringView rhs) noexcept
{
    return lhs.StringView() <= rhs.StringView();
}

///
/// @brief operator>
///
constexpr bool operator>(RegistryStringView lhs, RegistryStringView rhs) noexcept
{
    return lhs.StringView() > rhs.StringView();
}

///
/// @brief operator>=
///
constexpr bool operator>=(RegistryStringView lhs, RegistryStringView rhs) noexcept
{
    return lhs.StringView() >= rhs.StringView();
}

///
/// @brief operator<<
///        Overload of << to write the string from a RegistryStringView to ostream.
///
/// @return Reference to the ostream object used for writing, to enable chaining of << operations
///         as usual.
///
inline std::ostream& operator<<(std::ostream& os, RegistryStringView v)
{
    return os.write(v.Data(), static_cast<std::streamsize>(v.Length()));
}

}  // namespace score::utils::string_registry

namespace std
{

/// @brief std::hash specialization for RegistryStringView
///
/// @return Hash value for the given RegistryStringView
///
template <>
struct hash<::score::utils::string_registry::RegistryStringView>
{
    size_t operator()(const ::score::utils::string_registry::RegistryStringView& sv) const noexcept
    {
        // For the conversion of RegistryStringView data pointer to intptr_t, the reinterpret_cast
        // is necessary.
        // Pointer-to-integer conversion is required for memory address hashing in this case.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return std::hash<std::intptr_t>{}(reinterpret_cast<std::intptr_t>(sv.Data()));
    }
};

}  // namespace std

#endif  // SCORE_UTILS_STRING_REGISTRY_REGISTRY_STRING_VIEW_H
