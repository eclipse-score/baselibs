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

#![allow(dead_code, unused_imports, missing_docs)]

use score_log::{debug, error, info, trace, warn, Level, LevelFilter, Log, Metadata, Record};
use score_log_fmt::Arguments;
use std::sync::{Arc, Mutex};

struct State {
    last_log_level: Mutex<Option<Level>>,
    last_log_location: Mutex<Option<u32>>,
}

struct Logger(Arc<State>);

impl Log for Logger {
    fn enabled(&self, _: &Metadata) -> bool {
        true
    }

    fn context(&self) -> &str {
        "TEST"
    }

    fn log(&self, record: &Record) {
        *self.0.last_log_level.lock().unwrap() = Some(record.level());
        *self.0.last_log_location.lock().unwrap() = Some(record.line());
    }
    fn flush(&self) {}
}

fn test_filter(logger: &dyn Log, a: &State, filter: LevelFilter) {
    // tests to ensure logs with a level beneath 'max_level' are filtered out
    score_log::set_max_level(filter);
    error!(logger: logger, "");
    last(a, t(Level::Error, filter));
    warn!(logger: logger, "");
    last(a, t(Level::Warn, filter));
    info!(logger: logger, "");
    last(a, t(Level::Info, filter));
    debug!(logger: logger, "");
    last(a, t(Level::Debug, filter));
    trace!(logger: logger, "");
    last(a, t(Level::Trace, filter));

    fn t(lvl: Level, filter: LevelFilter) -> Option<Level> {
        if lvl <= filter {
            Some(lvl)
        } else {
            None
        }
    }
    fn last(state: &State, expected: Option<Level>) {
        let lvl = state.last_log_level.lock().unwrap().take();
        assert_eq!(lvl, expected);
    }
}

fn test_line_numbers(logger: &dyn Log, state: &State) {
    score_log::set_max_level(LevelFilter::Trace);

    info!(logger: logger, ""); // ensure check_line function follows log macro
    check_log_location(state);

    #[track_caller]
    fn check_log_location(state: &State) {
        let location = core::panic::Location::caller().line(); // get function calling location
        let line_number = state.last_log_location.lock().unwrap().take().unwrap(); // get location of most recent log
        assert_eq!(line_number, location - 1);
    }
}
