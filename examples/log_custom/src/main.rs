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

//! Example app containing basic logger implementation.

mod logger;

use crate::logger::ExampleLogger;
use score_log::{debug, error, fatal, info, trace, warn, LevelFilter};

fn main() {
    // Initialize logger.
    score_log::set_max_level(LevelFilter::Info);
    if let Err(e) = score_log::set_global_logger(Box::new(ExampleLogger)) {
        panic!("unable to set logger: {e}");
    }

    // Example logs.
    trace!("trace log - hidden!");
    debug!("debug log - hidden!");
    info!("info log");
    warn!("warn log");
    error!("error log");
    fatal!("fatal log");
}
