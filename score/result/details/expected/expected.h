/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_RESULT_EXPECTED_H
#define SCORE_LIB_RESULT_EXPECTED_H

#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

namespace score::details
{
namespace impl
{

template <class T, class W>
using converts_from_any_cvref = std::disjunction<std::is_constructible<T, W&>,
                                                 std::is_convertible<W&, T>,
                                                 std::is_constructible<T, W>,
                                                 std::is_convertible<W, T>,
                                                 std::is_constructible<T, const W&>,
                                                 std::is_convertible<const W&, T>,
                                                 std::is_constructible<T, const W>,
                                                 std::is_convertible<const W, T>>;
// Suppress "AUTOSAR C++14 A12-8-6" rule findings. The rule states "Copy and move constructors and copy assignment and
// move assignment operators shall be declared protected or defined "=delete" in base class." Defining  copy and move
// constructors in base class will make the expected non trivial and also the derived classes will be non copyable as
// stated by the rule https://timsong-cpp.github.io/cppwp/n4950/expected#object.cons-10
// coverity[autosar_cpp14_a12_8_6_violation]
class base_unexpected
{
};
// coverity[autosar_cpp14_a12_8_6_violation]
class base_expected
{
};
}  // namespace impl

class unexpect_t
{
  public:
    explicit unexpect_t() = default;
};

inline constexpr unexpect_t unexpect{};

template <typename E>
// We deliberately do not adhere to the rule of five here as both the destructor and the copy assignment operators
// Suppress "AUTOSAR C++14 A12-0-1" rule states: If a class declares a copy or move operation, or a destructor, either
// via “=default”, “=delete”, or via a user-provided declaration, then all others of these five special member functions
// shall be declared as well.". We deliberately do not adhere to the rule of five here as both the destructor and the
// copy assignment operators.
// NOLINTBEGIN(cppcoreguidelines-special-member-functions): shall be implicitly declared as per the C++23 standard.
// coverity[autosar_cpp14_a12_0_1_violation]
class unexpected : impl::base_unexpected
// NOLINTEND(cppcoreguidelines-special-member-functions): shall be implicitly declared as per the C++23 standard.
{
  public:
    constexpr unexpected(const unexpected&) = default;
    // We deliberately do not declare the move constructor noexcept according to the C++23 standard. In addition, since
    // it is defaulted, it will anyway inherit the noexcept
    // NOLINTNEXTLINE(performance-noexcept-move-constructor) qualification from the move constructor of E.
    constexpr unexpected(unexpected&&) = default;

    template <typename G = E,
              typename = std::enable_if_t<std::conjunction_v<
                  std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<G>>, unexpected>>,
                  std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<G>>, std::in_place_t>>,
                  std::is_constructible<E, G>>>>
    constexpr explicit unexpected(G&& e) : impl::base_unexpected{}, value_{std::forward<G>(e)}
    {
    }

    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
    constexpr explicit unexpected(std::in_place_t, Args&&... args)
        : impl::base_unexpected{}, value_{std::forward<Args>(args)...}
    {
    }

    template <typename U,
              typename... Args,
              typename = std::enable_if_t<std::is_constructible_v<E, std::initializer_list<U>&, Args...>>>
    constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args&&... args)
        : impl::base_unexpected{}, value_{il, std::forward<Args>(args)...}
    {
    }

    constexpr unexpected& operator=(const unexpected&) = default;
    // We deliberately do not declare the move assignment operator noexcept according to the C++23 standard.
    // In addition, since it is defaulted, it will anyway inherit the noexcept.
    // NOLINTNEXTLINE(performance-noexcept-move-constructor) qualification from the move assignment operator of E.
    constexpr unexpected& operator=(unexpected&&) = default;

    constexpr const E& error() const& noexcept
    {
        return value_;
    }
    constexpr E& error() & noexcept
    {
        // coverity[autosar_cpp14_a9_3_1_violation] by design
        return value_;
    }
    constexpr const E&& error() const&& noexcept
    {
        // Suppress AUTOSAR C++14 A18-9-3" rule violations. The rule states "The std::move shall not be used on objects
        // declared const or const&." the move operation on a const object does not cause unintended behavior it ensures
        // a proper copy/move of underlying values.
        // coverity[autosar_cpp14_a18_9_3_violation]
        return std::move(value_);
    }
    constexpr E&& error() && noexcept
    {
        return std::move(value_);
    }

    template <typename E2>
    // Suppress "AUTOSAR C++14 A13-5-5" rule violation". The rule states "Comparison operators shall be non-member
    // functions with identical parameter types and noexcept." The standard does not specify them noexcept and we follow
    // that.
    // Suppress "AUTOSAR C++14 A3-3-1" rule violations. The rule states "Objects or functions with external linkage
    // (including members of named namespaces) shall be declared in a header file". This is false positive, the
    // external linkage function is declared in the header file.
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    // coverity[autosar_cpp14_a13_5_5_violation]
    friend constexpr bool operator==(const unexpected& lhs, const unexpected<E2>& rhs)
    {
        return lhs.error() == rhs.error();
    }

    // DIVERGENCE FROM STANDARD
    // Inequality operators are automatically generated from C++20 onwards.
    template <typename E2>
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr bool operator!=(const unexpected& lhs, const unexpected<E2>& rhs)
    {
        return lhs.error() != rhs.error();
    }

    constexpr void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>)
    {
        static_assert(std::is_swappable_v<E>);
        std::swap(value_, other.value_);
    }

    // coverity[autosar_cpp14_a11_3_1_violation] friend is required for std::swap to work
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr void swap(unexpected& x, unexpected& y) noexcept(noexcept(x.swap(y)))
    {
        static_assert(std::is_swappable_v<E>);
        x.swap(y);
    }

  private:
    E value_;
};

template <class E>
unexpected(E) -> unexpected<E>;

// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Suppress "AUTOSAR C++14 M16-0-2" rule findings. This rule stated: "Macros shall only be #define’d or #undef’d in
// the global namespace".
// Tolerated by project to enable/disable enforce and expected nodiscard.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef SPP_ENFORCE_NODISCARD
// coverity[autosar_cpp14_a16_0_1_violation]
#define SPP_EXPECTED_NODISCARD [[nodiscard]]
// coverity[autosar_cpp14_a16_0_1_violation]
#else
// coverity[autosar_cpp14_m16_0_2_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#define SPP_EXPECTED_NODISCARD
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

template <typename T, typename E>
class SPP_EXPECTED_NODISCARD expected : impl::base_expected
{
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding. This rule states: "A lambda shall not be an operand to decltype or
    // typeid". False-positive, at this point "decltype" is not used with lambda.
    // coverity[autosar_cpp14_a5_1_7_violation : FALSE]
    using StorageType = std::variant<T, E>;

  public:
    // coverity[autosar_cpp14_a5_1_7_violation : FALSE]
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;
    template <typename U>
    using rebind = expected<U, error_type>;

    constexpr expected() = default;

    constexpr expected(const expected&) = default;
    // NOLINTBEGIN(performance-noexcept-move-constructor) Noexcept defined according to standard
    constexpr expected(expected&&) noexcept(
        std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<E>>) = default;
    // NOLINTEND(performance-noexcept-move-constructor)

    template <typename U,
              typename G,
              typename = std::enable_if_t<std::conjunction_v<
                  std::is_constructible<T, const U&>,
                  std::is_constructible<E, const G&>,
                  // coverity[autosar_cpp14_a2_11_1_violation] Used intentionally in compliance with the standard
                  std::disjunction<std::is_same<const volatile bool, T>,
                                   std::negation<impl::converts_from_any_cvref<T, expected<U, G>>>>,
                  std::negation<std::is_constructible<unexpected<E>, expected<U, G>&>>,
                  std::negation<std::is_constructible<unexpected<E>, expected<U, G>>>,
                  std::negation<std::is_constructible<unexpected<E>, const expected<U, G>&>>,
                  std::negation<std::is_constructible<unexpected<E>, const expected<U, G>>>>>>
    // DIVERGENCE FROM STANDARD: Explicitness cannot be specified according to standard, since this requires a C++20
    // feature. Anyway, only available if T can be implicitly constructed from UF,
    // or E can be implicitly constructed form GF.
    // NOLINTNEXTLINE(google-explicit-constructor) see justification above
    constexpr expected(const expected<U, G>& rhs)
        : impl::base_expected{},
          storage_{rhs.has_value() ? StorageType{std::in_place_index<0>, std::forward<const U&>(*rhs)}
                                   : StorageType{std::in_place_index<1>, std::forward<const G&>(rhs.error())}}
    {
    }

    template <typename U,
              typename G,
              typename = std::enable_if_t<std::conjunction_v<
                  std::is_constructible<T, U>,
                  std::is_constructible<E, G>,
                  // coverity[autosar_cpp14_a2_11_1_violation] Used intentionally in compliance with the standard
                  std::disjunction<std::is_same<const volatile bool, T>,
                                   std::negation<impl::converts_from_any_cvref<T, expected<U, G>>>>,
                  std::negation<std::is_constructible<unexpected<E>, expected<U, G>&>>,
                  std::negation<std::is_constructible<unexpected<E>, expected<U, G>>>,
                  std::negation<std::is_constructible<unexpected<E>, const expected<U, G>&>>,
                  std::negation<std::is_constructible<unexpected<E>, const expected<U, G>>>>>>
    // DIVERGENCE FROM STANDARD: Explicitness cannot be specified according to standard, since this requires a C++20
    // feature. Anyway, only available if T can be implicitly constructed from UF,
    // or E can be implicitly constructed form GF.
    // NOLINTNEXTLINE(google-explicit-constructor) see justification above
    constexpr expected(expected<U, G>&& rhs)
        : impl::base_expected{},
          storage_{rhs.has_value() ? StorageType{std::in_place_index<0>, std::forward<U>(*rhs)}
                                   : StorageType{std::in_place_index<1>, std::forward<G>(rhs.error())}}
    {
    }

    template <typename U = T,
              typename = std::enable_if_t<std::conjunction_v<
                  // NOLINTBEGIN(modernize-avoid-c-arrays): false-positive no c-arrays used
                  std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, std::in_place_t>>,
                  std::negation<std::is_same<expected, std::remove_cv_t<std::remove_reference_t<U>>>>,
                  std::negation<std::is_base_of<impl::base_unexpected, std::remove_cv_t<std::remove_reference_t<U>>>>,
                  std::is_constructible<T, U>,
                  std::disjunction<
                      // coverity[autosar_cpp14_a2_11_1_violation] Used intentionally in compliance with the standard
                      std::negation<std::is_same<const volatile bool, std::remove_cv_t<std::remove_reference_t<U>>>>,
                      std::is_base_of<base_expected, std::remove_cv_t<std::remove_reference_t<U>>>>>>>
    // NOLINTEND(modernize-avoid-c-arrays)
    // DIVERGENCE FROM STANDARD: Explicitness cannot be specified according to standard, since this requires a
    // C++20 feature. Anyway, only available if T can be implicitly constructed from U.
    // NOLINTNEXTLINE(google-explicit-constructor) see justification above
    constexpr expected(U&& value) : impl::base_expected{}, storage_{std::in_place_index<0>, std::forward<U>(value)}
    {
    }

    template <typename G, typename = std::enable_if_t<std::is_constructible_v<E, const G&>>>
    // DIVERGENCE FROM STANDARD: Explicitness cannot be specified according to standard, since this requires a C++20
    // feature. Anyway, only available if E can be implicitly constructed from G.
    // NOLINTNEXTLINE(google-explicit-constructor) see justification above
    constexpr expected(const unexpected<G>& error)
        : impl::base_expected{}, storage_{std::in_place_index<1>, std::forward<const G&>(error.error())}
    {
    }

    template <typename G, typename = std::enable_if_t<std::is_constructible_v<E, G>>>
    // DIVERGENCE FROM STANDARD: Explicitness cannot be specified according to standard, since this requires a C++20
    // feature. Anyway, only available if E can be implicitly constructed from G.
    // NOLINTNEXTLINE(google-explicit-constructor) see justification above
    constexpr expected(unexpected<G>&& error)
        : impl::base_expected{}, storage_{std::in_place_index<1>, std::forward<G>(error.error())}
    {
    }

    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit expected(std::in_place_t, Args&&... args)
        : impl::base_expected{}, storage_{std::in_place_index<0>, std::forward<Args>(args)...}
    {
    }

    template <typename U, typename... Args>
    // Suppress "AUTOSAR C++14 A13-3-1", The rule states: "A function that contains “forwarding reference” as its
    // argument shall not be overloaded".
    // As we here have a different parameters, so no confusion on which API to be used.
    // coverity[autosar_cpp14_a13_3_1_violation]
    constexpr explicit expected(std::in_place_t, std::initializer_list<U> il, Args&&... args)
        : impl::base_expected{}, storage_{std::in_place_index<0>, il, std::forward<Args>(args)...}
    {
    }

    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
    constexpr explicit expected(unexpect_t, Args&&... args)
        : impl::base_expected{}, storage_{std::in_place_index<1>, std::forward<Args>(args)...}
    {
    }

    template <typename U,
              typename... Args,
              typename = std::enable_if_t<std::is_constructible_v<E, std::initializer_list<U>&, Args...>>>
    constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args&&... args)
        : impl::base_expected{}, storage_{std::in_place_index<1>, il, std::forward<Args>(args)...}
    {
    }

    ~expected() noexcept = default;

    constexpr expected& operator=(const expected&) = default;
    // NOLINTBEGIN(performance-noexcept-move-constructor) Noexcept defined according to standard
    constexpr expected& operator=(expected&& other) noexcept(
        std::conjunction_v<std::is_nothrow_move_assignable<T>,
                           std::is_nothrow_move_constructible<T>,
                           std::is_nothrow_move_assignable<E>,
                           std::is_nothrow_move_constructible<E>>) = default;
    // NOLINTEND(performance-noexcept-move-constructor)

    template <typename U = T,
              typename = std::enable_if_t<std::conjunction_v<
                  // NOLINTBEGIN(modernize-avoid-c-arrays): false-positive no c-arrays used
                  std::negation<std::is_same<expected<T, E>, std::remove_cv_t<std::remove_reference_t<U>>>>,
                  std::negation<std::is_base_of<impl::base_unexpected, std::remove_cv_t<std::remove_reference_t<U>>>>,
                  std::is_constructible<T, U>,
                  std::is_assignable<T&, U>,
                  std::disjunction<std::is_nothrow_constructible<T, U>,
                                   std::is_nothrow_move_constructible<T>,
                                   std::is_nothrow_move_constructible<E>>>>>
    // NOLINTEND(modernize-avoid-c-arrays)
    // coverity[autosar_cpp14_a13_3_1_violation]
    constexpr expected& operator=(U&& v)
    {
        // Suppress "AUTOSAR C++14 A0-1-2" rule violatoin. The rule states "The value returned by a function having a
        // non-void return type that is not an overloaded operator shall be used." We are not interested in the return
        // and hence suppressed.
        // coverity[autosar_cpp14_a0_1_2_violation]
        storage_.template emplace<0>(std::forward<U>(v));
        return *this;
    }

    template <
        typename G,
        typename = std::enable_if_t<std::conjunction_v<std::is_constructible<E, const G&>,
                                                       std::is_assignable<E&, const G&>,
                                                       std::disjunction<std::is_nothrow_constructible<E, const G&>,
                                                                        std::is_nothrow_move_constructible<T>,
                                                                        std::is_nothrow_move_constructible<E>>>>>
    // coverity[autosar_cpp14_a13_3_1_violation]
    constexpr expected& operator=(const unexpected<G>& other)
    {
        // Suppress "AUTOSAR C++14 A0-1-2" rule violatoin. The rule states "The value returned by a function having a
        // non-void return type that is not an overloaded operator shall be used." We are not interested in the return
        // and hence suppressed.
        // coverity[autosar_cpp14_a0_1_2_violation]
        storage_.template emplace<1>(other.error());
        return *this;
    }

    template <typename G,
              typename = std::enable_if_t<std::conjunction_v<std::is_constructible<E, G>,
                                                             std::is_assignable<E&, G>,
                                                             std::disjunction<std::is_nothrow_constructible<E, G>,
                                                                              std::is_nothrow_move_constructible<T>,
                                                                              std::is_nothrow_move_constructible<E>>>>>
    // coverity[autosar_cpp14_a13_3_1_violation]
    constexpr expected& operator=(unexpected<G>&& other)
    {
        storage_ = StorageType{std::in_place_index<1>, std::move(other).error()};
        return *this;
    }

    template <typename... Args, typename = std::enable_if_t<std::is_nothrow_constructible_v<T, Args...>>>
    // Suppress "AUTOSAR C++14 A15-5-3" rule violation. The rule states "The std::terminate() function shall not be
    // called implicitly". API must be compatible with the standard hence cannot be not noexcept.
    // coverity[autosar_cpp14_a15_5_3_violation]
    constexpr T& emplace(Args&&... args) noexcept
    {
        // Refer on top for suppression justification.
        // coverity[autosar_cpp14_a0_1_2_violation]
        storage_.template emplace<0>(std::forward<Args>(args)...);
        return value();
    }

    template <typename U,
              typename... Args,
              typename = std::enable_if_t<std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>>>
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    // coverity[autosar_cpp14_a13_3_1_violation]
    constexpr T& emplace(std::initializer_list<U> il, Args&&... args) noexcept
    {
        // Refer on top for suppression justification.
        // coverity[autosar_cpp14_a0_1_2_violation]
        storage_.template emplace<0>(il, std::forward<Args>(args)...);
        return value();
    }

    constexpr void swap(expected& other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>,
                                                                     std::is_nothrow_swappable<T>,
                                                                     std::is_nothrow_move_constructible<E>,
                                                                     std::is_nothrow_swappable<E>>)
    {
        storage_.swap(other.storage_);
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr T& operator*() & noexcept
    {
        return value();
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr const T& operator*() const& noexcept
    {
        return value();
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr T&& operator*() && noexcept
    {
        return std::move(*this).value();
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr const T&& operator*() const&& noexcept
    {
        // Refer on top for suppression justification.
        // coverity[autosar_cpp14_a18_9_3_violation]
        return std::move(*this).value();
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr T* operator->() noexcept
    {
        return &operator*();
    };
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr const T* operator->() const noexcept
    {
        return &operator*();
    };

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr bool has_value() const noexcept
    {
        return storage_.index() == 0U;
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr T& value() &
    {
        return std::get<0>(storage_);
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr T&& value() &&
    {
        return std::get<0>(std::move(storage_));
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr const T& value() const&
    {
        return std::get<0>(storage_);
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    [[nodiscard]] constexpr const T&& value() const&&
    {
        // Refer on top for suppression justification.
        // coverity[autosar_cpp14_a18_9_3_violation]
        return std::get<0>(std::move(storage_));
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr E& error() & noexcept
    {
        return std::get<1>(storage_);
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr E&& error() && noexcept
    {
        return std::get<1>(std::move(storage_));
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr const E& error() const& noexcept
    {
        return std::get<1>(storage_);
    }
    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    [[nodiscard]] constexpr const E&& error() const&& noexcept
    {
        // Refer on top for suppression justification.
        // coverity[autosar_cpp14_a18_9_3_violation]
        return std::get<1>(std::move(storage_));
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename U>
    [[nodiscard]] constexpr T value_or(U&& default_val) const&
    {
        if (has_value())
        {
            return **this;
        }
        else
        {
            return static_cast<T>(std::forward<U>(default_val));
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename U>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr T value_or(U&& default_val) &&
    {
        if (has_value())
        {
            return std::move(**this);
        }
        else
        {
            return static_cast<T>(std::forward<U>(default_val));
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename G = E>
    [[nodiscard]] constexpr E error_or(G&& default_err) const&
    {
        if (!has_value())
        {
            return error();
        }
        else
        {
            return std::forward<G>(default_err);
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename G = E>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr E error_or(G&& default_err) &&
    {
        if (!has_value())
        {
            return std::move(error());
        }
        else
        {
            return std::forward<G>(default_err);
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    [[nodiscard]] constexpr auto and_then(F&& fun) &
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(value())>>>;

        if (has_value())
        {
            return std::invoke(std::forward<F>(fun), value());
        }
        else
        {
            return U{unexpect, error()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto and_then(F&& fun) const&
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(value())>>>;

        if (has_value())
        {
            return std::invoke(std::forward<F>(fun), value());
        }
        else
        {
            return U{unexpect, error()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto and_then(F&& fun) &&
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(value()))>>>;

        if (has_value())
        {
            return std::invoke(std::forward<F>(fun), std::move(*this).value());
        }
        else
        {
            return U{unexpect, error()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto and_then(F&& fun) const&&
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(value()))>>>;

        if (has_value())
        {
            // Refer on top for suppression justification.
            // coverity[autosar_cpp14_a18_9_3_violation]
            return std::invoke(std::forward<F>(fun), std::move(*this).value());
        }
        else
        {
            return U{unexpect, error()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    [[nodiscard]] constexpr auto or_else(F&& fun) &
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(error())>>>;

        if (!has_value())
        {
            return std::invoke(std::forward<F>(fun), error());
        }
        else
        {
            return G{std::in_place, value()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto or_else(F&& fun) const&
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(error())>>>;

        if (!has_value())
        {
            return std::invoke(std::forward<F>(fun), error());
        }
        else
        {
            return G{std::in_place, value()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto or_else(F&& fun) &&
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(error()))>>>;

        if (!has_value())
        {
            return std::invoke(std::forward<F>(fun), std::move(error()));
        }
        else
        {
            return G{std::in_place, std::move(value())};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto or_else(F&& fun) const&&
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(error()))>>>;

        if (!has_value())
        {
            // Refer on top for suppression justification.
            // coverity[autosar_cpp14_a18_9_3_violation]
            return std::invoke(std::forward<F>(fun), std::move(error()));
        }
        else
        {
            // Refer on top for suppression justification.
            // coverity[autosar_cpp14_a18_9_3_violation]
            return G{std::in_place, std::move(value())};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    [[nodiscard]] constexpr auto transform(F&& fun) &
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(value())>>>;

        if (has_value())
        {
            // Suppress "AUTOSAR C++14 A7-1-8" rule finding. This rule states: "A class, structure, or enumeration shall
            // not be declared in the definition of its type". This is a false positive because "if constexpr" is a
            // valid statement since C++17.
            // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
            if constexpr (!std::is_void_v<U>)
            {
                return expected<U, E>{std::in_place, std::invoke(std::forward<F>(fun), value())};
            }
            else
            {
                std::invoke(std::forward<F>(fun), value());
                return expected<U, E>{};
            }
        }
        else
        {
            return expected<U, E>{unexpect, error()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto transform(F&& fun) const&
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(value())>>>;

        if (has_value())
        {
            // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
            if constexpr (!std::is_void_v<U>)
            {
                return expected<U, E>{std::in_place, std::invoke(std::forward<F>(fun), value())};
            }
            else
            {
                std::invoke(std::forward<F>(fun), value());
                return expected<U, E>{};
            }
        }
        else
        {
            return expected<U, E>{unexpect, error()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto transform(F&& fun) &&
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(value()))>>>;

        if (has_value())
        {
            // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
            if constexpr (!std::is_void_v<U>)
            {
                return expected<U, E>{std::in_place, std::invoke(std::forward<F>(fun), std::move(value()))};
            }
            else
            {
                std::invoke(std::forward<F>(fun), std::move(value()));
                return expected<U, E>{};
            }
        }
        else
        {
            return expected<U, E>{unexpect, std::move(error())};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto transform(F&& fun) const&&
    {
        using U = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(value()))>>>;

        if (has_value())
        {
            // coverity[autosar_cpp14_a7_1_8_violation : FALSE]
            if constexpr (!std::is_void_v<U>)
            {
                // Refer on top for suppression justification.
                // coverity[autosar_cpp14_a18_9_3_violation]
                return expected<U, E>{std::in_place, std::invoke(std::forward<F>(fun), std::move(value()))};
            }
            else
            {
                std::invoke(std::forward<F>(fun), std::move(value()));
                return expected<U, E>{};
            }
        }
        else
        {
            // Refer on top for suppression justification.
            // coverity[autosar_cpp14_a18_9_3_violation]
            return expected<U, E>{unexpect, std::move(error())};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    [[nodiscard]] constexpr auto transform_error(F&& fun) &
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(error())>>>;

        if (!has_value())
        {
            return expected<T, G>{unexpect, std::invoke(std::forward<F>(fun), error())};
        }
        else
        {
            return expected<T, G>{std::in_place, value()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto transform_error(F&& fun) const&
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(error())>>>;

        if (!has_value())
        {
            return expected<T, G>{unexpect, std::invoke(std::forward<F>(fun), error())};
        }
        else
        {
            return expected<T, G>{std::in_place, value()};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto transform_error(F&& fun) &&
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(error()))>>>;

        if (!has_value())
        {
            return expected<T, G>{unexpect, std::invoke(std::forward<F>(fun), std::move(error()))};
        }
        else
        {
            return expected<T, G>{std::in_place, std::move(value())};
        }
    }

    // DIVERGENCE FROM STANDARD: Added [[nodiscard]] for better safety
    template <typename F>
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[nodiscard]] constexpr auto transform_error(F&& fun) const&&
    {
        using G = std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<F, decltype(std::move(error()))>>>;

        if (!has_value())
        {
            // Refer on top for suppression justification.
            // coverity[autosar_cpp14_a18_9_3_violation]
            return expected<T, G>{unexpect, std::invoke(std::forward<F>(fun), std::move(error()))};
        }
        else
        {
            // Refer on top for suppression justification.
            // coverity[autosar_cpp14_a18_9_3_violation]
            return expected<T, G>{std::in_place, std::move(value())};
        }
    }

    template <typename T2, typename E2>
    // Suppress "AUTOSAR C++14 A13-5-5", The rule states: "Comparison operators shall be non-member functions with
    // identical parameter types and noexcept.". There is no functional reason behind, we require comparing `expected`
    // and `expected<T2, E2>`.
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr bool operator==(const expected& lhs, const expected<T2, E2>& rhs)
    {
        if (lhs.has_value())
        {
            return rhs.has_value() && (lhs.value() == rhs.value());
        }
        else
        {
            return (!rhs.has_value()) && (lhs.error() == rhs.error());
        }
    }

    template <typename E2>
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr bool operator==(const expected& lhs, const unexpected<E2>& rhs)
    {
        return (!lhs.has_value()) && static_cast<bool>(lhs.error() == rhs.error());
    }

    template <typename T2>
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a15_5_3_violation]
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr bool operator==(const expected& lhs, const T2& rhs)
    {
        return lhs.has_value() && static_cast<bool>(lhs.value() == rhs);
    }

    // DIVERGENCE FROM STANDARD
    // Inequality operators are automatically generated from C++20 onwards.

    template <typename T2, typename E2>
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr bool operator!=(const expected& lhs, const expected<T2, E2>& rhs)
    {
        return !operator==(lhs, rhs);
    }

    template <typename E2>
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr bool operator!=(const expected& lhs, const unexpected<E2>& rhs)
    {
        return !operator==(lhs, rhs);
    }

    template <typename T2>
    // Refer on top for suppression justification.
    // coverity[autosar_cpp14_a13_5_5_violation]
    // coverity[autosar_cpp14_a3_3_1_violation : FALSE]
    friend constexpr bool operator!=(const expected& lhs, const T2& rhs)
    {
        return !operator==(lhs, rhs);
    }

  private:
    // coverity[autosar_cpp14_a5_1_7_violation : FALSE]
    StorageType storage_;
};

// DIVERGENCE FROM STANDARD
// Missing template specialization for template <typename E> expected<void, E>

}  // namespace score::details

// While it's usually UB to modify namespace std, for swap we need to do that so that we can provide an overload that
// is specialized for expected.
namespace std
{

template <typename T, typename E>
// Suppress "AUTOSAR C++14 A17-6-1", The rule states: "Non-standard entities shall not be added to standard namespaces.
// While it's usually UB to modify namespace std, for swap we need to do that so that we can provide an overload that
// is specialized for expected.
// NOLINTBEGIN(cert-dcl58-cpp) See comment on the namespace above
// coverity[autosar_cpp14_a17_6_1_violation]
constexpr void swap(::score::details::expected<T, E>& lhs, ::score::details::expected<T, E>& rhs)
// NOLINTEND(cert-dcl58-cpp) See comment on the namespace above
{
    lhs.swap(rhs);
}

}  // namespace std

#endif  // SCORE_LIB_RESULT_EXPECTED_H
