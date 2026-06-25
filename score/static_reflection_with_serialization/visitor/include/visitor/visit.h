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
#ifndef SCORE_COMMON_VISITOR_INCLUDE_VISITOR_VISIT_H
#define SCORE_COMMON_VISITOR_INCLUDE_VISITOR_VISIT_H

#include <utility>

namespace score
{

namespace common
{

namespace visitor
{

struct visitable_type
{
};

inline void forgot_to_define_as_visitable(visitable_type /*unused*/) {}

// fallback code to produce a nicer compiler error if visit_as() not defined;
// the error can be avoided in coverage tests by providing a conversion operator
// from T to visitable_type
template <typename V, typename T, typename Unused>
inline void visit_internal(V&& /*unused*/, T&& t, Unused&& /*unused*/)
{
    forgot_to_define_as_visitable(std::forward<T>(t));
}
// explicit nullptr_t parameter has a priority over template parameter in overload resolution
// explicit return type is used to participate in SFINAE
template <typename V, typename T>
// Unused variables needed for correct template deduction
// coverity[autosar_cpp14_a13_3_1_violation : FALSE]
inline auto visit_internal(V&& v, T&& t, std::nullptr_t /*unused*/)
    -> decltype(visit_as(std::forward<V>(v), std::forward<T>(t)))
{
    return visit_as(std::forward<V>(v), std::forward<T>(t));
}

template <typename V, typename T>
inline auto visit(V&& v, T&& t)
{
    return visit_internal(std::forward<V>(v), std::forward<T>(t), nullptr);
}

}  // namespace visitor

}  // namespace common

}  // namespace score

#endif  // SCORE_COMMON_VISITOR_INCLUDE_VISITOR_VISIT_H
