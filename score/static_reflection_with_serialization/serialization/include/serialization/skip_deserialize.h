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
#ifndef SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_SKIP_DESERIALIZE_H
#define SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_SKIP_DESERIALIZE_H

#include "serialization/for_logging.h"

namespace score
{
namespace common
{
namespace visitor
{

template <typename S>
struct skip_deserialize_serialized
{
    static_assert(is_serialized_type<std::remove_cv_t<S>>::value, "element type is not serialized type");
    S ignore;
};

template <typename S>
struct is_serialized_type<skip_deserialize_serialized<S>> : public std::true_type
{
};

template <typename T>
struct skip_deserialize
{
};

/* KW_SUPPRESS_START: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
template <typename A, typename S, typename T>
inline void serialize(const T& /*unused*/, serializer_helper<A>& /*unused*/, skip_deserialize_serialized<S>& /*unused*/)
{
    // static_assert(false), but needs to depend on template parameter to delay evaluation
    static_assert(sizeof(T*) == 0, "this filter type shall never be serialized itself");
}

template <typename A, typename S, typename T>
inline void deserialize(const skip_deserialize_serialized<S>& /*unused*/,
                        deserializer_helper<A>& /*unused*/,
                        T& /*unused*/)
{
    // intentionally empty
}

template <typename A, typename T>
struct skip_deserialize_serialized_descriptor
{
    using payload_tag = typename serialized_descriptor_t<A, T>::payload_tag;
    using payload_type = skip_deserialize_serialized<typename serialized_descriptor_t<A, T>::payload_type>;
};

template <typename A, typename T>
inline auto visit_as(serialized_visitor<A>& /*unused*/, const skip_deserialize<T>& /*unused*/)
{
    return skip_deserialize_serialized_descriptor<A, T>();
}
/* KW_SUPPRESS_END: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */

// This is supposed to be a guard protecting against incompatibilities between the payload layout
// of the original serialized type and the payload layout of the corresponding types containing
// skip_serialized members. The major use case is to help detect mismatched changes in the layouts
// of the related types if the original serialized type is modified during development.
// The current functionality of the guard is enough for compile-time detection of addition or
// removal of the members in the top-level structure. More thorough (recursive) analysis at compile
// time is possible and "nice to have", but complicated. It could be implemented in the future.
template <typename T1, typename T2>
constexpr inline bool is_payload_compatible()
{
    return sizeof(typename logging_serialized_descriptor<T1>::payload_type) ==
           sizeof(typename logging_serialized_descriptor<T2>::payload_type);
}

}  // namespace visitor
}  // namespace common
}  // namespace score

#endif  // SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_SKIP_DESERIALIZE_H
