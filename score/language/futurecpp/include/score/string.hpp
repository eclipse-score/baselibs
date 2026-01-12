///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.String component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_STRING_HPP
#define SCORE_LANGUAGE_FUTURECPP_STRING_HPP

#include <score/private/iterator/at.hpp>   // IWYU pragma: export
#include <score/private/iterator/data.hpp> // IWYU pragma: export
#include <score/private/iterator/size.hpp> // IWYU pragma: export

#include <score/memory_resource.hpp>

#include <score/hash.hpp>

#include <cstdint>
#include <string> // IWYU pragma: export

namespace score::cpp
{
namespace pmr
{

// basic_string using polymorphic allocator in namespace pmr
template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string = std::basic_string<CharT, Traits, polymorphic_allocator<CharT>>;

// basic_string typedef names using polymorphic allocator in namespace pmr
using string = basic_string<char>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;
using wstring = basic_string<wchar_t>;

string to_string(const std::int32_t value, memory_resource* const resource);
string to_string(const std::int64_t value, memory_resource* const resource);
string to_string(const std::uint32_t value, memory_resource* const resource);
string to_string(const std::uint64_t value, memory_resource* const resource);
string to_string(const double value, memory_resource* const resource);

} // namespace pmr
} // namespace score::cpp

namespace std
{
template <>
struct hash<score::cpp::pmr::string>
{
    std::size_t operator()(const score::cpp::pmr::string& s) const noexcept
    {
#if __cplusplus >= 201703L
        return std::hash<std::string_view>{}(std::string_view{s.data(), s.size()});
#else
        return score::cpp::hash_bytes(s.data(), s.size());
#endif
    }
};
template <>
struct hash<score::cpp::pmr::u16string>
{
    std::size_t operator()(const score::cpp::pmr::u16string& s) const noexcept
    {
#if __cplusplus >= 201703L
        return std::hash<std::u16string_view>{}(std::u16string_view{s.data(), s.size()});
#else
        return score::cpp::hash_bytes(s.data(), s.size());
#endif
    }
};
template <>
struct hash<score::cpp::pmr::u32string>
{
    std::size_t operator()(const score::cpp::pmr::u32string& s) const noexcept
    {
#if __cplusplus >= 201703L
        return std::hash<std::u32string_view>{}(std::u32string_view{s.data(), s.size()});
#else
        return score::cpp::hash_bytes(s.data(), s.size());
#endif
    }
};
template <>
struct hash<score::cpp::pmr::wstring>
{
    std::size_t operator()(const score::cpp::pmr::wstring& s) const noexcept
    {
#if __cplusplus >= 201703L
        return std::hash<std::wstring_view>{}(std::wstring_view{s.data(), s.size()});
#else
        return score::cpp::hash_bytes(s.data(), s.size());
#endif
    }
};

} // namespace std

#endif // SCORE_LANGUAGE_FUTURECPP_STRING_HPP
