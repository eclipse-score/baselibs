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

#![allow(missing_docs)]

use score_log::{log, log_enabled, Level, Log, Metadata, Record};

macro_rules! all_log_macros {
    ($($arg:tt)*) => ({
        ::score_log::trace!($($arg)*);
        ::score_log::debug!($($arg)*);
        ::score_log::info!($($arg)*);
        ::score_log::warn!($($arg)*);
        ::score_log::error!($($arg)*);
    });
}

// Not `Copy`
struct Logger;

impl Log for Logger {
    fn enabled(&self, _: &Metadata) -> bool {
        false
    }
    fn context(&self) -> &str {
        "TEST"
    }
    fn log(&self, _: &Record) {}
    fn flush(&self) {}
}

fn level_iter() -> impl Iterator<Item = Level> {
    [
        Level::Fatal,
        Level::Error,
        Level::Warn,
        Level::Info,
        Level::Debug,
        Level::Trace,
    ]
    .into_iter()
}

#[test]
fn no_args() {
    let logger = Logger;

    for lvl in level_iter() {
        log!(lvl, "hello");
        log!(lvl, "hello",);

        log!(context: "my_context", lvl, "hello");
        log!(context: "my_context", lvl, "hello",);

        log!(logger: logger, lvl, "hello");
        log!(logger: logger, lvl, "hello",);

        log!(logger: logger, context: "my_context", lvl, "hello");
        log!(logger: logger, context: "my_context", lvl, "hello",);
    }

    all_log_macros!("hello");
    all_log_macros!("hello",);

    all_log_macros!(context: "my_context", "hello");
    all_log_macros!(context: "my_context", "hello",);

    all_log_macros!(logger: logger, "hello");
    all_log_macros!(logger: logger, "hello",);

    all_log_macros!(logger: logger, context: "my_context", "hello");
    all_log_macros!(logger: logger, context: "my_context", "hello",);
}

#[test]
fn anonymous_args() {
    for lvl in level_iter() {
        log!(lvl, "hello {}", "world");
        log!(lvl, "hello {}", "world",);

        log!(context: "my_context", lvl, "hello {}", "world");
        log!(context: "my_context", lvl, "hello {}", "world",);

        log!(lvl, "hello {}", "world");
        log!(lvl, "hello {}", "world",);
    }

    all_log_macros!("hello {}", "world");
    all_log_macros!("hello {}", "world",);

    all_log_macros!(context: "my_context", "hello {}", "world");
    all_log_macros!(context: "my_context", "hello {}", "world",);

    let logger = Logger;

    all_log_macros!(logger: logger, "hello {}", "world");
    all_log_macros!(logger: logger, "hello {}", "world",);

    all_log_macros!(logger: logger, context: "my_context", "hello {}", "world");
    all_log_macros!(logger: logger, context: "my_context", "hello {}", "world",);
}

#[test]
fn named_args() {
    for lvl in level_iter() {
        log!(lvl, "hello {world}", world = "world");
        log!(lvl, "hello {world}", world = "world",);

        log!(context: "my_context", lvl, "hello {world}", world = "world");
        log!(context: "my_context", lvl, "hello {world}", world = "world",);

        log!(lvl, "hello {world}", world = "world");
        log!(lvl, "hello {world}", world = "world",);
    }

    all_log_macros!("hello {world}", world = "world");
    all_log_macros!("hello {world}", world = "world",);

    all_log_macros!(context: "my_context", "hello {world}", world = "world");
    all_log_macros!(context: "my_context", "hello {world}", world = "world",);

    let logger = Logger;

    all_log_macros!(logger: logger, "hello {world}", world = "world");
    all_log_macros!(logger: logger, "hello {world}", world = "world",);

    all_log_macros!(logger: logger, context: "my_context", "hello {world}", world = "world");
    all_log_macros!(logger: logger, context: "my_context", "hello {world}", world = "world",);
}

#[test]
fn enabled() {
    let logger = Logger;

    for lvl in level_iter() {
        let _enabled = log_enabled!(lvl);
        let _enabled = log_enabled!(context: "my_context", lvl);
        let _enabled = log_enabled!(logger: logger, context: "my_context", lvl);
        let _enabled = log_enabled!(logger: logger, lvl);
    }
}

#[test]
fn expr() {
    let logger = Logger;

    for lvl in level_iter() {
        log!(lvl, "hello");

        log!(logger: logger, lvl, "hello");
    }
}

#[test]
fn logger_short_lived() {
    all_log_macros!(logger: Logger, "hello");
    all_log_macros!(logger: &Logger, "hello");
}

#[test]
fn logger_expr() {
    all_log_macros!(logger: Logger, "hello");
}
