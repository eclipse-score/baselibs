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

use core::cmp;
use core::fmt;
use core::mem::MaybeUninit;
use score_log::fmt::{FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

/// An optional value, similar to [`Option`] in the Rust standard library.
///
/// This data structure has a stable, well-defined memory layout and satisfies the requirements for
/// [ABI-compatible types](https://eclipse-score.github.io/score/main/features/communication/abi_compatible_data_types/index.html).
/// Unlike the built-in [`Option`] or other enum-based types, this data structure will **not** be affected by
/// [*null-pointer optimization*](https://doc.rust-lang.org/core/option/index.html#representation),
/// where invalid byte representations of `T` are used to encode the `None` case.
///
/// **Note:** When encoding an instance of this type in other programming languages,
/// the `is_some` field must only be assigned the binary values `0x00` and `0x01`;
/// everything else results in *Undefined Behavior*.
#[derive(Clone, Copy)]
#[repr(C)]
pub struct InlineOption<T: Copy> {
    /// This is valid/initialized if and only if `is_some` is true.
    value: MaybeUninit<T>,
    is_some: bool,
}

impl<T: Copy> InlineOption<T> {
    /// Creates a new instance populated by `value`.
    pub const fn some(value: T) -> Self {
        Self {
            value: MaybeUninit::new(value),
            is_some: true,
        }
    }

    /// Creates an empty instance.
    pub const fn none() -> Self {
        Self {
            value: MaybeUninit::uninit(),
            is_some: false,
        }
    }

    /// Creates a new instance from a standard [`Option`].
    pub fn from_option(option: Option<T>) -> Self {
        if let Some(value) = option {
            Self::some(value)
        } else {
            Self::none()
        }
    }

    /// Converts this instance into a standard [`Option`].
    pub const fn into_option(self) -> Option<T> {
        if self.is_some {
            // SAFETY: `is_some` is true, so `value` must be valid as per its invariant.
            Some(unsafe { self.value.assume_init() })
        } else {
            None
        }
    }

    /// Returns an optional reference to the contained value.
    pub const fn as_ref(&self) -> Option<&T> {
        if self.is_some {
            // SAFETY: `is_some` is true, so `value` must be valid as per its invariant.
            Some(unsafe { self.value.assume_init_ref() })
        } else {
            None
        }
    }

    /// Returns an optional mutable reference to the contained value.
    pub const fn as_mut(&mut self) -> Option<&mut T> {
        if self.is_some {
            // SAFETY: `is_some` is true, so `value` must be valid as per its invariant.
            Some(unsafe { self.value.assume_init_mut() })
        } else {
            None
        }
    }
}

impl<T: Copy> Default for InlineOption<T> {
    fn default() -> Self {
        Self {
            value: MaybeUninit::uninit(),
            is_some: false,
        }
    }
}

impl<T: Copy> From<T> for InlineOption<T> {
    fn from(value: T) -> Self {
        Self {
            value: MaybeUninit::new(value),
            is_some: true,
        }
    }
}

impl<T: Copy> From<Option<T>> for InlineOption<T> {
    fn from(option: Option<T>) -> Self {
        Self::from_option(option)
    }
}

impl<T: Copy> From<InlineOption<T>> for Option<T> {
    fn from(option: InlineOption<T>) -> Self {
        option.into_option()
    }
}

impl<T: PartialEq + Copy> PartialEq for InlineOption<T> {
    fn eq(&self, other: &Self) -> bool {
        match (self.as_ref(), other.as_ref()) {
            (Some(a), Some(b)) => a.eq(b),
            (Some(_), None) => false,
            (None, Some(_)) => false,
            (None, None) => true,
        }
    }
}

impl<T: Eq + Copy> Eq for InlineOption<T> {}

impl<T: PartialOrd + Copy> PartialOrd for InlineOption<T> {
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering> {
        match (self.as_ref(), other.as_ref()) {
            (Some(a), Some(b)) => a.partial_cmp(b),
            (Some(_), None) => Some(cmp::Ordering::Greater),
            (None, Some(_)) => Some(cmp::Ordering::Less),
            (None, None) => Some(cmp::Ordering::Equal),
        }
    }
}

impl<T: Ord + Copy> Ord for InlineOption<T> {
    fn cmp(&self, other: &Self) -> cmp::Ordering {
        match (self.as_ref(), other.as_ref()) {
            (Some(a), Some(b)) => a.cmp(b),
            (Some(_), None) => cmp::Ordering::Greater,
            (None, Some(_)) => cmp::Ordering::Less,
            (None, None) => cmp::Ordering::Equal,
        }
    }
}

impl<T: Copy> fmt::Display for InlineOption<T>
where
    for<'a> Option<&'a T>: fmt::Display,
{
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(&self.as_ref(), f)
    }
}

impl<T: Copy> fmt::Debug for InlineOption<T>
where
    for<'a> Option<&'a T>: fmt::Debug,
{
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(&self.as_ref(), f)
    }
}

impl<T: Copy> ScoreDebug for InlineOption<T>
where
    for<'a> Option<&'a T>: ScoreDebug,
{
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> ScoreLogResult {
        ScoreDebug::fmt(&self.as_ref(), f, spec)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn from_and_into() {
        let original_some = Some(0x1234567890abcdef_u64);
        let inline_some = InlineOption::from_option(original_some);
        assert_eq!(inline_some.into_option(), original_some);

        let original_none: Option<u64> = None;
        let inline_none = InlineOption::from_option(original_none);
        assert_eq!(inline_none.into_option(), original_none);
    }

    #[test]
    fn as_ref() {
        let original_some = Some(0x1234567890abcdef_u64);
        let inline_some = InlineOption::from_option(original_some);
        assert_eq!(inline_some.as_ref(), original_some.as_ref());

        let original_none: Option<u64> = None;
        let inline_none = InlineOption::from_option(original_none);
        assert_eq!(inline_none.as_ref(), original_none.as_ref());
    }

    #[test]
    fn as_mut() {
        let mut original_some = Some(0x1234567890abcdef_u64);
        let mut inline_some = InlineOption::from_option(original_some);
        assert_eq!(inline_some.as_mut(), original_some.as_mut());
        *original_some.as_mut().unwrap() = 5;
        *inline_some.as_mut().unwrap() = 5;
        assert_eq!(inline_some.as_mut(), original_some.as_mut());

        let mut original_none: Option<u64> = None;
        let mut inline_none = InlineOption::from_option(original_none);
        assert_eq!(inline_none.as_mut(), original_none.as_mut());
    }

    #[test]
    fn eq() {
        #[track_caller]
        fn check(lhs: &InlineOption<i32>, rhs: &InlineOption<i32>) {
            // Check that InlineOption behaves exactly like Option, since Option's implementation is assumed to be correct.
            assert_eq!(lhs.eq(rhs), lhs.as_ref().eq(&rhs.as_ref()));
            assert_eq!(lhs.ne(rhs), lhs.as_ref().ne(&rhs.as_ref()));
        }

        let some_5 = InlineOption::some(5);
        let some_6 = InlineOption::some(6);
        let none = InlineOption::none();
        // Check all combinations
        for lhs in &[some_5, some_6, none] {
            for rhs in &[some_5, some_6, none] {
                check(lhs, rhs);
            }
        }
    }

    #[test]
    fn partial_cmp() {
        #[track_caller]
        fn check(lhs: &InlineOption<i32>, rhs: &InlineOption<i32>) {
            // Check that InlineOption behaves exactly like Option, since Option's implementation is assumed to be correct.
            assert_eq!(lhs.partial_cmp(rhs), lhs.as_ref().partial_cmp(&rhs.as_ref()));
            assert_eq!(lhs < rhs, lhs.as_ref() < rhs.as_ref());
            assert_eq!(lhs <= rhs, lhs.as_ref() <= rhs.as_ref());
            assert_eq!(lhs > rhs, lhs.as_ref() > rhs.as_ref());
            assert_eq!(lhs >= rhs, lhs.as_ref() >= rhs.as_ref());
        }

        let some_5 = InlineOption::some(5);
        let some_6 = InlineOption::some(6);
        let none = InlineOption::none();
        // Check all combinations
        for lhs in &[some_5, some_6, none] {
            for rhs in &[some_5, some_6, none] {
                check(lhs, rhs);
            }
        }
    }

    #[test]
    fn cmp() {
        #[track_caller]
        fn check(lhs: &InlineOption<i32>, rhs: &InlineOption<i32>) {
            // Check that InlineOption behaves exactly like Option, since Option's implementation is assumed to be correct.
            assert_eq!(lhs.cmp(rhs), lhs.as_ref().cmp(&rhs.as_ref()));
            assert_eq!(lhs.max(rhs).as_ref(), lhs.as_ref().max(rhs.as_ref()));
            assert_eq!(lhs.min(rhs).as_ref(), lhs.as_ref().min(rhs.as_ref()));
        }

        let some_5 = InlineOption::some(5);
        let some_6 = InlineOption::some(6);
        let none = InlineOption::none();
        // Check all combinations
        for lhs in &[some_5, some_6, none] {
            for rhs in &[some_5, some_6, none] {
                check(lhs, rhs);
            }
        }
    }
}
