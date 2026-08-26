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

//! Builds the CXX bridge and its FutureCpp stop-token dependencies for Cargo.

use std::path::PathBuf;

fn main() {
    let repository_root = PathBuf::from(std::env::var_os("CARGO_MANIFEST_DIR").unwrap())
        .ancestors()
        .nth(4)
        .unwrap()
        .to_owned();
    let futurecpp = repository_root.join("score/language/futurecpp");

    cxx_build::bridge("src/lib.rs")
        .file("cpp/stop_token_adapter.cpp")
        .file(futurecpp.join("src/assert.cpp"))
        .file(futurecpp.join("src/stop_token.cpp"))
        .include(&repository_root)
        .include(futurecpp.join("include"))
        .std("c++17")
        .compile("stop_token_cxxbridge");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=cpp/stop_token_adapter.cpp");
    println!("cargo:rerun-if-changed=cpp/stop_token_adapter.h");
    println!("cargo:rerun-if-changed=../../futurecpp/src/assert.cpp");
    println!("cargo:rerun-if-changed=../../futurecpp/src/stop_token.cpp");
    println!("cargo:rerun-if-changed=../../futurecpp/include/score/stop_token.hpp");
}
