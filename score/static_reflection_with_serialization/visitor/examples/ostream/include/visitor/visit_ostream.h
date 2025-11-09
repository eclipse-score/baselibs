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
#ifndef SCORE_COMMON_VISITOR_EXAMPLES_OSTREAM_INCLUDE_VISITOR_VISIT_OSTREAM_H
#define SCORE_COMMON_VISITOR_EXAMPLES_OSTREAM_INCLUDE_VISITOR_VISIT_OSTREAM_H

#include "visitor/visit.h"
#include "visitor/visit_as_struct.h"

#include <ostream>
#include <tuple>
#include <utility>
#include <vector>

namespace score
{

namespace common
{

namespace visitor
{

struct ostream_visitor
{
    std::ostream& out;
    ostream_visitor(std::ostream& o) : out(o) {}
};

// fallback to existing operator<<(ostream, T), except for pointers
template <typename T, std::enable_if_t<!std::is_pointer<std::decay_t<T>>::value, int> = 0>
inline auto visit_as(ostream_visitor v, T&& t) -> decltype(static_cast<void>(v.out << std::forward<T>(t)))
{
    v.out << std::forward<T>(t);
}

template <typename S, typename... Args>
inline void visit_as_struct(ostream_visitor v, S&&, Args&&... args)
{
    using visitable = struct_visitable<S>;
    v.out << "struct " << visitable::name() << "{";

    // pre-C++17 parameter pack folding idiom
    using expander = int[];
    std::size_t i = 0;
    void(expander{(v.out << visitable::field_name(i++) << "=",
                   visit_internal(v, std::forward<Args>(args), nullptr),
                   v.out << ";",
                   0)...});

    v.out << "}";
}

template <typename T1, typename T2>
inline void visit_as(ostream_visitor v, const std::pair<T1, T2>& t)
{
    v.out << "(";
    visit_internal(v, t.first, nullptr);
    v.out << ",";
    visit_internal(v, t.second, nullptr);
    v.out << ")";
}

template <typename T>
inline void visit_tuple_continue(ostream_visitor v, const T& t)
{
    visit_internal(v, t, nullptr);
}

template <typename T, typename... Args>
inline void visit_tuple_continue(ostream_visitor v, const T& t, Args&&... args)
{
    visit_internal(v, t, nullptr);
    v.out << ";";
    // static recursion version of parameter pack folding
    visit_tuple_continue(v, std::forward<Args>(args)...);
}

template <typename... T, std::size_t... I>
inline void visit_tuple_start(ostream_visitor v, const std::tuple<T...>& t, std::index_sequence<I...>)
{
    visit_tuple_continue(v, std::get<I>(t)...);
}

template <typename... T>
inline void visit_as(ostream_visitor v, const std::tuple<T...>& t)
{
    v.out << "(";
    visit_tuple_start(v, t, std::make_index_sequence<sizeof...(T)>());
    v.out << ")";
}

template <typename T>
inline void visit_as(ostream_visitor v, const std::vector<T>& t)
{
    v.out << "[";
    for (size_t i = 0; i < t.size(); ++i)
    {
        if (i)
        {
            v.out << ';';
        }
        visit_internal(v, t[i], nullptr);
    }
    v.out << "]";
}

template <typename T, size_t N>
inline void visit_as(ostream_visitor v, const std::array<T, N>& t)
{
    v.out << "[";
    for (size_t i = 0; i < N; ++i)
    {
        if (i)
        {
            v.out << ',';
        }
        visit_internal(v, t[i], nullptr);
    }
    v.out << "]";
}

template <typename T, size_t N>
inline void visit_as(ostream_visitor v, const T (&t)[N])
{
    v.out << "[";
    for (size_t i = 0; i < N; ++i)
    {
        if (i)
        {
            v.out << ',';
        }
        visit_internal(v, t[i], nullptr);
    }
    v.out << "]";
}

}  // namespace visitor

}  // namespace common

}  // namespace score

#endif  // SCORE_COMMON_VISITOR_EXAMPLES_OSTREAM_INCLUDE_VISITOR_VISIT_OSTREAM_H
