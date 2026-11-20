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
use core::cmp::Ordering;
use core::fmt as core_fmt;
use core::hash::{Hash, Hasher};
use core::mem::ManuallyDrop;
use core::ops::Deref;
use core::ptr::{drop_in_place, read, NonNull};
use core::sync::atomic;
use core::sync::atomic::{AtomicUsize, Ordering as AtomicOrdering};
use score_log::fmt as score_fmt;

use allocator::BasicAllocator;

/// A reference-counted smart pointer with custom allocator support like `std::sync::Arc`.
/// The `ArcIn` type provides shared ownership of a value of type `T`, allocated using the specified allocator `A`.
/// Cloning an `ArcIn` instance increases the reference count, and when the last `ArcIn` pointing to the same value is dropped,
/// the value is deallocated using the provided allocator.
///
/// # Notes
///  - This is a simplified version and does not include weak references.
///  - This provides limited functionality compared to `std::sync::Arc` and shall be used only when custom allocator support is required.
///
pub struct ArcIn<T: ?Sized, A: BasicAllocator> {
    ptr: NonNull<ArcInner<T>>,
    alloc: A,
}

/// Heap layout backing an [`ArcIn`].
/// Not a part of the stable public API.
#[doc(hidden)]
pub struct ArcInner<T: ?Sized> {
    strong: AtomicUsize,
    data: T,
}

impl<T, A: BasicAllocator + Clone> ArcIn<T, A> {
    /// Create a new ArcIn using the given allocator
    pub fn new_in(data: T, alloc: A) -> Self {
        let layout = Layout::new::<ArcInner<T>>();
        let ptr = match alloc.allocate(layout) {
            Ok(ptr) => ptr.cast::<ArcInner<T>>(),
            Err(err) => {
                panic!("Failed to allocate memory with error: {:?}", err);
            },
        };

        unsafe {
            ptr.as_ptr().write(ArcInner {
                strong: AtomicUsize::new(1),
                data,
            });
        }

        ArcIn { ptr, alloc }
    }
}

impl<T: ?Sized, A: BasicAllocator> ArcIn<T, A> {
    /// Get strong reference count
    pub fn strong_count(this: &Self) -> usize {
        // SAFETY: `this.ptr` is guaranteed to be valid because we keep at least one strong reference by `this`
        unsafe { this.ptr.as_ref().strong.load(AtomicOrdering::SeqCst) }
    }

    /// Decompose an `ArcIn` into its raw parts without touching the reference count.
    ///
    /// The returned pointer keeps ownership of the one strong reference held by `this`.
    /// Caller must eventually pass the parts back to [`ArcIn::from_raw_parts`] to avoid leaking.
    ///
    /// The pointer may be unsize-coerced before being handed back.
    /// E.g., `NonNull<ArcInner<T>>` to `NonNull<ArcInner<dyn Trait>>`.
    pub fn into_raw_parts(this: Self) -> (NonNull<ArcInner<T>>, A) {
        // Suppress `Drop` so the strong count is preserved and `alloc` can be moved out.
        let this = ManuallyDrop::new(this);
        let ptr = this.ptr;
        // SAFETY:
        // `this` is wrapped in `ManuallyDrop` and never dropped.
        // Reading `alloc` out of it does not risk a double drop.
        let alloc = unsafe { read(&this.alloc) };
        (ptr, alloc)
    }

    /// Reassemble an `ArcIn` from parts previously obtained via [`ArcIn::into_raw_parts`].
    ///
    /// # Safety
    ///
    /// Parameters must originate from a single [`ArcIn::into_raw_parts`] call.
    /// They must not be consumed by another call to [`ArcIn::from_raw_parts`].
    pub unsafe fn from_raw_parts(ptr: NonNull<ArcInner<T>>, alloc: A) -> Self {
        ArcIn { ptr, alloc }
    }
}

impl<T: ?Sized, A: BasicAllocator + Clone> Clone for ArcIn<T, A> {
    fn clone(&self) -> Self {
        // SAFETY: `self.ptr` is guaranteed to be valid because we keep at least one strong reference by `self`
        unsafe {
            self.ptr.as_ref().strong.fetch_add(1, AtomicOrdering::Relaxed);
        }

        ArcIn {
            ptr: self.ptr,
            alloc: self.alloc.clone(),
        }
    }
}

impl<T: ?Sized, A: BasicAllocator> Deref for ArcIn<T, A> {
    type Target = T;
    fn deref(&self) -> &T {
        unsafe { &self.ptr.as_ref().data }
    }
}

impl<T: Default, A: BasicAllocator + Clone + Default> Default for ArcIn<T, A> {
    fn default() -> Self {
        ArcIn::new_in(T::default(), A::default())
    }
}

impl<T: ?Sized + core_fmt::Debug, A: BasicAllocator> core_fmt::Debug for ArcIn<T, A> {
    fn fmt(&self, f: &mut core_fmt::Formatter<'_>) -> core_fmt::Result {
        self.deref().fmt(f)
    }
}

impl<T: ?Sized + score_fmt::ScoreDebug, A: BasicAllocator> score_fmt::ScoreDebug for ArcIn<T, A> {
    fn fmt(&self, f: score_fmt::Writer, spec: &score_fmt::FormatSpec) -> score_fmt::Result {
        self.deref().fmt(f, spec)
    }
}

impl<T: ?Sized, A: BasicAllocator> AsRef<T> for ArcIn<T, A> {
    fn as_ref(&self) -> &T {
        self.deref()
    }
}

impl<T: ?Sized + PartialEq, A: BasicAllocator> PartialEq for ArcIn<T, A> {
    fn eq(&self, other: &Self) -> bool {
        **self == **other
    }
}

impl<T: ?Sized + Eq, A: BasicAllocator> Eq for ArcIn<T, A> {}

impl<T: ?Sized + PartialOrd, A: BasicAllocator> PartialOrd for ArcIn<T, A> {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        (**self).partial_cmp(&**other)
    }
}

impl<T: ?Sized + Ord, A: BasicAllocator> Ord for ArcIn<T, A> {
    fn cmp(&self, other: &Self) -> Ordering {
        (**self).cmp(&**other)
    }
}

impl<T: ?Sized + Hash, A: BasicAllocator> Hash for ArcIn<T, A> {
    fn hash<H: Hasher>(&self, state: &mut H) {
        (**self).hash(state);
    }
}

unsafe impl<T: ?Sized + Send + Sync, A: BasicAllocator + Send> Send for ArcIn<T, A> {}
unsafe impl<T: ?Sized + Send + Sync, A: BasicAllocator + Sync> Sync for ArcIn<T, A> {}

impl<T: ?Sized, A: BasicAllocator> Drop for ArcIn<T, A> {
    fn drop(&mut self) {
        if unsafe { self.ptr.as_ref().strong.fetch_sub(1, AtomicOrdering::Release) } == 1 {
            // SYNC: Ensure all previous writes are visible before we drop the data. This is enough because
            // we are the last strong reference.
            atomic::fence(AtomicOrdering::Acquire);
            unsafe {
                let layout = Layout::for_value(self.ptr.as_ref());
                drop_in_place(&mut self.ptr.as_mut().data);
                self.alloc.deallocate(self.ptr.cast(), layout);
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use allocator::HeapAllocator;
    use core::hash::Hash;
    use std::collections::hash_map::DefaultHasher;

    #[test]
    fn arc_in_is_send_sync_with_thread_safe_allocators() {
        fn assert_send_sync<T: Send + Sync>() {}
        assert_send_sync::<ArcIn<u32, HeapAllocator>>();
    }

    #[test]
    fn new_and_deref() {
        let alloc = HeapAllocator;
        let arc = ArcIn::new_in(42, alloc);
        assert_eq!(*arc, 42);
        assert_eq!(ArcIn::strong_count(&arc), 1);
    }

    #[test]
    fn clone_increases_count() {
        let alloc = HeapAllocator;
        let arc1 = ArcIn::new_in(100, alloc);
        let arc2 = arc1.clone();
        assert_eq!(ArcIn::strong_count(&arc1), 2);
        assert_eq!(*arc2, 100);
    }

    #[test]
    fn drop_decreases_count() {
        let alloc = HeapAllocator;
        let arc1 = ArcIn::new_in(55, alloc);
        assert_eq!(ArcIn::strong_count(&arc1), 1);
        {
            let _arc2 = arc1.clone();
            assert_eq!(ArcIn::strong_count(&arc1), 2);
        }
        // arc2 dropped
        assert_eq!(ArcIn::strong_count(&arc1), 1);
    }

    #[test]
    fn debug_trait() {
        let alloc = HeapAllocator;
        let arc = ArcIn::new_in("hello", alloc);
        let s = format!("{:?}", arc);
        assert_eq!(s, "\"hello\"");
    }

    #[test]
    fn default_trait() {
        let arc: ArcIn<u32, HeapAllocator> = ArcIn::default();
        assert_eq!(*arc, 0);
        assert_eq!(ArcIn::strong_count(&arc), 1);
    }

    #[test]
    fn as_ref_trait() {
        let alloc = HeapAllocator;
        let arc = ArcIn::new_in("world".to_string(), alloc);
        let s: &str = arc.as_ref();
        assert_eq!(s, "world");
    }

    #[test]
    fn eq_ord_hash() {
        let alloc = HeapAllocator;
        let arc1 = ArcIn::new_in(5, alloc);
        let arc2 = ArcIn::new_in(5, alloc);
        let arc3 = ArcIn::new_in(10, alloc);

        assert_eq!(arc1, arc2);
        assert!(arc1 < arc3);

        let mut hasher1 = DefaultHasher::new();
        arc1.hash(&mut hasher1);

        let mut hasher2 = DefaultHasher::new();
        arc2.hash(&mut hasher2);

        assert_eq!(hasher1.finish(), hasher2.finish());
    }

    #[test]
    fn strong_count_multiple_clones() {
        let alloc = HeapAllocator;
        let arc = ArcIn::new_in(123, alloc);
        let clones: Vec<_> = (0..5).map(|_| arc.clone()).collect();
        assert_eq!(ArcIn::strong_count(&arc), 6);
        drop(clones);
        assert_eq!(ArcIn::strong_count(&arc), 1);
    }

    #[test]
    fn drop_on_zero() {
        struct DropCounter<'a>(&'a mut bool);
        impl<'a> Drop for DropCounter<'a> {
            fn drop(&mut self) {
                *self.0 = true;
            }
        }

        let alloc = HeapAllocator;
        let mut dropped = false;
        {
            let arc = ArcIn::new_in(DropCounter(&mut dropped), alloc);
            assert!(!*arc.deref().0);
        }
        assert!(dropped);
    }

    #[test]
    fn unsized_trait_object() {
        trait Speak {
            fn value(&self) -> i32;
        }
        struct Concrete(i32);
        impl Speak for Concrete {
            fn value(&self) -> i32 {
                self.0
            }
        }

        let alloc = HeapAllocator;
        let arc: ArcIn<Concrete, HeapAllocator> = ArcIn::new_in(Concrete(7), alloc);

        // Coerce `ArcIn<Concrete>` -> `ArcIn<dyn Speak>` via the raw-parts primitive.
        // The unsizing happens on `NonNull`, which is a stable coercion.
        let (ptr, alloc) = ArcIn::into_raw_parts(arc);
        let ptr: NonNull<ArcInner<dyn Speak>> = ptr;
        let arc: ArcIn<dyn Speak, HeapAllocator> = unsafe { ArcIn::from_raw_parts(ptr, alloc) };

        assert_eq!(arc.value(), 7);
        assert_eq!(ArcIn::strong_count(&arc), 1);

        let arc2 = arc.clone();
        assert_eq!(ArcIn::strong_count(&arc), 2);
        assert_eq!(arc2.value(), 7);
        drop(arc2);
        assert_eq!(ArcIn::strong_count(&arc), 1);
    }

    #[test]
    fn unsized_trait_object_drops_once() {
        // Check deallocation happens with the correct layout when accessed as `dyn`.
        static DROPS: AtomicUsize = AtomicUsize::new(0);

        trait Marker {}
        struct Tracked;
        impl Marker for Tracked {}
        impl Drop for Tracked {
            fn drop(&mut self) {
                DROPS.fetch_add(1, AtomicOrdering::SeqCst);
            }
        }

        let alloc = HeapAllocator;
        let arc = ArcIn::new_in(Tracked, alloc);
        let (ptr, alloc) = ArcIn::into_raw_parts(arc);
        let ptr: NonNull<ArcInner<dyn Marker>> = ptr;
        let arc: ArcIn<dyn Marker, HeapAllocator> = unsafe { ArcIn::from_raw_parts(ptr, alloc) };

        let arc2 = arc.clone();
        assert_eq!(DROPS.load(AtomicOrdering::SeqCst), 0);
        drop(arc);
        assert_eq!(DROPS.load(AtomicOrdering::SeqCst), 0);
        drop(arc2);
        assert_eq!(DROPS.load(AtomicOrdering::SeqCst), 1);
    }

    #[test]
    fn unsized_slice() {
        let alloc = HeapAllocator;
        let arc: ArcIn<[i32; 3], HeapAllocator> = ArcIn::new_in([10, 20, 30], alloc);

        let (ptr, alloc) = ArcIn::into_raw_parts(arc);
        let ptr: NonNull<ArcInner<[i32]>> = ptr;
        let arc: ArcIn<[i32], HeapAllocator> = unsafe { ArcIn::from_raw_parts(ptr, alloc) };

        assert_eq!(arc.len(), 3);
        assert_eq!(arc[1], 20);
        assert_eq!(&*arc, &[10, 20, 30]);
    }
}
