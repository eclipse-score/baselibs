///
/// \file
/// \copyright Copyright (C) 2020-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// References to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/n4808.pdf are of the form
/// [parallel] chapter paragraph
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DATA_TYPES_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DATA_TYPES_HPP

#include <score/private/simd/abi.hpp>

#include <score/assert.hpp>
#include <score/bit.hpp>
#include <score/span.hpp>
#include <score/type_traits.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE __attribute__((__always_inline__))

namespace score::cpp
{
namespace simd
{

template <typename T, typename Abi = native_abi<T>, bool IsEnabled = detail::is_abi_tag<Abi>::value>
class basic_vec;

template <typename T, typename Abi = native_abi<T>, bool IsEnabled = detail::is_abi_tag<Abi>::value>
class basic_mask;

struct element_aligned_tag
{
};
struct vector_aligned_tag
{
};
inline constexpr element_aligned_tag element_aligned{};
inline constexpr vector_aligned_tag vector_aligned{};

/// \brief If `value` is present returns `integral_constant<size_t, N>` with `N` identifing the alignment restrictions
/// on pointers used for (converting) loads and stores for the give type `T` on arrays of type `U`
///
/// - `T` shall be a vectorizable type
///
/// [parallel] 9.4 12, 13 and 14
/// \{
template <typename T>
struct alignment
{
};
template <typename T, typename Abi>
struct alignment<basic_vec<T, Abi, true>> : std::integral_constant<std::size_t, alignof(typename Abi::impl::type)>
{
};
template <typename T, typename U = typename T::value_type>
constexpr std::size_t alignment_v{alignment<T>::value};
/// \}

namespace detail
{

/// \brief If `T` is a specialization of the `simd` class template returns `true_type`, and `false_type` otherwise.
///
/// [parallel] 9.4 3 and 4
/// \{
template <typename T>
struct is_basic_vec : std::false_type
{
};
template <typename T, typename Abi>
struct is_basic_vec<basic_vec<T, Abi, true>> : std::true_type
{
};
template <typename T>
constexpr bool is_basic_vec_v{is_basic_vec<T>::value};
/// \}

/// \brief If `T` is a specialization of the `simd_mask` class template returns `true_type`, and `false_type` otherwise.
///
/// [parallel] 9.4 5 and 6
/// \{
template <typename T>
struct is_basic_mask : std::false_type
{
};
template <typename T, typename Abi>
struct is_basic_mask<basic_mask<T, Abi, true>> : std::true_type
{
};
template <typename T>
constexpr bool is_basic_mask_v{is_basic_mask<T>::value};
/// \}

/// \brief If `T` is one of `element_aligned_tag` or `vector_aligned_tag` returns `true_type`, and `false_type`
/// otherwise.
///
/// [parallel] 9.4 7 and 8
/// \{
template <typename T>
struct is_simd_flag_type : std::false_type
{
};
template <>
struct is_simd_flag_type<element_aligned_tag> : std::true_type
{
};
template <>
struct is_simd_flag_type<vector_aligned_tag> : std::true_type
{
};
template <typename T>
constexpr bool is_simd_flag_type_v{is_simd_flag_type<T>::value};
/// \}

/// \brief If `value` is present returns `integral_constant<size_t, N>` with `N` equal to the number of elements
/// in a `simd< T, Abi>` object
///
/// - `T` shall be a vectorizable type
/// - `is_abi_tag<Abi>` is true
///
/// [parallel] 9.4 9, 10 and 11
/// \{
template <typename T>
struct simd_size : std::integral_constant<std::size_t, 0U>
{
};
template <typename T, typename Abi>
struct simd_size<basic_vec<T, Abi, true>> : std::integral_constant<std::size_t, Abi::impl::width>
{
};
template <typename T, typename Abi>
constexpr std::size_t simd_size_v{simd_size<basic_vec<T, Abi>>::value};
/// \}

} // namespace detail

/// \brief The class template `simd_mask` is a data-parallel type with the element type bool.
///
/// `T` must be either an integral type, `std::is_integral_v<T> == true`, or a floating point type,
/// `std::is_floating_point_v<T> == true`.
///
/// A data-parallel type consists of elements of an underlying arithmetic type, called the element type. The number of
/// elements is a constant for each data-parallel type and called the width of that type.
///
/// An element-wise operation applies a specified operation to the elements of one or more data-parallel objects. Each
/// such application is unsequenced with respect to the others. A unary element-wise operation is an element-wise
/// operation that applies a unary operation to each element of a data-parallel object. A binary element-wise operation
/// is an element-wise operation that applies a binary operation to corresponding elements of two data-parallel objects.
///
/// [parallel] 9.8 ff
template <typename T, typename Abi>
class basic_mask<T, Abi, true>
{
    static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "T not a data-parallel type");
    static_assert(detail::is_abi_tag_v<Abi>, "Abi must be an abi tag");

    template <typename U>
    using is_forwarding_ref_overload = std::is_same<basic_mask, score::cpp::remove_cvref_t<U>>;

    template <typename G, typename std::size_t... Is>
    static constexpr bool is_generator_invocable(std::index_sequence<Is...>)
    {
        if constexpr ((... && std::is_invocable_v<G, std::integral_constant<std::size_t, Is>>))
        {
            return (... &&
                    std::is_same_v<bool, decltype(std::declval<G>()(std::integral_constant<std::size_t, Is>()))>);
        }
        return false;
    }

public:
    using value_type = bool;
    using abi_type = Abi;

    /// \brief The number of elements, i.e., the width, of `score::cpp::simd<T, Abi>`.
    ///
    /// [parallel] 9.8.2 ff
    static constexpr std::integral_constant<std::size_t, detail::simd_size_v<T, Abi>> size{};

    /// \brief Default initialize.
    ///
    /// Performs no initialization of the elements. Thus, leaves the elements in an indeterminate state.
    ///
    /// [parallel] 9.8.1 3
    basic_mask() noexcept = default;

    /// \brief Broadcast argument to all elements.
    ///
    /// [parallel] 9.8.3 1
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_mask(const value_type v) noexcept : v_{Abi::mask_impl::broadcast(v)} {}

    /// \brief Constructs an object where the ith element is initialized to `gen(integral_constant<size_t, i>())`.
    ///
    /// [parallel] none
    template <typename G,
              typename = std::enable_if_t<!is_forwarding_ref_overload<G>::value                                //
                                          && is_generator_invocable<G>(std::make_index_sequence<size.value>{}) //
                                          >>
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_mask(G&& gen) noexcept
        : v_{Abi::mask_impl::init(std::forward<G>(gen), std::make_index_sequence<size()>{})}
    {
    }

    /// \brief Convert from platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.8.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_mask(const typename Abi::mask_impl::type v) : v_{v} {}

    /// \brief Convert to platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.8.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator typename Abi::mask_impl::type() const { return v_; }

    /// \brief The value of the ith element.
    ///
    /// @pre i < size()
    ///
    /// [parallel] 9.8.3 4, 5 and 6
    value_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator[](const std::size_t i) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(i < size());
        return Abi::mask_impl::extract(v_, i);
    }

    /// \brief Applies "logical not" to each element.
    ///
    /// [parallel] 9.8.6 1
    basic_mask SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator!() const noexcept { return basic_mask{Abi::mask_impl::logical_not(v_)}; }

    /// \brief Applies "logical and" to each element.
    ///
    /// [parallel] 9.9.1 1
    ///
    /// \note This operator is not short-circuiting and always evaluating lhs and rhs.
    /// \note the order of evaluation of the operands is unspecified in versions prior to C++17.
    friend basic_mask SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator&&(const basic_mask<T, Abi>& lhs,
                                                        const basic_mask<T, Abi>& rhs) noexcept
    {
        using type = typename Abi::mask_impl::type;
        return basic_mask{Abi::mask_impl::logical_and(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Applies "logical or" to each element.
    ///
    /// [parallel] 9.9.1 1
    ///
    /// \note This operator is not short-circuiting and always evaluating lhs and rhs.
    /// \note the order of evaluation of the operands is unspecified in versions prior to C++17.
    friend basic_mask SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator||(const basic_mask& lhs, const basic_mask& rhs) noexcept
    {
        using type = typename Abi::mask_impl::type;
        return basic_mask{Abi::mask_impl::logical_or(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

private:
    typename Abi::mask_impl::type v_;
};

template <typename T, typename Abi>
class basic_mask<T, Abi, false>
{
public:
    using value_type = T;
    using abi_type = Abi;

    basic_mask() = delete;
    basic_mask(const basic_mask&) = delete;
    basic_mask& operator=(const basic_mask&) = delete;
    ~basic_mask() = delete;
};

template <typename T, std::size_t N = detail::simd_size_v<T, native_abi<T>>>
using mask = basic_mask<T, deduce_abi<T, N>>;

/// \brief Returns true if all boolean elements in v are true, false otherwise.
///
/// [parallel] 9.9.4 1
template <typename T, typename Abi>
inline bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const basic_mask<T, Abi>& v) noexcept
{
    return Abi::mask_impl::all_of(static_cast<typename Abi::mask_impl::type>(v));
}

/// \brief Returns true if at least one boolean element in v is true, false otherwise.
///
/// [parallel] 9.9.4 2
template <typename T, typename Abi>
inline bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const basic_mask<T, Abi>& v) noexcept
{
    return Abi::mask_impl::any_of(static_cast<typename Abi::mask_impl::type>(v));
}

/// \brief Returns true if none of the boolean elements in v is true, false otherwise.
///
/// [parallel] 9.9.4 3
template <typename T, typename Abi>
inline bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const basic_mask<T, Abi>& v) noexcept
{
    return Abi::mask_impl::none_of(static_cast<typename Abi::mask_impl::type>(v));
}

/// \brief The class template `simd` is a data-parallel type with the element type `T`.
///
/// `T` must be either an integral type, `std::is_integral_v<T> == true`, or a floating point type,
/// `std::is_floating_point_v<T> == true`.
///
/// A data-parallel type consists of elements of an underlying arithmetic type, called the element type. The number of
/// elements is a constant for each data-parallel type and called the width of that type.
///
/// An element-wise operation applies a specified operation to the elements of one or more data-parallel objects. Each
/// such application is unsequenced with respect to the others. A unary element-wise operation is an element-wise
/// operation that applies a unary operation to each element of a data-parallel object. A binary element-wise operation
/// is an element-wise operation that applies a binary operation to corresponding elements of two data-parallel objects.
///
/// [parallel] 9.6 ff
template <typename T, typename Abi>
class basic_vec<T, Abi, true>
{
    static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "T not a data-parallel type");
    static_assert(detail::is_abi_tag_v<Abi>, "Abi must be an abi tag");

    template <typename U>
    using is_forwarding_ref_overload = std::is_same<basic_vec, score::cpp::remove_cvref_t<U>>;

    template <typename G, typename std::size_t... Is>
    static constexpr bool is_generator_invocable(std::index_sequence<Is...>)
    {
        if constexpr ((... && std::is_invocable_v<G, std::integral_constant<std::size_t, Is>>))
        {
            // standard says "value preserving". currently be more strict with `is_same`
            return (... && std::is_same_v<T, decltype(std::declval<G>()(std::integral_constant<std::size_t, Is>()))>);
        }
        return false;
    }

    /// @brief Used to obtain the iterator type of the type Range.
    template <typename Range>
    using iterator_t = decltype(std::begin(std::declval<Range&>()));

    /// @brief Computes the value type of Iterator.
    template <typename Range>
    using iter_value_t = typename std::iterator_traits<score::cpp::remove_cvref_t<iterator_t<Range>>>::value_type;

public:
    using value_type = T;
    using mask_type = basic_mask<T, Abi>;
    using abi_type = Abi;

    /// \brief The number of elements, i.e., the width, of score::cpp::simd<T, Abi>.
    ///
    /// [parallel] 9.6.2 1
    static constexpr std::integral_constant<std::size_t, detail::simd_size_v<T, Abi>> size{};

    /// \brief Default initialize.
    ///
    /// Performs no initialization of the elements. Thus, leaves the elements in an indeterminate state.
    ///
    /// [parallel] 9.6.1 3
    basic_vec() noexcept = default;

    /// \brief Broadcast argument to all elements.
    ///
    /// \note This constructor does not allow conversion to `value_type` as opposed to standard.
    ///
    /// [parallel] 9.6.4 1 and 2
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_vec(const value_type v) noexcept : v_{Abi::impl::broadcast(v)} {}

    /// \brief Initializes the ith element with static_cast<T>(x[i]) for all i in the range of [0, size()).
    ///
    /// \note This constructor is always explicit even if the conversion is value preserving
    ///
    /// [simd.ctor] 29.10.7.2 (C++26)
    template <typename U, typename UAbi, typename = std::enable_if_t<size() == basic_vec<U, UAbi>::size()>>
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_vec(const basic_vec<U, UAbi>& v) noexcept
        : v_{UAbi::impl::convert(static_cast<typename UAbi::impl::type>(v), value_type{})}
    {
    }

    /// \brief Constructs an object where the ith element is initialized to `gen(integral_constant<size_t, i>())`.
    ///
    /// [parallel] 9.6.4 5, 6 and 7
    template <typename G,
              typename = std::enable_if_t<!is_forwarding_ref_overload<G>::value                                //
                                          && is_generator_invocable<G>(std::make_index_sequence<size.value>{}) //
                                          >>
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_vec(G&& gen) noexcept
        : v_{Abi::impl::init(std::forward<G>(gen), std::make_index_sequence<size()>{})}
    {
    }

    /// \brief Constructs the elements of the simd object from an aligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to score::cpp::simd::alignment_v<basic_vec>.
    ///
    /// [parallel] 9.6.4 8, 9 and 10
    template <typename R,
              typename U = std::remove_const_t<iter_value_t<R>>,
              typename = std::enable_if_t<!is_forwarding_ref_overload<R>::value                //
                                          && std::is_constructible_v<span<const U, size()>, R> //
                                          && std::is_convertible_v<R, span<const U, size()>>   //
                                          >>
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_vec(R&& range, vector_aligned_tag)
        : v_{[](auto&& r) {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((score::cpp::bit_cast<std::uintptr_t>(r.data()) % alignment_v<basic_vec>) == 0U);

            if constexpr (std::is_same_v<value_type, U>)
            {
                return Abi::impl::load_aligned(r.data());
            }
            else
            {
                return native_abi<U>::impl::convert(native_abi<U>::impl::load_aligned(r.data()), value_type{});
            }
        }(std::forward<R>(range))}
    {
        static_assert(std::is_integral_v<U> || std::is_floating_point_v<U>, "U not a vectorizable type");
    }

    /// \brief Constructs the elements of the simd object from an unaligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to alignof(value_type).
    ///
    /// [parallel] 9.6.4 8, 9 and 10
    template <typename R,
              typename U = std::remove_const_t<iter_value_t<R>>,
              typename = std::enable_if_t<!is_forwarding_ref_overload<R>::value                //
                                          && std::is_constructible_v<span<const U, size()>, R> //
                                          && std::is_convertible_v<R, span<const U, size()>>   //
                                          >>
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_vec(R&& range, element_aligned_tag = {})
        : v_{[](auto&& r) {
            if constexpr (std::is_same_v<value_type, U>)
            {
                return Abi::impl::load(r.data());
            }
            else
            {
                return native_abi<U>::impl::convert(native_abi<U>::impl::load(r.data()), value_type{});
            }
        }(std::forward<R>(range))}
    {
        static_assert(std::is_integral_v<U> || std::is_floating_point_v<U>, "U not a vectorizable type");
    }

    /// \brief Convert from platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.6.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_vec(const typename Abi::impl::type& v) noexcept : v_{v} {}

    /// \brief Convert to platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.6.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator typename Abi::impl::type() const { return v_; }

    /// \brief The value of the ith element.
    ///
    /// @pre i < size()
    ///
    /// [parallel] 9.6.6 5 4, 5 and 6
    value_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator[](const std::size_t i) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(i < size());
        return Abi::impl::extract(v_, i);
    }

    /// \brief Same as -1 * *this.
    ///
    /// [parallel] 9.6.7 14
    basic_vec SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-() const noexcept { return basic_vec{Abi::impl::negate(v_)}; }

    /// \brief Addition operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend basic_vec SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator+(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        basic_vec tmp{lhs};
        return tmp += rhs;
    }

    /// \brief Subtraction operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend basic_vec SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        basic_vec tmp{lhs};
        return tmp -= rhs;
    }

    /// \brief Multiplication operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend basic_vec SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator*(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        basic_vec tmp{lhs};
        return tmp *= rhs;
    }

    /// \brief Division operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend basic_vec SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator/(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        basic_vec tmp{lhs};
        return tmp /= rhs;
    }

    /// \brief Addition assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE basic_vec& operator+=(basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        lhs.v_ = Abi::impl::add(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Subtraction assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend basic_vec& SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-=(basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        lhs.v_ = Abi::impl::subtract(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Multiplication assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend basic_vec& SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator*=(basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        lhs.v_ = Abi::impl::multiply(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Division assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend basic_vec& SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator/=(basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        lhs.v_ = Abi::impl::divide(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Returns true if lhs is equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator==(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        using type = typename Abi::impl::type;
        return mask_type{Abi::impl::equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is not equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator!=(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        using type = typename Abi::impl::type;
        return mask_type{Abi::impl::not_equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is less than rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator<(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        using type = typename Abi::impl::type;
        return mask_type{Abi::impl::less_than(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is less than or equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator<=(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        using type = typename Abi::impl::type;
        return mask_type{Abi::impl::less_equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is greater than rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator>(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        using type = typename Abi::impl::type;
        return mask_type{Abi::impl::greater_than(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is greater than or equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator>=(const basic_vec& lhs, const basic_vec& rhs) noexcept
    {
        using type = typename Abi::impl::type;
        return mask_type{Abi::impl::greater_equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

private:
    typename Abi::impl::type v_;
};

template <typename T, typename Abi>
class basic_vec<T, Abi, false>
{
public:
    using value_type = T;
    using mask_type = basic_mask<T, Abi>;
    using abi_type = Abi;

    basic_vec() = delete;
    basic_vec(const basic_vec&) = delete;
    basic_vec& operator=(const basic_vec&) = delete;
    ~basic_vec() = delete;
};

template <typename T, std::size_t N = detail::simd_size_v<T, native_abi<T>>>
using vec = basic_vec<T, deduce_abi<T, N>>;

/// \brief Returns the smaller of a and b. Returns a if one operand is NaN.
///
/// [parallel] 9.7.6 1
template <typename T, typename Abi>
inline basic_vec<T, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const basic_vec<T, Abi>& a, const basic_vec<T, Abi>& b) noexcept
{
    using type = typename Abi::impl::type;
    return basic_vec<T, Abi>{Abi::impl::min(static_cast<type>(a), static_cast<type>(b))};
}

/// \brief Returns the greater of a and b. Returns a if one operand is NaN.
///
/// [parallel] 9.7.6 2
template <typename T, typename Abi>
inline basic_vec<T, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const basic_vec<T, Abi>& a, const basic_vec<T, Abi>& b) noexcept
{
    using type = typename Abi::impl::type;
    return basic_vec<T, Abi>{Abi::impl::max(static_cast<type>(a), static_cast<type>(b))};
}

/// \brief Returns low if v is less than low, high if high is less than v, otherwise v.
///
/// @pre low <= high
///
/// [parallel] 9.7.6 4 and 5
template <typename T, typename Abi>
inline basic_vec<T, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE clamp(const basic_vec<T, Abi>& v,
                                                      const basic_vec<T, Abi>& low,
                                                      const basic_vec<T, Abi>& high)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(all_of(low <= high));
    return score::cpp::simd::min(score::cpp::simd::max(v, low), high);
}

/// \brief The class abstracts the notion of selecting elements of a given object of a data-parallel type.
///
/// [parallel] 9.5 ff
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions) Follows literaly the C++ standard
template <typename M, typename T>
class where_expression
{
    static_assert(detail::is_basic_mask_v<M>, "M not a basic_mask type");
    static_assert(detail::is_basic_vec_v<T>, "T not a basic_vec type");
    static_assert(std::is_same<typename T::mask_type, M>::value, "simd and simd_mask are incompatible");

    using impl = typename T::abi_type::impl;
    using mask_impl = typename T::abi_type::mask_impl;

public:
    /// \brief Use `where()` function to get automatic type deduction for `M` and `T`.
    where_expression(const M& mask, T& value) : m_{mask}, v_{value} {}
    where_expression(const where_expression&) = delete;
    where_expression& operator=(const where_expression&) = delete;

    /// \brief Replace the elements of value with the elements of x for elements where mask is true.
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value + x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator+=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ + std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value - x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ - std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value * x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator*=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ * std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value / x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator/=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ / std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

private:
    const M m_;
    T& v_;
};

/// \brief Select elements of v where the corresponding elements of m are true.
///
/// Usage: `where(mask, value) @ other;`.
///
/// Where `@` denotes one of the operators of `where_expression<>`.
///
/// [parallel] 9.9.5 ff
template <typename T, typename Abi>
inline where_expression<basic_mask<T, Abi>, basic_vec<T, Abi>> where(const typename basic_vec<T, Abi>::mask_type& m,
                                                                     basic_vec<T, Abi>& v) noexcept
{
    return {m, v};
}

/// \brief Stores the elements of the simd object to an aligned range.
///
/// Note: `R` is currently more constrained than the standard requires. It expects `r` be convertible to
/// `span<T, N>{r}` with static_extent `N`, where `N` is `basic_vec<T, Abi>::size()`. The current definition of `span`
/// essentially restricts the value type of `R` to be also `T` and the size of `R` to be a constant expression.
///
/// @pre r shall point to storage aligned to score::cpp::simd::alignment_v<basic_vec>.
///
/// [parallel] 9.6.5 4, 5 and 6
template <typename T,
          typename Abi,
          typename R,
          typename = std::enable_if_t<std::is_constructible_v<span<T, basic_vec<T, Abi>::size()>, R>  //
                                      && std::is_convertible_v<R, span<T, basic_vec<T, Abi>::size()>> //
                                      >>
inline void unchecked_store(const basic_vec<T, Abi>& v, R&& r, vector_aligned_tag)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((score::cpp::bit_cast<std::uintptr_t>(std::data(r)) % alignment_v<basic_vec<T, Abi>>) == 0U);
    Abi::impl::store_aligned(std::data(std::forward<R>(r)), static_cast<typename Abi::impl::type>(v));
}

/// \brief Store the elements of the simd object to an unaligned range.
///
/// Note: `R` is currently more constrained than the standard requires. It expects `r` be convertible to
/// `span<T, N>{r}` with static_extent `N`, where `N` is `basic_vec<T, Abi>::size()`. The current definition of `span`
/// essentially restricts the value type of `R` to be also `T` and the size of `R` to be a constant expression.
///
/// @pre r shall point to storage aligned to alignof(value_type).
///
/// [parallel] 9.6.5 4, 5 and 6
template <typename T,
          typename Abi,
          typename R,
          typename = std::enable_if_t<std::is_constructible_v<span<T, basic_vec<T, Abi>::size()>, R>  //
                                      && std::is_convertible_v<R, span<T, basic_vec<T, Abi>::size()>> //
                                      >>
inline void unchecked_store(const basic_vec<T, Abi>& v, R&& r, element_aligned_tag = {})
{
    Abi::impl::store(std::data(std::forward<R>(r)), static_cast<typename Abi::impl::type>(v));
}

} // namespace simd
} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_DATA_TYPES_HPP
