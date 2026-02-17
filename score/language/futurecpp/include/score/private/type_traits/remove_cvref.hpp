/********************************************************************************
 * Copyright (c) 2017 Contributors to the Eclipse Foundation
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
/// \file
/// \copyright Copyright (c) 2017 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_REMOVE_CVREF_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_REMOVE_CVREF_HPP

#include <type_traits>

namespace score::cpp
{

///
/// \brief Removes reference, const and volatile from the given type.
///
/// If the type T is a reference type, provides the member typedef type which is the type referred to by T with
/// its topmost cv-qualifiers removed. Otherwise type is T with its topmost cv-qualifiers removed.
///
template <typename T>
struct remove_cvref
{
    /// \brief The type referred by T or T itself if it is not a reference, with top-level cv-qualifiers removed.
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template <typename T>
using remove_cvref_t = typename score::cpp::remove_cvref<T>::type;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_REMOVE_CVREF_HPP
