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

use core::alloc::Layout;
use core::ptr::NonNull;

#[derive(Debug, Clone, Copy)]
pub enum AllocationError {
    /// Memory allocation failed due to insufficient memory
    OutOfMemory,

    /// Memory allocation requested for zero size
    ZeroSizeAllocation,

    /// Fatal failure inside the allocator
    Internal,
}

pub trait BasicAllocator {
    /// Allocates a block of memory as described by `layout`.
    fn allocate(&self, layout: Layout) -> Result<NonNull<u8>, AllocationError>;

    /// Deallocates the memory block pointed to by `ptr` with the given `layout`.
    ///
    /// # Safety
    ///  - `ptr` must have been allocated by a previous call to `allocate` with the same `layout`.
    ///  - `layout` must match the layout used during allocation.
    ///
    unsafe fn deallocate(&self, ptr: NonNull<u8>, layout: Layout);
}
