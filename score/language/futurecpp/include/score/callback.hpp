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
/// \brief Score.Futurecpp.Callback component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_CALLBACK_HPP
#define SCORE_LANGUAGE_FUTURECPP_CALLBACK_HPP

#include <score/private/functional/move_only_function.hpp>

namespace score::cpp
{

template <class Signature,
          std::size_t Capacity = score::cpp::detail::default_capacity(),
          std::size_t Alignment = alignof(std::max_align_t)>
using callback = score::cpp::move_only_function<Signature, Capacity, Alignment>;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_CALLBACK_HPP
