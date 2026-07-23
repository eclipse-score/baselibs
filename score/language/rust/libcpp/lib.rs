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

//! This module provides FFI-compatible types and functions to interoperate between different
//! C++ standard library implementations and Rust.

#[cfg(not(any(feature = "libcxx_layout", feature = "libstdcpp_layout")))]
compile_error!("Either feature \"libcxx_layout\" or \"libstdcpp_layout\" must be enabled to select the correct standard type layout");

mod string_view;

pub use libcpp_derive::import;

pub use string_view::CStringView;
