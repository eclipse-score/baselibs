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

//! Builds ffi_test_helpers.cpp for Cargo.
//!
//! Bazel links this via `rust_test(deps = [":ffi_test_helpers"])`; Cargo has no equivalent
//! dependency graph, so this build script compiles the same self-contained source directly.

fn main() {
    cc::Build::new()
        .cpp(true)
        .file("ffi_test_helpers.cpp")
        .std("c++17")
        .compile("libcpp_ffi_test_helpers");

    println!("cargo:rerun-if-changed=ffi_test_helpers.cpp");
}
