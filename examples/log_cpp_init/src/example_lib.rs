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

//! Module contains functions printing example logs.
//! Based on `//score/mw/log/rust/score_log_bridge:example`.

use score_log::{debug, error, fatal, info, trace, warn, Log};
use stdout_logger::StdoutLoggerBuilder;

/// Show example logs.
#[no_mangle]
extern "C" fn show_logs() {
    // Regular log usage.
    trace!("This is a trace log - hidden");
    debug!("This is a debug log - hidden");
    info!("This is an info log");
    warn!("This is a warn log");
    error!("This is an error log");
    fatal!("This is a fatal log");

    // Log with modified context.
    trace!(context: "EX1", "This is a trace log - hidden");
    debug!(context: "EX1", "This is a debug log - hidden");
    info!(context: "EX1", "This is an info log");
    warn!(context: "EX1", "This is a warn log");
    error!(context: "EX1", "This is an error log");
    fatal!(context: "EX1", "This is a fatal log");

    // Log with numeric values.
    let x1 = 123.4;
    let x2 = 111;
    let x3 = true;
    let x4 = -0x3Fi8;
    error!(
        "This is an error log with numeric values: {} {} {} {:x}",
        x1, x2, x3, x4,
    );

    // Use logger instance with modified context.
    let logger = StdoutLoggerBuilder::new()
        .context("ALFA")
        .show_module(false)
        .show_file(true)
        .show_line(false)
        .build();

    // Log with provided logger.
    trace!(
        logger: logger,
        "This is a trace log - hidden"
    );
    debug!(logger: logger, "This is a debug log - hidden");
    info!(logger: logger, "This is an info log");
    warn!(logger: logger, "This is a warn log");
    error!(logger: logger, "This is an error log");
    fatal!(logger: logger, "This is an fatal log");

    // Log with provided logger and modified context.
    trace!(logger: logger, context: "EX2", "This is a trace log - hidden");
    debug!(logger: logger, context: "EX2", "This is a debug log - hidden");
    info!(logger: logger, context: "EX2", "This is an info log");
    warn!(logger: logger, context: "EX2", "This is a warn log");
    error!(logger: logger, context: "EX2", "This is an error log");
    fatal!(logger: logger, context: "EX2", "This is an fatal log");
}
