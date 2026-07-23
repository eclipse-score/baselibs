/*******************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

//! Safe Rust wrapper for C++ `score::cpp::pmr::memory_resource` handles.
//!
//! This crate exposes three kinds of PMR resources from the `score::cpp`
//! library:
//!
//! - [`ResourceKind::Monotonic`]: a monotonic buffer resource backed by a
//!   C++-allocated buffer of a fixed size.
//! - [`ResourceKind::UnsynchronizedPool`]: a single-threaded pool resource.
//! - The process-global default resource returned by
//!   `score::cpp::pmr::get_default_resource()`, created via
//!   [`MemoryResource::default_resource()`].
//!
//! Each resource carries a compile-time [`thread_safety`] marker (`Local` or
//! `Shared`) so that `Send`/`Sync` propagate soundly to every borrower; see
//! the [`MemoryResource`] documentation.
//!
//! Both owned resource kinds support a **hermetic** mode. When `hermetic =
//! true`, any attempt to allocate beyond the pre-allocated pool logs a fatal
//! message and calls `std::abort()` on the C++ side, preventing any fallback
//! to the system heap.  When `hermetic = false`, a warning is logged and the
//! allocation falls back to `score::cpp::pmr::new_delete_resource()`.

mod ffi {
    /// Opaque handle type corresponding to
    /// `score::language::rust::memres::MemResHandle` on the C++ side.
    ///
    /// Only pointers to this type are meaningful; it cannot be constructed in
    /// Rust.
    #[repr(C)]
    pub struct MemResHandle {
        _data: [u8; 0],
    }

    /// Opaque resource type corresponding to
    /// `score::cpp::pmr::memory_resource` on the C++ side.
    ///
    /// Only pointers to this type are meaningful; it cannot be constructed in
    /// Rust.  Expose via [`crate::ScoreMemoryResource`].
    #[repr(C)]
    pub struct ScoreMemoryResource {
        _data: [u8; 0],
    }

    #[link(name = "memres_cpp")]
    unsafe extern "C" {
        pub safe fn memres_new_monotonic(size: usize, hermetic: bool) -> *mut MemResHandle;
        pub safe fn memres_new_unsynchronized_pool(
            max_blocks_per_chunk: usize,
            largest_required_pool_block: usize,
            hermetic: bool,
        ) -> *mut MemResHandle;
        pub safe fn memres_new_default() -> *mut MemResHandle;

        // These two methods are explicitly not marked `safe` because their safety can only be guaranteed
        // by their callees (as the callee will provide a pointer, that may or may not be safe).
        // Therefore, each usage of them will need an `unsafe` block and justification.
        pub fn memres_destroy(handle: *mut MemResHandle);
        pub fn memres_as_memory_resource(handle: *mut MemResHandle) -> *mut ScoreMemoryResource;
    }
}

use core::marker::PhantomData;
pub use ffi::ScoreMemoryResource;

/// Compile-time markers describing whether a [`MemoryResource`] may cross
/// thread boundaries.
///
/// The thread-safety of a PMR resource is a property of the concrete C++
/// resource, not something the runtime can recover from an erased
/// `score::cpp::pmr::memory_resource*`.  Encoding it as a type parameter lets
/// the compiler propagate `Send`/`Sync` correctly to every borrower of the
/// resource instead of relying on an unconditional, easily-unsound
/// `unsafe impl`.
pub mod thread_safety {
    mod sealed {
        pub trait Sealed {}
    }

    /// Marker for a resource whose backing C++ resource synchronises
    /// internally and may therefore be sent to / shared between threads.
    pub struct Shared;

    /// Marker for a single-threaded resource that must not cross thread
    /// boundaries.
    pub struct Local;

    /// Sealed marker trait implemented only by [`Shared`] and [`Local`].
    pub trait ThreadSafety: sealed::Sealed {}

    impl sealed::Sealed for Shared {}
    impl ThreadSafety for Shared {}

    impl sealed::Sealed for Local {}
    impl ThreadSafety for Local {}
}

pub use thread_safety::{Local, Shared, ThreadSafety};

/// Options for pool-based memory resources.
///
/// A value of `0` for any field lets the C++ implementation choose a default.
#[derive(Debug, Clone, Copy, Default)]
pub struct PoolOptions {
    /// Maximum number of blocks per chunk allocated from upstream.
    /// `0` uses the implementation default.
    pub max_blocks_per_chunk: usize,
    /// Largest single allocation served by the pool; requests above this size
    /// go directly to upstream. `0` uses the implementation default.
    pub largest_required_pool_block: usize,
}

/// The kind of C++ PMR resource to create.
pub enum ResourceKind {
    /// A monotonic buffer resource backed by a C++-allocated buffer of `size`
    /// bytes. Allocations are served sequentially; individual deallocations are
    /// no-ops. When the buffer is exhausted, the `GuardedUpstream` is called.
    Monotonic { size: usize },
    /// An unsynchronized (single-threaded) pool resource.
    UnsynchronizedPool(PoolOptions),
}

/// A lifetime-bound raw pointer to the underlying
/// `score::cpp::pmr::memory_resource`.
///
/// This wrapper ties the pointer's validity to the borrow of the originating
/// [`MemoryResource`] via [`PhantomData`].  The borrow checker therefore
/// prevents the `MemoryResource` from being moved or dropped while this value
/// is live, eliminating the use-after-free hazard that a bare `*mut` pointer
/// would expose.
///
/// Obtain a value of this type via [`MemoryResource::as_memory_resource_ptr`].
pub struct MemoryResourcePtr<'a> {
    ptr: *mut ScoreMemoryResource,
    _marker: PhantomData<&'a ()>,
}

impl MemoryResourcePtr<'_> {
    /// Returns the underlying raw pointer to the
    /// `score::cpp::pmr::memory_resource`.
    ///
    /// The pointer is valid for as long as the originating [`MemoryResource`]
    /// is alive, which the borrow checker enforces through the lifetime `'a`
    /// on [`MemoryResourcePtr`].  Do not store this pointer beyond the scope
    /// in which `MemoryResourcePtr` lives.
    pub fn as_ptr(&self) -> *mut ScoreMemoryResource {
        self.ptr
    }
}

/// An owned handle to a C++ `score::cpp::pmr::memory_resource`.
///
/// On drop, the underlying C++ resource and all associated storage are freed.
///
/// # Thread safety
///
/// The marker type parameter `S` encodes whether the backing C++ resource is
/// safe to use across threads:
///
/// - [`Shared`]: the backing resource performs its own internal
///   synchronisation (currently only the process-global default resource,
///   whose `allocate`/`deallocate` forward to the data-race-free global
///   `operator new`/`operator delete`).  `MemoryResource<Shared>` is therefore
///   `Send + Sync`.
/// - [`Local`] (the default): the backing resource is single-threaded
///   (`monotonic_buffer_resource`, `unsynchronized_pool_resource`).  Concurrent
///   access would be a data race, so `MemoryResource<Local>` is `!Send + !Sync`
///   and the compiler prevents it from crossing thread boundaries.
pub struct MemoryResource<S: ThreadSafety = Local> {
    handle: *mut ffi::MemResHandle,
    _marker: PhantomData<S>,
}

// SAFETY: A `Shared` resource is backed by the process-global default
// resource, whose allocate/deallocate forward to the global `operator new`/
// `operator delete`. The C++ standard requires those to be data-race free, so
// the handle may be sent to and shared between threads soundly.
unsafe impl Send for MemoryResource<Shared> {}
unsafe impl Sync for MemoryResource<Shared> {}

impl MemoryResource<Local> {
    /// Creates a new single-threaded memory resource of the given kind.
    ///
    /// The resulting resource is `!Send + !Sync`: both [`ResourceKind`]
    /// variants (`Monotonic`, `UnsynchronizedPool`) wrap a C++ resource that
    /// must not be accessed from more than one thread.
    ///
    /// # Panics
    ///
    /// Panics if the C++ side fails to allocate the handle. This indicates an
    /// unrecoverable out-of-memory condition during initialisation.
    pub fn new(kind: ResourceKind, hermetic: bool) -> Self {
        // SAFETY: All FFI functions transfer ownership of the returned pointer
        // to this struct. They are noexcept on the C++ side. The pointer is
        // non-null on success and null only if the initial heap allocation
        // fails, which we assert below.
        let handle = match kind {
            ResourceKind::Monotonic { size } => ffi::memres_new_monotonic(size, hermetic),
            ResourceKind::UnsynchronizedPool(opts) => ffi::memres_new_unsynchronized_pool(
                opts.max_blocks_per_chunk,
                opts.largest_required_pool_block,
                hermetic,
            ),
        };
        assert!(
            !handle.is_null(),
            "memres: C++ failed to allocate memory resource handle"
        );
        Self {
            handle,
            _marker: PhantomData,
        }
    }
}

impl MemoryResource<Shared> {
    /// Returns a handle backed by the process-global default memory resource
    /// (`score::cpp::pmr::get_default_resource()`).
    ///
    /// The default resource synchronises internally, so the returned handle is
    /// `Send + Sync` and may be shared across threads.
    ///
    /// # Panics
    ///
    /// Panics if the C++ side fails to allocate the thin wrapper handle.
    pub fn default_resource() -> Self {
        // SAFETY: memres_new_default allocates a thin non-owning wrapper around
        // the global default resource; ownership of the wrapper is transferred
        // to this struct and released via memres_destroy on drop.
        let handle = ffi::memres_new_default();
        assert!(
            !handle.is_null(),
            "memres: C++ failed to allocate default resource handle"
        );
        Self {
            handle,
            _marker: PhantomData,
        }
    }
}

impl<S: ThreadSafety> MemoryResource<S> {
    /// Returns a lifetime-bound pointer to the underlying
    /// `score::cpp::pmr::memory_resource`.
    ///
    /// The returned [`MemoryResourcePtr`] borrows `self` for `'_`, preventing
    /// this `MemoryResource` from being moved or dropped while the pointer is
    /// live.  Call [`.as_ptr()`][MemoryResourcePtr::as_ptr] to obtain the
    /// typed raw pointer to pass to C++.
    pub fn as_memory_resource_ptr(&self) -> MemoryResourcePtr<'_> {
        MemoryResourcePtr {
            // SAFETY: handle is non-null (ensured by the constructors) and
            // uniquely owned.
            ptr: unsafe { ffi::memres_as_memory_resource(self.handle) },
            _marker: PhantomData,
        }
    }
}

impl<S: ThreadSafety> Drop for MemoryResource<S> {
    fn drop(&mut self) {
        // SAFETY: handle is non-null and uniquely owned;
        // It has been obtained from a call to ffi::memres_new_...;
        // this is the sole place it is freed.
        unsafe { ffi::memres_destroy(self.handle) }
    }
}
