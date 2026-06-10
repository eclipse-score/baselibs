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
#ifndef SCORE_LIB_CONTAINERS_POINTER_SPY_MOCK_H
#define SCORE_LIB_CONTAINERS_POINTER_SPY_MOCK_H

#include <gmock/gmock.h>

#include <cstddef>

namespace score::containers::test
{

/// \brief A spy mock for pointer operations used together with MockablePointer.
///
/// All methods return void because MockablePointer always performs real pointer arithmetic;
/// the spy only records calls so that test expectations can verify which operations are invoked.
/// This avoids the need to set up return values (e.g. WillOnce(ReturnRef(...))) which makes
/// tests much less brittle.
template <typename T>
class PointerSpyMock
{
  public:
    using difference_type = std::ptrdiff_t;

    MOCK_METHOD(void, Arrow, (T*), (const));
    MOCK_METHOD(void, Dereference, (), (const));
    MOCK_METHOD(void, Index, (difference_type idx), (const));
    MOCK_METHOD(void, PlusAssign, (difference_type offset));
    MOCK_METHOD(void, MinusAssign, (difference_type offset));
    MOCK_METHOD(void, PreIncrement, ());
    MOCK_METHOD(void, PostIncrement, ());
    MOCK_METHOD(void, PreDecrement, ());
    MOCK_METHOD(void, PostDecrement, ());
};

}  // namespace score::containers::test

#endif  // SCORE_LIB_CONTAINERS_POINTER_SPY_MOCK_H

