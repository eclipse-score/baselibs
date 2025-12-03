///
/// \file
/// \copyright Copyright (C) 2016-2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.Optional component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_OPTIONAL_HPP
#define SCORE_LANGUAGE_FUTURECPP_OPTIONAL_HPP

#include <score/private/functional/invoke.hpp>
#include <score/private/memory/construct_at.hpp>
#include <score/private/type_traits/invoke_traits.hpp>
#include <score/private/type_traits/is_expected.hpp>
#include <score/private/type_traits/is_optional.hpp>
#include <score/private/type_traits/remove_cvref.hpp>
#include <score/private/utility/ignore.hpp>
#include <score/private/utility/in_place_t.hpp> // IWYU pragma: export
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <score/assert.hpp>
#include <score/expected.hpp>

namespace score::cpp
{

/// Dispatch type used to construct or assign an optional with an empty state.
struct nullopt_t
{
    struct score_future_cpp_private_token
    {
    };
    constexpr explicit nullopt_t(score_future_cpp_private_token, score_future_cpp_private_token) noexcept {}
};

/// Instance of \a nullopt_t for use with \a optional.
constexpr nullopt_t nullopt{nullopt_t::score_future_cpp_private_token{}, nullopt_t::score_future_cpp_private_token{}};

/// \brief Is a wrapper for representing 'optional' (or 'nullable') objects who may not (yet) contain a valid value.
///
/// Suppose we want to read a parameter which represents some integral value. It is possible that this parameter is not
/// specified and such situation is no error. It is valid to not specify the parameter. Also, suppose that any possible
/// value of a given type is valid, so we cannot just use a dedicated value to represent "not available". In other
/// words, this class enhanced every type with the notion of not being available. Optional objects offer full value
/// semantics, they may be used inside containers. All copies are deep copies, no sharing takes place.
///
/// When you need an \c optional around a reference type, please use
/// \code
/// score::cpp::optional<std::reference_wrapper<T>> optional_reference{};
/// // Set an optional reference:
/// optional_reference = std::ref(some_value);
/// // extract the reference from an optional reference:
/// if(optional_reference.has_value()) {
///   T& ref = optional_reference.value();
///   // or inline (get here is a method of std::reference_wrapper):
///   optional_reference.value().get().some_method();
/// }
/// \endcode
/// It must be ensured, that the reference value stays valid for the whole lifetime of the optional when set.
///
/// See [`std::reference_wrapper`](https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper) and
/// [`std::ref, std::cref`](https://en.cppreference.com/w/cpp/utility/functional/ref).
///
/// See [Why Optional References Didn’t Make It In
/// C++17](https://www.fluentcpp.com/2018/10/05/pros-cons-optional-references/) and [Tip of the Week #163: Passing
/// absl::optional parameters](https://abseil.io/tips/163) for more details.
///
/// \tparam T The type which the class is supposed to wrap around
template <typename T>
class optional
{
public:
    static_assert(!std::is_same<std::decay_t<T>, nullopt_t>::value, "cannot instantiate optional of nullopt_t");
    static_assert(!std::is_reference<T>::value, "cannot instantiate optional with reference, see doxygen comments");

    using value_type = T;

    /// \brief Construct an empty object, i.e. the value is "not available".
    constexpr optional() noexcept : null_state_{}, has_value_{false} {}

    /// \brief Construct an empty object, i.e. the value is "not available".
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr optional(nullopt_t) noexcept : null_state_{}, has_value_{false} {}

    /// \brief Construct an object using direct-initialization.
    template <typename... Args, typename = typename std::enable_if<std::is_constructible<T, Args...>::value>::type>
    constexpr explicit optional(in_place_t, Args&&... args) : null_state_{}, has_value_{false}
    {
        construct(std::forward<Args>(args)...);
    }

    /// \brief Construct from a value.
    ///
    /// Using this constructor, an object is built, that contains the passed value.
    ///
    /// \param other The value to be placed into the newly built object.
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr optional(const value_type& other) : null_state_{}, has_value_{false} { construct(other); }

    /// \brief Construct from a value.
    ///
    /// Using this constructor, an object is built, that contains the passed value.
    ///
    /// \param other The value to be placed into the newly built object.
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr optional(value_type&& other) : null_state_{}, has_value_{false} { construct(std::move(other)); }

    /// \brief Construct from a value.
    ///
    /// Using this constructor, an object is built, that contains the passed value.
    ///
    /// \param other The value to be placed into the newly built object.
    template <typename U = std::remove_cv_t<T>,
              typename = typename std::enable_if<!std::is_same<optional, score::cpp::remove_cvref_t<U>>::value      //
                                                 && !std::is_same<in_place_t, score::cpp::remove_cvref_t<U>>::value //
                                                 && !is_expected<std::decay_t<U>>::value                     //
                                                 && std::is_constructible<T, U>::value                       //
                                                 >::type>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr optional(U&& other) : null_state_{}, has_value_{false}
    {
        construct(std::forward<U>(other));
    }

    /// \brief Copy constructor of optional.
    ///
    /// Using this constructor, an optional is built from another optional.
    ///
    /// \param other The optional to copy from.
    constexpr optional(const optional& other) : null_state_{}, has_value_{false}
    {
        static_assert(std::is_copy_constructible<T>::value, "failed");

        if (other.has_value())
        {
            construct(other.data_);
        }
    }

    /// \brief Move constructor of optional.
    ///
    /// Using this constructor, an optional is built from another optional.
    ///
    /// \param other The optional to copy from.
    constexpr optional(optional&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
        : null_state_{}, has_value_{false}
    {
        static_assert(std::is_move_constructible<T>::value, "failed");

        if (other.has_value())
        {
            construct(std::move(other.data_));
        }
    }

    /// \brief Construct from an expected.
    ///
    /// Using this constructor, an optional is built containing a value if other.has_value else empty
    ///
    /// \param other The value to be placed into the newly built object.
    template <typename E>
    optional(const score::cpp::expected<value_type, E>& other) : null_state_{}, has_value_{false}
    {
        static_assert(std::is_copy_constructible<T>::value, "failed");

        if (other.has_value())
        {
            construct(other.value());
        }
    }

    /// \brief Construct from an expected.
    ///
    /// Using this constructor, an optional is built containing a value if other.has_value else empty
    ///
    /// \param other The value to be placed into the newly built object.
    template <typename E>
    optional(score::cpp::expected<value_type, E>&& other) : null_state_{}, has_value_{false}
    {
        static_assert(std::is_move_constructible<T>::value, "failed");

        if (other.has_value())
        {
            construct(std::move(other).value());
        }
    }

    /// \brief Destruct an optional.
    ///
    /// When the optional holds a value, this value will be destructed as well.
    ~optional()
    {
        if (has_value())
        {
            data_.~value_type();
        }
    }

    /// \brief Assign a null-value, same as calling \a reset().
    ///
    /// Reset the optional to an empty state.
    optional& operator=(nullopt_t) noexcept
    {
        reset();
        return *this;
    }

    /// \brief Assigns from a value.
    ///
    /// Replaces contents of *this with the contents of the passed value.
    ///
    /// \param other The value to be placed into the newly built object.
    template <
        typename U = std::remove_cv_t<T>,
        typename = typename std::enable_if<!std::is_same<optional, score::cpp::remove_cvref_t<U>>::value                     //
                                           && std::is_constructible<T, U>::value && std::is_assignable<T&, U>::value  //
                                           && (!std::is_scalar<T>::value || !std::is_same<std::decay_t<U>, T>::value) //
                                           && !is_expected<std::decay_t<U>>::value                                    //
                                           >::type>
    optional& operator=(U&& other)
    {
        if (this->has_value())
        {
            data_ = std::forward<U>(other);
        }
        else
        {
            construct(std::forward<U>(other));
        }
        return *this;
    }

    /// \brief Assigns from another optional.
    ///
    /// Using this assignment operator, an optional is built containing a value if other.has_value else empty
    ///
    /// \param other Containing the value to be placed into the newly built object.
    optional& operator=(const optional& other)
    {
        // currently downstream code doesn't compile if `is_move_assignable` is checked. for now not an
        // issue because our code currently always moves construct and doesn't implement the table from
        // https://en.cppreference.com/w/cpp/utility/optional/operator%3D
        // issue: broken_link_g/swh/amp/issues/385
        static_assert(std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value, "failed");

        if (other.has_value())
        {
            if (this->has_value())
            { // should be: data_ = *other;
                if (this != &other)
                {
                    reset();
                    construct(*other);
                }
            }
            else
            {
                construct(*other);
            }
        }
        else
        {
            reset();
        }
        return *this;
    }

    /// \brief Assigns from another optional.
    ///
    /// Using this assignment operator, an optional is built containing a value if other.has_value else empty
    ///
    /// \param other Containing the value to be placed into the newly built object.
    optional& operator=(optional&& other) noexcept(
        std::is_nothrow_move_assignable<T>::value&& std::is_nothrow_move_constructible<T>::value)
    {
        // currently downstream code doesn't compile if `is_move_assignable` is checked. for now not an
        // issue because our code currently always moves construct and doesn't implement the table from
        // https://en.cppreference.com/w/cpp/utility/optional/operator%3D
        // issue: broken_link_g/swh/amp/issues/385
        static_assert(std::is_move_constructible<T>::value /*&& std::is_move_assignable<T>::value*/, "failed");

        if (other.has_value())
        {
            if (this->has_value())
            { // should be: data_ = std::move(*other);
                if (this != &other)
                {
                    reset();
                    construct(std::move(*other));
                }
            }
            else
            {
                construct(std::move(*other));
            }
        }
        else
        {
            reset();
        }
        return *this;
    }

    /// \brief Assigns from an expected.
    ///
    /// Using this assignment operator, an optional is built containing a value if other.has_value else empty
    ///
    /// \param other Containing the value to be placed into the newly built object.
    template <typename E>
    optional& operator=(const score::cpp::expected<value_type, E>& other)
    {
        static_assert(std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value, "failed");

        reset();
        if (other.has_value())
        {
            construct(other.value());
        }
        return *this;
    }

    /// \brief Assigns from an expected.
    ///
    /// Using this assignment operator, an optional is built containing a value if other.has_value else empty
    ///
    /// \param other Containing the value to be placed into the newly built object.
    template <typename E>
    optional& operator=(score::cpp::expected<value_type, E>&& other)
    {
        static_assert(std::is_move_constructible<T>::value && std::is_move_assignable<T>::value, "failed");

        reset();
        if (other.has_value())
        {
            construct(other.value());
        }
        return *this;
    }

    /// \brief Constructs the contained value in-place
    ///
    /// \warning If *this already contains a value it is destroyed
    ///
    /// \param args Arguments for constructing the value in-place
    /// \return A reference to the constructed value.
    template <typename... Args>
    value_type& emplace(Args&&... args)
    {
        reset();
        construct(std::forward<Args>(args)...);
        return data_;
    }

    /// \brief Accessor for the value
    ///
    /// \note Calling this function on an empty object calls abort instead of throwing std::bad_optional_access.
    ///
    /// The dereference operator operator*() does not check if this optional contains a value, which may be more
    /// efficient than value().
    ///
    /// \return A constant reference to the value.
    constexpr const value_type& value() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return data_;
    }

    /// \brief Accessor for the value
    ///
    /// \note Calling this function on an empty object calls abort instead of throwing std::bad_optional_access.
    ///
    /// The dereference operator operator*() does not check if this optional contains a value, which may be more
    /// efficient than value().
    ///
    /// \return A reference to the value.
    value_type& value()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return data_;
    }

    /// \brief Returns a reference to the contained value.
    ///
    /// \pre has_value() == true
    ///
    /// This operator does not check whether the optional contains a value! You can do so manually by using has_value()
    /// or simply operator bool(). Alternatively, if checked access is needed, value() or value_or() may be used.
    ///
    /// \return Reference to the contained value.
    constexpr const value_type& operator*() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(has_value());
        return data_;
    }

    /// \brief Returns a reference to the contained value.
    ///
    /// \pre has_value() == true
    ///
    /// This operator does not check whether the optional contains a value! You can do so manually by using has_value()
    /// or simply operator bool(). Alternatively, if checked access is needed, value() or value_or() may be used.
    ///
    /// \return Reference to the contained value.
    value_type& operator*()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(has_value());
        return data_;
    }

    /// \brief Returns a pointer to the contained value.
    ///
    /// \pre has_value() == true
    ///
    /// This operator does not check whether the optional contains a value! You can do so manually by using has_value()
    /// or simply operator bool(). Alternatively, if checked access is needed, value() or value_or() may be used.
    ///
    /// \return Pointer to the contained value.
    constexpr const value_type* operator->() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(has_value());
        return &data_;
    }

    /// \brief Returns a pointer to the contained value.
    ///
    /// \pre has_value() == true
    ///
    /// This operator does not check whether the optional contains a value! You can do so manually by using has_value()
    /// or simply operator bool(). Alternatively, if checked access is needed, value() or value_or() may be used.
    ///
    /// \return Pointer to the contained value.
    value_type* operator->()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(has_value());
        return &data_;
    }

    /// \brief Safe version of the direct value accessors. Uses provided default value, if internal value is not
    /// available.
    ///
    /// \note As opposed to \sa value, the function returns a copy. To prevent editing a temporary value and thereby
    /// possibly introducing a bug, is was made constant.
    ///
    /// \sa value
    /// \return The internal value if available, otherwise return the provided default
    constexpr const value_type value_or(const value_type& value) const
    {
        return this->has_value() ? this->value() : value;
    }

    /// \brief If *this contains a value, destroy that value as if by value().T::~T(). Otherwise, there are no
    /// effects.
    ///
    /// *this does not contain a value after this call.
    void reset()
    {
        if (has_value())
        {
            data_.~value_type();
            has_value_ = false;
        }
    }

    /// \brief Checks whether *this contains a value.
    ///
    /// \return true if *this contains a value, false if *this does not contain a value.
    constexpr bool has_value() const { return has_value_; }

    /// \brief Checks whether *this contains a value.
    ///
    /// \return true if *this contains a value, false if *this does not contain a value.
    constexpr explicit operator bool() const { return has_value(); }

    /// \brief If *this contains a value, invokes f with the contained value as an argument, and returns the result of
    /// that invocation; otherwise, returns an empty score::cpp::optional.
    ///
    /// \param f A suitable function or Callable object that returns an score::cpp::optional.
    /// \return The result of f or an empty score::cpp::optional, as described above.
    /// \{
    template <typename F>
    constexpr auto and_then(F&& f) &
    {
        using U = typename score::cpp::remove_cvref_t<score::cpp::invoke_result_t<F, decltype(**this)>>;

        static_assert(score::cpp::is_optional_v<U>, "The callable passed to and_then must return an score::cpp::optional");

        return has_value() ? score::cpp::detail::invoke(std::forward<F>(f), **this) : U{};
    }
    template <typename F>
    constexpr auto and_then(F&& f) const&
    {
        using U = typename score::cpp::remove_cvref_t<score::cpp::invoke_result_t<F, decltype(**this)>>;

        static_assert(score::cpp::is_optional_v<U>, "The callable passed to and_then must return an score::cpp::optional");

        return has_value() ? score::cpp::detail::invoke(std::forward<F>(f), **this) : U{};
    }
    template <typename F>
    constexpr auto and_then(F&& f) &&
    {
        using U = typename score::cpp::remove_cvref_t<score::cpp::invoke_result_t<F, decltype(std::move(**this))>>;

        static_assert(score::cpp::is_optional_v<U>, "The callable passed to and_then must return an score::cpp::optional");

        return has_value() ? score::cpp::detail::invoke(std::forward<F>(f), std::move(**this)) : U{};
    }
    template <typename F>
    constexpr auto and_then(F&& f) const&&
    {
        using U = typename score::cpp::remove_cvref_t<score::cpp::invoke_result_t<F, decltype(std::move(**this))>>;

        static_assert(score::cpp::is_optional_v<U>, "The callable passed to and_then must return an score::cpp::optional");

        return has_value() ? score::cpp::detail::invoke(std::forward<F>(f), std::move(**this)) : U{};
    }
    /// \}

    /// \brief Returns *this if it contains a value. Otherwise, returns the result of f.
    ///
    /// \param f A function or callable object that returns an score::cpp::optional<T>.
    /// \return *this or the result of f, as described above.
    /// \{
    template <typename F,
              typename = std::enable_if_t<score::cpp::is_invocable<F>::value && std::is_copy_constructible<T>::value>>
    constexpr optional or_else(F&& f) const&
    {
        static_assert(std::is_same<score::cpp::remove_cvref_t<score::cpp::invoke_result_t<F>>, optional>::value,
                      "The callable passed to or_else must return the same optional specialization");

        return has_value() ? *this : std::forward<F>(f)();
    }
    template <typename F,
              typename = std::enable_if_t<score::cpp::is_invocable<F>::value && std::is_move_constructible<T>::value>>
    constexpr optional or_else(F&& f) &&
    {
        static_assert(std::is_same<score::cpp::remove_cvref_t<score::cpp::invoke_result_t<F>>, optional>::value,
                      "The callable passed to or_else must return the same optional specialization");

        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }
    /// \}

    /// \brief If *this contains a value, invokes f with the contained value as an argument, and returns an
    /// score::cpp::optional that contains the result of that invocation; otherwise, returns an empty score::cpp::optional.
    /// \{
    template <typename F>
    constexpr auto transform(F&& f) &
    {
        using U = std::remove_cv_t<score::cpp::invoke_result_t<F, decltype(**this)>>;

        static_assert(!std::is_same<U, score::cpp::in_place_t>::value,
                      "The callable passed to transform must not return score::cpp::in_place_t");
        static_assert(!std::is_same<U, score::cpp::nullopt_t>::value,
                      "The callable passed to transform must not return score::cpp::nullopt_t");
        static_assert(!std::is_array<U>::value, "The callable passed to transform must not return an array");
        static_assert(
            std::is_constructible<U, score::cpp::invoke_result_t<F, decltype(**this)>>::value,
            "The declaration U u(invoke(std::forward<F>(f), *val)); must be well-formed for some invented variable u");

        return has_value() ? optional<U>{score::cpp::detail::invoke(std::forward<F>(f), **this)} : optional<U>{};
    }
    template <typename F>
    constexpr auto transform(F&& f) const&
    {
        using U = std::remove_cv_t<score::cpp::invoke_result_t<F, decltype(**this)>>;

        static_assert(!std::is_same<U, score::cpp::in_place_t>::value,
                      "The callable passed to transform must not return score::cpp::in_place_t");
        static_assert(!std::is_same<U, score::cpp::nullopt_t>::value,
                      "The callable passed to transform must not return score::cpp::nullopt_t");
        static_assert(!std::is_array<U>::value, "The callable passed to transform must not return an array");
        static_assert(
            std::is_constructible<U, score::cpp::invoke_result_t<F, decltype(**this)>>::value,
            "The declaration U u(invoke(std::forward<F>(f), *val)); must be well-formed for some invented variable u");

        return has_value() ? optional<U>{score::cpp::detail::invoke(std::forward<F>(f), **this)} : optional<U>{};
    }
    template <typename F>
    constexpr auto transform(F&& f) &&
    {
        using U = std::remove_cv_t<score::cpp::invoke_result_t<F, decltype(std::move(**this))>>;

        static_assert(!std::is_same<U, score::cpp::in_place_t>::value,
                      "The callable passed to transform must not return score::cpp::in_place_t");
        static_assert(!std::is_same<U, score::cpp::nullopt_t>::value,
                      "The callable passed to transform must not return score::cpp::nullopt_t");
        static_assert(!std::is_array<U>::value, "The callable passed to transform must not return an array");
        static_assert(std::is_constructible<U, score::cpp::invoke_result_t<F, decltype(std::move(**this))>>::value,
                      "The declaration U u(invoke(std::forward<F>(f), std::move(*val))); must be well-formed for some "
                      "invented variable u");

        return has_value() ? optional<U>{score::cpp::detail::invoke(std::forward<F>(f), std::move(**this))} : optional<U>{};
    }
    template <typename F>
    constexpr auto transform(F&& f) const&&
    {
        using U = std::remove_cv_t<score::cpp::invoke_result_t<F, decltype(std::move(**this))>>;

        static_assert(!std::is_same<U, score::cpp::in_place_t>::value,
                      "The callable passed to transform must not return score::cpp::in_place_t");
        static_assert(!std::is_same<U, score::cpp::nullopt_t>::value,
                      "The callable passed to transform must not return score::cpp::nullopt_t");
        static_assert(!std::is_array<U>::value, "The callable passed to transform must not return an array");
        static_assert(std::is_constructible<U, score::cpp::invoke_result_t<F, decltype(std::move(**this))>>::value,
                      "The declaration U u(invoke(std::forward<F>(f), std::move(*val))); must be well-formed for some "
                      "invented variable u");

        return has_value() ? optional<U>{score::cpp::detail::invoke(std::forward<F>(f), std::move(**this))} : optional<U>{};
    }
    // \}

private:
    template <class... Args>
    void construct(Args&&... args)
    {
        score::cpp::ignore = score::cpp::detail::construct_at(std::addressof(data_), std::forward<Args>(args)...);
        has_value_ = true;
    }

    // For performances reason this class is not using `std::variant` but implementing the storage as a raw union. This
    // class takes care by code review to only read from the active member of the union. The union is only visible
    // internally and not exposed to the user API.
    // coverity[misra_cpp_2023_rule_12_3_1_violation : SUPPRESS]
    union
    {
        // NOLINTNEXTLINE(readability-identifier-naming) keep `_` to make clear it is a member variable
        char null_state_;
        // NOLINTNEXTLINE(readability-identifier-naming) keep `_` to make clear it is a member variable
        std::remove_cv_t<value_type> data_;
    };
    bool has_value_;
};

template <typename U>
bool operator==(const optional<U>& lhs, const optional<U>& rhs)
{
    return (!lhs.has_value() && !rhs.has_value()) ||
           ((lhs.has_value() && rhs.has_value()) && (lhs.value() == rhs.value()));
}

template <typename U>
bool operator!=(const optional<U>& lhs, const optional<U>& rhs)
{
    return !(lhs == rhs);
}

template <typename U>
bool operator<(const optional<U>& lhs, const optional<U>& rhs)
{
    return (lhs.has_value() && rhs.has_value() && (lhs.value() < rhs.value())) || (!lhs.has_value() && rhs.has_value());
}

template <typename U>
bool operator<=(const optional<U>& lhs, const optional<U>& rhs)
{
    return !(rhs < lhs);
}

template <typename U>
bool operator>(const optional<U>& lhs, const optional<U>& rhs)
{
    return rhs < lhs;
}

template <typename U>
bool operator>=(const optional<U>& lhs, const optional<U>& rhs)
{
    return !(lhs < rhs);
}

template <typename U>
bool operator==(const optional<U>& lhs, score::cpp::nullopt_t)
{
    return !lhs.has_value();
}

template <typename U>
bool operator==(score::cpp::nullopt_t, const optional<U>& rhs)
{
    return !rhs.has_value();
}

template <typename U>
bool operator!=(const optional<U>& lhs, score::cpp::nullopt_t)
{
    return lhs.has_value();
}

template <typename U>
bool operator!=(score::cpp::nullopt_t, const optional<U>& rhs)
{
    return rhs.has_value();
}

template <typename U>
bool operator<(const score::cpp::optional<U>&, score::cpp::nullopt_t)
{
    return false;
}

template <typename U>
bool operator<(score::cpp::nullopt_t, const score::cpp::optional<U>& rhs)
{
    return rhs.has_value();
}

template <typename U>
bool operator<=(const score::cpp::optional<U>& lhs, score::cpp::nullopt_t)
{
    return !lhs.has_value();
}

template <typename U>
bool operator<=(score::cpp::nullopt_t, const score::cpp::optional<U>&)
{
    return true;
}

template <typename U>
bool operator>(const score::cpp::optional<U>& lhs, score::cpp::nullopt_t)
{
    return lhs.has_value();
}

template <typename U>
bool operator>(score::cpp::nullopt_t, const score::cpp::optional<U>&)
{
    return false;
}

template <typename U>
bool operator>=(const score::cpp::optional<U>&, score::cpp::nullopt_t)
{
    return true;
}

template <typename U>
bool operator>=(score::cpp::nullopt_t, const score::cpp::optional<U>& rhs)
{
    return !rhs.has_value();
}

template <typename T, typename U>
bool operator==(const optional<T>& lhs, const U& rhs)
{
    return lhs.has_value() ? (*lhs == rhs) : false;
}

template <typename T, typename U>
bool operator==(const T& lhs, const optional<U>& rhs)
{
    return rhs.has_value() ? (lhs == *rhs) : false;
}

template <typename T, typename U>
bool operator!=(const optional<T>& lhs, const U& rhs)
{
    return lhs.has_value() ? (*lhs != rhs) : true;
}

template <typename T, typename U>
bool operator!=(const T& lhs, const optional<U>& rhs)
{
    return rhs.has_value() ? (lhs != *rhs) : true;
}

template <typename T, typename U>
bool operator<(const optional<T>& lhs, const U& rhs)
{
    return lhs.has_value() ? (*lhs < rhs) : true;
}

template <typename T, typename U>
bool operator<(const T& lhs, const optional<U>& rhs)
{
    return rhs.has_value() ? (lhs < *rhs) : false;
}

template <typename T, typename U>
bool operator<=(const optional<T>& lhs, const U& rhs)
{
    return lhs.has_value() ? (*lhs <= rhs) : true;
}

template <typename T, typename U>
bool operator<=(const T& lhs, const optional<U>& rhs)
{
    return rhs.has_value() ? (lhs <= *rhs) : false;
}

template <typename T, typename U>
bool operator>(const optional<T>& lhs, const U& rhs)
{
    return lhs.has_value() ? (*lhs > rhs) : false;
}

template <typename T, typename U>
bool operator>(const T& lhs, const optional<U>& rhs)
{
    return rhs.has_value() ? (lhs > *rhs) : true;
}

template <typename T, typename U>
bool operator>=(const optional<T>& lhs, const U& rhs)
{
    return lhs.has_value() ? (*lhs >= rhs) : false;
}

template <typename T, typename U>
bool operator>=(const T& lhs, const optional<U>& rhs)
{
    return rhs.has_value() ? (lhs >= *rhs) : true;
}

/// \brief Creates an optional object from its arguments
///
/// For general documentation, please consult the C++ Standard
///
/// \details Albeit compliant with C++17 the `constexpr` is ineffective on make_optional. We put it for full interface
/// compatibility with C++17. Users are informed that attempting to call make_optional in contexts where compile-time
/// evaluation is targeted will break this evaluation; not because of `make_optional` but because score::cpp::optional not
/// being ready for it (see broken_link_g/swh/amp/issues/1530).
///
/// \see https://en.cppreference.com/w/cpp/utility/optional/make_optional

/// \{
/// \tparam Type The type of the underlying data
/// \param value The value if the underlying data
/// \return The optional filled with the given value
template <typename Type>
constexpr score::cpp::optional<std::decay_t<Type>> make_optional(Type&& value)
{
    return optional<std::decay_t<Type>>{std::forward<Type>(value)};
}

/// \tparam Type The type of the underlying data
/// \tparam Args The variadic pack that will be forwarded to construct type of the underlying data
/// \param args The arguments for constructing underlying data
/// \return The optional constructed in_place with the forwarded args
template <typename Type, typename... Args>
constexpr score::cpp::optional<Type> make_optional(Args&&... args)
{
    return optional<Type>(score::cpp::in_place, std::forward<Args>(args)...);
}

/// \tparam Type The type of the underlying data
/// \tparam ListType The type of the initializer_list for constructing the underlying data
/// \tparam Args The variadic pack that will be forwarded as construct type of the underlying data
/// \param il The initializer_list for constructing underlying data
/// \param args The arguments to be forwarded to for constructing underlying data
/// \return The optional constructed in_place with the initializer_list and forwarded args
template <typename Type, typename ListType, typename... Args>
constexpr score::cpp::optional<Type> make_optional(std::initializer_list<ListType> il, Args&&... args)
{
    return optional<Type>(score::cpp::in_place, il, std::forward<Args>(args)...);
}
/// \}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_OPTIONAL_HPP
