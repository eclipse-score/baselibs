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

//! Example app utilizing built-in backend implementation.

use score_log::{debug, error, fatal, info, trace, warn, LevelFilter};
use stdout_logger::StdoutLoggerBuilder;

fn main() {
    // Initialize logger.
    StdoutLoggerBuilder::new()
        .context("EXAMPLE")
        .show_module(true)
        .show_file(true)
        .show_line(true)
        .log_level(LevelFilter::Info)
        .set_as_default_logger();

    // Example logs.
    trace!("trace log - hidden!");
    debug!("debug log - hidden!");
    info!("info log");
    warn!("warn log");
    error!("error log");
    fatal!("fatal log");

    // Logs with changed context.
    trace!(context: "CHANGED", "trace log - hidden!");
    debug!(context: "CHANGED", "debug log - hidden!");
    info!(context: "CHANGED", "info log");
    warn!(context: "CHANGED", "warn log");
    error!(context: "CHANGED", "error log");
    fatal!(context: "CHANGED", "fatal log");
}
