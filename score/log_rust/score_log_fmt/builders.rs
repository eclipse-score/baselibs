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

//! Implementations of [`ScoreDebug`] implementation helper builders.

use crate::{FormatSpec, Result, ScoreDebug, Writer};

/// Output a formatted struct.
///
/// Useful as a part of [`ScoreDebug::fmt`] implementation.
#[must_use = "must eventually call `finish()` on ScoreDebug builders"]
pub struct DebugStruct<'a> {
    writer: Writer<'a>,
    spec: &'a FormatSpec,
    result: Result,
    has_fields: bool,
}

impl<'a> DebugStruct<'a> {
    /// Create `DebugStruct` instance.
    pub fn new(writer: Writer<'a>, spec: &'a FormatSpec, name: &str) -> Self {
        let result = writer.write_str(name, &FormatSpec::new());
        DebugStruct {
            writer,
            spec,
            result,
            has_fields: false,
        }
    }

    /// Adds a new field to the generated struct output.
    pub fn field(&mut self, name: &str, value: &dyn ScoreDebug) -> &mut Self {
        self.field_with(name, |f| value.fmt(f, self.spec))
    }

    /// Adds a new field to the generated struct output.
    ///
    /// This method is equivalent to [`DebugStruct::field`], but formats the value using a provided closure rather than by calling [`ScoreDebug::fmt`].
    pub fn field_with<F>(&mut self, name: &str, value_fmt: F) -> &mut Self
    where
        F: FnOnce(Writer) -> Result,
    {
        self.result = self.result.and_then(|_| {
            let prefix = if self.has_fields { ", " } else { " { " };
            let empty_spec = FormatSpec::new();
            self.writer.write_str(prefix, &empty_spec)?;
            self.writer.write_str(name, &empty_spec)?;
            self.writer.write_str(": ", &empty_spec)?;
            value_fmt(self.writer)
        });

        self.has_fields = true;
        self
    }

    /// Marks the struct as non-exhaustive, indicating to the reader that there are some other fields that are not shown in the debug representation.
    pub fn finish_non_exhaustive(&mut self) -> Result {
        self.result = self.result.and_then(|_| {
            let empty_spec = FormatSpec::new();
            if self.has_fields {
                self.writer.write_str(", .. }", &empty_spec)
            } else {
                self.writer.write_str(" { .. }", &empty_spec)
            }
        });
        self.result
    }

    /// Finishes output and returns any error encountered.
    pub fn finish(&mut self) -> Result {
        if self.has_fields {
            let empty_spec = FormatSpec::new();
            self.result = self.result.and_then(|_| self.writer.write_str(" }", &empty_spec));
        }
        self.result
    }
}

/// Output a formatted tuple.
///
/// Useful as a part of [`ScoreDebug::fmt`] implementation.
#[must_use = "must eventually call `finish()` on ScoreDebug builders"]
pub struct DebugTuple<'a> {
    writer: Writer<'a>,
    spec: &'a FormatSpec,
    result: Result,
    fields: usize,
    empty_name: bool,
}

impl<'a> DebugTuple<'a> {
    /// Create `DebugTuple` instance.
    pub fn new(writer: Writer<'a>, spec: &'a FormatSpec, name: &str) -> Self {
        let result = writer.write_str(name, &FormatSpec::new());
        DebugTuple {
            writer,
            spec,
            result,
            fields: 0,
            empty_name: name.is_empty(),
        }
    }

    /// Adds a new field to the generated tuple struct output.
    pub fn field(&mut self, value: &dyn ScoreDebug) -> &mut Self {
        self.field_with(|f| value.fmt(f, self.spec))
    }

    /// Adds a new field to the generated tuple struct output.
    ///
    /// This method is equivalent to [`DebugTuple::field`], but formats the value using a provided closure rather than by calling [`ScoreDebug::fmt`].
    pub fn field_with<F>(&mut self, value_fmt: F) -> &mut Self
    where
        F: FnOnce(Writer) -> Result,
    {
        self.result = self.result.and_then(|_| {
            let prefix = if self.fields == 0 { "(" } else { ", " };
            let empty_spec = FormatSpec::new();
            self.writer.write_str(prefix, &empty_spec)?;
            value_fmt(self.writer)
        });

        self.fields += 1;
        self
    }

    /// Marks the tuple struct as non-exhaustive, indicating to the reader that there are some other fields that are not shown in the debug representation.
    pub fn finish_non_exhaustive(&mut self) -> Result {
        self.result = self.result.and_then(|_| {
            let empty_spec = FormatSpec::new();
            if self.fields > 0 {
                self.writer.write_str(", ..)", &empty_spec)
            } else {
                self.writer.write_str("(..)", &empty_spec)
            }
        });
        self.result
    }

    /// Finishes output and returns any error encountered.
    pub fn finish(&mut self) -> Result {
        if self.fields > 0 {
            self.result = self.result.and_then(|_| {
                let empty_spec = FormatSpec::new();
                if self.fields == 1 && self.empty_name {
                    self.writer.write_str(",", &empty_spec)?;
                }
                self.writer.write_str(")", &empty_spec)
            });
        }
        self.result
    }
}

/// A helper used to print list-like items with no special formatting.
struct DebugInner<'a> {
    writer: Writer<'a>,
    spec: &'a FormatSpec,
    result: Result,
    has_fields: bool,
}

impl<'a> DebugInner<'a> {
    fn entry_with<F>(&mut self, entry_writer: F)
    where
        F: FnOnce(Writer) -> Result,
    {
        self.result = self.result.and_then(|_| {
            let empty_spec = FormatSpec::new();
            if self.has_fields {
                self.writer.write_str(", ", &empty_spec)?
            }
            entry_writer(self.writer)
        });

        self.has_fields = true;
    }
}

/// Output a formatted set of items.
///
/// Useful as a part of [`ScoreDebug::fmt`] implementation.
#[must_use = "must eventually call `finish()` on ScoreDebug builders"]
pub struct DebugSet<'a> {
    inner: DebugInner<'a>,
}

impl<'a> DebugSet<'a> {
    /// Create `DebugSet` instance.
    pub fn new(writer: Writer<'a>, spec: &'a FormatSpec) -> Self {
        let result = writer.write_str("{", &FormatSpec::new());
        DebugSet {
            inner: DebugInner {
                writer,
                spec,
                result,
                has_fields: false,
            },
        }
    }

    /// Adds a new entry to the set output.
    pub fn entry(&mut self, entry: &dyn ScoreDebug) -> &mut Self {
        self.inner.entry_with(|f| entry.fmt(f, self.inner.spec));
        self
    }

    /// Adds a new entry to the set output.
    ///
    /// This method is equivalent to [`DebugSet::entry`], but formats the entry using a provided closure rather than by calling [`ScoreDebug::fmt`].
    pub fn entry_with<F>(&mut self, entry_fmt: F) -> &mut Self
    where
        F: FnOnce(Writer) -> Result,
    {
        self.inner.entry_with(entry_fmt);
        self
    }

    /// Adds the contents of an iterator of entries to the set output.
    pub fn entries<D, I>(&mut self, entries: I) -> &mut Self
    where
        D: ScoreDebug,
        I: IntoIterator<Item = D>,
    {
        for entry in entries {
            self.entry(&entry);
        }
        self
    }

    /// Marks the set as non-exhaustive, indicating to the reader that there are some other elements that are not shown in the debug representation.
    pub fn finish_non_exhaustive(&mut self) -> Result {
        self.inner.result = self.inner.result.and_then(|_| {
            let empty_spec = FormatSpec::new();
            if self.inner.has_fields {
                self.inner.writer.write_str(", ..}", &empty_spec)
            } else {
                self.inner.writer.write_str("..}", &empty_spec)
            }
        });
        self.inner.result
    }

    /// Finishes output and returns any error encountered.
    pub fn finish(&mut self) -> Result {
        self.inner.result = self
            .inner
            .result
            .and_then(|_| self.inner.writer.write_str("}", &FormatSpec::new()));
        self.inner.result
    }
}

/// Output a formatted list of items.
///
/// Useful as a part of [`ScoreDebug::fmt`] implementation.
#[must_use = "must eventually call `finish()` on ScoreDebug builders"]
pub struct DebugList<'a> {
    inner: DebugInner<'a>,
}

impl<'a> DebugList<'a> {
    /// Create `DebugList` instance.
    pub fn new(writer: Writer<'a>, spec: &'a FormatSpec) -> Self {
        let result = writer.write_str("[", &FormatSpec::new());
        DebugList {
            inner: DebugInner {
                writer,
                spec,
                result,
                has_fields: false,
            },
        }
    }

    /// Adds a new entry to the list output.
    pub fn entry(&mut self, entry: &dyn ScoreDebug) -> &mut Self {
        self.inner.entry_with(|f| entry.fmt(f, self.inner.spec));
        self
    }

    /// Adds a new entry to the list output.
    ///
    /// This method is equivalent to [`DebugList::entry`], but formats the entry using a provided closure rather than by calling [`ScoreDebug::fmt`].
    pub fn entry_with<F>(&mut self, entry_fmt: F) -> &mut Self
    where
        F: FnOnce(Writer) -> Result,
    {
        self.inner.entry_with(entry_fmt);
        self
    }

    /// Adds the contents of an iterator of entries to the list output.
    pub fn entries<D, I>(&mut self, entries: I) -> &mut Self
    where
        D: ScoreDebug,
        I: IntoIterator<Item = D>,
    {
        for entry in entries {
            self.entry(&entry);
        }
        self
    }

    /// Marks the list as non-exhaustive, indicating to the reader that there are some other elements that are not shown in the debug representation.
    pub fn finish_non_exhaustive(&mut self) -> Result {
        self.inner.result.and_then(|_| {
            let empty_spec = FormatSpec::new();
            if self.inner.has_fields {
                self.inner.writer.write_str(", ..]", &empty_spec)
            } else {
                self.inner.writer.write_str("..]", &empty_spec)
            }
        })
    }

    /// Finishes output and returns any error encountered.
    pub fn finish(&mut self) -> Result {
        self.inner.result = self
            .inner
            .result
            .and_then(|_| self.inner.writer.write_str("]", &FormatSpec::new()));
        self.inner.result
    }
}

/// Output a formatted map of items.
///
/// Useful as a part of [`ScoreDebug::fmt`] implementation.
#[must_use = "must eventually call `finish()` on ScoreDebug builders"]
pub struct DebugMap<'a> {
    writer: Writer<'a>,
    spec: &'a FormatSpec,
    result: Result,
    has_fields: bool,
    has_key: bool,
}

impl<'a> DebugMap<'a> {
    /// Create `DebugMap` instance.
    pub fn new(writer: Writer<'a>, spec: &'a FormatSpec) -> Self {
        let result = writer.write_str("{", &FormatSpec::new());
        DebugMap {
            writer,
            spec,
            result,
            has_fields: false,
            has_key: false,
        }
    }

    /// Adds a new entry to the map output.
    pub fn entry(&mut self, key: &dyn ScoreDebug, value: &dyn ScoreDebug) -> &mut Self {
        self.key(key).value(value)
    }

    /// Adds the key part of a new entry to the map output.
    ///
    /// This method, together with `value`, is an alternative to `entry` that can be used when the complete entry isn't known upfront.
    /// Prefer the `entry` method when it's possible to use.
    ///
    /// # Panics
    ///
    /// `key` must be called before `value` and each call to `key` must be followed by a corresponding call to `value`.
    /// Otherwise this method will panic.
    pub fn key(&mut self, key: &dyn ScoreDebug) -> &mut Self {
        self.key_with(|f| key.fmt(f, self.spec))
    }

    /// Adds the key part of a new entry to the map output.
    ///
    /// This method is equivalent to [`DebugMap::key`], but formats the key using a provided closure rather than by calling [`ScoreDebug::fmt`].
    pub fn key_with<F>(&mut self, key_fmt: F) -> &mut Self
    where
        F: FnOnce(Writer) -> Result,
    {
        self.result = self.result.and_then(|_| {
            assert!(
                !self.has_key,
                "attempted to begin a new map entry \
                                    without completing the previous one"
            );

            let empty_spec = FormatSpec::new();
            if self.has_fields {
                self.writer.write_str(", ", &empty_spec)?
            }
            key_fmt(self.writer)?;
            self.writer.write_str(": ", &empty_spec)?;

            self.has_key = true;
            Ok(())
        });

        self
    }

    /// Adds the value part of a new entry to the map output.
    ///
    /// This method, together with `key`, is an alternative to `entry` that can be used when the complete entry isn't known upfront.
    /// Prefer the `entry` method when it's possible to use.
    ///
    /// # Panics
    ///
    /// `key` must be called before `value` and each call to `key` must be followed by a corresponding call to `value`.
    /// Otherwise this method will panic.
    pub fn value(&mut self, value: &dyn ScoreDebug) -> &mut Self {
        self.value_with(|f| value.fmt(f, self.spec))
    }

    /// Adds the value part of a new entry to the map output.
    ///
    /// This method is equivalent to [`DebugMap::value`], but formats the value using a provided closure rather than by calling [`ScoreDebug::fmt`].
    pub fn value_with<F>(&mut self, value_fmt: F) -> &mut Self
    where
        F: FnOnce(Writer) -> Result,
    {
        self.result = self.result.and_then(|_| {
            assert!(self.has_key, "attempted to format a map value before its key");
            value_fmt(self.writer)?;
            self.has_key = false;
            Ok(())
        });

        self.has_fields = true;
        self
    }

    /// Adds the contents of an iterator of entries to the map output.
    pub fn entries<K, V, I>(&mut self, entries: I) -> &mut Self
    where
        K: ScoreDebug,
        V: ScoreDebug,
        I: IntoIterator<Item = (K, V)>,
    {
        for (k, v) in entries {
            self.entry(&k, &v);
        }
        self
    }

    /// Marks the map as non-exhaustive, indicating to the reader that there are some other entries that are not shown in the debug representation.
    pub fn finish_non_exhaustive(&mut self) -> Result {
        self.result = self.result.and_then(|_| {
            assert!(!self.has_key, "attempted to finish a map with a partial entry");

            let empty_spec = FormatSpec::new();
            if self.has_fields {
                self.writer.write_str(", ..}", &empty_spec)
            } else {
                self.writer.write_str("..}", &empty_spec)
            }
        });
        self.result
    }

    /// Finishes output and returns any error encountered.
    ///
    /// # Panics
    ///
    /// `key` must be called before `value` and each call to `key` must be followed by a corresponding call to `value`.
    /// Otherwise this method will panic.
    pub fn finish(&mut self) -> Result {
        self.result = self.result.and_then(|_| {
            assert!(!self.has_key, "attempted to finish a map with a partial entry");
            let empty_spec = FormatSpec::new();
            self.writer.write_str("}", &empty_spec)
        });
        self.result
    }
}

#[cfg(test)]
mod tests {
    use crate::builders::{DebugList, DebugMap, DebugSet, DebugStruct, DebugTuple};
    use crate::test_utils::StringWriter;
    use crate::{DisplayHint, FormatSpec};

    #[test]
    fn test_struct_finish_non_exhaustive() {
        #[derive(Debug)]
        struct Point {
            x: i32,
            y: i32,
        }

        let v = Point { x: 123, y: 321 };

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugStruct::new(&mut writer, &spec, "Point")
            .field("x", &v.x)
            .field("y", &v.y)
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "Point { x: 123, y: 321, .. }");
    }

    #[test]
    fn test_struct_finish() {
        #[derive(Debug)]
        struct Point {
            x: i32,
            y: i32,
        }

        let v = Point { x: 123, y: 321 };

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugStruct::new(&mut writer, &spec, "Point")
            .field("x", &v.x)
            .field("y", &v.y)
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_struct_empty_finish_non_exhaustive() {
        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugStruct::new(&mut writer, &spec, "X")
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "X { .. }");
    }

    #[test]
    fn test_struct_empty_finish() {
        #[derive(Debug)]
        struct X;

        let v = X;

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugStruct::new(&mut writer, &spec, "X")
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_tuple_finish_non_exhaustive() {
        let v = (123, 456, 789);

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugTuple::new(&mut writer, &spec, "")
            .field(&v.0)
            .field(&v.1)
            .field(&v.2)
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "(123, 456, 789, ..)");
    }

    #[test]
    fn test_tuple_empty_non_exhaustive() {
        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugTuple::new(&mut writer, &spec, "")
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "(..)");
    }

    #[test]
    fn test_tuple_finish() {
        let v = (123, 456, 789);

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugTuple::new(&mut writer, &spec, "")
            .field(&v.0)
            .field(&v.1)
            .field(&v.2)
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_tuple_empty_finish() {
        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugTuple::new(&mut writer, &spec, "")
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "");
    }

    #[test]
    fn test_tuple_single_finish() {
        let v = (531,);

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugTuple::new(&mut writer, &spec, "")
            .field(&v.0)
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_set_finish_non_exhaustive() {
        let v = std::collections::BTreeSet::from([123, 456, 789]);

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugSet::new(&mut writer, &spec)
            .entries(v.clone())
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "{123, 456, 789, ..}");
    }

    #[test]
    fn test_set_empty_finish_non_exhaustive() {
        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugSet::new(&mut writer, &spec)
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "{..}");
    }

    #[test]
    fn test_set_finish() {
        let v = std::collections::HashSet::from([123, 456, 789]);

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugSet::new(&mut writer, &spec)
            .entries(v.clone())
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_set_empty_finish() {
        let v = std::collections::HashSet::<i32>::new();

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugSet::new(&mut writer, &spec)
            .entries(v.clone())
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_list_finish_non_exhaustive() {
        let v = [123, 456, 789];

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugList::new(&mut writer, &spec)
            .entries(v)
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "[123, 456, 789, ..]");
    }

    #[test]
    fn test_list_empty_finish_non_exhaustive() {
        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugList::new(&mut writer, &spec)
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "[..]");
    }

    #[test]
    fn test_list_finish() {
        let v = [123, 456, 789];

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugList::new(&mut writer, &spec)
            .entries(v)
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_list_empty_finish() {
        let v: [i32; 0] = [];

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugList::new(&mut writer, &spec)
            .entries(v)
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_map_finish_non_exhaustive() {
        let v = std::collections::BTreeMap::from([("first", 123), ("second", 456), ("third", 789)]);

        let mut writer = StringWriter::new();
        let mut spec = FormatSpec::new();
        spec.display_hint(DisplayHint::Debug);
        let _ = DebugMap::new(&mut writer, &spec)
            .entries(v.clone())
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "{\"first\": 123, \"second\": 456, \"third\": 789, ..}");
    }

    #[test]
    fn test_map_finish() {
        let v = std::collections::BTreeMap::from([("first", 123), ("second", 456), ("third", 789)]);

        let mut writer = StringWriter::new();
        let mut spec = FormatSpec::new();
        spec.display_hint(DisplayHint::Debug);
        let _ = DebugMap::new(&mut writer, &spec)
            .entries(v.clone())
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }

    #[test]
    fn test_map_empty_finish_non_exhaustive() {
        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugMap::new(&mut writer, &spec)
            .finish_non_exhaustive()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), "{..}");
    }

    #[test]
    fn test_map_empty_finish() {
        let v = std::collections::BTreeMap::<&str, i32>::new();

        let mut writer = StringWriter::new();
        let spec = FormatSpec::new();
        let _ = DebugMap::new(&mut writer, &spec)
            .entries(v.clone())
            .finish()
            .map_err(|_| panic!("failed to finish"));

        assert_eq!(writer.get(), format!("{:?}", v));
    }
}
