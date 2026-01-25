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
#include <score/assert.hpp>
#include <score/type_traits.hpp>
#include <score/utility.hpp>

namespace score::cpp
{

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
    return std::forward<Variant>(var).invoke_visitor(std::forward<Visitor>(vis));
}

/// \brief Provides a member value to access the number of alternatives in a variant as a compile-time constant
/// expression.
///
/// \tparam T Type of the variant.
template <typename T>
struct variant_size : std::integral_constant<std::size_t, T::variant_size>
{
};

/// \cond detail
namespace detail
{
template <typename T, typename Tuple>
struct tuple_index;

template <typename T, typename... Types>
struct tuple_index<T, std::tuple<T, Types...>>
{
    constexpr static const std::ptrdiff_t value{0};
};

template <typename T, typename U, typename... Types>
struct tuple_index<T, std::tuple<U, Types...>>
{
    constexpr static const std::ptrdiff_t value{1 + tuple_index<T, std::tuple<Types...>>::value};
};
} // namespace detail
/// \endcond

template <typename... T>
class variant;

/// \brief Provides compile-time indexed access to the types of the alternatives of the variant.
///
/// Provides a member type `type` denoting the type of Ith alternative of the variant.
///
/// \tparam I Index of the alternative.
/// \tparam T The variant.
template <std::size_t I, typename T>
struct variant_alternative;

template <std::size_t I, typename... Types>
struct variant_alternative<I, variant<Types...>>
{
    using type = std::tuple_element_t<I, std::tuple<Types...>>;
};
template <std::size_t I, typename... Types>
struct variant_alternative<I, const variant<Types...>>
{
    using type = std::tuple_element_t<I, const std::tuple<Types...>>;
};

template <std::size_t I, typename T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

/// \brief Checks if the variant v holds the alternative T.
///
/// \tparam T Alternative T to query.
/// \tparam U Type of the alternatives of score::cpp::variant.
/// \param v variant to examine.
/// \return true if the variant currently holds the alternative T, false otherwise.
template <typename T, typename... U>
constexpr bool holds_alternative(const variant<U...>& v) noexcept
{
    return detail::tuple_index<T, std::tuple<U...>>::value == v.index();
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
template <typename T, typename Variant>
constexpr T& get(Variant& v)
{
    static_assert(!std::is_void<detail::tuple_index<T, typename Variant::alternative_types>>::value, "no T in variant");
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(score::cpp::holds_alternative<T>(v));
    return reinterpret_cast<T&>(v.storage_[0U]);
}

template <std::size_t I, typename... T>
constexpr auto& get(score::cpp::variant<T...>& v)
{
    static_assert(I < variant_size<score::cpp::variant<T...>>::value, "Index is out of bounds.");
    using type = typename score::cpp::variant_alternative_t<I, score::cpp::variant<T...>>;
    return score::cpp::get<type>(v);
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
template <typename T, typename Variant>
constexpr const T& get(const Variant& v)
{
    static_assert(!std::is_void<detail::tuple_index<T, typename Variant::alternative_types>>::value, "no T in variant");
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(score::cpp::holds_alternative<T>(v));
    return reinterpret_cast<const T&>(v.storage_[0U]);
}

template <std::size_t I, typename... T>
constexpr const auto& get(const score::cpp::variant<T...>& v)
{
    static_assert(I < variant_size<score::cpp::variant<T...>>::value, "Index is out of bounds.");
    using type = typename score::cpp::variant_alternative_t<I, score::cpp::variant<T...>>;
    return score::cpp::get<type>(v);
}

/// \brief Type-based value accessor: If v holds the alternative T, returns a pointer to the value stored in v.
///
/// \tparam T The unique type to look up.
/// \tparam Variant The type of the variant.
/// \param v variant to examine.
///
/// \return Pointer to the value stored in the variant; or nullptr if the variant does not contain T.
template <typename T, typename Variant>
constexpr T* get_if(Variant* v) noexcept
{
    if (score::cpp::holds_alternative<T>(*v))
    {
        return reinterpret_cast<T*>(score::cpp::data(v->storage_));
    }
    else
    {
        return nullptr;
    }
}

/// \brief Type-based value accessor: If v holds the alternative T, returns a pointer to the value stored in v.
///
/// \tparam T The unique type to look up.
/// \tparam Variant The type of the variant.
/// \param v variant to examine.
///
/// \return Pointer to the value stored in the variant; or nullptr if the variant does not contain T.
template <typename T, typename Variant>
constexpr const T* get_if(const Variant* v) noexcept
{
    if (score::cpp::holds_alternative<T>(*v))
    {
        return reinterpret_cast<const T*>(score::cpp::data(v->storage_));
    }
    else
    {
        return nullptr;
    }
}

/// \cond detail
namespace detail
{
struct destruct
{
    template <typename U>
    void operator()(U& u) const
    {
        (void)u; // avoid warning for POD type destruction
        u.~U();
    }

    // specialization for array types
    template <typename U, std::size_t N>
    void operator()(U (&u)[N]) const
    {
        for (std::size_t i = 0U; i < N; ++i)
        {
            u[i].~U();
        }
    }
};

template <typename... U>
struct copy_construct
{
    explicit copy_construct(variant<U...>& target) : target_(&target) {}

    template <typename V>
    void operator()(const V& u) const
    {
        static_cast<void>(::new (score::cpp::data(target_->storage_)) V(u));
    }

private:
    variant<U...>* target_;
};

template <typename... U>
struct copy_assign
{
    explicit copy_assign(variant<U...>& target) : target_(&target) {}

    template <typename V>
    void operator()(const V& u) const
    {
        *target_ = u;
    }

private:
    variant<U...>* target_;
};

template <typename... U>
struct move_construct
{
    explicit move_construct(variant<U...>& target) : target_(&target) {}

    template <typename V>
    void operator()(V&& u) const
    {
        // unconditional cast to &&, i.e., use std::move instead of std::forward.
        static_cast<void>(::new (score::cpp::data(target_->storage_)) typename std::remove_reference<V>::type(std::move(u)));
    }

private:
    variant<U...>* target_;
};

template <typename... U>
struct move_assign
{
    explicit move_assign(variant<U...>& target) : target_(&target) {}

    template <typename V>
    void operator()(V&& u) const
    {
        // unconditional cast to &&, i.e., use std::move instead of std::forward.
        *target_ = std::move(u);
    }

private:
    variant<U...>* target_;
};

template <typename BinaryPredicate, typename... U>
struct binary_predicate_visitor
{
    explicit binary_predicate_visitor(const BinaryPredicate& pred, const variant<U...>& lhs) : pred_{pred}, lhs_{lhs} {}

    template <typename InternalType>
    bool operator()(const InternalType& rhs_internal_value) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(score::cpp::holds_alternative<InternalType>(lhs_));
        const InternalType& lhs_internal_value{get<InternalType>(lhs_)};
        return pred_(lhs_internal_value, rhs_internal_value);
    }

private:
    const BinaryPredicate& pred_;
    const variant<U...>& lhs_;
};

template <typename BinaryPredicate, typename... U>
binary_predicate_visitor<BinaryPredicate, U...> make_binary_predicate_visitor(const BinaryPredicate& op,
                                                                              const variant<U...>& lhs)
{
    return binary_predicate_visitor<BinaryPredicate, U...>{op, lhs};
}

template <std::size_t I, typename Visitor, typename... T>
constexpr decltype(auto) dispatch(Visitor&& vis, variant<T...>& var)
{
    return std::forward<Visitor>(vis)(score::cpp::get<I>(var));
}

// Different name for const overload because of MSVC error
template <std::size_t I, typename Visitor, typename... T>
constexpr decltype(auto) const_dispatch(Visitor&& vis, const variant<T...>& var)
{
    return std::forward<Visitor>(vis)(score::cpp::get<I>(var));
}

template <typename Visitor, typename... T, std::size_t... I>
constexpr decltype(auto)
visit_at(Visitor&& vis, variant<T...>& var, const std::ptrdiff_t index, std::index_sequence<I...>)
{
    using return_type = decltype(score::cpp::detail::dispatch<0U>(std::forward<Visitor>(vis), var));
    using f_ptr_type = return_type (*)(Visitor&&, variant<T...>&);
    const std::array<f_ptr_type, sizeof...(T)> f_table{score::cpp::detail::dispatch<I>...};
    return score::cpp::at(f_table, index)(std::forward<Visitor>(vis), var);
}

template <typename Visitor, typename... T, std::size_t... I>
constexpr decltype(auto)
visit_at(Visitor&& vis, const variant<T...>& var, const std::ptrdiff_t index, std::index_sequence<I...>)
{
    using return_type = decltype(score::cpp::detail::const_dispatch<0U>(std::forward<Visitor>(vis), var));
    using f_ptr_type = return_type (*)(Visitor&&, const variant<T...>&);
    const std::array<f_ptr_type, sizeof...(T)> f_table{score::cpp::detail::const_dispatch<I>...};
    return score::cpp::at(f_table, index)(std::forward<Visitor>(vis), var);
}

template <std::size_t... V>
constexpr std::size_t max_impl()
{
    std::size_t result{0U};
    (void)std::initializer_list<int>{((result = std::max(result, V)), 0)...};
    return result;
}

template <bool... B>
constexpr bool fold_or()
{
    bool result{false};
    (void)std::initializer_list<int>{((result = result || B), 0)...};
    return result;
}

template <typename... T>
constexpr std::size_t max_size()
{
    return score::cpp::detail::max_impl<sizeof(T)...>();
}

template <typename... T>
constexpr std::size_t max_alignment()
{
    return score::cpp::detail::max_impl<std::alignment_of<T>::value...>();
}
} // namespace detail
/// \endcond

///
/// \brief Generic type-safe union.
///
/// This class represents a generic type-safe union.
///
/// The variant can never be empty. It will be always default constructed with
/// the first type.
///
template <typename... T>
class variant
{
    template <typename U, typename Variant>
    friend constexpr U& get(Variant&); // to access storage_

    template <typename U, typename Variant>
    friend constexpr const U& get(const Variant&); // to access storage_

    template <typename U, typename Variant>
    friend constexpr U* get_if(Variant*) noexcept; // to access storage_

    template <typename U, typename Variant>
    friend constexpr const U* get_if(const Variant*) noexcept; // to access storage_

    template <typename... U>
    friend struct detail::copy_construct; // to access storage_

    template <typename... U>
    friend struct detail::move_construct; // to access storage_

    using alternative_types = std::tuple<T...>;

public:
    /// storage size, i.e., sizeof(Tx) where Tx is the largest type.
    static constexpr std::size_t storage_size{score::cpp::detail::max_size<T...>()};

    /// storage alignment, i.e., max(std::alignment_of(Tx)) of all Tx.
    static constexpr std::size_t storage_alignment{score::cpp::detail::max_alignment<T...>()};

    using storage_type = typename std::aligned_storage<storage_size, storage_alignment>::type[1];

    /// Provides access to the number of alternatives in a variant as a compile-time constant expression.
    static constexpr std::size_t variant_size{std::tuple_size<alternative_types>::value};

    /// \brief Default construct a variant.
    ///
    /// A default constructed variant will be constructed with the T0 provided type.
    variant() : index_{0}
    {
        static_cast<void>(::new (score::cpp::data(storage_)) typename std::tuple_element<0U, alternative_types>::type);
    }

    /// \brief Construct a variant which is a copy of \a rhs.
    ///
    /// \param rhs Object to construct the copy from.
    variant(const variant& rhs) : index_{rhs.index_} { score::cpp::visit(detail::copy_construct<T...>{*this}, rhs); }

    variant(variant&& rhs) : index_{rhs.index_} { score::cpp::visit(detail::move_construct<T...>{*this}, std::move(rhs)); }

    /// \brief Construct a variant from another type.
    ///
    /// \param other Other type this variant shall be constructed from.
    template <typename U,
              typename = typename std::enable_if<
                  score::cpp::detail::fold_or<std::is_same<score::cpp::remove_cvref_t<T>, score::cpp::remove_cvref_t<U>>::value...>() &&
                  !std::is_same<score::cpp::in_place_type_t<U>, score::cpp::remove_cvref_t<U>>::value>::type>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr variant(U&& other) : index_{detail::tuple_index<score::cpp::remove_cvref_t<U>, alternative_types>::value}
    {
        static_cast<void>(::new (score::cpp::data(storage_)) score::cpp::remove_cvref_t<U>(std::forward<U>(other)));
    }

    /// \brief Construct a variant using direct-initialization.
    template <typename U,
              typename... Args,
              typename = typename std::enable_if<std::is_constructible<U, Args...>::value>::type>
    constexpr explicit variant(score::cpp::in_place_type_t<U>, Args&&... args)
        : index_{detail::tuple_index<score::cpp::remove_cvref_t<U>, alternative_types>::value}
    {
        static_cast<void>(::new (score::cpp::data(storage_)) score::cpp::remove_cvref_t<U>(std::forward<Args>(args)...));
    }

    /// \brief Destruct the variant and the containing object.
    ~variant() { score::cpp::visit(detail::destruct{}, *this); }

    /// \brief Assignment operator of variant
    ///
    /// \param rhs Source of the copy.
    /// \return Reference to this object.
    variant& operator=(const variant& rhs)
    {
        score::cpp::visit(detail::copy_assign<T...>{*this}, rhs);
        return *this;
    }

    variant& operator=(variant&& rhs)
    {
        score::cpp::visit(detail::move_assign<T...>{*this}, std::move(rhs));
        return *this;
    }

    /// \brief Assign a new object to the variant
    ///
    /// \param rhs Right hand side of the copy assignment.
    ///
    /// The current object inside the variant will be destructed and the new object will be copied into the variant.
    template <typename U,
              typename = typename std::enable_if<
                  score::cpp::detail::fold_or<std::is_same<score::cpp::remove_cvref_t<T>, score::cpp::remove_cvref_t<U>>::value...>()>::type>
    variant& operator=(U&& rhs)
    {
        score::cpp::visit(detail::destruct{}, *this);
        index_ = detail::tuple_index<score::cpp::remove_cvref_t<U>, alternative_types>::value;
        static_cast<void>(::new (score::cpp::data(storage_)) score::cpp::remove_cvref_t<U>(std::forward<U>(rhs)));
        return *this;
    }

    /// \brief Function shall not be used directly - use score::cpp::visit().
    template <typename Visitor>
    decltype(auto) invoke_visitor(Visitor&& vis) const
    {
        return score::cpp::detail::visit_at(
            std::forward<Visitor>(vis), *this, index(), std::make_index_sequence<variant_size>{});
    }

    /// \brief Function shall not be used directly - use score::cpp::visit().
    ///
    /// Mutable version of the variant.
    template <typename Visitor>
    decltype(auto) invoke_visitor(Visitor&& vis)
    {
        return score::cpp::detail::visit_at(
            std::forward<Visitor>(vis), *this, index(), std::make_index_sequence<variant_size>{});
    }

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
        score::cpp::visit(detail::destruct{}, *this);
        index_ = detail::tuple_index<U, alternative_types>::value;
        return *(::new (score::cpp::data(storage_)) U(std::forward<Args>(args)...));
    }

    /// \brief Query current type index at run time.
    ///
    /// \return Current index at run time.
    std::ptrdiff_t index() const { return index_; }

private:
    std::ptrdiff_t index_;
    storage_type storage_;
};

/// \brief Compares two variants for equality.
///
/// \tparam T Internal types of the variant.
/// \param lhs Left hand side of the comparison.
/// \param rhs Right hand side of the comparison.
/// \return True if lhs and rhs do contain the same object type and if they compare equal to each other via the
/// operator== evaluated on both internal values.
template <typename... T>
bool operator==(const variant<T...>& lhs, const variant<T...>& rhs)
{
    if (lhs.index() != rhs.index())
    {
        return false;
    }

    return score::cpp::visit(
        score::cpp::detail::make_binary_predicate_visitor([](const auto& a, const auto& b) { return a == b; }, lhs), rhs);
}

/// \brief Compares two variants for inequality.
///
/// \tparam T Internal types of the variant.
/// \param lhs Left hand side of the comparison.
/// \param rhs Right hand side of the comparison.
/// \return True if lhs and rhs do not contain the same object type, otherwise the result of operator!= called on both
/// internal values.
template <typename... T>
bool operator!=(const variant<T...>& lhs, const variant<T...>& rhs)
{
    if (lhs.index() != rhs.index())
    {
        return true;
    }

    return score::cpp::visit(
        score::cpp::detail::make_binary_predicate_visitor([](const auto& a, const auto& b) { return a != b; }, lhs), rhs);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_VARIANT_HPP
