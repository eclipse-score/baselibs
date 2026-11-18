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

//! Builds ffi_test_helpers.cpp and its score::Result/futurecpp dependencies for Cargo.
//!
//! Bazel links these via `rust_test(deps = [":ffi_test_helpers"])`; Cargo has no equivalent
//! dependency graph, so this build script compiles the same sources directly.

use std::path::PathBuf;

fn main() {
    let repository_root = PathBuf::from(std::env::var_os("CARGO_MANIFEST_DIR").unwrap())
        .ancestors()
        .nth(3)
        .unwrap()
        .to_owned();
    let result = repository_root.join("score/result");
    let futurecpp = repository_root.join("score/language/futurecpp");

    cc::Build::new()
        .cpp(true)
        .file("ffi_test_helpers.cpp")
        .file(result.join("error.cpp"))
        .file(result.join("error_code.cpp"))
        .file(result.join("error_domain.cpp"))
        .file(result.join("details/expected/expected.cpp"))
        .file(result.join("details/expected/extensions.cpp"))
        .file(futurecpp.join("src/assert.cpp"))
        .include(&repository_root)
        .include(futurecpp.join("include"))
        .std("c++17")
        .compile("result_rs_ffi_test_helpers");

    println!("cargo:rerun-if-changed=ffi_test_helpers.cpp");
    println!("cargo:rerun-if-changed=../error.cpp");
    println!("cargo:rerun-if-changed=../error.h");
    println!("cargo:rerun-if-changed=../error_code.cpp");
    println!("cargo:rerun-if-changed=../error_code.h");
    println!("cargo:rerun-if-changed=../error_domain.cpp");
    println!("cargo:rerun-if-changed=../error_domain.h");
    println!("cargo:rerun-if-changed=../result.h");
    println!("cargo:rerun-if-changed=../details/expected/expected.cpp");
    println!("cargo:rerun-if-changed=../details/expected/expected.h");
    println!("cargo:rerun-if-changed=../details/expected/extensions.cpp");
    println!("cargo:rerun-if-changed=../details/expected/extensions.h");
    println!("cargo:rerun-if-changed=../../language/futurecpp/src/assert.cpp");
}
