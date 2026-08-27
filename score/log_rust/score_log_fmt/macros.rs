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

/// Writes data using provided writer.
///
/// This macro accepts a writer, a format string, and a list of arguments.
/// Arguments will be formatted according to the specified format string and the result will be passed to the writer.
#[macro_export]
macro_rules! score_write {
    ($dst:expr, $($arg:tt)*) => {
        $crate::write($dst, score_log::format_args!($($arg)*))
    };
}
