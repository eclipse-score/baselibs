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

//! Test to verify stdlib mismatch detection when C++ code is compiled with libc++.
//!
//! This test calls C++ functions compiled with libc++ (via ffi_test_helpers_libcxx)
//! while the Rust code expects libstdc++ (on Linux). This should trigger a panic
//! due to the stdlib mismatch detection in the Result FFI conversion.

use result_rs::Error;

use std::mem::MaybeUninit;

// External C++ functions compiled with libc++
extern "C" {
    fn create_result_int32_success_for_layout_test(
        test_value: i32,
        out_result: *mut result_rs::Result<i32>,
        bufsize: libc::size_t,
    ) -> bool;
}

#[test]
#[should_panic(expected = "The C++ library used by the Rust and C++ implementations differs!")]
fn test_libcxx_stdlib_mismatch_detection() {
    // Call C++ function that was compiled with libc++
    // On Linux, this should cause a mismatch because Rust code expects libstdc++
    let mut cpp_result_uninit = MaybeUninit::<result_rs::Result<i32>>::uninit();
    let success = unsafe {
        create_result_int32_success_for_layout_test(
            42,
            cpp_result_uninit.as_mut_ptr(),
            size_of_val(&cpp_result_uninit) as libc::size_t,
        )
    };
    assert!(success, "C++ function failed to create Result");
    let cpp_result = unsafe { cpp_result_uninit.assume_init() };

    // Attempt to convert to Rust Result should panic because
    // we forced C++ to use libc++ (result_libcxx.a) while Rust uses libstdc++
    let _: std::result::Result<i32, Error> = cpp_result.into();

    // Should never reach here on Linux
    #[cfg(target_os = "linux")]
    unreachable!("Expected panic due to stdlib mismatch on Linux");
}

#[test]
#[cfg(not(target_os = "linux"))]
fn test_libcxx_no_mismatch_on_qnx() {
    // On QNX, both sides use libc++, so no mismatch should occur
    let mut cpp_result_uninit = MaybeUninit::<result_rs::Result<i32>>::uninit();
    let success = unsafe {
        create_result_int32_success_for_layout_test(
            42,
            cpp_result_uninit.as_mut_ptr(),
            size_of_val(&cpp_result_uninit) as libc::size_t,
        )
    };
    assert!(success, "C++ function failed to create Result");
    let cpp_result = unsafe { cpp_result_uninit.assume_init() };
    let rust_result: std::result::Result<i32, Error> = cpp_result.into();

    assert_eq!(rust_result.unwrap(), 42);
}
