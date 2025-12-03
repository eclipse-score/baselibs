///
/// @file
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CONNECT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CONNECT_HPP

#include <score/private/execution/operation_state_t.hpp>
#include <score/private/execution/receiver_t.hpp>
#include <score/private/execution/sender_t.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{

namespace detail
{
namespace connect_t_disable_adl
{

struct connect_t
{
    template <typename Sender, typename Receiver>
    auto operator()(Sender&& s, Receiver&& r) const
    {
        static_assert(is_sender<Sender>::value, "not a sender");
        static_assert(is_receiver<Receiver>::value, "not a receiver");
        using return_type = decltype(std::forward<Sender>(s).connect(std::forward<Receiver>(r)));
        static_assert(is_operation_state<return_type>::value, "not an operation-state");

        return std::forward<Sender>(s).connect(std::forward<Receiver>(r));
    }
};

} // namespace connect_t_disable_adl
} // namespace detail

using detail::connect_t_disable_adl::connect_t;

/// \brief Connects senders with receivers.
///
/// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2300r7.html#design-connect
///
/// `connect_t` denotes a customization point. Define the customization point as a member function with `connect_t` as
/// the first argument. Follows https://wg21.link/p2855r1.
///
/// \return An operation state that will ensure that if start is called that one of the completion operations will be
/// called on the receiver passed to connect.
inline constexpr connect_t connect{};

template <typename S, typename R>
using connect_result_t = decltype(score::cpp::execution::connect(std::declval<S>(), std::declval<R>()));

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CONNECT_HPP
