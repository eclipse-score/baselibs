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

//! Implementation of formatting library.
//! Allows creation of message frames that are not exclusively text based.
//!
//! Replacement for [`core::fmt`].

mod builders;
mod fmt;
mod fmt_impl;
#[cfg(feature = "qm")]
mod fmt_impl_qm;
mod fmt_spec;
mod macros;

pub use builders::{DebugList, DebugMap, DebugSet, DebugStruct, DebugTuple};
pub use fmt::*;
pub use fmt_spec::*;

#[cfg(test)]
mod test_utils;
