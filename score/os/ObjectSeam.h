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
#ifndef SCORE_LIB_OS_OBJECTSEAM_H
#define SCORE_LIB_OS_OBJECTSEAM_H

#include <utility>

namespace score
{
namespace os
{

/// \brief Encapsulates helper methods for the object-seem approach to reduce code duplication
/// \tparam Object The object where the helper methods shall be available.
template <typename Object>
class ObjectSeam  // LCOV_EXCL_LINE: tooling issue
{
  public:
    /// \brief Interface for which a testing instance can be injected. Corresponds to the Object type parameter.
    using Interface = Object;

    ObjectSeam() = default;
    virtual ~ObjectSeam() = default;

    /// \brief Enables the injection of an user-owned testing instance. Without transferring ownership.
    /// \warning This function is not thread safe
    /// \param object the object that shall be returned by the singleton
    /// \post instance() will return afterwards object
    static void set_testing_instance(Object& object) noexcept
    {
        get_local_instance() = &object;
    }

    /// \brief Removes any instance that was set by set_testing_instance()
    /// \warning This function is not thread safe
    /// \post instance() will return production instance again and clear instance set by set_testing_instance()
    static void restore_instance() noexcept
    {
        get_local_instance() = nullptr;  // Next invocation of instance() will restore the pointer.
    }

  protected:
    /// \detail Invoking this function is thread safe stand-alone (as used in production code)
    ///         Invoking `restore_instance()` or `set_testing_instance()` is _not_ safe!
    static Object& select_instance(Object& instance)
    {
        if (get_local_instance() != nullptr)
        {
            return *get_local_instance();
        }
        return instance;
    }

    ObjectSeam(const ObjectSeam&) = default;
    ObjectSeam& operator=(const ObjectSeam&) = default;
    ObjectSeam(ObjectSeam&&) noexcept = default;
    ObjectSeam& operator=(ObjectSeam&&) noexcept = default;

  private:
    static Object*& get_local_instance() noexcept
    {
        static Object* instance_;
        return instance_;
    }
};

/// \brief Helper class that will automatically register an object as the test object for the specified interface.
///
/// Instead of manually registering a mock instance with the corresponding interface, you can use this wrapper to
/// instantiate and register the mock class and automatically unregister it on destruction. This way, you simply add
/// a class member to your test fixture class and it will automatically be used as the test instance.
///
/// The class is not movable because registering a test object will take the address of the instance and register it.
/// This class could be made movable by simply re-registering during the move, but for the envisioned use case this
/// isn't necessary.
///
/// \tparam M Name of the (mock) class to be registered
/// \tparam IF Interface for which the class shall be registered as the testing instance. If the mock class derives from
/// the class that derives from ObjectSeam, this parameter doesn't need to be specified.
template <typename M, typename IF = typename M::Interface>
class MockGuard
{
  public:
    MockGuard(const MockGuard&) = delete;
    MockGuard(MockGuard&& other) = delete;

    MockGuard& operator=(const MockGuard&) = delete;
    MockGuard& operator=(MockGuard&& other) = delete;

    /// \brief Construct and register the mock instance.
    /* std::forward is required here in forwarding references as we have variadic arguments */
    template <typename... Args>
    explicit MockGuard(Args&&... args) : mock_(std::forward<Args>(args)...)
    {
        IF::set_testing_instance(mock_);
    }

    /// \brief Unregister the mock instance.
    ~MockGuard() noexcept
    {
        IF::restore_instance();
    }

    /// \brief Access the wrapped mock instance.
    const M& operator*() const noexcept
    {
        return mock_;
    }

    /// \brief Access the wrapped mock instance.
    M& operator*() noexcept
    {
        return mock_;
    }

    /// \brief Access member of the wrapped mock instance.
    const M* operator->() const noexcept
    {
        return &mock_;
    }

    /// \brief Access member of the wrapped mock instance.
    M* operator->() noexcept
    {
        return &mock_;
    }

  private:
    M mock_;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_OBJECTSEAM_H
