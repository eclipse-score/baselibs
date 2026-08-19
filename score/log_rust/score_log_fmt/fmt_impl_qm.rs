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

//! `ScoreDebug` implementations for types that are not ASIL-B certified.

use crate::fmt::{Result, ScoreDebug, Writer};
use crate::fmt_spec::FormatSpec;
use core::net::AddrParseError;
use std::path::{Path, PathBuf};

// TODO: replace with `core::char::MAX_LEN_UTF8` once stable.
const MAX_LEN_UTF8: usize = 4;

impl ScoreDebug for Path {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> Result {
        let enc_bytes = self.as_os_str().as_encoded_bytes();
        let utf8_chunks = enc_bytes.utf8_chunks();

        for chunk in utf8_chunks {
            let valid = chunk.valid();
            // If we successfully decoded the whole chunk as a valid string then
            // we can return a direct formatting of the string which will also
            // respect various formatting flags if possible.
            if chunk.invalid().is_empty() {
                return ScoreDebug::fmt(valid, f, spec);
            }

            f.write_str(valid, spec)?;
            f.write_str(
                core::char::REPLACEMENT_CHARACTER.encode_utf8(&mut [0; MAX_LEN_UTF8]),
                spec,
            )?;
        }

        Ok(())
    }
}

impl ScoreDebug for PathBuf {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> Result {
        ScoreDebug::fmt(self.as_path(), f, spec)
    }
}

impl ScoreDebug for AddrParseError {
    fn fmt(&self, f: Writer, spec: &FormatSpec) -> Result {
        f.write_str(&format!("{:?}", self), spec)
    }
}

#[cfg(test)]
mod tests {
    use crate::test_utils::common_test_debug;
    use std::path::{Path, PathBuf};

    #[test]
    fn test_path_ref_debug() {
        common_test_debug(Path::new("/tmp/test_path"));
    }

    #[test]
    fn test_pathbuf_debug() {
        common_test_debug(PathBuf::from("/tmp/test_path"));
    }

    #[test]
    fn test_addr_parse_error_debug() {
        let a1 = "invalid address";
        let a2: Result<std::net::IpAddr, std::net::AddrParseError> = a1.parse();
        common_test_debug(a2.unwrap_err());
    }
}
