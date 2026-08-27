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

use core::ffi::c_char;
use core::slice::from_raw_parts;
use stdout_logger::StdoutLoggerBuilder;

/// Represents severity of a log message.
#[derive(Clone, Copy)]
#[repr(u8)]
#[allow(dead_code)]
enum LogLevel {
    Off = 0x00,
    Fatal = 0x01,
    Error = 0x02,
    Warn = 0x03,
    Info = 0x04,
    Debug = 0x05,
    Verbose = 0x06,
}

impl From<LogLevel> for score_log::LevelFilter {
    fn from(level: LogLevel) -> score_log::LevelFilter {
        match level {
            LogLevel::Off => score_log::LevelFilter::Off,
            LogLevel::Fatal => score_log::LevelFilter::Fatal,
            LogLevel::Error => score_log::LevelFilter::Error,
            LogLevel::Warn => score_log::LevelFilter::Warn,
            LogLevel::Info => score_log::LevelFilter::Info,
            LogLevel::Debug => score_log::LevelFilter::Debug,
            LogLevel::Verbose => score_log::LevelFilter::Trace,
        }
    }
}

#[no_mangle]
extern "C" fn set_default_logger(
    context_ptr: *const c_char,
    context_size: usize,
    show_module: *const bool,
    show_file: *const bool,
    show_line: *const bool,
    show_timestamp: *const bool,
    log_level: *const LogLevel,
) {
    let mut builder = StdoutLoggerBuilder::new();

    // Set parameters if non-null (option-like).
    if !context_ptr.is_null() {
        let context = unsafe {
            let slice = from_raw_parts(context_ptr.cast(), context_size);
            str::from_utf8_unchecked(slice)
        };
        builder = builder.context(context);
    }

    if !show_module.is_null() {
        builder = builder.show_module(unsafe { *show_module });
    }

    if !show_file.is_null() {
        builder = builder.show_file(unsafe { *show_file });
    }

    if !show_line.is_null() {
        builder = builder.show_line(unsafe { *show_line });
    }

    if !show_timestamp.is_null() {
        builder = builder.show_timestamp(unsafe { *show_timestamp });
    }

    if !log_level.is_null() {
        builder = builder.log_level(unsafe { (*log_level).into() });
    }

    builder.set_as_default_logger();
}
