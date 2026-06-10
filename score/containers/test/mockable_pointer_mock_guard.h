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
#ifndef SCORE_LIB_CONTAINERS_TEST_MOCKABLE_POINTER_MOCK_GUARD_H
#define SCORE_LIB_CONTAINERS_TEST_MOCKABLE_POINTER_MOCK_GUARD_H

#include "score/containers/test/mockable_pointer.h"
#include "score/containers/test/pointer_spy_mock.h"


namespace score::containers::test
{

/// \brief RAII guard for MockablePointer spy mock lifetime.
///
/// Sets the spy mock on construction and clears it on destruction, ensuring the mock
/// is always properly cleaned up even if the test throws an exception.
///
template <typename T>
class MockablePointerMockGuard
{
  public:
    explicit MockablePointerMockGuard(PointerSpyMock<T>* mock) noexcept
    {
        MockablePointer<T>::SetMock(mock);
    }

    ~MockablePointerMockGuard() noexcept
    {
        MockablePointer<T>::ClearMock();
    }

    MockablePointerMockGuard(const MockablePointerMockGuard&) = delete;
    MockablePointerMockGuard& operator=(const MockablePointerMockGuard&) = delete;
    MockablePointerMockGuard(MockablePointerMockGuard&&) = delete;
    MockablePointerMockGuard& operator=(MockablePointerMockGuard&&) = delete;
};

}  // namespace score::containers::test

#endif  // SCORE_LIB_CONTAINERS_TEST_MOCKABLE_POINTER_MOCK_GUARD_H