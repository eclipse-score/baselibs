/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Expected component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_EXPECTED_HPP
#define SCORE_LANGUAGE_FUTURECPP_EXPECTED_HPP

#include <score/private/type_traits/remove_cvref.hpp>
#include <score/assert.hpp>
#include <score/blank.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{

///
/// \brief A container for an unexpected value.
///
/// \tparam E the type of the \ref unexpected value that this class holds.
///
/// \note Intended to be used as through the \ref make_unexpected function.
///
template <typename E>
class unexpected
{
public:
    /// \brief Constructor from error value.
    explicit unexpected(const E& error) noexcept(std::is_nothrow_copy_constructible<E>::value) : error_{error} {}
    /// \brief Constructor from error rvalue.
    explicit unexpected(E&& error) noexcept(std::is_nothrow_move_constructible<E>::value) : error_{std::move(error)} {}
    /// \brief Getter for an error.
    const E& error() const& noexcept { return error_; }
    /// \brief Getter for an error.
    E& error() & noexcept { return error_; }
    /// \brief Getter for an error.
    E&& error() && noexcept { return std::move(error_); }
    /// \brief Getter for an error.
    const E&& error() const&& noexcept { return std::move(error_); }

private:
    E error_;
};

///
/// \brief A helper function to create an \ref unexpected instance.
///
/// \tparam E the type of the \ref unexpected value.
///
/// \return an instance of unexpected with the error forwarder to it.
///
template <typename E>
unexpected<std::decay_t<E>>
make_unexpected(E&& e) noexcept((std::is_rvalue_reference<E>::value && std::is_nothrow_move_constructible<E>::value) ||
                                (std::is_lvalue_reference<E>::value && std::is_nothrow_copy_constructible<E>::value))
{
    return unexpected<std::decay_t<E>>(std::forward<E>(e));
}

namespace detail
{
namespace expected
{

template <typename T, typename E, bool = std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<E>>
struct destructor_base
{
    static_assert(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<E>);

    destructor_base() {}

    // For performances reason this class is not using `std::variant` but implementing the storage as a raw union. This
    // class takes care by code review to only read from the active member of the union. The union is only visible
    // internally and not exposed to the user API.
    // coverity[misra_cpp_2023_rule_12_3_1_violation : SUPPRESS]
    union
    {
        /// Stored value.
        T value_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
        /// Stored error.
        E error_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
    };
    /// Indicator if this instance holds a value.
    bool has_value_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
};

template <typename T, typename E>
struct destructor_base<T, E, false>
{
    static_assert(!(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<E>));

    destructor_base() {}
    destructor_base(const destructor_base&) = default;
    destructor_base& operator=(const destructor_base&) = default;
    destructor_base(destructor_base&&) = default;
    destructor_base& operator=(destructor_base&&) = default;

    ~destructor_base() noexcept(std::is_nothrow_destructible<T>::value && std::is_nothrow_destructible<E>::value)
    {
        if (has_value_)
        {
            value_.~T();
        }
        else
        {
            error_.~E();
        }
    }

    // For performances reason this class is not using `std::variant` but implementing the storage as a raw union. This
    // class takes care by code review to only read from the active member of the union. The union is only visible
    // internally and not exposed to the user API.
    // coverity[misra_cpp_2023_rule_12_3_1_violation : SUPPRESS]
    union
    {
        /// Stored value.
        T value_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
        /// Stored error.
        E error_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
    };
    /// Indicator if this instance holds a value.
    bool has_value_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
};

template <typename T,
          typename E,
          bool = std::is_trivially_copy_constructible_v<T> && std::is_trivially_copy_constructible_v<E>>
struct copy_base : public destructor_base<T, E>
{
    static_assert(std::is_trivially_copy_constructible_v<T> && std::is_trivially_copy_constructible_v<E>);
};

template <typename T, typename E>
class copy_base<T, E, false> : public destructor_base<T, E>
{
    static_assert(!(std::is_trivially_copy_constructible_v<T> && std::is_trivially_copy_constructible_v<E>));

public:
    copy_base() = default;
    copy_base& operator=(const copy_base&) = default;
    copy_base(copy_base&&) = default;
    copy_base& operator=(copy_base&&) = default;
    ~copy_base() = default;

    copy_base(const copy_base& rhs) : destructor_base<T, E>{}
    {
        static_assert(std::is_copy_constructible<T>::value && std::is_copy_constructible<E>::value);

        this->has_value_ = rhs.has_value_;
        if (rhs.has_value_)
        {
            static_cast<void>(::new (&this->value_) T{rhs.value_});
        }
        else
        {
            static_cast<void>(::new (&this->error_) E{rhs.error_});
        }
    }
};

template <typename T,
          typename E,
          bool = std::is_trivially_destructible_v<T>          //
                 && std::is_trivially_copy_constructible_v<T> //
                 && std::is_trivially_copy_assignable_v<T>    //
                 && std::is_trivially_destructible_v<E>       //
                 && std::is_trivially_copy_constructible_v<E> //
                 && std::is_trivially_copy_assignable_v<E>>
struct copy_assign_base : public copy_base<T, E>
{
    static_assert(std::is_trivially_destructible_v<T>          //
                  && std::is_trivially_copy_constructible_v<T> //
                  && std::is_trivially_copy_assignable_v<T>    //
                  && std::is_trivially_destructible_v<E>       //
                  && std::is_trivially_copy_constructible_v<E> //
                  && std::is_trivially_copy_assignable_v<E>);
};

template <typename T, typename E>
class copy_assign_base<T, E, false> : public copy_base<T, E>
{
    static_assert(!(std::is_trivially_destructible_v<T>          //
                    && std::is_trivially_copy_constructible_v<T> //
                    && std::is_trivially_copy_assignable_v<T>    //
                    && std::is_trivially_destructible_v<E>       //
                    && std::is_trivially_copy_constructible_v<E> //
                    && std::is_trivially_copy_assignable_v<E>));

public:
    copy_assign_base() = default;
    copy_assign_base(const copy_assign_base&) = default;
    copy_assign_base(copy_assign_base&&) = default;
    copy_assign_base& operator=(copy_assign_base&&) = default;
    ~copy_assign_base() = default;

    copy_assign_base& operator=(const copy_assign_base& rhs)
    {
        static_assert(std::is_copy_assignable<T>::value       //
                      && std::is_copy_constructible<T>::value //
                      && std::is_copy_assignable<E>::value    //
                      && std::is_copy_constructible<E>::value //
                      &&
                      (std::is_nothrow_move_constructible<T>::value || std::is_nothrow_move_constructible<E>::value));

        if (this->has_value_)
        {
            if (rhs.has_value_)
            {
                this->value_ = rhs.value_;
            }
            else
            {
                this->value_.~T();
                static_cast<void>(::new (&this->error_) E{rhs.error_});
                this->has_value_ = false;
            }
        }
        else
        {
            if (rhs.has_value_)
            {
                this->error_.~E();
                static_cast<void>(::new (&this->value_) T{rhs.value_});
                this->has_value_ = true;
            }
            else
            {
                this->error_ = rhs.error_;
            }
        }
        return *this;
    }
};

template <typename T,
          typename E,
          bool = std::is_trivially_move_constructible_v<T> && std::is_trivially_move_constructible_v<E>>
struct move_base : public copy_assign_base<T, E>
{
    static_assert(std::is_trivially_move_constructible_v<T> && std::is_trivially_move_constructible_v<E>);
};

template <typename T, typename E>
class move_base<T, E, false> : public copy_assign_base<T, E>
{
    static_assert(!(std::is_trivially_move_constructible_v<T> && std::is_trivially_move_constructible_v<E>));

public:
    move_base() = default;
    move_base(const move_base&) = default;
    move_base& operator=(const move_base&) = default;
    move_base& operator=(move_base&&) = default;
    ~move_base() = default;

    template <
        typename U = T,
        typename F = E,
        std::enable_if_t<std::is_move_constructible<U>::value && std::is_move_constructible<F>::value, bool> = true>
    move_base(move_base&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                        std::is_nothrow_move_constructible<E>::value)
        : copy_assign_base<T, E>{}
    {
        this->has_value_ = rhs.has_value_;
        if (rhs.has_value_)
        {
            static_cast<void>(::new (&this->value_) T{std::move(rhs).value_});
        }
        else
        {
            static_cast<void>(::new (&this->error_) E{std::move(rhs).error_});
        }
    }
};

template <typename T,
          typename E,
          bool = std::is_trivially_destructible_v<T>          //
                 && std::is_trivially_move_constructible_v<T> //
                 && std::is_trivially_move_assignable_v<T>    //
                 && std::is_trivially_destructible_v<E>       //
                 && std::is_trivially_move_constructible_v<E> //
                 && std::is_trivially_move_assignable_v<E>>
struct move_assign_base : public move_base<T, E>
{
    static_assert(std::is_trivially_destructible_v<T>          //
                  && std::is_trivially_move_constructible_v<T> //
                  && std::is_trivially_move_assignable_v<T>    //
                  && std::is_trivially_destructible_v<E>       //
                  && std::is_trivially_move_constructible_v<E> //
                  && std::is_trivially_move_assignable_v<E>);
};

template <typename T, typename E>
class move_assign_base<T, E, false> : public move_base<T, E>
{
    static_assert(!(std::is_trivially_destructible_v<T>          //
                    && std::is_trivially_move_constructible_v<T> //
                    && std::is_trivially_move_assignable_v<T>    //
                    && std::is_trivially_destructible_v<E>       //
                    && std::is_trivially_move_constructible_v<E> //
                    && std::is_trivially_move_assignable_v<E>));

public:
    move_assign_base() = default;
    move_assign_base(const move_assign_base&) = default;
    move_assign_base& operator=(const move_assign_base&) = default;
    move_assign_base(move_assign_base&&) = default;
    ~move_assign_base() = default;

    template <typename U = T,
              typename F = E,
              std::enable_if_t<std::is_move_constructible<U>::value        //
                                   && std::is_move_assignable<U>::value    //
                                   && std::is_move_constructible<F>::value //
                                   && std::is_move_assignable<F>::value    //
                                   && (std::is_nothrow_move_constructible<U>::value ||
                                       std::is_nothrow_move_constructible<F>::value) //
                               ,
                               bool> = true>
    move_assign_base& operator=(move_assign_base&& rhs) noexcept(std::is_nothrow_move_assignable<T>::value &&
                                                                 std::is_nothrow_move_constructible<T>::value &&
                                                                 std::is_nothrow_move_assignable<E>::value &&
                                                                 std::is_nothrow_move_constructible<E>::value &&
                                                                 std::is_nothrow_destructible<T>::value &&
                                                                 std::is_nothrow_destructible<E>::value)
    {
        if (this->has_value_)
        {
            if (rhs.has_value_)
            {
                this->value_ = std::move(rhs).value_;
            }
            else
            {
                this->value_.~T();
                static_cast<void>(::new (&this->error_) E{std::move(rhs).error_});
                this->has_value_ = false;
            }
        }
        else
        {
            if (rhs.has_value_)
            {
                this->error_.~E();
                static_cast<void>(::new (&this->value_) T{std::move(rhs).value_});
                this->has_value_ = true;
            }
            else
            {
                this->error_ = std::move(rhs).error_;
            }
        }
        return *this;
    }
};

} // namespace expected
} // namespace detail

///
/// \brief A container for an expected value or an error.
///
/// An instance of score::cpp::expected can hold either a value or an error.
///
/// \tparam T the type of value that this class holds.
/// \tparam E the type of error that this class holds.
///
/// \note This implementation follows loosely Andrei Alexandrescu's take on expected.
///
template <typename T, typename E>
class expected : private detail::expected::move_assign_base<T, E>
{
    using base_t = detail::expected::move_assign_base<T, E>;

    ///
    /// \brief A compile time evaluator to decide whether the universal reference ctor is enabled
    ///
    /// \tparam U as evaluation criterion for the expression
    ///
    /// \note http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0323r7.html#expected.object.ctor
    ///
    template <typename U>
    static constexpr bool enable_universal_reference_ctor =
        !std::is_void<T>::value && std::is_constructible<T, U&&>::value &&
        !std::is_same<expected<T, E>, score::cpp::remove_cvref_t<U>>::value &&
        !std::is_same<unexpected<E>, score::cpp::remove_cvref_t<U>>::value;

public:
    ///
    /// \typedef value_type
    ///
    /// \brief Alias for the type of the expected value
    ///
    using value_type = T;

    ///
    /// \typedef error_type
    ///
    /// \brief Alias for the type of the unexpected value
    ///
    using error_type = E;

    /// \brief Default constructor. The instance is assumed to have a value.
    template <typename U = T, typename = std::enable_if_t<std::is_default_constructible<U>::value>>
    expected() noexcept(std::is_nothrow_default_constructible<T>::value) : base_t{}
    {
        this->has_value_ = true;
        static_cast<void>(::new (&this->value_) T{});
    }

    ///
    /// \brief Construct from an universal reference of U.
    ///
    /// \tparam U defaulted to T for usage as universal/forwarding reference.
    ///
    /// \param rhs a value from which we construct the instance of \ref expected.
    ///
    /// \note Explicit overload since U&& is not convertible to T
    ///
    template <
        typename U = T,
        typename = typename std::enable_if_t<enable_universal_reference_ctor<U> && !std::is_convertible<U&&, T>::value>>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    explicit constexpr expected(U&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value) : base_t{}
    {
        this->has_value_ = true;
        static_cast<void>(::new (&this->value_) T{std::forward<U>(rhs)});
    }

    ///
    /// \brief Construct from an universal reference of U.
    ///
    /// \tparam U defaulted to T for usage as universal/forwarding reference.
    ///
    /// \param rhs a value from which we construct the instance of \ref expected.
    ///
    /// \note Non-explicit overload since U&& is convertible to T
    ///
    template <
        typename U = T,
        typename = typename std::enable_if_t<enable_universal_reference_ctor<U> && std::is_convertible<U&&, T>::value>,
        typename = void>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr expected(U&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value) : base_t{}
    {
        this->has_value_ = true;
        static_cast<void>(::new (&this->value_) T{std::forward<U>(rhs)});
    }

    ///
    /// \brief Construct directly from \ref unexpected type.
    ///
    /// \param rhs an instance of \ref unexpected that holds an error.
    ///
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    expected(const unexpected<E>& rhs) noexcept(std::is_nothrow_copy_constructible<E>::value) : base_t{}
    {
        this->has_value_ = false;
        static_cast<void>(::new (&this->error_) E{rhs.error()});
    }

    ///
    /// \brief Move construct directly from unexpected type.
    ///
    /// \param rhs an instance of \ref unexpected that holds an error.
    ///
    template <typename ErrorType = E, std::enable_if_t<std::is_move_constructible<ErrorType>::value, bool> = true>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    expected(unexpected<E>&& rhs) noexcept(std::is_nothrow_move_constructible<E>::value) : base_t{}
    {
        this->has_value_ = false;
        static_cast<void>(::new (&this->error_) E{std::move(rhs).error()});
    }

    ///
    /// \brief Move assign directly from value.
    ///
    /// \param rhs an instance of \ref expected that can hold either a value or an error.
    ///
    /// \return \c *this
    ///
    /// \note This function is needed to disambiguate creation of \ref expected between the creation from a valid value
    /// and from \ref unexpected.
    ///
    expected& operator=(T&& rhs) noexcept(std::is_rvalue_reference<T>::value &&
                                          std::is_nothrow_destructible<T>::value &&
                                          std::is_nothrow_destructible<E>::value)
    {
        if (has_value())
        {
            this->value_ = std::move(rhs);
        }
        else
        {
            this->error_.~E();
            static_cast<void>(::new (&this->value_) T{std::move(rhs)});
            this->has_value_ = true;
        }
        return *this;
    }

    ///
    /// \brief Copy assign directly from unexpected error.
    ///
    /// \param error an instance of \ref unexpected that holds an error.
    ///
    /// \return \c *this
    ///
    expected& operator=(const unexpected<E>& error) noexcept(std::is_nothrow_copy_constructible<E>::value &&
                                                             std::is_nothrow_destructible<T>::value &&
                                                             std::is_nothrow_destructible<E>::value)
    {
        if (has_value())
        {
            this->value_.~T();
            static_cast<void>(::new (&this->error_) E{error.error()});
            this->has_value_ = false;
        }
        else
        {
            this->error_ = error.error();
        }
        return *this;
    }

    ///
    /// \brief Move assign directly from unexpected error.
    ///
    /// \param error an instance of \ref unexpected that holds an error.
    ///
    /// \return \c *this
    ///
    template <typename ErrorType = E, std::enable_if_t<std::is_move_constructible<ErrorType>::value, bool> = true>
    expected& operator=(unexpected<E>&& error) noexcept(std::is_nothrow_move_constructible<E>::value &&
                                                        std::is_nothrow_destructible<T>::value &&
                                                        std::is_nothrow_destructible<E>::value)
    {
        if (has_value())
        {
            this->value_.~T();
            static_cast<void>(::new (&this->error_) E{std::move(error).error()});
            this->has_value_ = false;
        }
        else
        {
            this->error_ = std::move(error).error();
        }
        return *this;
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return reference to expected value
    /// \{
    T& operator*() &
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return this->value_;
    }
    const T& operator*() const&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return this->value_;
    }
    T&& operator*() &&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(this->value_);
    }
    const T&& operator*() const&&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(this->value_);
    }
    /// \}

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return reference to expected value
    /// \{
    const T& value() const&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return this->value_;
    }
    T& value() &
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return this->value_;
    }
    T&& value() &&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(this->value_);
    }
    const T&& value() const&&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(this->value_);
    }
    /// \}

    ///
    /// \brief Get a pointer to the stored value or die if error is stored instead.
    ///
    /// \return pointer to expected value
    /// \{
    T* operator->() noexcept { return &**this; }
    const T* operator->() const noexcept { return &**this; }
    /// \}

    ///
    /// \brief Get stored error.
    ///
    /// \pre !this->has_value()
    /// \return reference to unexpected error
    ///\{
    const E& error() const&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return this->error_;
    }
    E& error() &
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return this->error_;
    }
    E&& error() &&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return std::move(this->error_);
    }
    const E&& error() const&&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return std::move(this->error_);
    }
    /// \}

    ///
    /// \brief Check if there is a stored value.
    ///
    /// \return true if \ref expected stores an expected value and false otherwise.
    /// \{
    inline bool has_value() const noexcept { return this->has_value_; }
    explicit operator bool() const noexcept { return this->has_value_; }
    /// \}

    /// \brief Swap current instance with another.
    ///
    /// In the implementation all 4 cases are considered:
    /// - value <-> value
    /// - error <-> value
    /// - value <-> error
    /// - error <-> error
    ///
    // misc-no-recursion: The recursion is bounded to a maximum depth of 2. Thus neither stack nor
    // runtime are growing beyond bounds
    // NOLINTNEXTLINE(misc-no-recursion)
    void swap(expected& rhs) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                      std::is_nothrow_move_assignable<T>::value &&
                                      std::is_nothrow_move_constructible<E>::value &&
                                      std::is_nothrow_move_assignable<E>::value &&
                                      std::is_nothrow_destructible<T>::value && std::is_nothrow_destructible<E>::value)
    {
        if (has_value())
        {
            if (rhs.has_value())
            {
                using std::swap;
                swap(this->value_, rhs.value_);
            }
            else
            {
                rhs.swap(*this);
            }
        }
        else
        {
            if (!rhs.has_value())
            {
                using std::swap;
                swap(this->error_, rhs.error_);
            }
            else
            {
                E t{std::move(this->error_)};
                this->error_.~E();
                static_cast<void>(::new (&this->value_) T{std::move(rhs.value_)});
                this->has_value_ = true;
                rhs.value_.~T();
                static_cast<void>(::new (&rhs.value_) E{std::move(t)});
                rhs.has_value_ = false;
            }
        }
    }
};

template <typename LhsT, typename LhsE, typename RhsT, typename RhsE>
bool operator==(const expected<LhsT, LhsE>& lhs, const expected<RhsT, RhsE>& rhs)
{
    const auto lhs_has_value = static_cast<bool>(lhs);
    if (lhs_has_value != static_cast<bool>(rhs))
    {
        return false;
    }
    if (!lhs_has_value)
    {
        return lhs.error() == rhs.error();
    }
    return lhs.value() == rhs.value();
}

template <typename LhsT, typename LhsE, typename RhsT, typename RhsE>
bool operator!=(const expected<LhsT, LhsE>& lhs, const expected<RhsT, RhsE>& rhs)
{
    const auto lhs_has_value = static_cast<bool>(lhs);
    if (lhs_has_value != static_cast<bool>(rhs))
    {
        return true;
    }
    if (!lhs_has_value)
    {
        return lhs.error() != rhs.error();
    }
    return lhs.value() != rhs.value();
}

/// \var typedef expected_blank
/// \brief A specification of expected for usage with functions that don't return a value.
///
/// \tparam E the error type that we expect in case of failure.
///
/// This is intended to be used as a return type instead of `void` for functions that do not return a value but can
/// still fail with a recoverable error. This specification can be constructed from an instance of Score.Futurecpp.Blank.
///
template <typename E>
using expected_blank = expected<score::cpp::blank, E>;

///
/// \example ../examples/expected_example.cpp
/// \brief An example usage of expected class.
///
/// This example showcases the usage of the expected class with a function that returns a value as well as with those
/// that don't. In addition to that it shows the possibility to use different types for errors.
///

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_EXPECTED_HPP
