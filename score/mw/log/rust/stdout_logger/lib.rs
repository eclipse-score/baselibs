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

//! String-based Rust backend for `score_log`.
//! Data is written to a fixed-size buffer.

mod timestamp;

use crate::timestamp::timestamp;
use core::cell::RefCell;
use core::fmt::Write;
use score_log::fmt::{score_write, Error, FormatSpec, Result, ScoreWrite};
use score_log::{LevelFilter, Log, Metadata, Record};
use std::time::{SystemTime, UNIX_EPOCH};

/// Fixed size buffer for strings.
struct FixedBuf<const N: usize> {
    buf: [u8; N],
    len: usize,
}

impl<const N: usize> FixedBuf<N> {
    pub const fn new() -> Self {
        Self { buf: [0; N], len: 0 }
    }

    /// Get buffer as a string.
    pub fn as_str(&self) -> &str {
        // SAFETY: All bytes in `self.buf[..self.len]` are guaranteed to form valid UTF-8.
        unsafe { core::str::from_utf8_unchecked(&self.buf[..self.len]) }
    }

    /// Reset buffer state.
    pub fn clear(&mut self) {
        self.len = 0;
    }

    /// Get number of remaining bytes in the buffer.
    pub fn remaining(&self) -> usize {
        N - self.len
    }
}

impl<const N: usize> Default for FixedBuf<N> {
    fn default() -> Self {
        Self::new()
    }
}

impl<const N: usize> Write for FixedBuf<N> {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        // Get number of remaining bytes in the buffer.
        // Return if buffer is full.
        let remaining = self.remaining();
        if remaining == 0 {
            return Ok(());
        }

        // Get provided string as bytes.
        let bytes = s.as_bytes();

        // Get number of bytes requested or remaining in the buffer.
        let mut end = bytes.len().min(remaining);

        // Move back until char boundary.
        // Return if buffer is full.
        while end > 0 && !s.is_char_boundary(end) {
            end -= 1;
        }
        if end == 0 {
            return Ok(());
        }

        // Write to underlying buffer.
        self.buf[self.len..self.len + end].copy_from_slice(&bytes[..end]);
        self.len += end;

        Ok(())
    }
}

/// Writer implementation based on fixed size buffer.
#[derive(Default)]
struct FixedBufWriter<const N: usize> {
    buf: FixedBuf<N>,
}

impl<const N: usize> FixedBufWriter<N> {
    /// Create `FixedBufWriter` instance.
    pub fn new() -> Self {
        Self { buf: FixedBuf::new() }
    }

    /// Get data from buffer.
    pub fn get(&self) -> &str {
        self.buf.as_str()
    }

    /// Reset buffer state.
    pub fn clear(&mut self) {
        self.buf.clear();
    }
}

impl<const N: usize> ScoreWrite for FixedBufWriter<N> {
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

/// Builder for the `StdoutLogger`.
pub struct StdoutLoggerBuilder(StdoutLogger);

impl StdoutLoggerBuilder {
    /// Create builder with default parameters.
    pub fn new() -> Self {
        Self::default()
    }

    /// Set context for the `StdoutLogger`.
    pub fn context(mut self, context: &str) -> Self {
        self.0.context = context.to_string();
        self
    }

    /// Show module name in logs.
    pub fn show_module(mut self, show_module: bool) -> Self {
        self.0.show_module = show_module;
        self
    }

    /// Show file name in logs.
    pub fn show_file(mut self, show_file: bool) -> Self {
        self.0.show_file = show_file;
        self
    }

    /// Show line number in logs.
    pub fn show_line(mut self, show_line: bool) -> Self {
        self.0.show_line = show_line;
        self
    }

    /// Show timestamp.
    ///
    /// UTC timestamp in the following format:
    /// `[year]/[month]/[day] [hour]:[minute]:[second].[subsecond digits:7]`
    ///
    /// Example:
    /// `2026/01/27 11:33:41.1420089`
    pub fn show_timestamp(mut self, show_timestamp: bool) -> Self {
        self.0.show_timestamp = show_timestamp;
        self
    }

    /// Filter logs by level.
    pub fn log_level(mut self, log_level: LevelFilter) -> Self {
        self.0.log_level = log_level;
        self
    }

    /// Build the `StdoutLogger` with provided context and configuration.
    pub fn build(self) -> StdoutLogger {
        self.0
    }

    /// Build the `StdoutLogger` and set it as the default logger.
    pub fn set_as_default_logger(self) {
        if let Err(e) = self.try_set_as_default_logger() {
            panic!("unable to set logger: {e}");
        }
    }

    /// Build the `StdoutLogger` and try to set it as the default logger.
    pub fn try_set_as_default_logger(self) -> core::result::Result<(), score_log::SetLoggerError> {
        let logger = self.build();
        let level = logger.log_level();
        score_log::set_global_logger(Box::new(logger))?;
        score_log::set_max_level(level);
        Ok(())
    }
}

impl Default for StdoutLoggerBuilder {
    fn default() -> Self {
        Self(StdoutLogger {
            context: "DFLT".to_string(),
            show_module: false,
            show_file: false,
            show_line: false,
            show_timestamp: true,
            log_level: LevelFilter::Info,
        })
    }
}

thread_local! {
    static WRITER: RefCell<FixedBufWriter<2048>> = RefCell::new(FixedBufWriter::new());
}

/// String-based logger implementation.
pub struct StdoutLogger {
    context: String,
    show_module: bool,
    show_file: bool,
    show_line: bool,
    show_timestamp: bool,
    log_level: LevelFilter,
}

impl StdoutLogger {
    /// Current log level.
    pub fn log_level(&self) -> LevelFilter {
        self.log_level
    }
}

impl Log for StdoutLogger {
    fn enabled(&self, metadata: &Metadata) -> bool {
        metadata.level() <= self.log_level()
    }

    fn context(&self) -> &str {
        &self.context
    }

    fn log(&self, record: &Record) {
        // Finish early if not enabled for requested level.
        let metadata = record.metadata();
        if !self.enabled(metadata) {
            return;
        }

        // Operate in a scope of borrowed writer.
        WRITER.with_borrow_mut(|writer| {
            // Write timestamp.
            if self.show_timestamp {
                if let Ok(now) = SystemTime::now().duration_since(UNIX_EPOCH) {
                    let timestamp_u8 = timestamp(now);
                    let timestamp_str = unsafe { str::from_utf8_unchecked(timestamp_u8.as_slice()) };
                    let _ = score_write!(writer, "[{}]", timestamp_str);
                }
            }

            // Write module, file and line.
            if self.show_module || self.show_file || self.show_line {
                let _ = score_write!(writer, "[");
                if self.show_module {
                    let _ = score_write!(writer, "{}:", record.module_path());
                }
                if self.show_file {
                    let _ = score_write!(writer, "{}:", record.file());
                }
                if self.show_line {
                    let _ = score_write!(writer, "{}", record.line());
                }
                let _ = score_write!(writer, "]");
            }

            // Write context, log level, log data.
            let context = record.context();
            let level = metadata.level().as_str();
            let pid = std::process::id();
            let _ = score_write!(writer, "[{}][{}][{}] {}", pid, context, level, record.args());

            // Print to stdout.
            println!("{}", writer.get());

            // Reset buffer.
            writer.clear();
        });
    }

    fn flush(&self) {
        use std::io::Write;
        let mut stdout = std::io::stdout();
        stdout.flush().unwrap();
    }
}
