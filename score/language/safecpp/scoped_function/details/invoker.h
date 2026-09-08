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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_INVOKER_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_INVOKER_H

#include <score/assert.hpp>
#include <score/optional.hpp>

#include <utility>

namespace score::safecpp::details
{

template <class FunctionType>
class CallOperatorInterface;

template <class ReturnTypeT, class... Args>
class CallOperatorInterface<ReturnTypeT(Args...)>
{
  public:
    using ReturnType = ReturnTypeT;

    CallOperatorInterface() = default;
    virtual ~CallOperatorInterface() = default;

    virtual ReturnType operator()(Args... args) = 0;

  protected:
    CallOperatorInterface(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface(const CallOperatorInterface&) = default;
    CallOperatorInterface& operator=(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface& operator=(const CallOperatorInterface&) = default;
};

template <class ReturnTypeT, class... Args>
class CallOperatorInterface<ReturnTypeT(Args...) const>
{
  public:
    using ReturnType = ReturnTypeT;

    CallOperatorInterface() = default;
    virtual ~CallOperatorInterface() = default;

    virtual ReturnType operator()(Args... args) const = 0;

  protected:
    CallOperatorInterface(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface(const CallOperatorInterface&) = default;
    CallOperatorInterface& operator=(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface& operator=(const CallOperatorInterface&) = default;
};

template <class ReturnTypeT, class... Args>
class CallOperatorInterface<ReturnTypeT(Args...) noexcept>
{
  public:
    using ReturnType = ReturnTypeT;

    CallOperatorInterface() = default;
    virtual ~CallOperatorInterface() = default;

    virtual ReturnType operator()(Args... args) noexcept = 0;

  protected:
    CallOperatorInterface(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface(const CallOperatorInterface&) = default;
    CallOperatorInterface& operator=(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface& operator=(const CallOperatorInterface&) = default;
};

template <class ReturnTypeT, class... Args>
class CallOperatorInterface<ReturnTypeT(Args...) const noexcept>
{
  public:
    using ReturnType = ReturnTypeT;

    CallOperatorInterface() = default;
    virtual ~CallOperatorInterface() = default;

    virtual ReturnType operator()(Args... args) const noexcept = 0;

  protected:
    CallOperatorInterface(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface(const CallOperatorInterface&) = default;
    CallOperatorInterface& operator=(CallOperatorInterface&&) noexcept = default;
    CallOperatorInterface& operator=(const CallOperatorInterface&) = default;
};

// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule states:"The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Rationale: This class is declared within non-global namespace.
template <template <bool, class> class BaseInvoker,
          template <class> class Interface,
          class Container,
          class FunctionType,
          class = std::enable_if_t<std::is_base_of_v<CallOperatorInterface<FunctionType>, Interface<FunctionType>>>>
// coverity[autosar_cpp14_m7_3_1_violation: FALSE]
class Invoker;

template <template <bool, class> class BaseInvoker,
          template <class> class Interface,
          class Container,
          class ReturnTypeT,
          class... Args>
class Invoker<BaseInvoker, Interface, Container, ReturnTypeT(Args...)> : public Interface<ReturnTypeT(Args...)>
{
  public:
    using ReturnType = ReturnTypeT;

    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    ReturnType operator()(Args... args) override
    {
        auto& container = *(static_cast<Container*>(this));
        return BaseInvoker<false, ReturnTypeT(Args...)>::template ActualInvoker<Container>()(
            container, std::forward<Args>(args)...);
    }
};

// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule states:"The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Rationale: This class is declared within non-global namespace.
template <template <bool, class> class BaseInvoker,
          template <class> class Interface,
          class Container,
          class ReturnTypeT,
          class... Args>
// coverity[autosar_cpp14_m7_3_1_violation: FALSE]
class Invoker<BaseInvoker, Interface, Container, ReturnTypeT(Args...) const>
    : public Interface<ReturnTypeT(Args...) const>
{
  public:
    using ReturnType = ReturnTypeT;

    ReturnType operator()(Args... args) const override
    {
        auto& container{*(static_cast<const Container*>(this))};
        return BaseInvoker<false, ReturnTypeT(Args...)>::template ActualInvoker<const Container>()(
            container, std::forward<Args>(args)...);
    }
};

template <template <bool, class> class BaseInvoker,
          template <class> class Interface,
          class Container,
          class ReturnTypeT,
          class... Args>
class Invoker<BaseInvoker, Interface, Container, ReturnTypeT(Args...) noexcept>
    : public Interface<ReturnTypeT(Args...) noexcept>
{
  public:
    using ReturnType = ReturnTypeT;

    ReturnType operator()(Args... args) noexcept override
    {
        auto& container{*(static_cast<Container*>(this))};
        return BaseInvoker<true, ReturnTypeT(Args...)>::template ActualInvoker<Container>()(
            container, std::forward<Args>(args)...);
    }
};

// Suppress "AUTOSAR C++14 M7-3-1" rule finding. This rule states:"The global namespace shall only contain main,
// namespace declarations and extern "C" declarations.".
// Rationale: This class is declared within non-global namespace.
template <template <bool, class> class BaseInvoker,
          template <class> class Interface,
          class Container,
          class ReturnTypeT,
          class... Args>
// coverity[autosar_cpp14_m7_3_1_violation: FALSE]
class Invoker<BaseInvoker, Interface, Container, ReturnTypeT(Args...) const noexcept>
    : public Interface<ReturnTypeT(Args...) const noexcept>
{
  public:
    using ReturnType = ReturnTypeT;

    ReturnType operator()(Args... args) const noexcept override
    {
        auto& container{*(static_cast<const Container*>(this))};
        return BaseInvoker<true, ReturnTypeT(Args...)>::template ActualInvoker<const Container>()(
            container, std::forward<Args>(args)...);
    }
};

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_INVOKER_H
