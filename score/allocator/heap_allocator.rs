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

extern crate alloc;

use alloc::alloc::{alloc, dealloc};
use core::alloc::Layout;
use core::ptr::NonNull;

use crate::allocator_traits::{AllocationError, BasicAllocator};

/// Global allocator.
pub static GLOBAL_ALLOCATOR: HeapAllocator = HeapAllocator;

/// Proxy to global heap allocation in Rust.
#[derive(Debug, Default, Clone, Copy)]
pub struct HeapAllocator;

impl BasicAllocator for HeapAllocator {
    fn allocate(&self, layout: Layout) -> Result<NonNull<u8>, AllocationError> {
        if layout.size() == 0 {
            return Err(AllocationError::ZeroSizeAllocation);
        }

        unsafe {
            let ptr = alloc(layout);
            if ptr.is_null() {
                return Err(AllocationError::OutOfMemory);
            }

            // SAFETY: already checked for null.
            Ok(NonNull::new_unchecked(ptr))
        }
    }

    unsafe fn deallocate(&self, ptr: NonNull<u8>, layout: Layout) {
        dealloc(ptr.as_ptr(), layout);
    }
}
