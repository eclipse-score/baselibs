///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_HPP

#include <score/private/execution/back_binder.hpp>
#include <score/private/execution/sender_t.hpp>
#include <score/private/execution/then_sender.hpp>
#include <score/private/type_traits/remove_cvref.hpp>

#include <utility>

namespace score::cpp
{
namespace execution
{

namespace detail
{
namespace then_t_disable_adl
{

struct then_t
{
    template <typename Sender, typename Invocable>
    auto operator()(Sender&& sender, Invocable&& invocable) const
    {
        static_assert(is_sender<Sender>::value, "not a sender");
        return score::cpp::execution::detail::make_then_sender(std::forward<Sender>(sender),
                                                        std::forward<Invocable>(invocable));
    }

    template <typename Invocable>
    auto operator()(Invocable&& invocable) const
    {
        return back_binder<then_t, score::cpp::remove_cvref_t<Invocable>>{std::forward<Invocable>(invocable)};
    }
};

} // namespace then_t_disable_adl
} // namespace detail

using detail::then_t_disable_adl::then_t;

/// \brief Attaches an invocable as a continuation for an input sender’s value completion operation
///
/// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2300r7.html#design-sender-adaptor-then
///
/// The call `then(sender, invocable)` returns a sender describing the task graph described by the input sender, with an
/// added node of invoking the provided function with the values sent by the input sender as arguments. `then` is
/// guaranteed to not begin executing function until the returned sender is started.
///
/// `then` also supports the pipe syntax
///
/// ```
///     ... | then([](){});
/// ```
inline constexpr then_t then{};

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_HPP
