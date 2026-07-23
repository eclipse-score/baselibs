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

use std::fmt;
use std::ops::Deref;

///////////////////////////////////////////////////////////////////////////
/// std::string_view
///////////////////////////////////////////////////////////////////////////

#[repr(C)]
#[derive(Clone, Copy, Debug, PartialEq, Default)]
#[cfg(feature = "libstdcpp_layout")]
struct CStringViewStorage {
    len: libc::size_t,
    data: *const std::ffi::c_char,
}

#[repr(C)]
#[derive(Clone, Copy, Debug, PartialEq, Default)]
#[cfg(feature = "libcxx_layout")]
struct CStringViewStorage {
    data: *const std::ffi::c_char,
    len: libc::size_t,
}

/// Binary compatible Rust version of `std::string_view`
///
/// This type can be safely sent across FFI boundaries where `std::string_view` is expected. It
/// offers conversions to Rust string types, assuming a UTF-8 encoding. It also offers
/// conversions to byte slices, so if any other encoding is used, the caller may use this slice
/// and covert it themselves.
///
/// Assumptions on the layout:
/// * Length is of C++ type `std::size_t`
/// * Characters are of type `char` (i.e. [`std::ffi::c_char`] in Rust)
/// * On some platforms (e.g. QNX with libcxx), the field order is swapped
#[repr(transparent)]
#[derive(Clone, Copy, Debug, PartialEq, Default)]
pub struct CStringView {
    storage: CStringViewStorage,
}

impl CStringView {
    fn as_char_slice(&self) -> &[std::ffi::c_char] {
        if self.storage.len > 0 {
            assert!(!self.storage.data.is_null());
            // SAFETY: We checked that data is not null and len > 0. Everything else is
            // accounted by either the fact that this came across FFI (which is unsafe anyway)
            // or by the careful caller of from_parts (which is unsafe as well).
            unsafe { std::slice::from_raw_parts(self.storage.data, self.storage.len) }
        } else {
            &[]
        }
    }

    fn as_byte_slice(&self) -> &[u8] {
        assert_eq!(
            std::mem::size_of::<std::ffi::c_char>(),
            std::mem::size_of::<u8>(),
            "c_char must be of the same size as u8."
        );
        assert_eq!(
            std::mem::align_of::<std::ffi::c_char>(),
            std::mem::align_of::<u8>(),
            "c_char must be of the same alignment as u8."
        );
        if self.storage.len > 0 {
            assert!(
                !self.storage.data.is_null(),
                "String data is null but length is non-zero"
            );
            // SAFETY: This is safe because c_char is guaranteed to have the same size and
            // alignment as u8.
            unsafe { &*(self.as_char_slice() as *const [std::ffi::c_char] as *const [u8]) }
        } else {
            &[]
        }
    }

    /// Create a string view from a pointer to C chars and a length.
    ///
    /// # Panics
    /// This function will panic if `data` is null.
    ///
    /// # Safety
    /// The caller must ensure that `data` points to a valid memory region of at least `len`
    /// bytes, and that this memory remains valid for the lifetime of the returned
    /// `CStringView`.
    pub unsafe fn from_parts(data: *const std::ffi::c_char, len: usize) -> Self {
        assert!(!data.is_null());
        Self {
            storage: CStringViewStorage {
                data,
                len: len as libc::size_t,
            },
        }
    }

    /// Convert to a UTF-8 Rust string slice.
    ///
    /// This method uses [`std::str::from_utf8`] internally, so all the same rules apply.
    ///
    /// # Panics
    /// This function will panic if `data` is null.
    pub fn to_str(&self) -> std::result::Result<&str, std::str::Utf8Error> {
        std::str::from_utf8(self.as_ref())
    }

    /// Convert to a UTF-8 Rust string slice, replacing invalid sequences with U+FFFD.
    ///
    /// This method uses [`std::string::String::from_utf8_lossy`] internally, so all the same
    /// rules apply.
    ///
    /// # Panics
    /// This function will panic if `data` is null.
    pub fn to_string_lossy(&self) -> std::borrow::Cow<'_, str> {
        String::from_utf8_lossy(self.as_ref())
    }
}

impl Deref for CStringView {
    type Target = [std::ffi::c_char];

    fn deref(&self) -> &Self::Target {
        self.as_char_slice()
    }
}

impl AsRef<[u8]> for CStringView {
    fn as_ref(&self) -> &[u8] {
        self.as_byte_slice()
    }
}

impl fmt::Display for CStringView {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(self.to_string_lossy().as_ref())
    }
}
#[cfg(test)]
mod tests {
    use super::*;
    use std::mem;

    extern "C" {
        // Functions to create and test std::string_view layout compatibility
        fn create_cpp_string_view(data: *const std::ffi::c_char, len: libc::size_t) -> *mut std::ffi::c_void;
        fn create_empty_cpp_string_view() -> *mut std::ffi::c_void;
        fn destroy_cpp_string_view(sv: *mut std::ffi::c_void);
        fn get_cpp_string_view_size() -> libc::size_t;
        fn verify_string_view_layout(
            rust_view: *const std::ffi::c_void,
            expected_data: *const std::ffi::c_char,
            expected_len: libc::size_t,
        ) -> bool;
    }

    #[test]
    fn test_cstring_view_layout_compatibility() {
        unsafe {
            // Test that CStringView has the same memory layout as std::string_view
            let test_str = "Hello, FFI World!";
            let c_str = std::ffi::CString::new(test_str).unwrap();

            // Create our Rust CStringView
            let rust_string_view = CStringView::from_parts(c_str.as_ptr(), test_str.len());

            println!("\n=== CStringView Layout Verification ===");
            println!("Test string: \"{}\"", test_str);
            println!("String length: {}", test_str.len());

            // Verify size compatibility
            let cpp_string_view_size = get_cpp_string_view_size();
            let rust_string_view_size = mem::size_of::<CStringView>();

            println!("Size comparison:");
            println!("  C++ std::string_view: {} bytes", cpp_string_view_size);
            println!("  Rust CStringView:     {} bytes", rust_string_view_size);
            println!("  Sizes match: {}", cpp_string_view_size == rust_string_view_size);

            assert_eq!(
                cpp_string_view_size, rust_string_view_size,
                "CStringView size must match std::string_view size"
            );

            // Verify field layout by checking memory positions
            let rust_view_ptr = &rust_string_view as *const CStringView as *const std::ffi::c_void;

            // Test that C++ can correctly interpret our Rust CStringView
            let layout_compatible =
                verify_string_view_layout(rust_view_ptr, c_str.as_ptr(), test_str.len() as libc::size_t);

            println!("Layout compatibility:");
            println!("  C++ can read Rust CStringView: {}", layout_compatible);

            assert!(
                layout_compatible,
                "C++ should be able to correctly read Rust CStringView fields"
            );

            // Create C++ string_view and verify we can read it correctly
            let cpp_string_view = create_cpp_string_view(c_str.as_ptr(), test_str.len() as libc::size_t);
            assert!(!cpp_string_view.is_null(), "C++ should create valid string_view");

            let rust_read_cpp_view = &*(cpp_string_view as *const CStringView);

            println!("Reverse compatibility:");
            println!("  Rust reading C++ string_view:");
            println!(
                "    Length: {} (expected: {})",
                rust_read_cpp_view.storage.len,
                test_str.len()
            );
            println!(
                "    Data ptr match: {}",
                rust_read_cpp_view.storage.data == c_str.as_ptr()
            );

            assert_eq!(
                rust_read_cpp_view.storage.len as usize,
                test_str.len(),
                "Length field should match when reading C++ string_view from Rust"
            );
            assert_eq!(
                rust_read_cpp_view.storage.data,
                c_str.as_ptr(),
                "Data pointer should match when reading C++ string_view from Rust"
            );

            // Verify we can convert the C++ created string_view to a Rust string
            let converted_str = rust_read_cpp_view.to_str().expect("Should convert to valid UTF-8");
            println!("    Converted string: \"{}\"", converted_str);
            assert_eq!(converted_str, test_str, "Converted string should match original");

            // Clean up
            destroy_cpp_string_view(cpp_string_view);

            println!("✓ CStringView layout is compatible with std::string_view");
        }
    }

    #[test]
    fn test_equality_of_defaulted_string_instances() {
        unsafe {
            let cpp_created_string_view = (create_empty_cpp_string_view() as *mut CStringView).as_mut().unwrap();
            let default_cstringview = CStringView::default();
            assert_eq!(
                cpp_created_string_view.storage.len, default_cstringview.storage.len,
                "Lengths of default CStringView and C++ created empty string_view should match"
            );
            assert_eq!(
                cpp_created_string_view.storage.data, default_cstringview.storage.data,
                "Data pointers of default CStringView and C++ created empty string_view should match"
            );
            destroy_cpp_string_view(cpp_created_string_view as *mut _ as *mut libc::c_void);
        }
    }

    #[test]
    fn test_cstring_view_field_offsets() {
        // This test specifically verifies our layout assumptions about field ordering
        println!("\n=== CStringView Field Layout Analysis ===");

        // Check field offsets
        let len_offset = mem::offset_of!(CStringViewStorage, len);
        let data_offset = mem::offset_of!(CStringViewStorage, data);

        println!("Field offsets:");
        println!("  len field:  {} bytes from start", len_offset);
        println!("  data field: {} bytes from start", data_offset);

        // Verify field ordering based on feature flag
        #[cfg(feature = "libcxx_layout")]
        {
            println!("  Using QNX layout (data first, then length)");
            // QNX layout: data pointer comes first, then length
            assert_eq!(
                data_offset, 0,
                "Data field should be at offset 0 (first field) in QNX layout"
            );
            assert_eq!(
                len_offset,
                mem::size_of::<*const std::ffi::c_char>(),
                "Length field should come after data field in QNX layout"
            );

            println!("  ✓ Data field is first (offset 0)");
            println!("  ✓ Length field is second (offset {})", len_offset);
        }

        #[cfg(feature = "libstdcpp_layout")]
        {
            println!("  Using standard layout (length first, then data)");
            // Standard layout: length comes first, then data pointer
            assert_eq!(
                len_offset, 0,
                "Length field should be at offset 0 (first field) in standard layout"
            );
            assert_eq!(
                data_offset,
                mem::size_of::<libc::size_t>(),
                "Data field should come after length field in standard layout"
            );

            println!("  ✓ Length field is first (offset 0)");
            println!("  ✓ Data field is second (offset {})", data_offset);
        }

        // Verify total size is as expected
        let expected_size = mem::size_of::<libc::size_t>() + mem::size_of::<*const std::ffi::c_char>();
        let actual_size = mem::size_of::<CStringView>();

        println!("Size analysis:");
        println!("  size_t: {} bytes", mem::size_of::<libc::size_t>());
        println!("  char*:  {} bytes", mem::size_of::<*const std::ffi::c_char>());
        println!("  Expected CStringView size: {} bytes", expected_size);
        println!("  Actual CStringView size:   {} bytes", actual_size);

        // Note: actual size might be larger due to padding/alignment
        assert!(
            actual_size >= expected_size,
            "CStringView should be at least as large as its constituent fields"
        );

        println!("  ✓ Size is consistent with field layout");

        // Test with actual data to verify field access
        let test_data = "Field offset test";
        let c_str = std::ffi::CString::new(test_data).unwrap();
        let string_view = unsafe { CStringView::from_parts(c_str.as_ptr(), test_data.len()) };

        // Access fields and verify they contain expected values
        assert_eq!(string_view.storage.len, test_data.len());
        assert_eq!(string_view.storage.data, c_str.as_ptr());

        // Verify field access through raw pointer manipulation
        let view_ptr = &string_view as *const CStringView;
        unsafe {
            #[cfg(feature = "libcxx_layout")]
            {
                let data_ptr = view_ptr as *const *const std::ffi::c_char;
                let len_ptr = (view_ptr as *const u8).add(len_offset) as *const libc::size_t;

                let data_via_offset = *data_ptr;
                let len_via_offset = *len_ptr;

                println!("Direct field access verification (QNX layout):");
                println!("  Data ptr via offset matches: {}", data_via_offset == c_str.as_ptr());
                println!(
                    "  Length via offset: {} (expected: {})",
                    len_via_offset,
                    test_data.len()
                );

                assert_eq!(data_via_offset, c_str.as_ptr());
                assert_eq!(len_via_offset as usize, test_data.len());
            }

            #[cfg(feature = "libstdcpp_layout")]
            {
                let len_ptr = view_ptr as *const libc::size_t;
                let data_ptr = (view_ptr as *const u8).add(data_offset) as *const *const std::ffi::c_char;

                let len_via_offset = *len_ptr;
                let data_via_offset = *data_ptr;

                println!("Direct field access verification (standard layout):");
                println!(
                    "  Length via offset: {} (expected: {})",
                    len_via_offset,
                    test_data.len()
                );
                println!("  Data ptr via offset matches: {}", data_via_offset == c_str.as_ptr());

                assert_eq!(len_via_offset, test_data.len());
                assert_eq!(data_via_offset, c_str.as_ptr());
            }
        }

        #[cfg(feature = "libcxx_layout")]
        println!("✓ libcxx field layout assumptions verified");

        #[cfg(feature = "libstdcpp_layout")]
        println!("✓ libstdc++ field layout assumptions verified");
    }
}
