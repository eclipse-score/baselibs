/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Pmr.MemoryResource component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_FREE_LIST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_FREE_LIST_HPP

#include <score/assert.hpp>

#include <memory>
#include <utility>

namespace score::cpp
{
namespace pmr
{
namespace detail
{

class free_list
{
public:
    bool empty() const { return next_ == nullptr; }

    void* pop_front()
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return std::exchange(next_, next_->next_);
    }

    void push_front(void* const p)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(p != nullptr);
        free_list* const new_head{::new (p) free_list{}};
        new_head->next_ = next_;
        next_ = new_head;
    }

    void clear() { next_ = nullptr; }

private:
    free_list* next_{};
};

} // namespace detail
} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_FREE_LIST_HPP
