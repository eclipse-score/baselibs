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
/// \brief Score.Futurecpp.MoveOnlyFunction component
///
///
/// \details Move-only heap-free replacement of std::function
/// This is a combination of
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4543.pdf (unique_function)
/// and
/// https://github.com/WG21-SG14/SG14/blob/master/Docs/Proposals/NonAllocatingStandardFunction.pdf (inplace_function)
///
/// It is not complaint to neither one of them, since they both are not part of the standard yet
/// and neither fulfills our requirements. Which are:
/// * support move-only
/// * no heap allocation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_MOVE_ONLY_FUNCTION_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_MOVE_ONLY_FUNCTION_HPP

#include <score/private/functional/invoke.hpp>
#include <score/private/type_traits/is_move_only_function.hpp>
#include <score/type_traits.hpp>

#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace score::cpp
{

namespace detail
{

/// \brief The default capacity that score::cpp::move_only_function is using for its internal storage
constexpr std::size_t default_capacity() { return 32U; }

} // namespace detail

/// \brief general-purpose polymorphic stack-based function wrapper
///
/// \tparam Signature Must meet the requirements Callable and MoveConstructable
/// \tparam Capacity The capacity of the storage that will be allocated on the stack
/// \tparam Alignment The alignment used for the stack allocation
template <class Signature,
          std::size_t Capacity = score::cpp::detail::default_capacity(),
          std::size_t Alignment = alignof(std::max_align_t)>
class move_only_function;

template <typename ReturnType, typename... ArgTypes, std::size_t Capacity, std::size_t Alignment>
class move_only_function<ReturnType(ArgTypes...), Capacity, Alignment>
{
private:
    template <typename U>
    using is_forwarding_ref_overload_for_move_only_function = std::is_same<move_only_function, score::cpp::remove_cvref_t<U>>;

    template <typename VT>
    using is_callable_nullptr_checkable =
        std::integral_constant<bool,
                               std::is_function<std::remove_pointer_t<VT>>::value ||
                                   std::is_member_pointer<VT>::value || is_move_only_function<VT>::value>;

    template <typename VT>
    static constexpr bool is_callable(const VT&, std::false_type)
    {
        return true;
    }

    template <typename VT, typename... Args>
    static constexpr bool is_callable(const VT& f, std::true_type)
    {
        return f != nullptr;
    }

    /// \brief Base class that acts as type erasure
    class wrapper_base
    {
    public:
        wrapper_base() = default;
        virtual ~wrapper_base() = default;

        /// \brief Invokes the underlying stored callable
        ///
        /// \param args Arguments necessary to invoke underlying stored callable
        /// \return Return value of the underlying stored callable
        virtual ReturnType invoke(ArgTypes... args) const = 0;

        /// \brief Helper function to move (relocate) the underlying stored callable
        ///
        /// \param destination memory region to move the callable
        /// \return typed pointer to object in new source memory region
        virtual wrapper_base* relocate(void* destination) = 0;

    protected:
        wrapper_base(const wrapper_base&) = default;
        wrapper_base& operator=(const wrapper_base&) = default;
        wrapper_base(wrapper_base&&) noexcept = default;
        wrapper_base& operator=(wrapper_base&&) noexcept = default;
    };

    /// \brief typed child class of wrapper_base that actually stored the callable
    ///
    /// \tparam Callable Type of the callable that shall be stored
    template <typename Callable>
    class wrapper : public wrapper_base
    {
        template <typename U>
        using is_forwarding_ref_overload_for_wrapper = std::is_same<wrapper, score::cpp::remove_cvref_t<U>>;

        mutable Callable callback_;

    public:
        /// \brief Constructs the wrapper by moving the callable inside
        ///
        /// \param callback The specific callable that shall be stored
        template <typename T, typename = std::enable_if_t<!is_forwarding_ref_overload_for_wrapper<T>::value>>
        explicit wrapper(T&& callback) noexcept(std::is_nothrow_move_constructible<Callable>::value)
            : wrapper_base(), callback_(std::forward<T>(callback))
        {
        }

        ~wrapper() override = default;

        ReturnType invoke(ArgTypes... args) const override
        {
            return score::cpp::detail::invoke(callback_, std::forward<ArgTypes>(args)...);
        }

        wrapper_base* relocate(void* destination) override
        {
            return ::new (destination) wrapper<Callable>{std::move(this->callback_)};
        }
    };

public:
    /// \brief The capacity this instance storage holds on the stack
    using capacity_t = std::integral_constant<std::size_t, Capacity>;
    /// \brief The alignment used for the storage
    using alignment_t = std::integral_constant<std::size_t, Alignment>;

    /// \brief Default constructor. No function will be associated.
    move_only_function() noexcept = default;

    /// \brief Construct from nullptr. No function will be associated.
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    move_only_function(std::nullptr_t) noexcept : move_only_function() {}

    /// \brief Construct from any Callable
    ///
    /// \tparam Callable the type (auto deduced) of the callable that shall be stored.
    /// needs to meet the requirements Callable and MoveConstructable. If it is a member function, it must
    /// not be ref-qualified.
    /// \param f The callable that shall be used
    ///
    /// This overload only participates in overload resolution if Callable is callable with `ArgTypes` and returns
    /// `ReturnType`.
    template <typename Callable,
              typename VT = std::decay_t<Callable>,
              typename = std::enable_if_t<!is_forwarding_ref_overload_for_move_only_function<Callable>::value &&
                                          score::cpp::is_invocable_r<ReturnType, VT, ArgTypes...>::value &&
                                          score::cpp::is_invocable_r<ReturnType, VT&, ArgTypes...>::value>>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    move_only_function(Callable&& f)
    {
        using erased_type = wrapper<std::decay_t<Callable>>;

        static_assert(std::is_move_constructible<Callable>::value, "Callable is not MoveConstructable");
        static_assert(sizeof(erased_type) <= Capacity, "Configured capacity too small.");
        static_assert(Alignment % alignof(erased_type) == 0, "Configured alignment does not match.");

        if (!is_callable<VT>(f, is_callable_nullptr_checkable<VT>{}))
        {
            return;
        }

        wrapper_ = ::new (&storage_) erased_type(std::forward<Callable>(f));
    }

    /// \brief move_only_function is intentionally NOT copy constructable
    move_only_function(const move_only_function&) = delete;

    /// \brief Move Constructor. Move construct from other move_only_function instance
    ///
    /// \param other The move_only_function to construct from
    move_only_function(move_only_function&& other) noexcept
    {
        if (other.wrapper_ != nullptr)
        {
            this->wrapper_ = other.wrapper_->relocate(&storage_);
            other.wrapper_->~wrapper_base();
            other.wrapper_ = nullptr;
        }
    }

    /// \brief move_only_function is intentionally NOT copy assignable
    move_only_function& operator=(const move_only_function&) = delete;

    /// \brief Move Assignment. Move assign from other move_only_function instance
    ///
    /// \param other The move_only_function to assign from
    move_only_function& operator=(move_only_function&& other)
    {
        if (this->wrapper_ != nullptr)
        {
            this->wrapper_->~wrapper_base();
            this->wrapper_ = nullptr;
        }
        if (other.wrapper_ != nullptr)
        {
            this->wrapper_ = other.wrapper_->relocate(&storage_);
            other.wrapper_->~wrapper_base();
            other.wrapper_ = nullptr;
        }
        return *this;
    };

    ~move_only_function()
    {
        if (this->wrapper_ != nullptr)
        {
            this->wrapper_->~wrapper_base();
        }
    }

    /// \brief Invokes the underlying callable that this instance was constructed from
    ///
    /// \pre !this->empty()
    /// \param args The arguments the callable is using
    /// \return The return type of the underlying callable
    ReturnType operator()(ArgTypes... args) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(!this->empty());
        return wrapper_->invoke(std::forward<ArgTypes>(args)...);
    }

    /// \brief Check if instance holds an underlying callable
    ///
    /// \return This call returns true if it does not hold a callable.
    bool empty() const noexcept { return wrapper_ == nullptr; }

    /// \{
    /// Compares an score::cpp::move_only_function with std::nullptr_t. Empty wrappers (that is, wrappers without a callable
    /// target) compare equal, non-empty functions compare non-equal.
    friend bool operator==(const move_only_function& f, std::nullptr_t) noexcept { return f.empty(); }

    friend bool operator==(std::nullptr_t, const move_only_function& f) noexcept { return f.empty(); }

    friend bool operator!=(const move_only_function& f, std::nullptr_t) noexcept { return !f.empty(); }

    friend bool operator!=(std::nullptr_t, const move_only_function& f) noexcept { return !f.empty(); }
    /// \}

private:
    std::aligned_storage_t<Capacity, Alignment> storage_;
    wrapper_base* wrapper_{nullptr};
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_MOVE_ONLY_FUNCTION_HPP
