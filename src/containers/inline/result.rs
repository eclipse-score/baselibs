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
use core::mem::ManuallyDrop;
use score_log::fmt::{FormatSpec, Result as ScoreLogResult, ScoreDebug, Writer};

/// An result value for error handling, similar to [`Result`] in the Rust standard library.
///
/// This data structure has a stable, well-defined memory layout and satisfies the requirements for
/// [ABI-compatible types](https://eclipse-score.github.io/score/main/features/communication/abi_compatible_data_types/index.html).
///
/// **Note:** When encoding an instance of this type in other programming languages,
/// the `is_ok` field must only be assigned the binary values `0x00` and `0x01`;
/// everything else results in *Undefined Behavior*.
#[derive(Clone, Copy)]
#[repr(C)]
pub struct InlineResult<T: Copy, E: Copy> {
    /// When `is_ok` is true, then `value.ok` is valid, otherwise `value.err`.
    value: ResultUnion<T, E>,
    is_ok: bool,
}

#[derive(Clone, Copy)]
#[repr(C)]
union ResultUnion<T: Copy, E: Copy> {
    ok: ManuallyDrop<T>,
    err: ManuallyDrop<E>,
}

impl<T: Copy, E: Copy> InlineResult<T, E> {
    /// Creates a new instance representing success with `value` as payload.
    pub const fn ok(value: T) -> Self {
        Self {
            value: ResultUnion {
                ok: ManuallyDrop::new(value),
            },
            is_ok: true,
        }
    }

    /// Creates a new instance representing failure with `error` as payload.
    pub const fn err(error: E) -> Self {
        Self {
            value: ResultUnion {
                err: ManuallyDrop::new(error),
            },
            is_ok: false,
        }
    }

    /// Creates a new instance from a standard [`Result`].
    pub fn from_result(result: Result<T, E>) -> Self {
        match result {
            Ok(value) => Self::ok(value),
            Err(error) => Self::err(error),
        }
    }

    /// Converts this instance into a standard [`Result`].
    pub const fn into_result(self) -> Result<T, E> {
        if self.is_ok {
            // SAFETY: `is_ok` is true, so `value.ok` must be valid as per its invariant.
            Ok(ManuallyDrop::into_inner(unsafe { self.value.ok }))
        } else {
            // SAFETY: `is_ok` is false, so `value.err` must be valid as per its invariant.
            Err(ManuallyDrop::into_inner(unsafe { self.value.err }))
        }
    }

    /// Returns a result-reference to the contained value.
    pub fn as_ref(&self) -> Result<&T, &E> {
        if self.is_ok {
            // SAFETY: `is_ok` is true, so `value.ok` must be valid as per its invariant.
            Ok(unsafe { &self.value.ok })
        } else {
            // SAFETY: `is_ok` is false, so `value.err` must be valid as per its invariant.
            Err(unsafe { &self.value.err })
        }
    }

    /// Returns a mutable result-reference to the contained value.
    pub fn as_mut(&mut self) -> Result<&mut T, &mut E> {
        if self.is_ok {
            // SAFETY: `is_ok` is true, so `value.ok` must be valid as per its invariant.
            Ok(unsafe { &mut self.value.ok })
        } else {
            // SAFETY: `is_ok` is false, so `value.err` must be valid as per its invariant.
            Err(unsafe { &mut self.value.err })
        }
    }
}

impl<T: Default + Copy, E: Copy> Default for InlineResult<T, E> {
    fn default() -> Self {
        Self {
            value: ResultUnion {
                ok: ManuallyDrop::new(T::default()),
            },
            is_ok: true,
        }
    }
}

impl<T: Copy, E: Copy> From<T> for InlineResult<T, E> {
    fn from(value: T) -> Self {
        Self {
            value: ResultUnion {
                ok: ManuallyDrop::new(value),
            },
            is_ok: true,
        }
    }
}

impl<T: Copy, E: Copy> From<Result<T, E>> for InlineResult<T, E> {
    fn from(result: Result<T, E>) -> Self {
        Self::from_result(result)
    }
}

impl<T: Copy, E: Copy> From<InlineResult<T, E>> for Result<T, E> {
    fn from(result: InlineResult<T, E>) -> Self {
        result.into_result()
    }
}

impl<T: PartialEq + Copy, E: PartialEq + Copy> PartialEq for InlineResult<T, E> {
    fn eq(&self, other: &Self) -> bool {
        match (self.as_ref(), other.as_ref()) {
            (Ok(a), Ok(b)) => a.eq(b),
            (Ok(_), Err(_)) => false,
            (Err(_), Ok(_)) => false,
            (Err(a), Err(b)) => a.eq(b),
        }
    }
}

impl<T: Eq + Copy, E: Eq + Copy> Eq for InlineResult<T, E> {}

impl<T: PartialOrd + Copy, E: PartialOrd + Copy> PartialOrd for InlineResult<T, E> {
    fn partial_cmp(&self, other: &Self) -> Option<cmp::Ordering> {
        match (self.as_ref(), other.as_ref()) {
            (Ok(a), Ok(b)) => a.partial_cmp(b),
            (Ok(_), Err(_)) => Some(cmp::Ordering::Less),
            (Err(_), Ok(_)) => Some(cmp::Ordering::Greater),
            (Err(a), Err(b)) => a.partial_cmp(b),
        }
    }
}

impl<T: Ord + Copy, E: Ord + Copy> Ord for InlineResult<T, E> {
    fn cmp(&self, other: &Self) -> cmp::Ordering {
        match (self.as_ref(), other.as_ref()) {
            (Ok(a), Ok(b)) => a.cmp(b),
            (Ok(_), Err(_)) => cmp::Ordering::Less,
            (Err(_), Ok(_)) => cmp::Ordering::Greater,
            (Err(a), Err(b)) => a.cmp(b),
        }
    }
}

impl<T: Copy, E: Copy> fmt::Display for InlineResult<T, E>
where
    for<'a> Result<&'a T, &'a E>: fmt::Display,
{
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(&self.as_ref(), f)
    }
}

impl<T: Copy, E: Copy> fmt::Debug for InlineResult<T, E>
where
    for<'a> Result<&'a T, &'a E>: fmt::Debug,
{
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Debug::fmt(&self.as_ref(), f)
    }
}

impl<T: Copy, E: Copy> ScoreDebug for InlineResult<T, E>
where
    for<'a> Result<&'a T, &'a E>: ScoreDebug,
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
        let original_ok: Result<u64, f64> = Ok(0x1234567890abcdef_u64);
        let inline_ok = InlineResult::from_result(original_ok);
        assert_eq!(inline_ok.into_result(), original_ok);

        let original_err: Result<u64, f64> = Err(1.2345_f64);
        let inline_err = InlineResult::from_result(original_err);
        assert_eq!(inline_err.into_result(), original_err);
    }

    #[test]
    fn as_ref() {
        let original_ok: Result<u64, f64> = Ok(0x1234567890abcdef_u64);
        let inline_ok = InlineResult::from_result(original_ok);
        assert_eq!(inline_ok.as_ref(), original_ok.as_ref());

        let original_err: Result<u64, f64> = Err(1.2345_f64);
        let inline_err = InlineResult::from_result(original_err);
        assert_eq!(inline_err.as_ref(), original_err.as_ref());
    }

    #[test]
    fn as_mut() {
        let mut original_ok: Result<u64, f64> = Ok(0x1234567890abcdef_u64);
        let mut inline_ok = InlineResult::from_result(original_ok);
        assert_eq!(inline_ok.as_mut(), original_ok.as_mut());
        *original_ok.as_mut().unwrap() = 5;
        *inline_ok.as_mut().unwrap() = 5;
        assert_eq!(inline_ok.as_mut(), original_ok.as_mut());

        let mut original_err: Result<u64, f64> = Err(1.2345_f64);
        let mut inline_err = InlineResult::from_result(original_err);
        assert_eq!(inline_err.as_mut(), original_err.as_mut());
    }

    #[test]
    fn eq() {
        #[track_caller]
        fn check(lhs: &InlineResult<i32, i32>, rhs: &InlineResult<i32, i32>) {
            // Check that InlineResult behaves exactly like Result, since Result's implementation is assumed to be correct.
            assert_eq!(lhs.eq(rhs), lhs.as_ref().eq(&rhs.as_ref()));
            assert_eq!(lhs.ne(rhs), lhs.as_ref().ne(&rhs.as_ref()));
        }

        let ok_5 = InlineResult::ok(5);
        let ok_6 = InlineResult::ok(6);
        let err_5 = InlineResult::err(5);
        let err_6 = InlineResult::err(6);
        // Check all combinations
        for lhs in &[ok_5, ok_6, err_5, err_6] {
            for rhs in &[ok_5, ok_6, err_5, err_6] {
                check(lhs, rhs);
            }
        }
    }

    #[test]
    fn partial_cmp() {
        #[track_caller]
        fn check(lhs: &InlineResult<i32, i32>, rhs: &InlineResult<i32, i32>) {
            // Check that InlineResult behaves exactly like Result, since Result's implementation is assumed to be correct.
            assert_eq!(lhs.partial_cmp(rhs), lhs.as_ref().partial_cmp(&rhs.as_ref()));
            assert_eq!(lhs < rhs, lhs.as_ref() < rhs.as_ref());
            assert_eq!(lhs <= rhs, lhs.as_ref() <= rhs.as_ref());
            assert_eq!(lhs > rhs, lhs.as_ref() > rhs.as_ref());
            assert_eq!(lhs >= rhs, lhs.as_ref() >= rhs.as_ref());
        }

        let ok_5 = InlineResult::ok(5);
        let ok_6 = InlineResult::ok(6);
        let err_5 = InlineResult::err(5);
        let err_6 = InlineResult::err(6);
        // Check all combinations
        for lhs in &[ok_5, ok_6, err_5, err_6] {
            for rhs in &[ok_5, ok_6, err_5, err_6] {
                check(lhs, rhs);
            }
        }
    }

    #[test]
    fn cmp() {
        #[track_caller]
        fn check(lhs: &InlineResult<i32, i32>, rhs: &InlineResult<i32, i32>) {
            // Check that InlineResult behaves exactly like Result, since Result's implementation is assumed to be correct.
            assert_eq!(lhs.cmp(rhs), lhs.as_ref().cmp(&rhs.as_ref()));
            assert_eq!(lhs.max(rhs).as_ref(), lhs.as_ref().max(rhs.as_ref()));
            assert_eq!(lhs.min(rhs).as_ref(), lhs.as_ref().min(rhs.as_ref()));
        }

        let ok_5 = InlineResult::ok(5);
        let ok_6 = InlineResult::ok(6);
        let err_5 = InlineResult::err(5);
        let err_6 = InlineResult::err(6);
        // Check all combinations
        for lhs in &[ok_5, ok_6, err_5, err_6] {
            for rhs in &[ok_5, ok_6, err_5, err_6] {
                check(lhs, rhs);
            }
        }
    }
}
