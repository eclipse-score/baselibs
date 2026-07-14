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

//! Custom allocator support.

mod allocator_traits;
mod arena_allocator;
mod atomic;
mod heap_allocator;
mod memory_pool;
mod memory_region;

pub use allocator_traits::{AllocationError, BasicAllocator};
pub use arena_allocator::ArenaAllocator;
pub use heap_allocator::{HeapAllocator, GLOBAL_ALLOCATOR};
pub use memory_pool::MemoryPool;
pub use memory_region::{page_size, MemoryRegion, MmapMemoryRegion};
