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

#include "gmock/gmock.h"

namespace test
{

class HasLockOnly
{
  public:
    void lock() {}
};

class HasUnlockOnly
{
  public:
    void unlock() {}
};

class HasLockAndUnlockMismatchedSignature
{
  public:
    void lock(int&) {}
    void unlock(int*) {}
};

class BasicLockableArchetype
{
  public:
    void lock() {}
    void unlock() {}

    void swap(BasicLockableArchetype&) noexcept {}
};

class MockMutex
{
  public:
    void lock()
    {
        locked_ = true;
    }
    void unlock()
    {
        locked_ = false;
    }
    bool is_locked() const
    {
        return locked_;
    }

  private:
    bool locked_{false};
};

class MockSharedMutex : public MockMutex
{
  public:
    void lock_shared()
    {
        shared_locked_ = true;
    }
    void unlock_shared()
    {
        shared_locked_ = false;
    }
    bool is_shared_locked() const
    {
        return shared_locked_;
    }

  private:
    bool shared_locked_{false};
};

class MockMutexParameterized
{
  public:
    MockMutexParameterized(const std::string&, int) {}

    MOCK_METHOD(void, lock, (), ());
    MOCK_METHOD(void, unlock, (), ());
};

}  // namespace test
