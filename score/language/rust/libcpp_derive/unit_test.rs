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

use std::mem::{ManuallyDrop, MaybeUninit};

#[libcpp_derive::import]
pub type TestVariant = std::variant<*const i32, i8, cxx::UniquePtr<cxx::CxxString>>;

mod ffi {
    unsafe extern "C" {
        pub fn create_test_variant_i8(variant: *mut super::TestVariant, size: u32) -> bool;
        pub fn create_test_variant_i32_ptr(variant: *mut super::TestVariant, size: u32) -> bool;
        pub fn delete_test_variant(variant: *mut super::TestVariant);
        pub fn create_test_variant_ptr_to_str(variant: *mut super::TestVariant, size: u32) -> u64;
    }
}

macro_rules! test_simple_data_type {
    {#[test] fn $test_name:ident() {$variant_name:ident = $ffi_call:ident() => $value_assertion:tt}} => {
        #[test]
        fn $test_name() {
            let mut uninit_variant = MaybeUninit::<TestVariant>::uninit();
            let variant = unsafe {
                assert!(ffi::$ffi_call(uninit_variant.as_mut_ptr(), std::mem::size_of::<TestVariant>() as u32));
                uninit_variant.assume_init()
            };
            let assertions = |$variant_name: TestVariant| $value_assertion;
            assertions(variant);
        }
    }
}

test_simple_data_type! {
    #[test]
    fn create_simple_data_type_i8() {
        variant = create_test_variant_i8() => {
            assert_eq!(variant.get_index(), 1);
            assert_eq!(*variant.get_1(), 15);
        }
    }
}

test_simple_data_type! {
    #[test]
    fn create_simple_data_type_i32_ptr() {
        variant = create_test_variant_i32_ptr() => {
            assert_eq!(variant.get_index(), 0);
            let mut content_ptr: *const i32 = *variant.get_0();
            let mut expected = [7i32,3,5,3].iter();
            loop {
                let x = unsafe { *content_ptr };
                if x == 0 {
                    assert!(expected.next().is_none(), "Expected no more values");
                    break;
                } else {
                    content_ptr = unsafe { content_ptr.add(1) };
                    assert_eq!(x, *expected.next().unwrap());
                }
            }
        }
    }
}

#[test]
fn create_and_destroy_ptr_to_str() {
    let mut uninit_variant = MaybeUninit::<TestVariant>::uninit();
    let cpp_offset;
    let mut variant = unsafe {
        cpp_offset =
            ffi::create_test_variant_ptr_to_str(uninit_variant.as_mut_ptr(), std::mem::size_of::<TestVariant>() as u32);
        assert!(cpp_offset < u64::MAX);
        ManuallyDrop::new(uninit_variant.assume_init())
    };
    assert_eq!(variant.get_index(), 2);
    assert_eq!(variant.get_2().to_str().unwrap(), "Hello, World!");

    // Check the relative location of the payload inside variant and compare with what C++ says
    let unique_ptr_addr = variant.get_2() as *const cxx::UniquePtr<cxx::CxxString> as *const u8;
    let variant_addr = std::ptr::addr_of!(variant) as *const u8;
    let offset = unsafe { unique_ptr_addr.offset_from(variant_addr) };
    assert!(offset >= 0);
    assert_eq!(offset as u64, cpp_offset);

    unsafe { ffi::delete_test_variant(&mut *variant) };
}
