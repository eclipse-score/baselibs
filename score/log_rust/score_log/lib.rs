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

//! A lightweight logging facade.

#![deny(unconditional_recursion)]

extern crate alloc;

use core::str::FromStr;
use core::sync::atomic::{AtomicUsize, Ordering};
use core::{cmp, mem};
pub use score_log_fmt as fmt;
use score_log_fmt::Arguments;
pub use score_log_fmt_macro::{score_log_format_args as format_args, ScoreDebug};
use std::sync::{LazyLock, OnceLock};

#[macro_use]
mod macros;

/// Global logger.
static LOGGER: OnceLock<Box<dyn Log>> = OnceLock::new();

static MAX_LOG_LEVEL_FILTER: AtomicUsize = AtomicUsize::new(0);

static LOG_LEVEL_NAMES: [&str; 7] = ["OFF", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"];

/// An enum representing the available verbosity levels of the logger.
#[repr(usize)]
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug, Hash)]
pub enum Level {
    /// Highest level, for extremely serious errors.
    Fatal = 1,
    /// Designates very serious errors.
    Error,
    /// Designates hazardous situations.
    Warn,
    /// Designates useful information.
    Info,
    /// Designates lower priority information.
    Debug,
    /// Designates very low priority, often extremely verbose, information.
    Trace,
}

impl PartialEq<LevelFilter> for Level {
    #[inline]
    fn eq(&self, other: &LevelFilter) -> bool {
        *self as usize == *other as usize
    }
}

impl PartialOrd<LevelFilter> for Level {
    #[inline]
    fn partial_cmp(&self, other: &LevelFilter) -> Option<cmp::Ordering> {
        Some((*self as usize).cmp(&(*other as usize)))
    }
}

impl FromStr for Level {
    type Err = ParseLevelError;
    fn from_str(level: &str) -> Result<Level, Self::Err> {
        LOG_LEVEL_NAMES
            .iter()
            .position(|&name| name.eq_ignore_ascii_case(level))
            .into_iter()
            .filter(|&idx| idx != 0)
            .map(|idx| Level::from_usize(idx).unwrap())
            .next()
            .ok_or(ParseLevelError(()))
    }
}

impl core::fmt::Display for Level {
    fn fmt(&self, fmt: &mut core::fmt::Formatter) -> core::fmt::Result {
        fmt.pad(self.as_str())
    }
}

impl Level {
    fn from_usize(u: usize) -> Option<Level> {
        match u {
            1 => Some(Level::Fatal),
            2 => Some(Level::Error),
            3 => Some(Level::Warn),
            4 => Some(Level::Info),
            5 => Some(Level::Debug),
            6 => Some(Level::Trace),
            _ => None,
        }
    }

    /// Returns the most verbose logging level.
    #[inline]
    pub fn max() -> Level {
        Level::Trace
    }

    /// Converts the [`Level`] to the equivalent [`LevelFilter`].
    #[inline]
    pub fn to_level_filter(&self) -> LevelFilter {
        LevelFilter::from_usize(*self as usize).unwrap()
    }

    /// Returns the string representation of the [`Level`].
    ///
    /// This returns the same string as the [`fmt::Display`] implementation.
    pub fn as_str(&self) -> &'static str {
        LOG_LEVEL_NAMES[*self as usize]
    }
}

/// An enum representing the available verbosity level filters of the logger.
///
/// A [`LevelFilter`] may be compared directly to a [`Level`].
#[repr(usize)]
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug, Hash)]
pub enum LevelFilter {
    /// A level lower than all log levels.
    Off = 0,
    /// Corresponds to the [`Level::Fatal`] log level.
    Fatal,
    /// Corresponds to the [`Level::Error`] log level.
    Error,
    /// Corresponds to the [`Level::Warn`] log level.
    Warn,
    /// Corresponds to the [`Level::Info`] log level.
    Info,
    /// Corresponds to the [`Level::Debug`] log level.
    Debug,
    /// Corresponds to the [`Level::Trace`] log level.
    Trace,
}

impl PartialEq<Level> for LevelFilter {
    #[inline]
    fn eq(&self, other: &Level) -> bool {
        other.eq(self)
    }
}

impl PartialOrd<Level> for LevelFilter {
    #[inline]
    fn partial_cmp(&self, other: &Level) -> Option<cmp::Ordering> {
        Some((*self as usize).cmp(&(*other as usize)))
    }
}

impl FromStr for LevelFilter {
    type Err = ParseLevelError;
    fn from_str(level: &str) -> Result<LevelFilter, Self::Err> {
        LOG_LEVEL_NAMES
            .iter()
            .position(|&name| name.eq_ignore_ascii_case(level))
            .map(|p| LevelFilter::from_usize(p).unwrap())
            .ok_or(ParseLevelError(()))
    }
}

impl core::fmt::Display for LevelFilter {
    fn fmt(&self, fmt: &mut core::fmt::Formatter) -> core::fmt::Result {
        fmt.pad(self.as_str())
    }
}

impl LevelFilter {
    fn from_usize(u: usize) -> Option<LevelFilter> {
        match u {
            0 => Some(LevelFilter::Off),
            1 => Some(LevelFilter::Fatal),
            2 => Some(LevelFilter::Error),
            3 => Some(LevelFilter::Warn),
            4 => Some(LevelFilter::Info),
            5 => Some(LevelFilter::Debug),
            6 => Some(LevelFilter::Trace),
            _ => None,
        }
    }

    /// Returns the most verbose logging level filter.
    #[inline]
    pub fn max() -> LevelFilter {
        LevelFilter::Trace
    }

    /// Converts [`LevelFilter`] to the equivalent [`Level`].
    ///
    /// Returns [`None`] if [`LevelFilter`] is [`LevelFilter::Off`].
    #[inline]
    pub fn to_level(&self) -> Option<Level> {
        Level::from_usize(*self as usize)
    }

    /// Returns the string representation of the [`LevelFilter`].
    ///
    /// This returns the same string as the [`fmt::Display`] implementation.
    pub fn as_str(&self) -> &'static str {
        LOG_LEVEL_NAMES[*self as usize]
    }
}

/// The "payload" of a log message.
#[derive(Clone)]
pub struct Record<'a> {
    metadata: Metadata<'a>,
    args: Arguments<'a>,
    module_path: &'a str,
    file: &'a str,
    line: u32,
}

impl<'a> Record<'a> {
    /// Create `Record`.
    #[inline]
    pub fn new(args: Arguments<'a>, metadata: Metadata<'a>, module_path: &'a str, file: &'a str, line: u32) -> Self {
        Self {
            args,
            metadata,
            module_path,
            file,
            line,
        }
    }

    /// The message body.
    #[inline]
    pub fn args(&self) -> &Arguments<'a> {
        &self.args
    }

    /// Metadata about the log directive.
    #[inline]
    pub fn metadata(&self) -> &Metadata<'a> {
        &self.metadata
    }

    /// The verbosity level of the message.
    #[inline]
    pub fn level(&self) -> Level {
        self.metadata.level()
    }

    /// The name of the context of the directive.
    #[inline]
    pub fn context(&self) -> &'a str {
        self.metadata.context()
    }

    /// The module path of the message.
    #[inline]
    pub fn module_path(&self) -> &'a str {
        self.module_path
    }

    /// The source file containing the message.
    #[inline]
    pub fn file(&self) -> &'a str {
        self.file
    }

    /// The line containing the message.
    #[inline]
    pub fn line(&self) -> u32 {
        self.line
    }
}

/// Metadata about a log message.
#[derive(Clone, Eq, PartialEq, Ord, PartialOrd, Hash)]
pub struct Metadata<'a> {
    level: Level,
    context: &'a str,
}

impl<'a> Metadata<'a> {
    /// Create `Metadata`.
    #[inline]
    pub fn new(level: Level, context: &'a str) -> Self {
        Self { level, context }
    }

    /// The verbosity level of the message.
    #[inline]
    pub fn level(&self) -> Level {
        self.level
    }

    /// The name of the context of the directive.
    #[inline]
    pub fn context(&self) -> &'a str {
        self.context
    }
}

/// A trait encapsulating the operations required of a logger.
pub trait Log: Sync + Send {
    /// Determines if a log message with the specified metadata would be logged.
    fn enabled(&self, metadata: &Metadata) -> bool;

    /// Default logger context name.
    fn context(&self) -> &str;

    /// Logs the [`Record`].
    ///
    /// # For implementors
    ///
    /// Note that [`Log::enabled`] is *not* necessarily called before this method.
    /// Implementations should perform all necessary filtering internally.
    fn log(&self, record: &Record);

    /// Flushes any buffered records.
    fn flush(&self);
}

/// A dummy initial value for LOGGER.
struct NopLogger;

impl Log for NopLogger {
    fn enabled(&self, _: &Metadata) -> bool {
        false
    }

    fn context(&self) -> &str {
        ""
    }

    fn log(&self, _: &Record) {}

    fn flush(&self) {}
}

impl<T: ?Sized + Log> Log for &'_ T {
    fn enabled(&self, metadata: &Metadata) -> bool {
        (**self).enabled(metadata)
    }

    fn context(&self) -> &str {
        (**self).context()
    }

    fn log(&self, record: &Record) {
        (**self).log(record);
    }

    fn flush(&self) {
        (**self).flush();
    }
}

impl<T: ?Sized + Log> Log for alloc::boxed::Box<T> {
    fn enabled(&self, metadata: &Metadata) -> bool {
        self.as_ref().enabled(metadata)
    }

    fn context(&self) -> &str {
        self.as_ref().context()
    }

    fn log(&self, record: &Record) {
        self.as_ref().log(record);
    }

    fn flush(&self) {
        self.as_ref().flush();
    }
}

/// Sets the global maximum log level.
///
/// Generally, this should only be called by the active logging implementation.
///
/// Note that [`LevelFilter::Trace`] is the maximum level, because it provides the maximum amount of detail in the emitted logs.
#[inline]
pub fn set_max_level(level: LevelFilter) {
    MAX_LOG_LEVEL_FILTER.store(level as usize, Ordering::Relaxed);
}

/// Returns the current maximum log level.
///
/// Logging macros check this value and discard any message logged at a higher level.
/// The maximum log level is set by the [`set_max_level`] function.
#[inline(always)]
pub fn max_level() -> LevelFilter {
    // Since `LevelFilter` is `repr(usize)`,
    // this transmute is sound if and only if `MAX_LOG_LEVEL_FILTER`
    // is set to a usize that is a valid discriminant for `LevelFilter`.
    // Since `MAX_LOG_LEVEL_FILTER` is private, the only time it's set
    // is by `set_max_level` above, i.e. by casting a `LevelFilter` to `usize`.
    // So any usize stored in `MAX_LOG_LEVEL_FILTER` is a valid discriminant.
    unsafe { mem::transmute(MAX_LOG_LEVEL_FILTER.load(Ordering::Relaxed)) }
}

/// Sets the global logger to a `Box<dyn Log>`.
///
/// This function may only be called once in the lifetime of a program.
/// Any log events that occur before the call to [`set_global_logger`] completes will be ignored.
///
/// This function does not typically need to be called manually.
/// Logger implementations should provide an initialization method that installs the logger internally.
///
/// # Errors
///
/// An error is returned if a logger has already been set.
pub fn set_global_logger(logger: Box<dyn Log>) -> Result<(), SetLoggerError> {
    LOGGER.set(logger).map_err(|_| SetLoggerError(()))
}

/// The type returned by [`set_global_logger`] if [`set_global_logger`] has already been called.
pub struct SetLoggerError(());

impl core::fmt::Display for SetLoggerError {
    fn fmt(&self, fmt: &mut core::fmt::Formatter) -> core::fmt::Result {
        fmt.write_str("attempted to set a logger after the logging system was already initialized")
    }
}

/// The type returned by [`core::str::FromStr::from_str`] implementations when the string doesn't match any of the log levels.
#[derive(PartialEq, Eq)]
pub struct ParseLevelError(());

impl core::fmt::Display for ParseLevelError {
    fn fmt(&self, fmt: &mut core::fmt::Formatter) -> core::fmt::Result {
        fmt.write_str("attempted to convert a string that doesn't match an existing log level")
    }
}

/// Returns a reference to the logger.
///
/// If a logger has not been set, a no-op implementation is returned.
pub fn global_logger() -> &'static dyn Log {
    static NOP_LOGGER: LazyLock<Box<dyn Log>> = LazyLock::new(|| {
        eprintln!("warn: logger not initialized");
        Box::new(NopLogger)
    });
    LOGGER.get().unwrap_or_else(|| &NOP_LOGGER)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashSet;

    fn level_iter() -> impl Iterator<Item = Level> {
        (1..7).map(|i| Level::from_usize(i).unwrap())
    }

    fn level_filter_iter() -> impl Iterator<Item = LevelFilter> {
        (0..7).map(|i| LevelFilter::from_usize(i).unwrap())
    }

    #[test]
    fn test_level_partial_eq_with_level_filter() {
        // Pairs that should result in true.
        let matching_pairs = HashSet::from([
            (Level::Fatal, LevelFilter::Fatal),
            (Level::Error, LevelFilter::Error),
            (Level::Warn, LevelFilter::Warn),
            (Level::Info, LevelFilter::Info),
            (Level::Debug, LevelFilter::Debug),
            (Level::Trace, LevelFilter::Trace),
        ]);

        // Iterate through levels.
        for level in level_iter() {
            // Iterate through level filters.
            for level_filter in level_filter_iter() {
                let is_matching = matching_pairs.contains(&(level, level_filter));
                assert_eq!(level.eq(&level_filter), is_matching);
            }
        }
    }

    #[test]
    fn test_level_partial_cmp_with_level_filter() {
        // Iterate through levels.
        for level in level_iter() {
            let matching_filter = level.to_level_filter();
            // Iterate through level filters.
            for level_filter in level_filter_iter() {
                if matching_filter < level_filter {
                    assert!(level < level_filter);
                } else if matching_filter > level_filter {
                    assert!(level > level_filter);
                } else {
                    assert!(level == level_filter);
                }
            }
        }
    }

    #[test]
    fn test_level_from_str() {
        let tests = [
            ("OFF", Err(ParseLevelError(()))),
            ("fatal", Ok(Level::Fatal)),
            ("error", Ok(Level::Error)),
            ("warn", Ok(Level::Warn)),
            ("info", Ok(Level::Info)),
            ("debug", Ok(Level::Debug)),
            ("trace", Ok(Level::Trace)),
            ("FATAL", Ok(Level::Fatal)),
            ("ERROR", Ok(Level::Error)),
            ("WARN", Ok(Level::Warn)),
            ("INFO", Ok(Level::Info)),
            ("DEBUG", Ok(Level::Debug)),
            ("TRACE", Ok(Level::Trace)),
            ("asdf", Err(ParseLevelError(()))),
        ];
        for &(s, ref expected) in &tests {
            assert!(expected == &s.parse());
        }
    }

    #[test]
    fn test_level_display() {
        assert_eq!("FATAL", Level::Fatal.to_string());
        assert_eq!("ERROR", Level::Error.to_string());
        assert_eq!("WARN", Level::Warn.to_string());
        assert_eq!("INFO", Level::Info.to_string());
        assert_eq!("DEBUG", Level::Debug.to_string());
        assert_eq!("TRACE", Level::Trace.to_string());
    }

    #[test]
    fn test_level_from_usize() {
        let tests = [
            None,
            Some(Level::Fatal),
            Some(Level::Error),
            Some(Level::Warn),
            Some(Level::Info),
            Some(Level::Debug),
            Some(Level::Trace),
            None,
        ];
        for (i, expected) in tests.iter().enumerate() {
            assert_eq!(expected, &Level::from_usize(i));
        }
    }

    #[test]
    fn test_level_max() {
        assert_eq!(Level::max(), Level::Trace);
    }

    #[test]
    fn test_level_to_level_filter() {
        let tests = [
            (Level::Fatal, LevelFilter::Fatal),
            (Level::Error, LevelFilter::Error),
            (Level::Warn, LevelFilter::Warn),
            (Level::Info, LevelFilter::Info),
            (Level::Debug, LevelFilter::Debug),
            (Level::Trace, LevelFilter::Trace),
        ];
        for (level, level_filter) in tests {
            assert_eq!(level_filter, level.to_level_filter());
        }
    }

    #[test]
    fn test_level_as_str() {
        let tests = &[
            (Level::Fatal, "FATAL"),
            (Level::Error, "ERROR"),
            (Level::Warn, "WARN"),
            (Level::Info, "INFO"),
            (Level::Debug, "DEBUG"),
            (Level::Trace, "TRACE"),
        ];
        for (input, expected) in tests {
            assert_eq!(*expected, input.as_str());
        }
    }

    #[test]
    fn test_level_filter_partial_eq_with_level() {
        // Pairs that should result in true.
        let matching_pairs = HashSet::from([
            (LevelFilter::Fatal, Level::Fatal),
            (LevelFilter::Error, Level::Error),
            (LevelFilter::Warn, Level::Warn),
            (LevelFilter::Info, Level::Info),
            (LevelFilter::Debug, Level::Debug),
            (LevelFilter::Trace, Level::Trace),
        ]);

        // Iterate through level filters.
        for level_filter in level_filter_iter() {
            // Iterate through levels.
            for level in level_iter() {
                let is_matching = matching_pairs.contains(&(level_filter, level));
                assert_eq!(level_filter.eq(&level), is_matching)
            }
        }
    }

    #[test]
    fn test_level_filter_partial_cmp_with_level() {
        // Iterate through level filters.
        // Skip `LevelFilter::Off`.
        for level_filter in level_filter_iter().skip(1) {
            let matching_level = level_filter.to_level().unwrap();
            // Iterate through levels.
            for level in level_iter() {
                if matching_level < level {
                    assert!(level_filter < level);
                } else if matching_level > level {
                    assert!(level_filter > level);
                } else {
                    assert!(level_filter == level);
                }
            }
        }
    }

    #[test]
    fn test_level_filter_from_str() {
        let tests = [
            ("off", Ok(LevelFilter::Off)),
            ("fatal", Ok(LevelFilter::Fatal)),
            ("error", Ok(LevelFilter::Error)),
            ("warn", Ok(LevelFilter::Warn)),
            ("info", Ok(LevelFilter::Info)),
            ("debug", Ok(LevelFilter::Debug)),
            ("trace", Ok(LevelFilter::Trace)),
            ("OFF", Ok(LevelFilter::Off)),
            ("FATAL", Ok(LevelFilter::Fatal)),
            ("ERROR", Ok(LevelFilter::Error)),
            ("WARN", Ok(LevelFilter::Warn)),
            ("INFO", Ok(LevelFilter::Info)),
            ("DEBUG", Ok(LevelFilter::Debug)),
            ("TRACE", Ok(LevelFilter::Trace)),
            ("asdf", Err(ParseLevelError(()))),
        ];
        for &(s, ref expected) in &tests {
            assert!(expected == &s.parse());
        }
    }

    #[test]
    fn test_level_filter_display() {
        assert_eq!("OFF", LevelFilter::Off.to_string());
        assert_eq!("FATAL", LevelFilter::Fatal.to_string());
        assert_eq!("ERROR", LevelFilter::Error.to_string());
        assert_eq!("WARN", LevelFilter::Warn.to_string());
        assert_eq!("INFO", LevelFilter::Info.to_string());
        assert_eq!("DEBUG", LevelFilter::Debug.to_string());
        assert_eq!("TRACE", LevelFilter::Trace.to_string());
    }

    #[test]
    fn test_level_filter_from_usize() {
        let tests = [
            Some(LevelFilter::Off),
            Some(LevelFilter::Fatal),
            Some(LevelFilter::Error),
            Some(LevelFilter::Warn),
            Some(LevelFilter::Info),
            Some(LevelFilter::Debug),
            Some(LevelFilter::Trace),
            None,
        ];
        for (i, expected) in tests.iter().enumerate() {
            assert_eq!(expected, &LevelFilter::from_usize(i));
        }
    }

    #[test]
    fn test_level_filter_max() {
        assert_eq!(LevelFilter::max(), LevelFilter::Trace);
    }

    #[test]
    fn test_level_filter_to_level() {
        let tests = [
            (LevelFilter::Off, None),
            (LevelFilter::Fatal, Some(Level::Fatal)),
            (LevelFilter::Error, Some(Level::Error)),
            (LevelFilter::Warn, Some(Level::Warn)),
            (LevelFilter::Info, Some(Level::Info)),
            (LevelFilter::Debug, Some(Level::Debug)),
            (LevelFilter::Trace, Some(Level::Trace)),
        ];
        for (level_filter, level) in tests {
            assert_eq!(level, level_filter.to_level());
        }
    }

    #[test]
    fn test_level_filter_as_str() {
        let tests = &[
            (LevelFilter::Off, "OFF"),
            (LevelFilter::Fatal, "FATAL"),
            (LevelFilter::Error, "ERROR"),
            (LevelFilter::Warn, "WARN"),
            (LevelFilter::Info, "INFO"),
            (LevelFilter::Debug, "DEBUG"),
            (LevelFilter::Trace, "TRACE"),
        ];
        for (input, expected) in tests {
            assert_eq!(*expected, input.as_str());
        }
    }

    #[test]
    fn test_record_new_and_params() {
        // Local import to avoid name clash.
        use super::format_args;
        // Fix for self-reference in `score_log_format_args`.
        use crate as score_log;

        let level = Level::Info;
        let context = "context";
        let metadata = Metadata::new(level, context);

        let args = format_args!("test_string_{}", 123);
        let module_path = "module_path";
        let file = "file";
        let line_num = 123u32;

        let record = Record::new(args, metadata.clone(), module_path, file, line_num);

        assert_eq!(record.args().0.len(), 2);
        assert_eq!(record.level(), metadata.level());
        assert_eq!(record.metadata().level(), record.level());
        assert_eq!(record.context(), metadata.context());
        assert_eq!(record.metadata().context(), record.context());
        assert_eq!(record.module_path(), module_path);
        assert_eq!(record.file(), file);
        assert_eq!(record.line(), line_num);
    }

    #[test]
    fn test_metadata_new_and_params() {
        let level = Level::Info;
        let context = "context";
        let metadata = Metadata::new(level, context);
        assert_eq!(metadata.level(), level);
        assert_eq!(metadata.context(), context);
    }

    struct StubLogger<'a> {
        context: &'a str,
    }

    impl<'a> Log for StubLogger<'a> {
        fn enabled(&self, _: &Metadata) -> bool {
            unimplemented!()
        }

        fn context(&self) -> &str {
            self.context
        }

        fn log(&self, _: &Record) {
            unimplemented!()
        }

        fn flush(&self) {
            unimplemented!()
        }
    }

    #[test]
    fn test_set_global_logger_and_global_logger() {
        // `set_global_logger` and `global_logger` operate on global state.
        // All operations are done in a single test to ensure state is as expected.

        // Set logger.
        {
            let logger = Box::new(StubLogger { context: "ctx1" });
            let result = set_global_logger(logger);
            assert!(result.is_ok());
        }

        // Get logger.
        {
            let logger = global_logger();
            assert_eq!(logger.context(), StubLogger { context: "ctx1" }.context());
        }

        // Set for the second time.
        {
            // Make sure second set result in an error.
            let new_logger = Box::new(StubLogger { context: "ctx2" });
            let result = set_global_logger(new_logger);
            assert!(result.is_err());

            // Make sure state didn't change.
            let old_logger = global_logger();
            assert_eq!(old_logger.context(), StubLogger { context: "ctx1" }.context());
        }
    }

    // Test that the `impl Log for Foo` blocks work
    // This test mostly operates on a type level, so failures will be compile errors
    #[test]
    fn test_foreign_impl() {
        fn assert_is_log<T: Log + ?Sized>() {}

        assert_is_log::<&dyn Log>();

        assert_is_log::<Box<dyn Log>>();

        // Assert these statements for all T: Log + ?Sized
        #[allow(unused)]
        fn forall<T: Log + ?Sized>() {
            assert_is_log::<Box<T>>();

            assert_is_log::<&T>();
        }
    }

    #[test]
    fn test_max_level_and_set_max_level() {
        // NOTE: `max_level` and `set_max_level` operate on a global state.
        // Changing it affects all tests.

        // Check default value.
        assert_eq!(max_level(), LevelFilter::Off);

        // Set new value and check.
        set_max_level(LevelFilter::Trace);
        assert_eq!(max_level(), LevelFilter::Trace);

        // Reset to original state.
        set_max_level(LevelFilter::Off);
    }

    #[test]
    fn test_set_global_logger_error_message() {
        let e = SetLoggerError(());
        assert_eq!(
            &e.to_string(),
            "attempted to set a logger after the logging system \
             was already initialized"
        );
    }

    #[test]
    fn test_parse_level_error_message() {
        let e = ParseLevelError(());
        assert_eq!(
            &e.to_string(),
            "attempted to convert a string that doesn't match an existing log level"
        );
    }
}
