//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

///
/// \file
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///
/// The implementation is based on \c std::thread from https://github.com/llvm/llvm-project/tree/main/libcxx with
/// modifications listed in \c NOTICE.
///
/// \brief Score.Futurecpp.Jthread component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_JTHREAD_HPP
#define SCORE_LANGUAGE_FUTURECPP_JTHREAD_HPP

#include <score/private/thread/this_thread.hpp> // IWYU pragma: export
#include <score/private/thread/thread.hpp>      // IWYU pragma: export

#include <pthread.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

#include <score/private/thread/pthread_attr.hpp>
#include <score/private/type_traits/invoke_traits.hpp>
#include <score/private/utility/ignore.hpp>
#include <score/private/utility/make_offset_index_sequence.hpp>
#include <score/apply.hpp>
#include <score/stop_token.hpp>
#include <score/string.hpp>
#include <score/type_traits.hpp>

namespace score::cpp
{

/// \brief Represents a single thread of execution.
///
/// An implementation of the C++20 standard library class std::jthread based on Pthreads for use on POSIX operating
/// systems.
///
/// The class jthread represents a single thread of execution. It has the same general behavior as std::thread, except
/// that jthread automatically rejoins on destruction, and can be cancelled/stopped in certain situations.
///
/// Threads begin execution immediately upon construction of the associated thread object (pending any OS scheduling
/// delays), starting at the top-level function provided as a constructor argument. The return value of the top-level
/// function is ignored and if it terminates by throwing an exception, std::terminate is called. The top-level function
/// may communicate its return value or an exception to the caller via std::promise or by modifying shared variables
/// (which may require synchronization, see std::mutex and std::atomic)
///
/// Unlike std::thread, the jthread logically holds an internal private member of type score::cpp::stop_source, which maintains
/// a shared stop-state. The jthread constructor accepts a function that takes a score::cpp::stop_token as its first argument,
/// which will be passed in by the jthread from its internal stop_source. This allows the function to check if stop has
/// been requested during its execution, and return if it has.
///
/// score::cpp::jthread objects may also be in the state that does not represent any thread (after default construction, move
/// from, detach, or join), and a thread of execution may be not associated with any jthread objects (after detach).
///
/// No two score::cpp::jthread objects may represent the same thread of execution; score::cpp::jthread is not CopyConstructible or
/// CopyAssignable, although it is MoveConstructible and MoveAssignable.
class jthread
{
    template <typename T>
    using is_thread_attribute = std::disjunction<std::is_same<score::cpp::thread::stack_size_hint, score::cpp::remove_cvref_t<T>>,
                                                 std::is_same<score::cpp::thread::priority_hint, score::cpp::remove_cvref_t<T>>,
                                                 std::is_same<score::cpp::thread::name_hint, score::cpp::remove_cvref_t<T>>>;

public:
    using id = score::cpp::thread::id;
    using stack_size_hint = score::cpp::thread::stack_size_hint;
    using priority_hint = score::cpp::thread::priority_hint;
    using name_hint = score::cpp::thread::name_hint;
    using native_handle_type = score::cpp::thread::native_handle_type;

    /// \brief Creates new jthread object which does not represent a thread.
    ///
    /// \post get_id() equal to score::cpp::jthread::id() (i.e. joinable is false) and get_stop_source().stop_possible() is
    /// false
    jthread() noexcept : stop_source_{nostopstate_t{}}, native_handle_{} {}

    /// \brief Creates new jthread object and associates it with a thread of execution.
    ///
    /// The new thread of execution starts executing
    ///
    /// std::invoke(decay_copy(std::forward<Function>(f)),
    ///             get_stop_token(),
    ///             decay_copy(std::forward<Args>(args))...);
    ///
    /// if the function f accepts a score::cpp::stop_token for its first argument; otherwise it starts executing
    ///
    /// std::invoke(decay_copy(std::forward<Function>(f)),
    ///             decay_copy(std::forward<Args>(args))...);
    ///
    /// In either case, decay_copy is defined as
    ///
    /// template \<class T\>
    /// std::decay_t<T> decay_copy(T&& v) { return std::forward<T>(v); }
    ///
    /// Except that the calls to decay_copy are evaluated in the context of the caller, so that any exceptions thrown
    /// during evaluation and copying/moving of the arguments are thrown in the current thread, without starting the new
    /// thread.
    ///
    /// The completion of the invocation of the constructor synchronizes-with (as defined in std::memory_order) the
    /// beginning of the invocation of the copy of f on the new thread of execution.
    ///
    /// Thread attributes can be used to define additional implementation-defined behaviors on a `jthread` object.
    /// The following thread attributes are supported
    ///  - stack_size A hint to set the desired stack size.
    ///  - priority A hint to set the desired thread priority.
    ///  - name A hint to set the thread name.
    ///
    /// The stack_size must be have a minimum value that depend on the operating system. On Unix-like operating systems,
    /// this minimum value is defined by PTHREAD_STACK_MIN. If `stack_size` is 0 the thread attribute will be ignored.
    ///
    /// The priority must follow the platform dependent restrictions. On some platforms setting the priority needs
    /// special rights.
    ///
    /// The name must follow the platform dependent restrictions. On Linux the name length must not exceed 16
    /// characters. On QNX `_NTO_THREAD_NAME_MAX`. If the name cannot be set no error is reported.
    ///
    /// \param args The general pattern is "<attribute>{0,} <invocable>{1} <arguments>{0,}".
    ///
    /// \post get_id() not equal to score::cpp::jthread::id() (i.e. joinable is true), and get_stop_source().stop_possible() is
    /// true.
    ///
    template <typename... Args,
              typename Arg0 = typename std::tuple_element<0U, std::tuple<Args..., void>>::type,
              typename = std::enable_if_t<(sizeof...(Args) > 0U) &&
                                          (!std::is_same<score::cpp::remove_cvref_t<Arg0>, jthread>::value)>>
    explicit jthread(Args&&... args) : stop_source_{}, native_handle_{}
    {
        // According to http://wg21.link/p2019r8
        // Let `i` be the smallest value such that `decay_t<Args...[i]>` is not a thread attribute type.
        static constexpr auto i{find_first_non_thread_attribute<Args...>()};
        // If no such `i` exists, the program is ill-formed.
        static_assert(i != sizeof...(Args), "no invocable found");

        //  0 1 2 3 4 5
        //  i                   no attribute and invocable with 5 arguments
        //      i               2 attributes and invocable with 3 arguments
        //            i         5 attributes and invocable with 0 arguments
        //              i       6 attributes and no invocable -> ill-formed
        split_arguments(std::forward_as_tuple(std::forward<Args>(args)...),
                        std::make_index_sequence<i>{},
                        score::cpp::make_offset_index_sequence<i, sizeof...(Args) - i>{});
    }

    /// \brief Move constructor.
    ///
    /// Constructs the jthread object to represent the thread of execution that was represented by other. After this
    /// call other no longer represents a thread of execution.
    ///
    /// \post other.get_id() equal to score::cpp::jthread::id() and get_id() returns the value of other.get_id() prior to the
    /// start of construction
    jthread(jthread&& other) noexcept
        : stop_source_{std::move(other.stop_source_)}, native_handle_{other.native_handle_}
    {
        other.stop_source_ = stop_source{nostopstate_t{}};
        other.native_handle_ = native_handle_type{};
    }

    /// \see https://en.cppreference.com/w/cpp/thread/jthread/operator%3D
    // NOLINTNEXTLINE(bugprone-exception-escape) as per C++ Standard is `noexcept` although `join()` may throw
    jthread& operator=(jthread&& other) noexcept
    {
        if (joinable())
        {
            score::cpp::ignore = request_stop();
            join();
        }
        stop_source_ = std::move(other.stop_source_);
        native_handle_ = other.native_handle_;
        other.stop_source_ = stop_source{nostopstate_t{}};
        other.native_handle_ = native_handle_type{};
        return *this;
    }

    /// \brief The copy constructor is deleted; threads are not copyable.
    ///
    /// No two std::jthread objects may represent the same thread of execution.
    jthread(const jthread&) = delete;
    jthread& operator=(const jthread& other) = delete;

    /// \brief Destroys the jthread object.
    ///
    /// If *this has an associated thread (joinable() == true), calls request_stop() and then join().
    // NOLINTNEXTLINE(bugprone-exception-escape) as per C++ Standard is not `noexcept`
    ~jthread()
    {
        if (joinable())
        {
            score::cpp::ignore = request_stop();
            join();
        }
    }

    /// \brief Checks if the score::cpp::jthread object identifies an active thread of execution.
    ///
    /// Specifically, returns true if get_id() != score::cpp::jthread::id(). So a default constructed jthread is not joinable.
    ///
    /// A thread that has finished executing code, but has not yet been joined is still considered an active thread of
    /// execution and is therefore joinable.
    ///
    /// \return true if the jthread object identifies an active thread of execution, false otherwise
    bool joinable() const noexcept { return native_handle_ != native_handle_type{}; }

    /// \brief Returns a value of score::cpp::jthread::id identifying the thread associated with *this.
    ///
    /// \return A value of type score::cpp::jthread::id identifying the thread associated with *this. If there is no thread
    /// associated, default constructed score::cpp::jthread::id is returned.
    id get_id() const noexcept { return native_handle_; }

    /// \brief Returns the implementation defined underlying thread handle.
    // NOLINTNEXTLINE(readability-make-member-function-const) as per C++ Standard is non-const function
    native_handle_type native_handle() { return native_handle_; }

    /// \brief Returns the number of concurrent threads supported by the implementation. The value should be considered
    /// only a hint.
    ///
    /// \return Number of concurrent threads supported. If the value is not well defined or not computable, returns 0.
    static unsigned int hardware_concurrency() noexcept;

    /// \brief Blocks the current thread until the thread identified by *this finishes its execution.
    ///
    /// The completion of the thread identified by *this synchronizes with the corresponding successful return from
    /// join().
    ///
    /// No synchronization is performed on *this itself. Concurrently calling join() on the same jthread object from
    /// multiple threads constitutes a data race that results in undefined behavior.
    ///
    /// \post joinable() is false
    void join()
    {
        int status{EINVAL};
        if (joinable())
        {
            status = ::pthread_join(native_handle_, nullptr);
            if (status == 0)
            {
                native_handle_ = native_handle_type{};
            }
        }

        if (status != 0)
        {
            throw std::system_error{status, std::system_category(), "pthread_join"};
        }
    }

    /// \brief Separates the thread of execution from the jthread object, allowing execution to continue independently.
    ///
    /// Any allocated resources will be freed once the thread exits.
    ///
    /// After calling detach *this no longer owns any thread.
    ///
    /// \post joinable() is false
    void detach()
    {
        int status{EINVAL};
        if (joinable())
        {
            status = ::pthread_detach(native_handle_);
            if (status == 0)
            {
                native_handle_ = native_handle_type{};
            }
        }

        if (status != 0)
        {
            throw std::system_error{status, std::system_category(), "pthread_detach"};
        }
    }

    /// \brief Exchanges the underlying handles of two jthread objects.
    ///
    /// \param other the jthread to swap with
    void swap(jthread& other) noexcept
    {
        using std::swap;
        swap(stop_source_, other.stop_source_);
        swap(native_handle_, other.native_handle_);
    }

    /// \brief Overloads the std::swap algorithm for std::jthread.
    ///
    /// \see https://en.cppreference.com/w/cpp/thread/jthread/swap2
    friend void swap(jthread& lhs, jthread& rhs) noexcept { lhs.swap(rhs); }

    /// \brief Returns a score::cpp::stop_source with the same shared stop-state as held internally by the jthread object.
    stop_source get_stop_source() const noexcept { return stop_source_; }

    /// \brief Returns a score::cpp::stop_token associated with the same shared stop-state held internally by the jthread
    /// object.
    stop_token get_stop_token() const noexcept { return stop_source_.get_token(); }

    /// \brief Issues a stop request to the internal stop-state, if it has not yet already had stop requested.
    ///
    /// The determination is made atomically, and if stop was requested, the stop-state is atomically updated to avoid
    /// race conditions, such that:
    ///
    ///  - stop_requested() and stop_possible() can be concurrently invoked on other score::cpp::stop_tokens and
    ///    score::cpp::stop_sources of the same shared stop-state
    ///  - request_stop() can be concurrently invoked from multiple threads on the same jthread object or on other
    ///    score::cpp::stop_source objects associated with the same stop-state, and only one will actually perform the stop
    ///    request
    ///
    /// However, see the Notes section.
    ///
    /// \return true if this invocation made a stop request, otherwise false
    ///
    /// \post For a score::cpp::stop_token retrieved by get_stop_token() or a score::cpp::stop_source retrieved by get_stop_source(),
    /// stop_requested() is true.
    ///
    /// \note If the request_stop() does issue a stop request (i.e., returns true), then any score::cpp::stop_callbacks
    /// registered for the same associated stop-state will be invoked synchronously, on the same thread request_stop()
    /// is issued on. If an invocation of a callback exits via an exception, std::terminate is called.
    ///
    /// If a stop request has already been made, this function returns false. However there is no guarantee that another
    /// thread or score::cpp::stop_source object which has just (successfully) requested stop for the same stop-state is not
    /// still in the middle of invoking a score::cpp::stop_callback function.
    ///
    /// If the request_stop() does issue a stop request (i.e., returns true), then all condition variables of base type
    /// std::condition_variable_any registered with an interruptible wait for score::cpp::stop_tokens associated with the
    /// jthread's internal stop-state will be awoken.
    bool request_stop() noexcept { return stop_source_.request_stop(); }

private:
    template <typename T>
    static void* start_routine(void* arg)
    {
        // According to https://en.cppreference.com/w/cpp/thread/jthread/jthread, the completion of the invocation
        // of the constructor synchronizes-with (as defined in std::memory_order) the beginning of the invocation of the
        // copy of f on the new thread of execution. pthread_create() ensures this memory synchronization.
        // https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_12

        // This function is called by `pthread_create` which takes a callable `void* foo(void*)`. This is a C-style
        // function where the argument has to be casted to the correct type again. The original pointer type is provided
        // by the template argument `T`.
        // coverity[misra_cpp_2023_rule_8_2_6_violation : SUPPRESS]
        std::unique_ptr<T> invocable{static_cast<T*>(arg)}; // acquires ownership of *arg

        const score::cpp::pmr::string& thread_name{std::get<0U>(*invocable)};
        if (!thread_name.empty())
        {
            score::cpp::ignore = ::pthread_setname_np(::pthread_self(), thread_name.c_str());
        }

        score::cpp::apply(std::move(std::get<1U>(*invocable)), std::move(std::get<2U>(*invocable)));
        return nullptr;
    }

    template <typename F, typename... FArgs>
    void create_thread(const std::optional<stack_size_hint>& stack_size,
                       const std::optional<priority_hint>& priority,
                       const score::cpp::string_view name,
                       F&& f,
                       FArgs&&... args)
    {
        using invocable = std::tuple<score::cpp::pmr::string, std::decay_t<F>, std::tuple<std::decay_t<FArgs>...>>;
        auto p = std::make_unique<invocable>(
            name.to_string(), std::forward<F>(f), std::make_tuple(std::forward<FArgs>(args)...));

        int status{};
        if ((!stack_size.has_value()) && (!priority.has_value()))
        {
            status = ::pthread_create(&native_handle_, nullptr, &start_routine<invocable>, p.get());
        }
        else
        {
            detail::pthread_attr attr{};
            if (stack_size.has_value())
            {
                attr.set_stack_size(stack_size->value());
            }
            if (priority.has_value())
            {
                attr.set_priority(priority->value());
            }

            status = ::pthread_create(&native_handle_, &attr.native_handle(), &start_routine<invocable>, p.get());
        }

        if (status != 0)
        {
            throw std::system_error{status, std::system_category(), "pthread_create"};
        }

        score::cpp::ignore = p.release(); // ownership was transferred to start_routine()
    }

    template <typename F, typename... FArgs>
    void select_stop_token(const std::optional<stack_size_hint>& stack_size,
                           const std::optional<priority_hint>& priority,
                           const score::cpp::string_view name,
                           F&& f,
                           FArgs&&... fargs)
    {
        static_assert(std::is_invocable_v<std::decay_t<F>, std::decay_t<FArgs>...> ||
                      std::is_invocable_v<std::decay_t<F>, stop_token, std::decay_t<FArgs>...>);

        if constexpr (std::is_invocable_v<std::decay_t<F>, std::decay_t<FArgs>...>)
        {
            create_thread(stack_size, priority, name, std::forward<F>(f), std::forward<FArgs>(fargs)...);
        }
        else
        {
            create_thread(stack_size,
                          priority,
                          name,
                          std::forward<F>(f),
                          stop_source_.get_token(),
                          std::forward<FArgs>(fargs)...);
        }
    }

    template <typename... Ts>
    static constexpr std::size_t find_first_non_thread_attribute()
    {
        constexpr std::array<bool, sizeof...(Ts)> mask{is_thread_attribute<Ts>::value...};
        for (std::size_t i{0}; i < mask.size(); ++i)
        {
            if (!mask[i])
            {
                return i;
            }
        }
        return sizeof...(Ts);
    }

    template <typename Attr, typename... Ts>
    struct is_unique_attribute : std::true_type
    {
    };
    template <typename Attr, typename T, typename... Ts>
    struct is_unique_attribute<Attr, T, Ts...> : std::integral_constant<bool,
                                                                        !std::is_same_v<Attr, score::cpp::remove_cvref_t<T>> //
                                                                            && is_unique_attribute<Attr, Ts...>::value>
    {
    };

    template <typename Attr>
    static const Attr* get_attribute()
    {
        return nullptr;
    }
    template <typename Attr, typename T, typename... Ts>
    static const Attr* get_attribute(const T& head, const Ts&... tail)
    {
        if constexpr (std::is_same_v<T, Attr>)
        {
            static_assert(is_unique_attribute<Attr, Ts...>::value, "attribute is not unique");
            return &head;
        }
        return get_attribute<Attr>(tail...);
    }

    template <typename Tuple, size_t... As, size_t... Fs, typename = std::enable_if_t<(sizeof...(Fs) > 0U)>>
    void split_arguments(Tuple&& all_args, std::index_sequence<As...>, std::index_sequence<Fs...>)
    {
        static_assert(sizeof...(As) + sizeof...(Fs) == std::tuple_size_v<Tuple>);

        std::optional<stack_size_hint> stack_size{};
        std::optional<priority_hint> priority{};
        score::cpp::string_view name{""};

        if (const auto* const attr{get_attribute<stack_size_hint>(std::get<As>(all_args)...)}; attr != nullptr)
        {
            // According to http://wg21.link/p2019r8
            //  If `size == 0` is true the thread attribute should be ignored.
            if (attr->value() != 0U)
            {
                stack_size = *attr;
            }
        }
        if (const auto* const attr{get_attribute<priority_hint>(std::get<As>(all_args)...)}; attr != nullptr)
        {
            priority = *attr;
        }
        if (const auto* const attr{get_attribute<name_hint>(std::get<As>(all_args)...)}; attr != nullptr)
        {
            name = attr->value();
        }

        select_stop_token(stack_size, priority, name, std::get<Fs>(std::forward<Tuple>(all_args))...);
    }

    stop_source stop_source_;
    native_handle_type native_handle_;
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_JTHREAD_HPP
