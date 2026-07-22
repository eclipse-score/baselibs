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

//! This module provides FFI-compatible types and functions to interoperate with bmw::Result and
//! associated types.
//!
//! It also offers integration with cxx, allowing type aliases of bmw::Result to be used as types
//! of arguments or return values of C++ functions and methods. See the documentation of
//! [`import_cpp_results!`] for an overview how to do this.

mod ffi {
    use libcpp::CStringView;
    use std::fmt;
    use std::mem::ManuallyDrop;

    unsafe extern "C" {
        // SAFETY: The caller must make sure that error_domain points to a valid error domain, and that
        // result points to a valid CStringView instance where the result will be written to.
        #[link_name = "LibResultErrorDomainGetMessageForErrorCode"]
        fn get_message_for_error_code(
            error_domain: *const ErrorDomain,
            error_code: ErrorCode,
            result: *mut CStringView,
        );
    }

    /// Opaque type representing bmw::ErrorDomain
    ///
    /// Since we do not support creating ErrorDomains in Rust, we only need an opaque representation
    /// here since all usages are trough references and pointers.
    #[repr(C)]
    pub struct ErrorDomain {
        _dummy: [u8; 0],
    }

    impl ErrorDomain {
        /// Retrieve a human-readable message for a certain error of a domain.
        ///
        /// error_code must be a valid number for the respective domain, otherwise the result is
        /// undefined (but not unsafe - usually some default gets returned).
        pub fn get_message_for_error_code(&self, error_code: ErrorCode) -> CStringView {
            let mut error_domain_string = CStringView::default();
            // SAFETY: Since self is valid, and we provide a valid result string, calling this
            // function is safe.
            unsafe {
                get_message_for_error_code(self, error_code, &mut error_domain_string);
            }
            error_domain_string
        }
    }

    /// Rust representation of bmw::result::ErrorCode
    pub type ErrorCode = i32;

    #[repr(C)]
    #[derive(Clone, Copy, Debug, PartialEq)]
    /// Binary compatible representation of `bmw::result::Error`
    ///
    /// This type can be safely sent across FFI boundaries where `bmw::result::Error` is expected.
    pub struct Error {
        code: ErrorCode,
        domain: *const ErrorDomain,
        message: CStringView,
    }

    impl fmt::Display for Error {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
            // SAFETY: We can assume that domain is valid as long as the Error instance is valid
            // (type invariant).
            let error_domain_string = unsafe {
                self.domain
                    .as_ref()
                    .expect("Domain is nullptr!")
                    .get_message_for_error_code(self.code)
            };
            write!(f, "Error {} ({}): {}", error_domain_string, self.code, self.message)
        }
    }

    impl Error {
        /// Create an Error from its parts, namely. This is unsafe because the caller must ensure
        /// that the `domain` pointer is valid for the lifetime of the Error instance.
        ///
        /// # Safety
        /// The caller must ensure that `domain` points to a valid bmw::error:ErrorDomain instance
        /// that lives at least as long as the returned Error instance.
        pub unsafe fn from_parts(code: ErrorCode, domain: *const ErrorDomain, message: CStringView) -> Self {
            Self { code, domain, message }
        }

        /// Returns the error code.
        pub fn code(&self) -> ErrorCode {
            self.code
        }

        /// Returns the error message given by the user during creation.
        pub fn message(&self) -> CStringView {
            self.message
        }

        /// Returns the error domain.
        ///
        /// # Panics
        /// This function will panic if the domain is null.
        pub fn domain(&self) -> &ErrorDomain {
            // SAFETY: This is safe as long as the user of from_parts ensured that domain is valid,
            // or because the Error was created in C++ and passed to Rust in a valid way.
            unsafe { self.domain.as_ref().expect("Domain not set") }
        }
    }

    impl std::error::Error for Error {}

    #[derive(Debug, PartialEq, Copy, Clone)]
    enum ExpectedDiscriminant {
        Value,
        Error,
        Stale,
    }

    const VALUE_DISCRIMINANT: usize = 0;
    const ERROR_DISCRIMINANT: usize = 1;
    const STALE_DISCRIMINANT: usize = 2;

    impl From<usize> for ExpectedDiscriminant {
        fn from(val: usize) -> Self {
            match val {
                VALUE_DISCRIMINANT => Self::Value,
                ERROR_DISCRIMINANT => Self::Error,
                STALE_DISCRIMINANT => Self::Stale,
                _ => panic!("Unknown discriminant {val}"),
            }
        }
    }

    #[repr(C)]
    #[derive(Default, Copy, Clone, Eq, PartialEq, Debug)]
    struct StaleTag {
        _dummy: u8,
    }

    #[libcpp::import]
    type Variant<T, E> = std::variant<T, E, StaleTag>;

    #[repr(C)]
    pub struct Expected<T, E> {
        variant: Variant<T, E>,
    }

    impl<T, E> Drop for Expected<T, E> {
        fn drop(&mut self) {
            match self.discriminant() {
                // SAFETY: We only manually drop if the discriminant indicates the existence of
                // either a value or an error. In the stale case, nothing will be done.
                ExpectedDiscriminant::Value => unsafe { std::mem::ManuallyDrop::drop(self.variant.get_0_mut()) },
                ExpectedDiscriminant::Error => unsafe { std::mem::ManuallyDrop::drop(self.variant.get_1_mut()) },
                ExpectedDiscriminant::Stale => (), // Do nothing, this is a moved-from result
            }
        }
    }

    impl<T, E> Expected<T, E> {
        fn value(&self) -> &T {
            self.variant.get_0()
        }

        fn error(&self) -> &E {
            self.variant.get_1()
        }

        fn discriminant(&self) -> ExpectedDiscriminant {
            ExpectedDiscriminant::from(self.variant.get_index())
        }

        /// Create an Expected instance containing a value.
        pub fn from_value(value: T) -> Self {
            Self {
                variant: Variant::from_0(value),
            }
        }

        /// Create an Expected instance containing an error.
        pub fn from_error(error: E) -> Self {
            Self {
                variant: Variant::from_1(error),
            }
        }

        /// Returns true if this instance contains a value, false if it contains an error.
        pub fn has_value(&self) -> bool {
            match self.discriminant() {
                ExpectedDiscriminant::Value => true,
                ExpectedDiscriminant::Error => false,
                ExpectedDiscriminant::Stale => {
                    unreachable!("has_value called on a moved-from value")
                },
            }
        }

        /// Returns a reference to the contained value, or None if this instance contains an error.
        pub fn get_value(&self) -> Option<&T> {
            match self.discriminant() {
                // SAFETY: We only access the value part of the union if the discriminant indicates
                // the presence of this value.
                ExpectedDiscriminant::Value => Some(self.value()),
                ExpectedDiscriminant::Error => None,
                ExpectedDiscriminant::Stale => {
                    unreachable!("get_value called on a moved-from value")
                },
            }
        }

        /// Returns a reference to the contained error, or None if this instance contains a value.
        pub fn get_error(&self) -> Option<&E> {
            match self.discriminant() {
                ExpectedDiscriminant::Value => None,
                // SAFETY: We only access the error part of the union if the discriminant indicates
                // the presence of such an error.
                ExpectedDiscriminant::Error => Some(self.error()),
                ExpectedDiscriminant::Stale => {
                    unreachable!("get_error called on a moved-from value")
                },
            }
        }
    }

    impl<T: Clone, E: Clone> Clone for Expected<T, E> {
        fn clone(&self) -> Self {
            if let Some(error) = self.get_error() {
                Self::from_error(error.clone())
            } else if let Some(value) = self.get_value() {
                Self::from_value(value.clone())
            } else {
                unreachable!("clone called on a moved-from value")
            }
        }
    }

    impl<T: fmt::Debug, E: fmt::Debug> fmt::Debug for Expected<T, E> {
        fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
            match self.discriminant() {
                ExpectedDiscriminant::Value => {
                    let value = self.get_value();
                    f.debug_tuple("Expected::Value").field(&value).finish()
                },
                ExpectedDiscriminant::Error => {
                    let error = self.get_error();
                    f.debug_tuple("Expected::Error").field(&error).finish()
                },
                ExpectedDiscriminant::Stale => write!(f, "Expected::Stale (moved-from)"),
            }
        }
    }

    impl<T: PartialEq, E: PartialEq> PartialEq for Expected<T, E> {
        fn eq(&self, other: &Self) -> bool {
            if self.discriminant() != other.discriminant() {
                return false;
            }
            // SAFETY: Since we only use the union members that fit the discriminant, this is safe.
            match self.discriminant() {
                ExpectedDiscriminant::Value => {
                    let self_value = self.value();
                    let other_value = other.value();
                    self_value == other_value
                },
                ExpectedDiscriminant::Error => {
                    let self_error = self.error();
                    let other_error = other.error();
                    self_error == other_error
                },
                ExpectedDiscriminant::Stale => true, // Both are stale, consider equal
            }
        }
    }

    impl<T: Eq, E: Eq> Eq for Expected<T, E> {}

    impl<T, E> From<Expected<T, E>> for std::result::Result<T, E> {
        fn from(mut expected: Expected<T, E>) -> Self {
            // SAFETY: Since we only use the union members that fit the discriminant, this is safe.
            let result = match expected.discriminant() {
                ExpectedDiscriminant::Value => Ok(unsafe { ManuallyDrop::take(expected.variant.get_0_mut()) }),
                ExpectedDiscriminant::Error => Err(unsafe { ManuallyDrop::take(expected.variant.get_1_mut()) }),
                ExpectedDiscriminant::Stale => unreachable!("from called on a moved-from value"),
            };

            expected.variant = Variant::from_2(StaleTag::default());
            result
        }
    }

    /// This is the main type to be used for FFI interop with `bmw::Result<T>`. It is a type alias
    /// for `Expected<T, bmw::result::Error>`. See the documentation of [`Expected`] for more
    /// details.
    pub type Result<T> = Expected<T, Error>;

    impl<T> From<T> for Result<T> {
        fn from(value: T) -> Self {
            Self::from_value(value)
        }
    }

    #[cfg(test)]
    mod tests {
        use super::*;
        use std::mem;
        use std::ptr;

        // External C functions that work with real bmw::Result instances
        extern "C" {
            fn create_bmw_result_success_int32(value: i32) -> *mut std::ffi::c_void;
            fn create_bmw_result_error_int32(
                error_code: i32,
                message: *const std::ffi::c_char,
            ) -> *mut std::ffi::c_void;
            fn create_bmw_result_success_bool(value: bool) -> *mut std::ffi::c_void;
            fn create_bmw_result_error_bool(error_code: i32, message: *const std::ffi::c_char)
                -> *mut std::ffi::c_void;

            fn destroy_bmw_result_int32(result: *mut std::ffi::c_void);
            fn destroy_bmw_result_bool(result: *mut std::ffi::c_void);

            fn get_bmw_result_int32_size() -> libc::size_t;
            fn get_bmw_result_bool_size() -> libc::size_t;
            fn get_bmw_error_size() -> libc::size_t;

            fn verify_rust_result_as_cpp_int32(
                rust_result: *const std::ffi::c_void,
                should_have_value: bool,
                expected_value_or_error_code: i32,
            ) -> bool;

            fn print_bmw_result_memory_layout();

            // FFI functions for binary layout testing
            fn create_result_int32_success_for_layout_test(
                test_value: i32,
                out_result: *mut Result<i32>,
                bufsize: libc::size_t,
            ) -> bool;
            fn create_result_int32_error_for_layout_test(
                error_code: i32,
                out_result: *mut Result<i32>,
                bufsize: libc::size_t,
            ) -> bool;
        }

        #[test]
        fn test_binary_compatibility_sizes() {
            unsafe {
                let cpp_result_int32_size = get_bmw_result_int32_size();
                let cpp_result_bool_size = get_bmw_result_bool_size();
                let cpp_error_size = get_bmw_error_size();

                let rust_result_int32_size = mem::size_of::<Result<i32>>();
                let rust_result_bool_size = mem::size_of::<Result<bool>>();
                let rust_error_size = mem::size_of::<Error>();

                println!("\n=== Size Compatibility Analysis ===");
                println!("Result<i32>:");
                println!("  C++ bmw::Result<int32_t>: {} bytes", cpp_result_int32_size);
                println!("  Rust ffi::Result<i32>:    {} bytes", rust_result_int32_size);
                println!("  Compatible: {}", cpp_result_int32_size == rust_result_int32_size);

                println!("\nResult<bool>:");
                println!("  C++ bmw::Result<bool>:    {} bytes", cpp_result_bool_size);
                println!("  Rust ffi::Result<bool>:   {} bytes", rust_result_bool_size);
                println!("  Compatible: {}", cpp_result_bool_size == rust_result_bool_size);

                println!("\nError:");
                println!("  C++ bmw::result::Error:   {} bytes", cpp_error_size);
                println!("  Rust ffi::Error:          {} bytes", rust_error_size);
                println!("  Compatible: {}", cpp_error_size == rust_error_size);

                // Print C++ memory layout for reference
                print_bmw_result_memory_layout();

                // These assertions will fail if our assumptions are wrong
                assert_eq!(
                    cpp_result_int32_size, rust_result_int32_size,
                    "Size mismatch between C++ bmw::Result<int32_t> and Rust Result<i32>"
                );
                // Note: We might need to relax this for Error and bool Result due to different layouts
            }
        }

        /* Testing the C++'s object layout cannot be done directly, as the fields are private
         * and adding "friend" declarations would pollute the public API.
         * Instead, we create test helper functions in C++ that create Result and Error instances with known values,
         * and then we read their raw bytes and "transmute" them into Rust types to verify that the layout
         * matches our expectations.
         * As the transmuting takes into consideration our expected sizes, any mismatch in layout, size or value
         * will cause a panic or assertion failure.
         * On the contrary, if everything passes, we can be reasonably sure that our layout assumptions are correct.
         */
        #[test]
        fn test_binary_layout_verification() {
            fn print_bytes_hex(label: &str, bytes: &[u8]) {
                println!("  {}:", label);
                for (i, byte) in bytes.iter().enumerate() {
                    if i % 16 == 0 {
                        print!("    ");
                    }
                    print!("{:02x} ", byte);
                    if (i + 1) % 16 == 0 {
                        println!();
                    }
                }
                if bytes.len() % 16 != 0 {
                    println!();
                }
            }

            println!("\n=== Binary Layout Verification Test ===\n");

            // Test 1: Create a Result<int32_t> with success value 0x01020304
            const TEST_VALUE: i32 = 0x01020304;
            const MAX_SIZE: usize = 40;
            println!("Test 1: Result<int32_t> success with value {:#x}", TEST_VALUE);

            unsafe {
                let mut result_uninit = std::mem::MaybeUninit::<Result<i32>>::uninit();
                let success = create_result_int32_success_for_layout_test(
                    TEST_VALUE,
                    result_uninit.as_mut_ptr(),
                    size_of_val(&result_uninit) as libc::size_t,
                );
                assert!(success, "Byte size does not match Expected<i32, Error> size");

                let cpp_result_slice =
                    std::slice::from_raw_parts(result_uninit.as_mut_ptr() as *const u8, size_of_val(&result_uninit));
                print_bytes_hex("C++ Result<int32_t> raw bytes", cpp_result_slice);

                // Now transmute these bytes to Rust Result and verify
                // We need to create a properly sized array first
                let mut result_bytes_array = [0u8; MAX_SIZE];
                result_bytes_array.copy_from_slice(&cpp_result_slice[..std::mem::size_of::<Expected<i32, Error>>()]);
                let rust_result: Expected<i32, Error> = std::mem::transmute(result_bytes_array);

                println!("  Interpretation in Rust:");
                println!("    has_value(): {}", rust_result.has_value());
                if rust_result.has_value() {
                    let value = rust_result.get_value().unwrap();
                    println!("    value(): {:#x}", value);
                    assert_eq!(
                        *value, TEST_VALUE,
                        "Value mismatch! C++ layout may differ from Rust expectations"
                    );
                } else {
                    panic!("Expected success result, but has_value() returned false!");
                }

                // Test 2: Create a Result<int32_t> with error
                const ERROR_CODE: i32 = 12345;
                println!("\nTest 2: Result<int32_t> with error code {}", ERROR_CODE);

                let mut result_uninit = std::mem::MaybeUninit::<Result<i32>>::uninit();
                let success = create_result_int32_error_for_layout_test(
                    ERROR_CODE,
                    result_uninit.as_mut_ptr(),
                    size_of_val(&result_uninit) as libc::size_t,
                );
                assert!(success, "Byte size does not match Expected<i32, Error> size");
                let cpp_error_result_slice =
                    std::slice::from_raw_parts(result_uninit.as_mut_ptr() as *const u8, size_of_val(&result_uninit));

                let mut error_result_array = [0u8; MAX_SIZE];
                error_result_array
                    .copy_from_slice(&cpp_error_result_slice[..std::mem::size_of::<Expected<i32, Error>>()]);
                let rust_error_result: Expected<i32, Error> = std::mem::transmute(error_result_array);

                println!("  Interpretation in Rust:");
                println!("    has_value(): {}", rust_error_result.has_value());
                if !rust_error_result.has_value() {
                    let error = rust_error_result.get_error().unwrap();
                    println!("    error code: {}", error.code);
                    assert_eq!(
                        error.code, ERROR_CODE,
                        "Error code mismatch! C++ layout may differ from Rust expectations"
                    );
                } else {
                    panic!("Expected error result, but has_value() returned true!");
                }

                println!("\n✓ All binary layout tests passed!");
            }
        }

        #[test]
        fn test_cpp_created_results_in_rust() {
            unsafe {
                let test_message = std::ffi::CString::new("Test error from C++").unwrap();

                // Test C++ created success result
                let cpp_success = create_bmw_result_success_int32(12345);
                assert!(!cpp_success.is_null(), "C++ should have created a valid success result");

                // Cast to our Rust type and test
                let rust_success = &*(cpp_success as *const Result<i32>);
                println!("C++ created success result analysis:");
                println!("  has_value(): {}", rust_success.has_value());

                if rust_success.has_value() {
                    let value = rust_success.get_value().unwrap();
                    println!("  value: {}", value);
                    assert_eq!(*value, 12345, "Value should match what was set in C++");
                } else {
                    panic!("C++ created success result should have a value when viewed from Rust");
                }

                // Test C++ created error result
                let cpp_error = create_bmw_result_error_int32(54321, test_message.as_ptr());
                assert!(!cpp_error.is_null(), "C++ should have created a valid error result");

                let rust_error = &*(cpp_error as *const Result<i32>);
                println!("\nC++ created error result analysis:");
                println!("  has_value(): {}", rust_error.has_value());

                if !rust_error.has_value() {
                    let error = rust_error.get_error().unwrap();
                    println!("  error code: {}", error.code);
                    assert_eq!(error.code, 54321, "Error code should match what was set in C++");
                    let friendly_error_message = error.to_string();
                    assert_eq!(
                        &friendly_error_message,
                        "Error Test error from C++ (54321): Test error from C++"
                    );
                } else {
                    panic!("C++ created error result should have an error when viewed from Rust");
                }

                // Clean up
                destroy_bmw_result_int32(cpp_success);
                destroy_bmw_result_int32(cpp_error);
            }
        }

        #[test]
        fn test_rust_created_results_in_cpp() {
            unsafe {
                // Create success result in Rust
                let rust_success = Result::from(67890i32);
                let rust_success_ptr = &rust_success as *const Result<i32> as *const std::ffi::c_void;

                println!("\nRust created success result analysis:");
                let cpp_verified = verify_rust_result_as_cpp_int32(rust_success_ptr, true, 67890);
                println!("  C++ verification passed: {}", cpp_verified);
                assert!(cpp_verified, "C++ should be able to read Rust-created success result");

                // Create error result in Rust
                let test_str = "Rust created error";
                let c_str = std::ffi::CString::new(test_str).unwrap();
                let string_view = CStringView::from_parts(c_str.as_ptr(), test_str.len());

                let error = Error {
                    code: 98765,
                    domain: ptr::null(),
                    message: string_view,
                };

                let rust_error = Result::<i32>::from_error(error);
                let rust_error_ptr = &rust_error as *const Result<i32> as *const std::ffi::c_void;

                println!("\nRust created error result analysis:");
                let cpp_error_verified = verify_rust_result_as_cpp_int32(rust_error_ptr, false, 98765);
                println!("  C++ verification passed: {}", cpp_error_verified);
                assert!(
                    cpp_error_verified,
                    "C++ should be able to read Rust-created error result"
                );
            }
        }

        #[test]
        fn test_round_trip_compatibility() {
            unsafe {
                // Test different value types to see if our assumptions hold across different sizes

                // Test with bool (smaller than i32)
                let test_message = std::ffi::CString::new("Bool test error").unwrap();

                let cpp_bool_success = create_bmw_result_success_bool(true);
                let rust_bool_success = &*(cpp_bool_success as *const Result<bool>);

                println!("\nBool result compatibility:");
                println!(
                    "  C++ created bool success has_value: {}",
                    rust_bool_success.has_value()
                );

                if rust_bool_success.has_value() {
                    let value = rust_bool_success.get_value().unwrap();
                    println!("  Bool value: {}", value);
                    assert!(*value);
                }

                let cpp_bool_error = create_bmw_result_error_bool(111, test_message.as_ptr());
                let rust_bool_error = &*(cpp_bool_error as *const Result<bool>);

                println!("  C++ created bool error has_value: {}", rust_bool_error.has_value());

                if !rust_bool_error.has_value() {
                    let error = rust_bool_error.get_error().unwrap();
                    println!("  Bool error code: {}", error.code);
                    assert_eq!(error.code, 111);
                }

                // Clean up
                destroy_bmw_result_bool(cpp_bool_success);
                destroy_bmw_result_bool(cpp_bool_error);
            }
        }

        #[test]
        fn test_discriminant_interpretation() {
            // Create Rust results and verify discriminant values are as expected
            let success_result = Result::from(42i32);
            let error_result = {
                let test_str = "Discriminant test error";
                let c_str = std::ffi::CString::new(test_str).unwrap();
                let string_view = unsafe { CStringView::from_parts(c_str.as_ptr(), test_str.len()) };
                let error = Error {
                    code: 123,
                    domain: ptr::null(),
                    message: string_view,
                };
                Result::<i32>::from_error(error)
            };

            // Check discriminant values directly
            let success_discriminant = success_result.discriminant();
            let error_discriminant = error_result.discriminant();

            assert_eq!(success_discriminant, ExpectedDiscriminant::Value);
            assert_eq!(error_discriminant, ExpectedDiscriminant::Error);

            // Verify the actual functionality matches the discriminant
            assert!(success_result.has_value());
            assert!(!error_result.has_value());
        }
    }
}

pub use ffi::{Error, Expected, Result};

#[macro_export]
#[doc(hidden)]
macro_rules! import_result {
    ($name:ident, $typename:ty, $cxxid:tt) => {
        #[repr(transparent)]
        struct $name($crate::Result<$typename>);

        impl From<$name> for ::std::result::Result<$typename, $crate::Error> {
            fn from(cpp_result: $name) -> Self {
                Self::from(cpp_result.0)
            }
        }

        // SAFETY: The calling macro forces the user to declare the type as unsafe. We pass this
        // to the `ExternType` trait.
        unsafe impl cxx::ExternType for $name {
            type Id = cxx::type_id!($cxxid);
            type Kind = cxx::kind::Trivial;
        }
    };
}

#[macro_export]
/// Declare Result types for use with cxx.
///
/// Usage of this macro looks like this:
///
/// ```ignore
/// import_cpp_results! {
///     unsafe type <Rust type name> = Result<<Rust type>> as "<ns1::ns2::CppTypeName>";
/// }
/// ```
///
/// # Example usage
///
/// For types on C++ side that shall be usable with cxx, we need to generate some extra boilerplate.
/// Since bmw::Result isn't directly integrated into cxx, we need to make cxx familiar with the FFI
/// of bmw::Result _for each distinct type_.
///
/// For example, importing two C++ that look like this on C++ side
///
/// ```c++
/// using Int32Result = bmw::Result<int32_t>;
/// using UniquePtrResult = bmw::Result<std::unique_ptr<Storage>>;
///
/// ```
/// ...macro usage on Rust side would look like this:
///
/// ```ignore
/// import_cpp_results! {
///     unsafe type ResultInt32 = Result<i32> as "Int32Result";
///     unsafe type ResultUniquePtrStorage = Result<UniquePtr<Storage>> as "UniquePtrResult";
/// }
///
/// #[cxx::bridge]
/// mod ffi {
///     type ResultInt32 = super::ResultInt32;
///     type ResultUniquePtrStorage = super::ResultUniquePtrStorage;
///
///     extern "C++" {
///         unsafe fn cpp_function(values: CxxVector<u32>) -> ResultInt32;
///
///         type Storage;
///         unsafe fn cpp_create_storage(file: &str) -> ResultUniquePtrStorage;
///     }
/// }
///```
///
/// # Safety
///
/// Declaring these types is inherently unsafe for multiple reasons:
/// 1. The compiler cannot know whether the type on C++ side really is a `Result<T>` with the same T
///    as on Rust side.
/// 2. The compiler cannot verify whether the type is trivial and thus relocatable. While there is a
///    check on C++ side inside the generated code, we cannot be sure whether this check gets
///    overridden in an invalid way.
///
macro_rules! import_cpp_results {
    { $(unsafe type $name:ident = Result<$typename:ty> as $cxxid:tt;)* } => {
        $($crate::import_result!($name, $typename, $cxxid);)*
    }
}

#[cfg(test)]
mod main_module_tests {
    use super::ffi::{Error, Result};
    use libcpp::CStringView;
    use std::ptr;

    // External C functions that work with real bmw::Result instances
    extern "C" {
        fn create_bmw_result_success_int32(value: i32) -> *mut std::ffi::c_void;
        fn create_bmw_result_error_int32(error_code: i32, message: *const std::ffi::c_char) -> *mut std::ffi::c_void;
        fn create_bmw_result_success_bool(value: bool) -> *mut std::ffi::c_void;
        fn create_bmw_result_error_bool(error_code: i32, message: *const std::ffi::c_char) -> *mut std::ffi::c_void;

        fn destroy_bmw_result_int32(result: *mut std::ffi::c_void);
        fn destroy_bmw_result_bool(result: *mut std::ffi::c_void);
    }

    #[test]
    fn test_cpp_result_to_std_result_success() {
        unsafe {
            // Create a success result on C++ side
            let cpp_success = create_bmw_result_success_int32(42);
            assert!(!cpp_success.is_null(), "C++ should create a valid success result");

            // Cast to our Rust Result type
            let rust_result = &*(cpp_success as *const Result<i32>);

            // Convert to std::result::Result
            let std_result: std::result::Result<i32, Error> = std::ptr::read(rust_result).into();

            println!("C++ success -> std::result::Result conversion:");
            println!("  std_result.is_ok(): {}", std_result.is_ok());

            assert!(std_result.is_ok(), "Converted result should be Ok");
            assert_eq!(std_result.unwrap(), 42, "Value should match C++ created value");

            // Clean up
            destroy_bmw_result_int32(cpp_success);
        }
    }

    #[test]
    fn test_cpp_result_to_std_result_error() {
        unsafe {
            let test_message = std::ffi::CString::new("C++ error for std conversion").unwrap();

            // Create an error result on C++ side
            let cpp_error = create_bmw_result_error_int32(999, test_message.as_ptr());
            assert!(!cpp_error.is_null(), "C++ should create a valid error result");

            // Cast to our Rust Result type
            let rust_result = &*(cpp_error as *const Result<i32>);

            // Convert to std::result::Result
            let std_result: std::result::Result<i32, Error> = std::ptr::read(rust_result).into();

            println!("C++ error -> std::result::Result conversion:");
            println!("  std_result.is_err(): {}", std_result.is_err());

            assert!(std_result.is_err(), "Converted result should be Err");
            let error = std_result.unwrap_err();
            assert_eq!(error.code(), 999, "Error code should match C++ created error");

            // Clean up
            destroy_bmw_result_int32(cpp_error);
        }
    }

    #[test]
    fn test_cpp_bool_result_to_std_result() {
        unsafe {
            // Test bool success case
            let cpp_bool_success = create_bmw_result_success_bool(true);
            let rust_bool_result = &*(cpp_bool_success as *const Result<bool>);

            let std_bool_result: std::result::Result<bool, Error> = std::ptr::read(rust_bool_result).into();

            assert!(std_bool_result.is_ok(), "Bool success should convert to Ok");
            assert!(std_bool_result.unwrap(), "Bool value should be preserved");

            // Test bool error case
            let test_message = std::ffi::CString::new("Bool error test").unwrap();
            let cpp_bool_error = create_bmw_result_error_bool(777, test_message.as_ptr());
            let rust_bool_error_result = &*(cpp_bool_error as *const Result<bool>);

            let std_bool_error_result: std::result::Result<bool, Error> = std::ptr::read(rust_bool_error_result).into();

            assert!(std_bool_error_result.is_err(), "Bool error should convert to Err");
            assert_eq!(
                std_bool_error_result.unwrap_err().code(),
                777,
                "Error code should be preserved"
            );

            // Clean up
            destroy_bmw_result_bool(cpp_bool_success);
            destroy_bmw_result_bool(cpp_bool_error);
        }
    }

    #[test]
    fn test_multiple_cpp_results_conversion() {
        unsafe {
            let test_message = std::ffi::CString::new("Multiple conversion test").unwrap();

            // Create multiple C++ results
            let cpp_results = [
                (create_bmw_result_success_int32(100), true, 100),
                (create_bmw_result_success_int32(200), true, 200),
                (create_bmw_result_error_int32(300, test_message.as_ptr()), false, 300),
                (create_bmw_result_error_int32(400, test_message.as_ptr()), false, 400),
            ];

            println!("Testing multiple C++ results conversion:");

            for (i, &(cpp_result_ptr, should_be_ok, expected_value_or_code)) in cpp_results.iter().enumerate() {
                assert!(!cpp_result_ptr.is_null(), "C++ result {} should be valid", i);

                let rust_result = &*(cpp_result_ptr as *const Result<i32>);
                let std_result: std::result::Result<i32, Error> = std::ptr::read(rust_result).into();

                if should_be_ok {
                    assert!(std_result.is_ok(), "Result {} should be Ok", i);
                    assert_eq!(
                        std_result.unwrap(),
                        expected_value_or_code,
                        "Result {} value should match",
                        i
                    );
                    println!("  Result {}: Ok({}) ✓", i, expected_value_or_code);
                } else {
                    assert!(std_result.is_err(), "Result {} should be Err", i);
                    assert_eq!(
                        std_result.unwrap_err().code(),
                        expected_value_or_code,
                        "Result {} error code should match",
                        i
                    );
                    println!("  Result {}: Err(code={}) ✓", i, expected_value_or_code);
                }

                destroy_bmw_result_int32(cpp_result_ptr);
            }
        }
    }

    #[test]
    fn test_cpp_result_conversion_with_rust_operations() {
        unsafe {
            // Create C++ success result
            let cpp_success = create_bmw_result_success_int32(50);
            let rust_result = &*(cpp_success as *const Result<i32>);

            // Convert to std::result::Result and perform Rust operations
            let std_result: std::result::Result<i32, Error> = std::ptr::read(rust_result).into();

            // Test map operation
            let mapped_result = std_result.map(|x| x * 2);
            assert!(mapped_result.is_ok());
            assert_eq!(mapped_result.unwrap(), 100);

            // Create another C++ result for chaining test
            let cpp_success2 = create_bmw_result_success_int32(25);
            let rust_result2 = &*(cpp_success2 as *const Result<i32>);
            let std_result2: std::result::Result<i32, Error> = std::ptr::read(rust_result2).into();

            // Test and_then operation
            let chained_result = std_result2.and_then(|x| {
                if x > 20 {
                    Ok(x + 10)
                } else {
                    // Create a Rust error for testing
                    let error_str = "Value too small";
                    let c_str = std::ffi::CString::new(error_str).unwrap();
                    let string_view = CStringView::from_parts(c_str.as_ptr(), error_str.len());
                    let error = Error::from_parts(999, ptr::null(), string_view);
                    Err(error)
                }
            });

            assert!(chained_result.is_ok());
            assert_eq!(chained_result.unwrap(), 35);

            println!("C++ result -> std::result conversions with Rust operations: ✓");

            // Clean up
            destroy_bmw_result_int32(cpp_success);
            destroy_bmw_result_int32(cpp_success2);
        }
    }

    #[test]
    fn test_cpp_error_result_conversion_with_error_handling() {
        unsafe {
            let test_message = std::ffi::CString::new("Error handling test").unwrap();

            // Create C++ error result
            let cpp_error = create_bmw_result_error_int32(404, test_message.as_ptr());
            let rust_result = &*(cpp_error as *const Result<i32>);

            // Convert to std::result::Result and test error handling
            let std_result: std::result::Result<i32, Error> = std::ptr::read(rust_result).into();

            // Test map_err operation
            let mapped_error_result = std_result.map_err(|err| {
                println!("Mapped error code: {} -> {}", err.code(), err.code() + 1000);
                Error::from_parts(err.code() + 1000, err.domain(), err.message())
            });

            assert!(mapped_error_result.is_err());
            assert_eq!(mapped_error_result.unwrap_err().code(), 1404);

            // Test or_else operation
            let cpp_success_fallback = create_bmw_result_success_int32(999);
            let rust_fallback_result = &*(cpp_success_fallback as *const Result<i32>);
            let std_fallback_result: std::result::Result<i32, Error> = std::ptr::read(rust_fallback_result).into();

            let fallback_result = std_result.or(std_fallback_result);
            assert!(fallback_result.is_ok());
            assert_eq!(fallback_result.unwrap(), 999);

            println!("C++ error result -> std::result error handling operations: ✓");

            // Clean up
            destroy_bmw_result_int32(cpp_error);
            destroy_bmw_result_int32(cpp_success_fallback);
        }
    }
}
