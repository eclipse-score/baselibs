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

#[derive(Copy, Clone)]
struct Stale {}

#[libcpp::import]
type MyVariant = std::variant<i32, f64, Stale>;

#[test]
fn use_cpp_variant() {
    let v1 = MyVariant::from_0(42);
    assert_eq!(v1.get_index(), 0);
    assert_eq!(*v1.get_0(), 42);
}

#[test]
#[should_panic]
fn use_cpp_variant_wrong_index() {
    let v1 = MyVariant::from_0(42);
    assert_eq!(v1.get_index(), 0);
    // This should panic because the variant holds an i32, not an f64
    let _ = *v1.get_1();
}
