// *******************************************************************************
// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// <https://www.apache.org/licenses/LICENSE-2.0>
//
// SPDX-License-Identifier: Apache-2.0
// *******************************************************************************

//! Unified `errno` access.

#[cfg(target_os = "linux")]
use libc::__errno_location as errno_ptr;

#[cfg(target_os = "nto")]
use libc::__get_errno_ptr as errno_ptr;

/// Current errno value.
pub fn errno() -> crate::c_int {
    // SAFETY: `errno_ptr` returns a pointer to a thread-local variable.
    unsafe { *errno_ptr() }
}
