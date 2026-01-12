///
/// \file
/// \copyright Copyright (C) 2021, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// \brief Score.Futurecpp.StopToken component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_TOKEN_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_TOKEN_HPP

#include <score/private/stop_token/stop_state.hpp>

#include <memory>
#include <utility>

namespace score::cpp
{
class stop_source;
class stop_callback;

/// \brief The stop_token class provides the means to check if a stop request has been made or can be made, for its
/// associated score::cpp::stop_source object. It is essentially a thread-safe "view" of the associated stop-state.
///
/// The stop_token can also be passed to the constructor of score::cpp::stop_callback, such that the callback will be invoked
/// if the stop_token's associated score::cpp::stop_source is requested to stop. And stop_token can be passed to the
/// interruptible waiting functions of std::condition_variable_any, to interrupt the condition variable's wait if stop
/// is requested.
class stop_token
{
public:
    /// \brief Constructs an empty stop_token with no associated stop-state.
    ///
    /// \post stop_possible() and stop_requested() are both false
    stop_token() noexcept = default;

    /// \brief Copy constructor. Constructs a stop_token whose associated stop-state is the same as that of other.
    ///
    /// \param other another stop_token object to construct this stop_token object with
    /// \post *this and other share the same associated stop-state and compare equal
    stop_token(const stop_token& other) noexcept = default;

    /// \brief Move constructor. Constructs a stop_token whose associated stop-state is the same as that of other;
    /// other is left empty.
    ///
    /// \param other another stop_token object to construct this stop_token object with
    /// \post *this has other's previously associated stop-state, and other.stop_possible() is false
    stop_token(stop_token&& other) noexcept = default;

    /// \brief Copy-assigns the associated stop-state of other to that of *this. Equivalent to
    /// stop_token(other).swap(*this)
    ///
    /// \param other another stop_token object to construct this stop_token object with
    /// \return The constructed stop_token
    stop_token& operator=(const stop_token& other) noexcept = default;

    /// \brief Move-assigns the associated stop-state of other to that of *this. After the assignment, *this contains
    /// the previous associated stop-state of other, and other has no associated stop-state. Equivalent to
    /// stop_token(std::move(other)).swap(*this)
    ///
    /// \param other another stop_token object to construct this stop_token object with
    /// \return The constructed stop_token
    stop_token& operator=(stop_token&& other) noexcept = default;

    /// \brief Destroys the stop_token object.
    ///
    /// If *this has associated stop-state, releases ownership of it.
    ~stop_token() = default;

    /// \brief Exchanges the associated stop-state of *this and other.
    ///
    /// \param other stop_token to exchange the contents with
    void swap(stop_token& other) noexcept { std::swap(state_, other.state_); }

    /// \brief Checks if the stop_token object has associated stop-state and that state has received a stop request.
    /// A default constructed stop_token has no associated stop-state, and thus has not had stop requested.
    ///
    /// \return true if the stop_token object has associated stop-state and it received a stop request, false otherwise
    bool stop_requested() const noexcept
    {
        if (state_ != nullptr)
        {
            return state_->stop_requested();
        }
        return false;
    }

    /// \brief Checks if the stop_token object has associated stop-state, and that state either has already had a
    /// stop requested or it has associated score::cpp::stop_source object(s).
    ///
    /// A default constructed stop_token has no associated stop-state, and thus cannot be stopped; associated
    /// stop-state for which no score::cpp::stop_source object(s) exist can also not be stopped if such a request
    /// has not already been made.
    ///
    /// \details If the stop_token object has associated stop-state and a stop request has already been made, this
    /// function still returns true.
    ///
    /// If the stop_token object has associated stop-state from a score::cpp::jthread - for example the stop_token was
    /// retrieved by invoking get_stop_token() on a score::cpp::jthread object - then this function always returns true. A
    /// score::cpp::jthread always has an internal score::cpp::stop_source object even if the thread's invoking function does not
    /// check it.
    ///
    /// \return false if the stop_token object has no associated stop-state, or it did not yet receive a stop request
    /// and there are no associated score::cpp::stop_source object(s); true otherwise
    bool stop_possible() const noexcept { return state_ != nullptr && state_->stop_possible(); }

    /// \brief Compares two stop_token objects.
    ///
    /// This function is not visible to ordinary unqualified or qualified lookup, and can only be found by
    /// argument-dependent lookup when score::cpp::stop_token is an associated class of the arguments.
    ///
    /// \param lhs stop_tokens to compare
    /// \param rhs stop_tokens to compare
    /// \return true if lhs and rhs have the same associated stop-state, or both have no associated
    /// stop-state, otherwise false
    friend bool operator==(const stop_token& lhs, const stop_token& rhs) noexcept { return lhs.state_ == rhs.state_; }

    /// \brief Compares two stop_token objects.
    ///
    /// \param lhs stop_tokens to compare
    /// \param rhs stop_tokens to compare
    /// \return true if lhs and rhs have the a different associated stop-state, otherwise false
    friend bool operator!=(const stop_token& lhs, const stop_token& rhs) noexcept { return lhs.state_ != rhs.state_; }

    /// \brief Overloads the std::swap algorithm for score::cpp::stop_token. Exchanges the associated stop-state of lhs with
    /// that of rhs. Effectively calls lhs.swap(rhs).
    ///
    /// This function is not visible to ordinary unqualified or qualified lookup, and can only be found by
    /// argument-dependent lookup when score::cpp::stop_token is an associated class of the arguments.
    ///
    /// \param lhs stop_tokens to swap
    /// \param rhs stop_tokens to swap
    friend void swap(stop_token& lhs, stop_token& rhs) noexcept { std::swap(lhs.state_, rhs.state_); }

private:
    friend class stop_callback;
    friend class stop_source;
    explicit stop_token(std::shared_ptr<detail::stop_state> state) : state_{std::move(state)} {}

    std::shared_ptr<detail::stop_state> state_{nullptr};
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_STOP_TOKEN_HPP
