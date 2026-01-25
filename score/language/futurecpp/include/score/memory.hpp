/********************************************************************************
 * Copyright (c) 2018 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2018 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Memory component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MEMORY_HPP
#define SCORE_LANGUAGE_FUTURECPP_MEMORY_HPP

#include <score/private/memory/make_obj_using_allocator.hpp>      // IWYU pragma: export
#include <score/private/memory/make_shared.hpp>                   // IWYU pragma: export
#include <score/private/memory/uninitialized_move.hpp>            // IWYU pragma: export
#include <score/private/memory/uninitialized_value_construct.hpp> // IWYU pragma: export
#include <score/private/memory/unique_ptr.hpp>                    // IWYU pragma: export

#include <score/utility.hpp> // to be removed but removing them may break downstream

#endif // SCORE_LANGUAGE_FUTURECPP_MEMORY_HPP
