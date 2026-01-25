/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.StaticVector component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_STATIC_VECTOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_STATIC_VECTOR_HPP

#include <score/inplace_vector.hpp>

namespace score::cpp
{

template <typename T, std::size_t MaxSize>
using static_vector = inplace_vector<T, MaxSize>;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_STATIC_VECTOR_HPP
