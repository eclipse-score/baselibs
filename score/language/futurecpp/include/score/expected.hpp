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
/// \tparam ErrorType the type of the \ref unexpected value that this class holds.
///
/// \note Intended to be used as through the \ref make_unexpected function.
///
template <typename ErrorType>
class unexpected
{
public:
    /// \brief Constructor from error value.
    explicit unexpected(const ErrorType& error) noexcept(std::is_nothrow_copy_constructible<ErrorType>::value)
        : error_{error}
    {
    }
    /// \brief Constructor from error rvalue.
    explicit unexpected(ErrorType&& error) noexcept(std::is_nothrow_move_constructible<ErrorType>::value)
        : error_{std::move(error)}
    {
    }
    /// \brief Getter for an error.
    const ErrorType& error() const& noexcept { return error_; }
    /// \brief Getter for an error.
    ErrorType& error() & noexcept { return error_; }
    /// \brief Getter for an error.
    ErrorType&& error() && noexcept { return std::move(error_); }
    /// \brief Getter for an error.
    const ErrorType&& error() const&& noexcept { return std::move(error_); }

private:
    ErrorType error_;
};

///
/// \brief A helper function to create an \ref unexpected instance.
///
/// \tparam ErrorType the type of the \ref unexpected value.
///
/// \return an instance of unexpected with the error forwarder to it.
///
template <typename ErrorType>
unexpected<std::decay_t<ErrorType>> make_unexpected(ErrorType&& e) noexcept(
    (std::is_rvalue_reference<ErrorType>::value && std::is_nothrow_move_constructible<ErrorType>::value) ||
    (std::is_lvalue_reference<ErrorType>::value && std::is_nothrow_copy_constructible<ErrorType>::value))
{
    return unexpected<std::decay_t<ErrorType>>(std::forward<ErrorType>(e));
}

///
/// \brief A container for an expected value or an error.
///
/// An instance of score::cpp::expected can hold either a value or an error.
///
/// \tparam ValueType the type of value that this class holds.
/// \tparam ErrorType the type of error that this class holds.
///
/// \note This implementation follows loosely Andrei Alexandrescu's take on expected.
///
template <typename ValueType, typename ErrorType>
class expected
{
    ///
    /// \brief A compile time evaluator to decide whether the universal reference ctor is enabled
    ///
    /// \tparam DummyType as evaluation criterion for the expression
    ///
    /// \note http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0323r7.html#expected.object.ctor
    ///
    template <typename DummyType>
    static constexpr bool enable_universal_reference_ctor =
        !std::is_void<ValueType>::value && std::is_constructible<ValueType, DummyType&&>::value &&
        !std::is_same<expected<ValueType, ErrorType>, score::cpp::remove_cvref_t<DummyType>>::value &&
        !std::is_same<unexpected<ErrorType>, score::cpp::remove_cvref_t<DummyType>>::value;

public:
    ///
    /// \typedef value_type
    ///
    /// \brief Alias for the type of the expected value
    ///
    using value_type = ValueType;

    ///
    /// \typedef error_type
    ///
    /// \brief Alias for the type of the unexpected value
    ///
    using error_type = ErrorType;

    /// \brief Default constructor. The instance is assumed to have a value.
    template <typename U = ValueType, typename = std::enable_if_t<std::is_default_constructible<U>::value>>
    expected() noexcept(std::is_nothrow_default_constructible<ValueType>::value) : has_value_{true}
    {
        static_cast<void>(::new (&value_) ValueType{});
    }

    ///
    /// \brief Construct from an universal reference of DummyType.
    ///
    /// \tparam DummyType defaulted to ValueType for usage as universal/forwarding reference.
    ///
    /// \param rhs a value from which we construct the instance of \ref expected.
    ///
    /// \note Explicit overload since DummyType&& is not convertible to ValueType
    ///
    template <typename DummyType = ValueType,
              typename = typename std::enable_if_t<enable_universal_reference_ctor<DummyType> &&
                                                   !std::is_convertible<DummyType&&, ValueType>::value>>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    explicit constexpr expected(DummyType&& rhs) noexcept(std::is_nothrow_move_constructible<ValueType>::value)
        : has_value_{true}
    {
        static_cast<void>(::new (&value_) ValueType{std::forward<DummyType>(rhs)});
    }

    ///
    /// \brief Construct from an universal reference of DummyType.
    ///
    /// \tparam DummyType defaulted to ValueType for usage as universal/forwarding reference.
    ///
    /// \param rhs a value from which we construct the instance of \ref expected.
    ///
    /// \note Non-explicit overload since DummyType&& is convertible to ValueType
    ///
    template <typename DummyType = ValueType,
              typename = typename std::enable_if_t<enable_universal_reference_ctor<DummyType> &&
                                                   std::is_convertible<DummyType&&, ValueType>::value>,
              typename = void>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    constexpr expected(DummyType&& rhs) noexcept(std::is_nothrow_move_constructible<ValueType>::value)
        : has_value_{true}
    {
        static_cast<void>(::new (&value_) ValueType{std::forward<DummyType>(rhs)});
    }

    ///
    /// \brief Construct directly from \ref unexpected type.
    ///
    /// \param rhs an instance of \ref unexpected that holds an error.
    ///
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    expected(const unexpected<ErrorType>& rhs) noexcept(std::is_nothrow_copy_constructible<ErrorType>::value)
        : has_value_{false}
    {
        static_cast<void>(::new (&error_) ErrorType{rhs.error()});
    }

    ///
    /// \brief Move construct directly from unexpected type.
    ///
    /// \param rhs an instance of \ref unexpected that holds an error.
    ///
    template <typename E = ErrorType, std::enable_if_t<std::is_move_constructible<E>::value, bool> = true>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    expected(unexpected<ErrorType>&& rhs) noexcept(std::is_nothrow_move_constructible<ErrorType>::value)
        : has_value_{false}
    {
        static_cast<void>(::new (&error_) ErrorType{std::move(rhs).error()});
    }

    ///
    /// \brief Copy constructor.
    ///
    /// \param rhs an instance of \ref expected that can hold either a value or an error.
    ///
    expected(const expected& rhs) noexcept(
        std::is_nothrow_copy_constructible<ValueType>::value&& std::is_nothrow_copy_constructible<ErrorType>::value)
        : has_value_{rhs.has_value()}
    {
        if (has_value())
        {
            static_cast<void>(::new (&value_) ValueType{rhs.value_});
        }
        else
        {
            static_cast<void>(::new (&error_) ErrorType{rhs.error_});
        }
    }

    ///
    /// \brief Move constructor.
    ///
    /// \tparam DummyType defaulted to ValueType. Used to delay template instantiation.
    ///
    /// \param rhs an instance of \ref expected that can hold either a value or an error.
    ///
    template <
        typename V = ValueType,
        typename E = ErrorType,
        std::enable_if_t<std::is_move_constructible<V>::value && std::is_move_constructible<E>::value, bool> = true>
    expected(expected&& rhs) noexcept(
        std::is_nothrow_move_constructible<ValueType>::value&& std::is_nothrow_move_constructible<ErrorType>::value)
        : has_value_{rhs.has_value()}
    {
        if (has_value())
        {
            static_cast<void>(::new (&value_) ValueType{std::move(rhs).value_});
        }
        else
        {
            static_cast<void>(::new (&error_) ErrorType{std::move(rhs).error_});
        }
    }

    ///
    /// \brief Copy assignment operator.
    ///
    /// \tparam DummyType only needed to enable SFINAE in case ErrorType or ValueType do not meet our expectations.
    ///
    /// \param rhs an instance of \ref expected that can hold either a value or an error.
    ///
    /// \return \c *this
    ///
    expected& operator=(const expected& rhs) noexcept(
        std::is_nothrow_copy_constructible<ValueType>::value&& std::is_nothrow_copy_constructible<ErrorType>::value&&
            std::is_nothrow_destructible<ValueType>::value&& std::is_nothrow_destructible<ErrorType>::value)
    {
        if (has_value())
        {
            value_.~ValueType();
            if (rhs.has_value())
            {
                static_cast<void>(::new (&value_) ValueType{rhs.value_});
            }
            else
            {
                static_cast<void>(::new (&error_) ErrorType{rhs.error_});
                has_value_ = false;
            }
        }
        else
        {
            error_.~ErrorType();
            if (rhs.has_value())
            {
                static_cast<void>(::new (&value_) ValueType{rhs.value_});
                has_value_ = true;
            }
            else
            {
                static_cast<void>(::new (&error_) ErrorType{rhs.error_});
            }
        }
        return *this;
    }

    ///
    /// \brief Move assignment operator.
    ///
    /// \param rhs an instance of \ref expected that can hold either a value or an error.
    ///
    /// \return \c *this
    ///
    template <
        typename V = ValueType,
        typename E = ErrorType,
        std::enable_if_t<std::is_move_constructible<V>::value && std::is_move_constructible<E>::value, bool> = true>
    expected& operator=(expected&& rhs) noexcept(
        std::is_nothrow_move_constructible<ValueType>::value&& std::is_nothrow_move_constructible<ErrorType>::value&&
            std::is_nothrow_destructible<ValueType>::value&& std::is_nothrow_destructible<ErrorType>::value)
    {
        if (has_value())
        {
            value_.~ValueType();
            if (rhs.has_value())
            {
                static_cast<void>(::new (&value_) ValueType{std::move(rhs).value_});
            }
            else
            {
                static_cast<void>(::new (&error_) ErrorType{std::move(rhs).error_});
                has_value_ = false;
            }
        }
        else
        {
            error_.~ErrorType();
            if (rhs.has_value())
            {
                static_cast<void>(::new (&value_) ValueType{std::move(rhs).value_});
                has_value_ = true;
            }
            else
            {
                static_cast<void>(::new (&error_) ErrorType{std::move(rhs).error_});
            }
        }
        return *this;
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
    expected& operator=(ValueType&& rhs) noexcept(
        std::is_rvalue_reference<ValueType>::value&& std::is_nothrow_destructible<ValueType>::value&&
            std::is_nothrow_destructible<ErrorType>::value)
    {
        if (has_value())
        {
            value_.~ValueType();
            static_cast<void>(::new (&value_) ValueType{std::move(rhs)});
        }
        else
        {
            error_.~ErrorType();
            has_value_ = true;
            static_cast<void>(::new (&value_) ValueType{std::move(rhs)});
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
    expected& operator=(const unexpected<ErrorType>& error) noexcept(
        std::is_nothrow_copy_constructible<ErrorType>::value&& std::is_nothrow_destructible<ValueType>::value&&
            std::is_nothrow_destructible<ErrorType>::value)
    {
        if (has_value())
        {
            value_.~ValueType();
            has_value_ = false;
        }
        else
        {
            error_.~ErrorType();
        }
        static_cast<void>(::new (&error_) ErrorType{error.error()});
        return *this;
    }

    ///
    /// \brief Move assign directly from unexpected error.
    ///
    /// \param error an instance of \ref unexpected that holds an error.
    ///
    /// \return \c *this
    ///
    template <typename E = ErrorType, std::enable_if_t<std::is_move_constructible<E>::value, bool> = true>
    expected& operator=(unexpected<ErrorType>&& error) noexcept(
        std::is_nothrow_move_constructible<ErrorType>::value&& std::is_nothrow_destructible<ValueType>::value&&
            std::is_nothrow_destructible<ErrorType>::value)
    {
        if (has_value())
        {
            value_.~ValueType();
            has_value_ = false;
        }
        else
        {
            error_.~ErrorType();
        }
        static_cast<void>(::new (&error_) ErrorType{std::move(error).error()});
        return *this;
    }

    /// \brief Destructor that cleans up after either value or error.
    ~expected() noexcept(
        std::is_nothrow_destructible<ValueType>::value&& std::is_nothrow_destructible<ErrorType>::value)
    {
        if (has_value())
        {
            value_.~ValueType();
        }
        else
        {
            error_.~ErrorType();
        }
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    ValueType& operator*() &
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return value_;
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    const ValueType& operator*() const&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return value_;
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    ValueType&& operator*() &&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(value_);
    }

    ///
    /// \brief Get a pointer to the stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    const ValueType&& operator*() const&&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(value_);
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    const ValueType& value() const&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return value_;
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    ValueType& value() &
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return value_;
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    ValueType&& value() &&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(value_);
    }

    ///
    /// \brief Get stored value.
    ///
    /// \pre this->has_value()
    /// \return \ref expected::value_
    ///
    const ValueType&& value() const&&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(has_value());
        return std::move(value_);
    }

    ///
    /// \brief Get a pointer to the stored value or die if error is stored instead.
    ///
    /// \return pointer to \ref expected::value_
    ///
    ValueType* operator->() noexcept { return &**this; }

    ///
    /// \brief Get a pointer to the stored value or die if error is stored instead.
    ///
    /// \return pointer to \ref expected::value_
    ///
    const ValueType* operator->() const noexcept { return &**this; }

    ///
    /// \brief Get stored error.
    ///
    /// \pre !this->has_value()
    /// \return \ref expected::error_
    ///
    const ErrorType& error() const&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return error_;
    }

    ///
    /// \brief Get stored error.
    ///
    /// \pre !this->has_value()
    /// \return \ref expected::error_
    ///
    ErrorType& error() &
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return error_;
    }

    ///
    /// \brief Get stored error.
    ///
    /// \pre !this->has_value()
    /// \return \ref expected::error_
    ///
    ErrorType&& error() &&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return std::move(error_);
    }

    ///
    /// \brief Get stored error.
    ///
    /// \pre !this->has_value()
    /// \return \ref expected::error_
    ///
    const ErrorType&& error() const&&
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(!has_value());
        return std::move(error_);
    }

    ///
    /// \brief Check if there is a stored value.
    ///
    /// \return true if \ref expected stores a \ref expected::value_ and false otherwise.
    ///
    inline bool has_value() const noexcept { return has_value_; }

    ///
    /// \brief Check if there is a stored value as an implicit conversion to bool.
    /// \return true if \ref expected stores a \ref expected::value_ and false otherwise.
    ///
    explicit operator bool() const noexcept { return has_value_; }

    /// \brief Swap current instance with another.
    ///
    /// In the implementation all 4 cases are considered:
    /// - value <-> value
    /// - error <-> value
    /// - value <-> error
    /// - error <-> error
    ///
    void swap(expected& rhs) noexcept(
        std::is_nothrow_move_constructible<ValueType>::value&& std::is_nothrow_move_assignable<ValueType>::value&&
            std::is_nothrow_move_constructible<ErrorType>::value&& std::is_nothrow_move_assignable<ErrorType>::value&&
                std::is_nothrow_destructible<ValueType>::value&& std::is_nothrow_destructible<ErrorType>::value)
    {
        if (has_value())
        {
            if (rhs.has_value())
            {
                using std::swap;
                swap(value_, rhs.value_);
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
                swap(error_, rhs.error_);
            }
            else
            {
                ErrorType t{std::move(error_)};
                error_.~ErrorType();
                static_cast<void>(::new (&value_) ValueType{std::move(rhs.value_)});
                has_value_ = true;
                rhs.value_.~ValueType();
                static_cast<void>(::new (&rhs.value_) ErrorType{std::move(t)});
                rhs.has_value_ = false;
            }
        }
    }

private:
    // For performances reason this class is not using `std::variant` but implementing the storage as a raw union. This
    // class takes care by code review to only read from the active member of the union. The union is only visible
    // internally and not exposed to the user API.
    // coverity[misra_cpp_2023_rule_12_3_1_violation : SUPPRESS]
    union
    {
        /// Stored value.
        ValueType value_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
        /// Stored error.
        ErrorType error_; // NOLINT(readability-identifier-naming) keep `_` to make clear it is a member variable
    };
    bool has_value_; ///< Indicator if this instance holds a value.
};

template <typename LhsV, typename LhsE, typename RhsV, typename RhsE>
bool operator==(const expected<LhsV, LhsE>& lhs, const expected<RhsV, RhsE>& rhs)
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

template <typename LhsV, typename LhsE, typename RhsV, typename RhsE>
bool operator!=(const expected<LhsV, LhsE>& lhs, const expected<RhsV, RhsE>& rhs)
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
/// \tparam ErrorType the error type that we expect in case of failure.
///
/// This is intended to be used as a return type instead of `void` for functions that do not return a value but can
/// still fail with a recoverable error. This specification can be constructed from an instance of Score.Futurecpp.Blank.
///
template <typename ErrorType>
using expected_blank = expected<score::cpp::blank, ErrorType>;

///
/// \example ../examples/expected_example.cpp
/// \brief An example usage of expected class.
///
/// This example showcases the usage of the expected class with a function that returns a value as well as with those
/// that don't. In addition to that it shows the possibility to use different types for errors.
///

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_EXPECTED_HPP
