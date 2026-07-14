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

//! Atomic primitives re-export.
//! Required for testing with Loom.

#[cfg(not(loom))]
pub(crate) use core::sync::atomic::{fence, AtomicU8, AtomicUsize, Ordering};
#[cfg(loom)]
pub(crate) use loom::sync::atomic::{fence, AtomicU8, AtomicUsize, Ordering};
