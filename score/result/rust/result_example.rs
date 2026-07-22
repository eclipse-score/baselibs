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

result_rs::import_cpp_results! {
    unsafe type IntResult = Result<i32> as "IntResult";
    unsafe type UniquePtrResult = Result<cxx::UniquePtr<ffi::OpaqueInt>> as "UniquePtrResult";
    unsafe type SharedPtrResult = Result<cxx::SharedPtr<ffi::OpaqueInt>> as "SharedPtrResult";
}

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("score/result/rust/result_example_cpp.h");

        type IntResult = super::IntResult;
        type OpaqueInt;
        type UniquePtrResult = super::UniquePtrResult;
        type SharedPtrResult = super::SharedPtrResult;

        #[cxx_name = "ExecuteCppFunction"]
        fn execute_cpp_function(val: &str) -> IntResult;

        #[cxx_name = "CreateUniquePtrResult"]
        fn create_unique_ptr_result() -> UniquePtrResult;

        #[cxx_name = "CreateSharedPtrResult"]
        fn create_shared_ptr_result() -> SharedPtrResult;

        fn get_i32(self: &OpaqueInt) -> i32;
    }

    impl UniquePtr<OpaqueInt> {}
    impl SharedPtr<OpaqueInt> {}
}

#[test]
fn create_and_check_result() {
    let res = ffi::execute_cpp_function("42");
    let rust_result: Result<i32, result_rs::Error> = res.into();
    assert!(rust_result.is_ok());
    assert_eq!(rust_result.unwrap(), 42);
}

#[test]
fn create_error_result() {
    let res = ffi::execute_cpp_function("not a number");
    let rust_result: Result<i32, result_rs::Error> = res.into();
    assert!(rust_result.is_err());
    let err = rust_result.err().unwrap();
    assert_eq!(err.code(), 17);
    assert_eq!(
        err.message().to_str().expect("Unable to convert message to UTF-8"),
        "Dummy message"
    );
}

#[test]
fn create_unique_ptr_result_test() {
    let res = ffi::create_unique_ptr_result();
    let rust_result: Result<cxx::UniquePtr<ffi::OpaqueInt>, result_rs::Error> = res.into();
    assert!(rust_result.is_ok());
    let unique_ptr = rust_result.unwrap();
    assert!(!unique_ptr.is_null());
    let value = unique_ptr.get_i32();
    assert_eq!(value, 15);
}

#[test]
fn create_shared_ptr_result_test() {
    let res = ffi::create_shared_ptr_result();
    let rust_result: Result<cxx::SharedPtr<ffi::OpaqueInt>, result_rs::Error> = res.into();
    assert!(rust_result.is_ok());
    let shared_ptr = rust_result.unwrap();
    assert!(!shared_ptr.is_null());
    let value = shared_ptr.get_i32();
    assert_eq!(value, 55);
    println!("Test is over");
}
