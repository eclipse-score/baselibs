/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.StringView component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_STRING_VIEW_HPP
#define SCORE_LANGUAGE_FUTURECPP_STRING_VIEW_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <score/algorithm.hpp>
#include <score/assert.hpp>
#include <score/hash.hpp>
#include <score/memory_resource.hpp>
#include <score/string.hpp>
#include <score/utility.hpp>

namespace score::cpp
{

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view
{
public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = CharT*;
    using const_pointer = const CharT*;
    using reference = CharT&;
    using const_reference = const CharT&;
    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using size_type = std::size_t;

    /// \brief This is a special value equal to the maximum value representable by the type size_type.
    ///
    /// The exact meaning depends on context, but it is generally used either as end of view indicator by the functions
    /// that expect a view index or as the error indicator by the functions that return a view index.
    static constexpr size_type npos{std::numeric_limits<size_type>::max()};

    /// \brief Constructs an empty basic_string_view.
    ///
    /// After construction, data() is equal to nullptr, and size() is equal to 0.
    constexpr basic_string_view() noexcept : basic_string_view{nullptr, 0U} {}

    /// \brief Constructs from character array.
    ///
    /// Constructs a view of the first count characters of the character array starting with the element pointed by s. s
    /// can contain null characters. The behavior is undefined if [s, s+count) is not a valid range (even though the
    /// constructor may not access any of the elements of this range). After construction, data() is equal to s, and
    /// size() is equal to count.
    constexpr basic_string_view(const CharT* s, const size_type count) : data_{s}, size_{count} {}

    /// \brief Constructs from character array.
    ///
    /// Constructs a view of the null-terminated character string pointed to by s, not including the terminating null
    /// character. The length of the view is determined as if by \code Traits::length(s) \endcode. The behavior is
    /// undefined if \code [s, s+Traits::length(s)) \endcode is not a valid range. After construction, data() is equal
    /// to s, and size() is equal to \code Traits::length(s) \endcode.
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr basic_string_view(const CharT* s) : basic_string_view{s, (s == nullptr) ? 0U : traits_type::length(s)} {}

    /// \brief Constructs from std::basic_string.
    template <typename Allocator>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr basic_string_view(const std::basic_string<CharT, Traits, Allocator>& s) noexcept
        : basic_string_view{s.data(), s.size()}
    {
    }

    /// \brief Conversion from std::basic_string_view.
    ///
    /// \note non-standard. but simplifies transition to `std::string_view`
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversion from `std::string_view`
    constexpr basic_string_view(const std::basic_string_view<CharT> s) noexcept : basic_string_view{s.data(), s.size()}
    {
    }

    /// \brief Conversion to std::basic_string_view.
    ///
    /// \note non-standard. but simplifies transition to `std::string_view`
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversion to `std::string_view`
    constexpr operator std::basic_string_view<CharT>() const noexcept { return {data(), size()}; }

    /// \brief Returns a pointer to the underlying character array.
    ///
    /// \note Unlike std::basic_string::data() and string literals, data() may return a pointer to a buffer that is not
    /// null-terminated. Therefore it is typically a mistake to pass data() to a routine that takes just a const CharT*
    /// and expects a null-terminated string.
    constexpr const_pointer data() const noexcept { return data_; }

    /// \brief Returns the number of CharT elements in the view.
    constexpr size_type size() const noexcept { return size_; }

    /// \brief Checks if the view has no characters.
    constexpr bool empty() const noexcept { return 0U == size(); }

    /// \{
    /// \brief Returns an iterator to the first character of the view.
    constexpr const_iterator begin() const noexcept { return cbegin(); }
    constexpr const_iterator cbegin() const noexcept { return data(); }
    /// \}

    /// \{
    /// \brief Returns an iterator to the character following the last character of the view.
    ///
    /// This character acts as a placeholder, attempting to access it results in undefined behavior.
    constexpr const_iterator end() const noexcept { return cend(); }
    constexpr const_iterator cend() const noexcept { return data() + size(); }
    /// \}

    /// \brief Returns reference to the first character in the view.
    ///
    /// \pre !empty()
    constexpr const_reference front() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return *begin();
    }

    /// \brief Returns reference to the last character in the view.
    ///
    /// \pre !empty()
    constexpr const_reference back() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return *(end() - 1);
    }

    /// \brief Lexicographically compares the characters of the character strings *this and other.
    ///
    /// If count is zero, strings are considered equal.
    ///
    /// \return negative value if this view is less than the other character sequence, zero if the both character
    /// sequences are equal, positive value if this view is greater than the other character sequence.
    int compare(const basic_string_view other) const noexcept
    {
        const std::size_t str_size{std::min(size(), other.size())};
        int result{traits_type::compare(data(), other.data(), str_size)};
        if (result == 0)
        {
            result = (size() == other.size()) ? 0 : ((size() < other.size()) ? -1 : 1);
        }
        return result;
    }

    /// \{
    /// \brief Finds the first substring equal to the given character sequence.
    ///
    /// \return Position of the first character of the found substring, or npos if no such substring is found.
    size_type find(const CharT c) const noexcept
    {
        const CharT* const pos{traits_type::find(data(), size(), c)};
        return (nullptr == pos) ? npos : score::cpp::narrow_cast<size_type>(std::distance(begin(), pos));
    }
    size_type find(const basic_string_view v) const noexcept
    {
        if (v.empty())
        {
            return 0U;
        }
        if (size() < v.size())
        {
            return npos;
        }

        for (size_type i{0U}; i < (size() - v.size() + 1U); ++i)
        {
            if (traits_type::eq(v.front(), *(begin() + i)) &&
                (0 == traits_type::compare(begin() + i, v.data(), v.size())))
            {
                return i;
            }
        }

        return npos;
    }
    /// \}

    /// \{
    /// \brief Checks if the string view begins with the given prefix.
    ///
    /// \return true if the string view begins with the provided prefix, false otherwise.
    bool starts_with(const CharT c) const noexcept
    {
        if (empty())
        {
            return false;
        }
        return traits_type::eq(front(), c);
    }
    bool starts_with(const basic_string_view v) const noexcept
    {
        if (size() < v.size())
        {
            return false;
        }
        return 0 == traits_type::compare(begin(), v.data(), v.size());
    }
    /// \}

    /// \{
    /// \brief Checks if the string view ends with the given suffix.
    ///
    /// \return true if the string view ends with the provided suffix, false otherwise.
    bool ends_with(const CharT c) const noexcept
    {
        if (empty())
        {
            return false;
        }
        return traits_type::eq(back(), c);
    }
    bool ends_with(const basic_string_view v) const noexcept
    {
        if (size() < v.size())
        {
            return false;
        }
        return 0 == traits_type::compare(end() - v.size(), v.data(), v.size());
    }
    /// \}

    /// \brief Moves the start of the view forward by n characters.
    ///
    /// \pre n <= size()
    constexpr void remove_prefix(const size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(n <= size());
        data_ += n;
        size_ -= n;
    }

    /// \brief Moves the end of the view back by n characters.
    ///
    /// \pre n <= size()
    constexpr void remove_suffix(const size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(n <= size());
        size_ -= n;
    }

    /// \{
    /// \brief Checks if the string view contains the given substring.
    constexpr bool contains(basic_string_view sview) const noexcept { return find(sview) != npos; }

    constexpr bool contains(value_type c) const noexcept { return find(c) != npos; }

    constexpr bool contains(const value_type* str) const { return find(str) != npos; }
    /// \}

    /// \brief Returns a string from *this.
    ///
    /// \param allocator Allocator to be used to construct the string.
    /// \return A pmr::string from *this.
    score::cpp::pmr::basic_string<CharT, Traits> to_string(const score::cpp::pmr::polymorphic_allocator<CharT>& allocator = {}) const
    {
        return score::cpp::pmr::basic_string<CharT, Traits>{begin(), end(), allocator};
    }

private:
    const_pointer data_;
    size_type size_;
};

template <typename CharT, typename Traits>
constexpr typename basic_string_view<CharT, Traits>::size_type basic_string_view<CharT, Traits>::npos;

template <typename CharT, typename Traits>
constexpr bool operator==(const basic_string_view<CharT, Traits> lhs,
                          const basic_string_view<CharT, Traits> rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    return lhs.compare(rhs) == 0;
}
template <typename CharT, typename Traits>
constexpr bool operator!=(const basic_string_view<CharT, Traits> lhs,
                          const basic_string_view<CharT, Traits> rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return true;
    }
    return lhs.compare(rhs) != 0;
}
template <typename CharT, typename Traits>
constexpr bool operator<(const basic_string_view<CharT, Traits> lhs,
                         const basic_string_view<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}
template <typename CharT, typename Traits>
constexpr bool operator<=(const basic_string_view<CharT, Traits> lhs,
                          const basic_string_view<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}
template <typename CharT, typename Traits>
constexpr bool operator>(const basic_string_view<CharT, Traits> lhs,
                         const basic_string_view<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}
template <typename CharT, typename Traits>
constexpr bool operator>=(const basic_string_view<CharT, Traits> lhs,
                          const basic_string_view<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

using string_view = basic_string_view<char>;

inline namespace literals
{
inline namespace string_view_literals
{
inline constexpr score::cpp::string_view operator""_sv(const char* str, std::size_t len) noexcept { return {str, len}; }
} // namespace string_view_literals
} // namespace literals

} // namespace score::cpp

namespace std
{
template <>
struct hash<score::cpp::string_view>
{
    std::size_t operator()(const score::cpp::string_view s) const noexcept
    {
        return std::hash<std::string_view>{}({s.data(), s.size()});
    }
};

} // namespace std

#endif // SCORE_LANGUAGE_FUTURECPP_STRING_VIEW_HPP
