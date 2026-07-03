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

//! `ScoreDebug` implementations for common types.

use crate::builders::{DebugList, DebugStruct, DebugTuple};
use crate::fmt::{Error, Result as FmtResult, ScoreDebug, Writer};
use crate::fmt_spec::{DisplayHint, FormatSpec};
use crate::DebugMap;

macro_rules! impl_debug_for_t {
    ($t:ty, $fn:ident) => {
        impl ScoreDebug for $t {
            fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
                f.$fn(self, spec)
            }
        }
    };
}

impl_debug_for_t!(bool, write_bool);
impl_debug_for_t!(f32, write_f32);
impl_debug_for_t!(f64, write_f64);
impl_debug_for_t!(i8, write_i8);
impl_debug_for_t!(i16, write_i16);
impl_debug_for_t!(i32, write_i32);
impl_debug_for_t!(i64, write_i64);
impl_debug_for_t!(u8, write_u8);
impl_debug_for_t!(u16, write_u16);
impl_debug_for_t!(u32, write_u32);
impl_debug_for_t!(u64, write_u64);

impl ScoreDebug for () {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        f.write_str("()", spec)
    }
}

impl ScoreDebug for str {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        match spec.get_display_hint() {
            DisplayHint::Debug => {
                let queue_spec = FormatSpec::new();
                f.write_str("\"", &queue_spec)?;
                f.write_str(self, spec)?;
                f.write_str("\"", &queue_spec)
            },
            _ => f.write_str(self, spec),
        }
    }
}

impl ScoreDebug for String {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&self.as_str(), f, spec)
    }
}

impl ScoreDebug for core::str::Utf8Error {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        let mut debug_struct = DebugStruct::new(f, spec, "Utf8Error");
        debug_struct
            .field("valid_up_to", &self.valid_up_to())
            .field("error_len", &self.error_len())
            .finish()
    }
}

impl ScoreDebug for std::string::FromUtf8Error {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        let mut debug_struct = DebugStruct::new(f, spec, "FromUtf8Error");
        debug_struct
            .field("bytes", &self.as_bytes())
            .field("error", &self.utf8_error())
            .finish()
    }
}

impl ScoreDebug for core::time::Duration {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        f.write_f64(&self.as_secs_f64(), spec)?;
        f.write_str("s", spec)
    }
}

macro_rules! impl_debug_for_t_casted {
    ($ti:ty, $to:ty, $fn:ident) => {
        impl ScoreDebug for $ti {
            fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
                let casted = <$to>::try_from(*self).map_err(|_| Error)?;
                f.$fn(&casted, spec)
            }
        }
    };
}

#[cfg(target_pointer_width = "32")]
impl_debug_for_t_casted!(isize, i32, write_i32);
#[cfg(target_pointer_width = "64")]
impl_debug_for_t_casted!(isize, i64, write_i64);
#[cfg(target_pointer_width = "32")]
impl_debug_for_t_casted!(usize, u32, write_u32);
#[cfg(target_pointer_width = "64")]
impl_debug_for_t_casted!(usize, u64, write_u64);

impl<T: ScoreDebug + ?Sized> ScoreDebug for &T {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&**self, f, spec)
    }
}

impl<T: ScoreDebug + ?Sized> ScoreDebug for &mut T {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&**self, f, spec)
    }
}

impl<T: ScoreDebug> ScoreDebug for [T] {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        let mut debug_list = DebugList::new(f, spec);
        debug_list.entries(self.iter()).finish()
    }
}

impl<T: ScoreDebug, const N: usize> ScoreDebug for [T; N] {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&&self[..], f, spec)
    }
}

impl ScoreDebug for core::array::TryFromSliceError {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        let mut debug_tuple = DebugTuple::new(f, spec, "TryFromSliceError");
        debug_tuple.field(&()).finish()
    }
}

impl<T: ScoreDebug> ScoreDebug for Vec<T> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&**self, f, spec)
    }
}

impl<T: ScoreDebug> ScoreDebug for std::rc::Rc<T> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&**self, f, spec)
    }
}

impl<T: ScoreDebug> ScoreDebug for std::sync::Arc<T> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&**self, f, spec)
    }
}

impl<T: ScoreDebug> ScoreDebug for Option<T> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        match self {
            Some(v) => {
                let outer_spec = FormatSpec::new();
                f.write_str("Some(", &outer_spec)?;
                ScoreDebug::fmt(v, f, spec)?;
                f.write_str(")", &outer_spec)
            },
            None => f.write_str("None", spec),
        }
    }
}

impl<T: ScoreDebug, E: ScoreDebug> ScoreDebug for Result<T, E> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        match self {
            Ok(v) => {
                let outer_spec = FormatSpec::new();
                f.write_str("Ok(", &outer_spec)?;
                ScoreDebug::fmt(v, f, spec)?;
                f.write_str(")", &outer_spec)
            },
            Err(e) => {
                let outer_spec = FormatSpec::new();
                f.write_str("Err(", &outer_spec)?;
                ScoreDebug::fmt(e, f, spec)?;
                f.write_str(")", &outer_spec)
            },
        }
    }
}

impl<T: ScoreDebug + ?Sized> ScoreDebug for Box<T> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        ScoreDebug::fmt(&**self, f, spec)
    }
}

impl<K, V, S> ScoreDebug for std::collections::HashMap<K, V, S>
where
    K: ScoreDebug,
    V: ScoreDebug,
{
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        let mut debug_map = DebugMap::new(f, spec);
        debug_map.entries(self.iter()).finish()
    }
}

impl<T> ScoreDebug for std::sync::PoisonError<T> {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        let mut debug_struct = DebugStruct::new(f, spec, "PoisonError");
        debug_struct.finish_non_exhaustive()
    }
}

impl<A: ScoreDebug, B: ScoreDebug> ScoreDebug for (A, B) {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        DebugTuple::new(f, spec, "").field(&self.0).field(&self.1).finish()
    }
}

impl<A: ScoreDebug, B: ScoreDebug, C: ScoreDebug> ScoreDebug for (A, B, C) {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        DebugTuple::new(f, spec, "")
            .field(&self.0)
            .field(&self.1)
            .field(&self.2)
            .finish()
    }
}

impl<A: ScoreDebug, B: ScoreDebug, C: ScoreDebug, D: ScoreDebug> ScoreDebug for (A, B, C, D) {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        DebugTuple::new(f, spec, "")
            .field(&self.0)
            .field(&self.1)
            .field(&self.2)
            .field(&self.3)
            .finish()
    }
}

impl<A: ScoreDebug, B: ScoreDebug, C: ScoreDebug, D: ScoreDebug, E: ScoreDebug> ScoreDebug for (A, B, C, D, E) {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> FmtResult {
        DebugTuple::new(f, spec, "")
            .field(&self.0)
            .field(&self.1)
            .field(&self.2)
            .field(&self.3)
            .field(&self.4)
            .finish()
    }
}

#[cfg(test)]
mod tests {
    use crate::test_utils::common_test_debug;

    #[test]
    fn test_bool_debug() {
        common_test_debug(true);
    }

    #[test]
    fn test_f32_debug() {
        common_test_debug(123.4f32);
    }

    #[test]
    fn test_f64_debug() {
        common_test_debug(123.4f64);
    }

    #[test]
    fn test_i8_debug() {
        common_test_debug(-123i8);
    }

    #[test]
    fn test_i16_debug() {
        common_test_debug(-1234i16);
    }

    #[test]
    fn test_i32_debug() {
        common_test_debug(-123456i32);
    }

    #[test]
    fn test_i64_debug() {
        common_test_debug(-1200000000000000000i64);
    }

    #[test]
    fn test_u8_debug() {
        common_test_debug(123u8);
    }

    #[test]
    fn test_u16_debug() {
        common_test_debug(1234u16);
    }

    #[test]
    fn test_u32_debug() {
        common_test_debug(123456u32);
    }

    #[test]
    fn test_u64_debug() {
        common_test_debug(1200000000000000000u64);
    }

    #[test]
    fn test_unit_debug() {
        common_test_debug(());
    }

    #[test]
    fn test_str_debug() {
        common_test_debug("test");
    }

    #[test]
    fn test_string_debug() {
        common_test_debug(String::from("test"));
    }

    #[test]
    fn test_utf8_error_debug() {
        let a1 = vec![0xa0, 0xa1];
        let a2 = core::str::from_utf8(&a1);
        common_test_debug(a2.unwrap_err());
    }

    #[test]
    fn test_from_utf8_error_debug() {
        let a1 = vec![0xa0, 0xa1];
        let a2: Result<String, std::string::FromUtf8Error> = a1.try_into();
        common_test_debug(a2.unwrap_err());
    }

    #[test]
    fn test_isize_debug() {
        common_test_debug(-1200000000000000000isize);
    }

    #[test]
    fn test_usize_debug() {
        common_test_debug(1200000000000000000usize);
    }

    #[test]
    fn test_slice_debug() {
        common_test_debug([123, 456, 789].as_slice());
    }

    #[test]
    fn test_array_debug() {
        common_test_debug([123, 456, 789]);
    }

    #[test]
    fn test_try_from_slice_error_debug() {
        let a1 = vec![123, 456];
        let a2: Result<[i32; 3], core::array::TryFromSliceError> = a1.as_slice().try_into();
        common_test_debug(a2.unwrap_err());
    }

    #[test]
    fn test_vec_debug() {
        common_test_debug(vec![987, 654, 321, 159]);
    }

    #[test]
    fn test_rc_debug() {
        let rc = std::rc::Rc::new(444);
        common_test_debug(rc);
    }

    #[test]
    fn test_arc_debug() {
        let arc = std::sync::Arc::new(654);
        common_test_debug(arc);
    }

    #[test]
    fn test_option_debug() {
        common_test_debug(Some(123));
        common_test_debug(Option::<i32>::None);
    }

    #[test]
    fn test_result_debug() {
        let r1: Result<i32, &'static str> = Ok(123);
        common_test_debug(r1);
        let r2: Result<i32, &'static str> = Err("fail");
        common_test_debug(r2);
    }

    #[test]
    fn test_box_debug() {
        common_test_debug(Box::new(432.1));
    }

    #[test]
    fn test_duration_debug() {
        common_test_debug(core::time::Duration::new(123, 456789));
    }

    #[test]
    fn test_hashmap_debug() {
        common_test_debug(std::collections::HashMap::from([("x", 123), ("y", 321), ("z", 444)]));
    }

    #[test]
    fn test_poison_error_debug() {
        let pe = std::sync::PoisonError::new(123.0);
        common_test_debug(pe);
    }

    #[test]
    fn test_tuples_debug() {
        common_test_debug((2.1f32, "abc"));
        common_test_debug((28, Box::new(46), true));
        common_test_debug((
            (
                std::collections::HashMap::from([("x", 123), ("y", 321), ("z", 444)]),
                "abc",
            ),
            Some(123),
            std::sync::Arc::new(654),
            vec![987, 654],
        ));
        common_test_debug(("a", "b", (r"0x64", 10, false), "0.1", "true"));
    }
}
