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
#ifndef SCORE_MW_LOG_DETAIL_WAIT_FREE_STACK_WAIT_FREE_STACK_H
#define SCORE_MW_LOG_DETAIL_WAIT_FREE_STACK_WAIT_FREE_STACK_H

#include "score/callback.hpp"
#include "score/optional.hpp"
#include "score/span.hpp"
#include "score/concurrency/atomic/atomic_indirector.h"

#include <atomic>
#include <utility>
#include <vector>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

template <typename Element>
using FindPredicate = score::cpp::callback<bool(const Element&)>;

/// \brief Wait- and lock-free, push-only stack with fixed capacity.
template <typename Element, template <class> class AtomicIndirectorType = concurrency::atomic::AtomicIndirectorReal>
class WaitFreeStack
{
  public:
    explicit WaitFreeStack(const size_t max_number_of_elements) noexcept;

    /// \brief Inserts an element if capacity is left.
    /// Returns a reference to the element in the stack if push was successful.
    score::cpp::optional<std::reference_wrapper<Element>> TryPush(Element&& element) noexcept;

    /// \brief returns the first matching element
    /// Returns a reference to the element in the stack if element was found.
    score::cpp::optional<std::reference_wrapper<Element>> Find(const FindPredicate<Element>&) noexcept;

    using AtomicIndex = std::atomic_size_t;
    using AtomicBool = std::atomic_bool;

  private:
    std::vector<score::cpp::optional<Element>> elements_;
    std::vector<std::atomic_int> elements_written_;
    AtomicIndex write_index_;
    AtomicBool capacity_full_;
};

template <typename Element, template <class> class AtomicIndirectorType>
WaitFreeStack<Element, AtomicIndirectorType>::WaitFreeStack(const size_t max_number_of_elements) noexcept
    : elements_(max_number_of_elements),
      elements_written_(max_number_of_elements),
      write_index_{},
      capacity_full_{false}
{
}

template <typename Element, template <class> class AtomicIndirectorType>
auto WaitFreeStack<Element, AtomicIndirectorType>::TryPush(Element&& element) noexcept
    -> score::cpp::optional<std::reference_wrapper<Element>>
{
    if (capacity_full_.load())
    {
        return score::cpp::nullopt;
    }

    const auto current_write_index =
        AtomicIndirectorType<std::size_t>::fetch_add(write_index_, 1UL, std::memory_order_seq_cst);

    if (current_write_index >= elements_.size())
    {
        capacity_full_.store(true);
        return score::cpp::nullopt;
    }

    elements_[current_write_index] = std::forward<Element>(element);

    std::atomic_thread_fence(std::memory_order_release);
    elements_written_[current_write_index].store(1);

    return elements_[current_write_index].value();
}

template <typename Element, template <class> class AtomicIndirectorType>
auto WaitFreeStack<Element, AtomicIndirectorType>::Find(const FindPredicate<Element>& predicate) noexcept
    -> score::cpp::optional<std::reference_wrapper<Element>>
{
    // AUTOSAR M6-5-5 Deviation: The loop condition contains an atomic load operation.
    // Justification: The atomic load operation does not modify 'i' or any loop-control variable.
    // The rule intends to prevent side effects in the loop condition, but atomic loads are
    // read-only operations and do not introduce side effects or modifications.
    // LCOV_EXCL_BR_START : false positive
    for (auto i = 0UL; (i < elements_.size()) &&
                       // coverity[autosar_cpp14_m6_5_5_violation] see above
                       (i <= AtomicIndirectorType<std::size_t>::load(write_index_, std::memory_order_seq_cst));
         i++)
    {
        if (elements_written_[i].load() != 0)
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            if (predicate(elements_[i].value()) == true)
            {
                return elements_[i].value();
            }
        }
    }
    // LCOV_EXCL_BR_STOP : false positive

    return score::cpp::nullopt;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_WAIT_FREE_STACK_WAIT_FREE_STACK_H
