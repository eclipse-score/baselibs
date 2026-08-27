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

use core::fmt::Write;
use score_log_fmt::{Error, FormatSpec, Result, ScoreWrite};

/// Writer implementation.
/// Writes everything to a string, so it can be compared with `format` macro.
pub(crate) struct StringWriter {
    buf: String,
}

impl StringWriter {
    pub fn new() -> Self {
        Self { buf: String::new() }
    }

    pub fn get(&self) -> &str {
        self.buf.as_str()
    }
}

impl ScoreWrite for StringWriter {
    fn write_bool(&mut self, v: &bool, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_f32(&mut self, v: &f32, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_f64(&mut self, v: &f64, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_i8(&mut self, v: &i8, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_i16(&mut self, v: &i16, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_i32(&mut self, v: &i32, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_i64(&mut self, v: &i64, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_u8(&mut self, v: &u8, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_u16(&mut self, v: &u16, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_u32(&mut self, v: &u32, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_u64(&mut self, v: &u64, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }

    fn write_str(&mut self, v: &str, _spec: &FormatSpec) -> Result {
        write!(self.buf, "{}", v).map_err(|_| Error)
    }
}
