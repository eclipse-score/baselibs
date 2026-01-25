/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_BACK_BINDER_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_BACK_BINDER_HPP

#include <score/private/execution/sender_adaptor_closure.hpp>
#include <score/private/execution/sender_t.hpp>
#include <score/private/type_traits/invoke_traits.hpp>
#include <score/private/type_traits/remove_cvref.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{
namespace detail
{

// similar to `std::bind_back`: https://en.cppreference.com/w/cpp/utility/functional/bind_front
template <typename Function, typename BoundArg>
class back_binder : public sender_adaptor_closure<back_binder<Function, BoundArg>>
{
    static_assert(std::is_object<Function>::value, "function is not an object type");
    static_assert(std::is_object<BoundArg>::value, "argument is not an object type");

    template <typename T>
    using is_forwarding_ref_overload_for_back_binder = std::is_same<back_binder, score::cpp::remove_cvref_t<T>>;

public:
    template <typename U, typename = std::enable_if_t<!is_forwarding_ref_overload_for_back_binder<U>::value>>
    explicit back_binder(U&& arg) : bound_arg_{std::forward<U>(arg)}
    {
    }

    template <typename Sender>
    auto operator()(Sender&& sender) &
    {
        static_assert(is_invocable<Function, Sender, BoundArg&>::value, "not invocable");
        static_assert(is_sender<Sender>::value, "not a sender");
        return Function{}(std::forward<Sender>(sender), bound_arg_);
    }

    template <typename Sender>
    auto operator()(Sender&& sender) &&
    {
        static_assert(is_invocable<Function, Sender, BoundArg>::value, "not invocable");
        static_assert(is_sender<Sender>::value, "not a sender");
        return Function{}(std::forward<Sender>(sender), std::move(bound_arg_));
    }

private:
    BoundArg bound_arg_;
};

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_BACK_BINDER_HPP
