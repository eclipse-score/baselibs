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

use crate::FormatSpec;
use core::marker::PhantomData;
use core::ptr::NonNull;

/// The type returned by writer methods.
pub type Result = core::result::Result<(), Error>;

/// The type of the writer.
pub type Writer<'a> = &'a mut dyn ScoreWrite;

/// The error type which is returned from writing a message.
///
/// This type does not support transmission of an error other than an error occurred.
/// This is because, despite the existence of this error, writing is considered an infallible operation.
/// `fmt()` implementors should not return this `Error` unless the received it from their [`ScoreWrite`] implementation.
#[derive(Copy, Clone, Default, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct Error;

/// A trait for writing into message frames.
///
/// This trait accepts multiple data types.
/// Implementation is responsible for output formatting based on provided spec.
pub trait ScoreWrite {
    /// Write a `bool` into this writer.
    fn write_bool(&mut self, v: &bool, spec: &FormatSpec) -> Result;
    /// Write a `f32` into this writer.
    fn write_f32(&mut self, v: &f32, spec: &FormatSpec) -> Result;
    /// Write a `f64` into this writer.
    fn write_f64(&mut self, v: &f64, spec: &FormatSpec) -> Result;
    /// Write a `i8` into this writer.
    fn write_i8(&mut self, v: &i8, spec: &FormatSpec) -> Result;
    /// Write a `i16` into this writer.
    fn write_i16(&mut self, v: &i16, spec: &FormatSpec) -> Result;
    /// Write a `i32` into this writer.
    fn write_i32(&mut self, v: &i32, spec: &FormatSpec) -> Result;
    /// Write a `i64` into this writer.
    fn write_i64(&mut self, v: &i64, spec: &FormatSpec) -> Result;
    /// Write a `u8` into this writer.
    fn write_u8(&mut self, v: &u8, spec: &FormatSpec) -> Result;
    /// Write a `u16` into this writer.
    fn write_u16(&mut self, v: &u16, spec: &FormatSpec) -> Result;
    /// Write a `u32` into this writer.
    fn write_u32(&mut self, v: &u32, spec: &FormatSpec) -> Result;
    /// Write a `u64` into this writer.
    fn write_u64(&mut self, v: &u64, spec: &FormatSpec) -> Result;
    /// Write a `&str` into this writer.
    fn write_str(&mut self, v: &str, spec: &FormatSpec) -> Result;
}

/// Data placeholder in message.
pub struct Placeholder<'a> {
    value: NonNull<()>,
    formatter: fn(NonNull<()>, Writer, &FormatSpec) -> Result,
    spec: FormatSpec,
    _lifetime: PhantomData<&'a ()>,
}

impl<'a> Placeholder<'a> {
    /// Create the placeholder to be represented using `ScoreDebug`.
    pub const fn new<T: ScoreDebug>(value: &'a T, spec: FormatSpec) -> Self {
        let value = NonNull::from_ref(value).cast();
        let formatter = |v: NonNull<()>, f: Writer, spec: &FormatSpec| {
            // SAFETY: borrow checker will ensure that value won't be mutated for as long as the returned `Self` instance is alive.
            let typed = unsafe { v.cast::<T>().as_ref() };
            typed.fmt(f, spec)
        };
        Self {
            value,
            formatter,
            spec,
            _lifetime: PhantomData,
        }
    }

    /// Get format spec of this placeholder.
    pub fn format_spec(&self) -> &FormatSpec {
        &self.spec
    }

    /// Write requested representation of data to the provided writer.
    pub fn fmt(&self, f: Writer, spec: &FormatSpec) -> Result {
        (self.formatter)(self.value, f, spec)
    }
}

/// Message fragment.
/// A string literal or data placeholder.
pub enum Fragment<'a> {
    /// Fragment is a string literal, with no additional formatting.
    Literal(&'a str),
    /// Fragment is a placeholder for provided data.
    Placeholder(Placeholder<'a>),
}

/// Array of message parts.
/// Consists of [`Fragment`] entities.
#[derive(Copy, Clone)]
pub struct Arguments<'a>(pub &'a [Fragment<'a>]);

impl ScoreDebug for Arguments<'_> {
    fn fmt(&self, f: Writer, _spec: &FormatSpec) -> Result {
        write(f, *self)
    }
}

/// `ScoreDebug` provides the output in a programmer-facing, debugging context.
/// Replacement for [`core::fmt::Debug`].
pub trait ScoreDebug {
    /// Write debug representation of `self` to the provided writer.
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> Result;
}

/// Write [`Arguments`] into provided `output` writer.
///
/// The arguments will be formatted according to provided format spec.
pub fn write(output: Writer, args: Arguments<'_>) -> Result {
    for fragment in args.0 {
        match fragment {
            Fragment::Literal(s) => output.write_str(s, &FormatSpec::new()),
            Fragment::Placeholder(ph) => ph.fmt(output, &ph.spec),
        }?;
    }

    Ok(())
}

#[cfg(test)]
mod tests {
    use crate::test_utils::StringWriter;
    use crate::{write, Arguments, FormatSpec, Fragment, Placeholder, ScoreDebug};

    #[test]
    fn test_arguments_debug() {
        let mut w = StringWriter::new();
        let fragments = [
            Fragment::Literal("test_"),
            Fragment::Placeholder(Placeholder::new(&true, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&123.4f32, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&432.2f64, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-100i8, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-1234i16, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-123456i32, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-1200000000000000000i64, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&123u8, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&1234u16, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&123456u32, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&1200000000000000000u64, FormatSpec::new())),
            Fragment::Literal("_string"),
        ];
        let args = Arguments(&fragments);

        let result = ScoreDebug::fmt(&args, &mut w, &FormatSpec::new());
        assert!(result == Ok(()));
        assert!(
            w.get() == "test_true123.4432.2-100-1234-123456-120000000000000000012312341234561200000000000000000_string"
        )
    }

    #[test]
    fn test_write_empty() {
        let mut w = StringWriter::new();
        let args = Arguments(&[]);
        assert!(write(&mut w, args) == Ok(()));
    }

    #[test]
    fn test_write_literals_only() {
        let mut w = StringWriter::new();
        let args = Arguments(&[Fragment::Literal("test_"), Fragment::Literal("string")]);
        assert!(write(&mut w, args) == Ok(()));
        assert!(w.get() == "test_string");
    }

    #[test]
    fn test_write_placeholders_only() {
        let mut w = StringWriter::new();
        let fragments = [
            Fragment::Placeholder(Placeholder::new(&true, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&123.4f32, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&432.2f64, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-100i8, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-1234i16, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-123456i32, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&-1200000000000000000i64, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&123u8, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&1234u16, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&123456u32, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&1200000000000000000u64, FormatSpec::new())),
            Fragment::Placeholder(Placeholder::new(&"test", FormatSpec::new())),
        ];
        let args = Arguments(&fragments);
        assert!(write(&mut w, args) == Ok(()));

        let exp_pattern = "true123.4432.2-100-1234-123456-120000000000000000012312341234561200000000000000000test";
        assert_eq!(w.get(), exp_pattern);
    }

    #[test]
    fn test_write_mixed() {
        let mut w = StringWriter::new();
        let fragments = [
            Fragment::Literal("test_"),
            Fragment::Placeholder(Placeholder::new(&123i8, FormatSpec::new())),
            Fragment::Literal("_string"),
        ];
        let args = Arguments(&fragments);
        assert!(write(&mut w, args) == Ok(()));
        assert!(w.get() == "test_123_string");
    }
}
