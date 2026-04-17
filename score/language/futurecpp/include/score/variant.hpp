/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Variant Component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_VARIANT_HPP
#define SCORE_LANGUAGE_FUTURECPP_VARIANT_HPP

#include <score/private/iterator/data.hpp>
#include <score/private/utility/in_place_type_t.hpp> // IWYU pragma: export

#include <algorithm>
#include <array>
#include <cstdint>
#include <new>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <score/assert.hpp>
#include <score/type_traits.hpp>
#include <score/utility.hpp>

namespace score::cpp
{

namespace detail
{

template <bool... B>
constexpr bool fold_and()
{
    return (... && B);
}

template <bool... B>
constexpr bool fold_or()
{
    return (... || B);
}

} // namespace detail

///
/// \brief Generic type-safe union.
///
/// This class represents a generic type-safe union.
///
/// The variant can never be empty. It will be always default constructed with
/// the first type.
///
template <typename... Ts>
class variant
{
    static_assert(score::cpp::detail::fold_and<!std::is_reference_v<Ts>...>(), "variant alteranatives must not be reference");
    static_assert(score::cpp::detail::fold_and<!std::is_array_v<Ts>...>(), "variant alteranatives must not be array");
    static_assert(score::cpp::detail::fold_and<!std::is_void_v<Ts>...>(), "variant alteranatives must not be void");

public:
    /// Provides access to the number of alternatives in a variant as a compile-time constant expression.
    static constexpr std::size_t variant_size{std::variant_size<std::variant<Ts...>>::value};

    /// \brief Default construct a variant.
    ///
    /// A default constructed variant will be constructed with the T0 provided type.
    variant() = default;

    /// \brief Construct a variant from another type.
    ///
    /// \param other Other type this variant shall be constructed from.
    template <typename U,
              typename = typename std::enable_if<
                  !std::is_same<variant, score::cpp::remove_cvref_t<U>>::value                                              //
                  && !std::is_same<score::cpp::in_place_type_t<U>, score::cpp::remove_cvref_t<U>>::value                           //
                  && score::cpp::detail::fold_or<std::is_same<score::cpp::remove_cvref_t<Ts>, score::cpp::remove_cvref_t<U>>::value...>() //
                  >::type>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr variant(U&& other) : storage_{std::forward<U>(other)}
    {
    }

    /// \brief Construct a variant using direct-initialization.
    template <typename U,
              typename... Args,
              typename = typename std::enable_if<std::is_constructible<U, Args...>::value>::type>
    constexpr explicit variant(score::cpp::in_place_type_t<U>, Args&&... args)
        : storage_{std::in_place_type<U>, std::forward<Args>(args)...}
    {
    }

    /// \brief Conversion from std::variant.
    ///
    /// \note non-standard. but simplifies transition to `std::variant`
    /// \{
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversion from `std::variant`
    variant(const std::variant<Ts...>& other) : storage_{other} {}
    variant(std::variant<Ts...>&& other) : storage_{std::move(other)} {}
    /// \}

    /// \brief Assign a new object to the variant
    ///
    /// \param rhs Right hand side of the copy assignment.
    ///
    /// The current object inside the variant will be destructed and the new object will be copied into the variant.
    template <
        typename U,
        typename = typename std::enable_if<
            !std::is_same<variant, score::cpp::remove_cvref_t<U>>::value //
            && score::cpp::detail::fold_or<std::is_same<score::cpp::remove_cvref_t<Ts>, score::cpp::remove_cvref_t<U>>::value...>()>::type>
    variant& operator=(U&& rhs)
    {
        storage_ = std::forward<U>(rhs);
        return *this;
    }

    /// \brief Conversion to std::variant.
    ///
    /// \note non-standard. but simplifies transition to `std::variant`
    /// \{
    // NOLINTNEXTLINE(google-explicit-constructor) allow implicit conversion to `std::variant`
    operator std::variant<Ts...>&() & { return storage_; }
    operator const std::variant<Ts...>&() const& { return storage_; }
    operator std::variant<Ts...>() && { return std::move(storage_); }
    /// \}

    /// \brief Emplace a new object inside the variant.
    ///
    /// \tparam U target object type.
    /// \param args constructor arguments to use when constructing the new value
    /// \return A reference to the new contained value
    template <typename U,
              typename... Args,
              typename = typename std::enable_if_t<std::is_constructible<U, Args...>::value>>
    U& emplace(Args&&... args)
    {
        return storage_.template emplace<U>(std::forward<Args>(args)...);
    }

    /// \{
    template <typename Visitor>
    decltype(auto) visit(Visitor&& vis) &
    {
        return std::visit(std::forward<Visitor>(vis), storage_);
    }
    template <typename Visitor>
    decltype(auto) visit(Visitor&& vis) const&
    {
        return std::visit(std::forward<Visitor>(vis), storage_);
    }
    template <typename Visitor>
    decltype(auto) visit(Visitor&& vis) &&
    {
        return std::visit(std::forward<Visitor>(vis), std::move(storage_));
    }
    template <typename Visitor>
    decltype(auto) visit(Visitor&& vis) const&&
    {
        return std::visit(std::forward<Visitor>(vis), std::move(storage_));
    }
    /// \}

    /// \brief Query current type index at run time.
    ///
    /// \return Current index at run time.
    std::ptrdiff_t index() const { return static_cast<std::ptrdiff_t>(storage_.index()); }

private:
    std::variant<Ts...> storage_;
};

/// \brief Applies the visitor vis to the variant var.
///
/// A basic implementation of the C++17 standard library function std::visit from chapter 20.7.6, Visitation. In
/// contrast to the standard the function only accepts one variant var, i.e., only supports a single dispatch.
///
/// Visitation also works well with score::cpp::overload(). score::cpp::overload() allows a more concise definition of the
/// visitor.
///
/// \code
/// const std::string str = score::cpp::visit(score::cpp::overload([](int) -> std::string { return "int"; },
///                                                  [](double) -> std::string { return "double"; }),
///                                    variant_object);
/// \endcode
///
/// \tparam Visitor A callable which is derived from score::cpp::static_visitor.
/// \tparam Variant The type of the variant.
/// \param vis A callable that accepts every possible alternative from the variant.
/// \param var The variant to pass to the visitor.
/// \return The value returned by the selected invocation of the visitor. The value is of type Visitor::result_type.
template <typename Visitor, typename Variant>
decltype(auto) visit(Visitor&& vis, Variant&& var)
{
    return std::forward<Variant>(var).visit(std::forward<Visitor>(vis));
}

/// \brief Provides a member value to access the number of alternatives in a variant as a compile-time constant
/// expression.
///
/// \tparam T Type of the variant.
template <typename T>
struct variant_size;

template <typename... Ts>
struct variant_size<score::cpp::variant<Ts...>> : std::variant_size<std::variant<Ts...>>
{
};

/// \brief Provides compile-time indexed access to the types of the alternatives of the variant.
///
/// Provides a member type `type` denoting the type of Ith alternative of the variant.
///
/// \tparam I Index of the alternative.
/// \tparam T The variant.
template <std::size_t I, typename T>
struct variant_alternative;

template <std::size_t I, typename... Ts>
struct variant_alternative<I, score::cpp::variant<Ts...>> : std::variant_alternative<I, std::variant<Ts...>>
{
};

template <std::size_t I, typename... Ts>
struct variant_alternative<I, const score::cpp::variant<Ts...>> : std::variant_alternative<I, const std::variant<Ts...>>
{
};

template <std::size_t I, typename T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

/// \brief Checks if the variant v holds the alternative T.
///
/// \tparam T Alternative T to query.
/// \tparam U Type of the alternatives of score::cpp::variant.
/// \param v variant to examine.
/// \return true if the variant currently holds the alternative T, false otherwise.
template <typename T, typename... Ts>
constexpr bool holds_alternative(const variant<Ts...>& v) noexcept
{
    return std::holds_alternative<T>(static_cast<const std::variant<Ts...>&>(v));
}

/// \brief Type-based value accessor: If var holds the alternative T, returns a reference to the value stored in var.
///
/// \note No checks are done to confirm that the internal type is the same type as the cast target type.
///
/// \tparam T The unique type to look up.
/// \tparam Variant The type of the variant.
/// \param v variant to examine.
///
/// \return Reference to the value stored in the variant.
template <typename T, typename... Ts>
constexpr T& get(score::cpp::variant<Ts...>& v)
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(score::cpp::holds_alternative<T>(v));
    return std::get<T>(static_cast<std::variant<Ts...>&>(v));
}

template <std::size_t I, typename... Ts>
constexpr auto& get(score::cpp::variant<Ts...>& v)
{
    static_assert(I < variant_size<score::cpp::variant<Ts...>>::value, "Index is out of bounds.");
    return std::get<I>(static_cast<std::variant<Ts...>&>(v));
}

/// \brief Type-based value accessor: If var holds the alternative T, returns a reference to the value stored in var.
///
/// \note No checks are done to confirm that the internal type is the same type as the cast target type.
///
/// \tparam T The unique type to look up.
/// \tparam Variant The type of the variant.
/// \param v variant to examine.
///
/// \return Reference to the value stored in the variant.
template <typename T, typename... Ts>
constexpr const T& get(const score::cpp::variant<Ts...>& v)
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(score::cpp::holds_alternative<T>(v));
    return std::get<T>(static_cast<const std::variant<Ts...>&>(v));
}

template <std::size_t I, typename... Ts>
constexpr const auto& get(const score::cpp::variant<Ts...>& v)
{
    static_assert(I < variant_size<score::cpp::variant<Ts...>>::value, "Index is out of bounds.");
    return std::get<I>(static_cast<const std::variant<Ts...>&>(v));
}

/// \brief Type-based value accessor: If v holds the alternative T, returns a pointer to the value stored in v.
///
/// \tparam T The unique type to look up.
/// \tparam Variant The type of the variant.
/// \param v variant to examine.
///
/// \return Pointer to the value stored in the variant; or nullptr if the variant does not contain T.
template <typename T, typename... Ts>
constexpr T* get_if(score::cpp::variant<Ts...>* v) noexcept
{
    if (v == nullptr)
    {
        return nullptr;
    }

    return std::get_if<T>(&static_cast<std::variant<Ts...>&>(*v));
}

/// \brief Type-based value accessor: If v holds the alternative T, returns a pointer to the value stored in v.
///
/// \tparam T The unique type to look up.
/// \tparam Variant The type of the variant.
/// \param v variant to examine.
///
/// \return Pointer to the value stored in the variant; or nullptr if the variant does not contain T.
template <typename T, typename... Ts>
constexpr const T* get_if(const score::cpp::variant<Ts...>* v) noexcept
{
    if (v == nullptr)
    {
        return nullptr;
    }

    return std::get_if<T>(&static_cast<const std::variant<Ts...>&>(*v));
}

/// \brief Compares two variants for equality.
///
/// \tparam T Internal types of the variant.
/// \param lhs Left hand side of the comparison.
/// \param rhs Right hand side of the comparison.
/// \return True if lhs and rhs do contain the same object type and if they compare equal to each other via the
/// operator== evaluated on both internal values.
template <typename... Ts>
bool operator==(const variant<Ts...>& lhs, const variant<Ts...>& rhs)
{
    return static_cast<const std::variant<Ts...>&>(lhs) == static_cast<const std::variant<Ts...>&>(rhs);
}

/// \brief Compares two variants for inequality.
///
/// \tparam T Internal types of the variant.
/// \param lhs Left hand side of the comparison.
/// \param rhs Right hand side of the comparison.
/// \return True if lhs and rhs do not contain the same object type, otherwise the result of operator!= called on both
/// internal values.
template <typename... Ts>
bool operator!=(const variant<Ts...>& lhs, const variant<Ts...>& rhs)
{
    return static_cast<const std::variant<Ts...>&>(lhs) != static_cast<const std::variant<Ts...>&>(rhs);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_VARIANT_HPP
