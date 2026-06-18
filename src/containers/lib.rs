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

extern crate alloc;

pub mod fixed_capacity;
pub(crate) mod generic;
pub mod inline;
pub(crate) mod storage;

use core::fmt;

/// Indicates that an operation failed because the container doesn't have enough remaining capacity.
///
/// Note that this doesn't necessarily mean that the container is full.
#[derive(Clone, Copy, Default, Debug)]
pub struct InsufficientCapacity;

impl fmt::Display for InsufficientCapacity {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "insufficient capacity for this operation")
    }
}

impl core::error::Error for InsufficientCapacity {}
