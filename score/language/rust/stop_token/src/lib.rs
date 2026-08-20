// *******************************************************************************
// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0
// *******************************************************************************

//! Lifetime-safe Rust bindings for C++ `score::cpp::stop_token` and `score::cpp::stop_source`.

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("score/language/rust/stop_token/cpp/stop_token_adapter.h");

        /// Opaque C++ `score::cpp::stop_token`.
        #[namespace = "score::cpp"]
        type stop_token;

        /// Opaque C++ `score::cpp::stop_source`.
        #[namespace = "score::cpp"]
        type stop_source;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopTokenStopRequested"]
        fn stop_token_stop_requested(token: &stop_token) -> bool;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopTokenStopPossible"]
        fn stop_token_stop_possible(token: &stop_token) -> bool;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopTokenEqual"]
        fn stop_token_equal(lhs: &stop_token, rhs: &stop_token) -> bool;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopSourceStopRequested"]
        fn stop_source_stop_requested(source: &stop_source) -> bool;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopSourceStopPossible"]
        fn stop_source_stop_possible(source: &stop_source) -> bool;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopSourceRequestStop"]
        fn stop_source_request_stop(source: &stop_source) -> bool;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopSourceGetToken"]
        fn stop_source_get_token(source: &stop_source) -> UniquePtr<stop_token>;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "StopSourceEqual"]
        fn stop_source_equal(lhs: &stop_source, rhs: &stop_source) -> bool;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "MakeStopToken"]
        fn make_stop_token() -> UniquePtr<stop_token>;

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "MakeStopSource"]
        fn make_stop_source() -> UniquePtr<stop_source>;
    }

    impl UniquePtr<stop_token> {}
    impl UniquePtr<stop_source> {}
}

impl PartialEq for ffi::stop_token {
    fn eq(&self, other: &Self) -> bool {
        ffi::stop_token_equal(self, other)
    }
}

impl Eq for ffi::stop_token {}

/// Owned Rust wrapper around C++ `score::cpp::stop_token`.
#[derive(Eq, PartialEq)]
pub struct StopToken(cxx::UniquePtr<ffi::stop_token>);

/// SAFETY: StopToken is meant to be used on multi-threaded use-cases as is described at:
/// score/language/futurecpp/include/score/private/stop_token/stop_token.hpp
/// Therefore it is safe to move and share these objects between threads.
unsafe impl Send for StopToken {}
unsafe impl Sync for StopToken {}

impl StopToken {
    /// Creates an owned, default-constructed token without associated stop state.
    pub fn new() -> Self {
        Self::from_inner(ffi::make_stop_token())
    }

    fn from_inner(inner: cxx::UniquePtr<ffi::stop_token>) -> Self {
        if inner.is_null() {
            std::process::abort();
        }

        Self(inner)
    }

    fn as_ffi(&self) -> &ffi::stop_token {
        self.0.as_ref().unwrap_or_else(|| std::process::abort())
    }

    /// Returns whether a stop request has been made on this token's stop state.
    pub fn stop_requested(&self) -> bool {
        ffi::stop_token_stop_requested(self.as_ffi())
    }

    /// Returns whether this token has associated stop state that can be stopped.
    pub fn stop_possible(&self) -> bool {
        ffi::stop_token_stop_possible(self.as_ffi())
    }
}

impl Default for StopToken {
    fn default() -> Self {
        Self::new()
    }
}

impl PartialEq for ffi::stop_source {
    fn eq(&self, other: &Self) -> bool {
        ffi::stop_source_equal(self, other)
    }
}

impl Eq for ffi::stop_source {}

/// Owned Rust wrapper around C++ `score::cpp::stop_source`.
#[derive(Eq, PartialEq)]
pub struct StopSource(cxx::UniquePtr<ffi::stop_source>);

/// SAFETY: StopSource is a means to share a `stop_state` and that class uses atomics to guarantee
/// that the state is consistent between threads, as described at:
/// score/language/futurecpp/include/score/private/stop_token/stop_state.hpp
/// Therefore it is safe to move and share these objects between threads.
unsafe impl Send for StopSource {}
unsafe impl Sync for StopSource {}

impl StopSource {
    /// Creates an owned source with a new associated stop state.
    pub fn new() -> Self {
        let source_ptr = ffi::make_stop_source();

        if source_ptr.is_null() {
            std::process::abort();
        }

        Self(source_ptr)
    }

    fn as_ffi(&self) -> &ffi::stop_source {
        self.0.as_ref().unwrap_or_else(|| std::process::abort())
    }

    /// Returns whether a stop request has been made on this source's stop state.
    pub fn stop_requested(&self) -> bool {
        ffi::stop_source_stop_requested(self.as_ffi())
    }

    /// Returns whether this source has associated stop state.
    pub fn stop_possible(&self) -> bool {
        ffi::stop_source_stop_possible(self.as_ffi())
    }

    /// Requests stop on the associated stop state.
    pub fn request_stop(&self) -> bool {
        ffi::stop_source_request_stop(self.as_ffi())
    }

    /// Returns an owned token associated with this source's stop state.
    pub fn get_token(&self) -> StopToken {
        StopToken::from_inner(ffi::stop_source_get_token(self.as_ffi()))
    }
}

impl Default for StopSource {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::{StopSource, StopToken};

    #[test]
    fn default_token_has_no_stop_state() {
        let token = StopToken::new();

        assert!(!token.stop_requested());
        assert!(!token.stop_possible());
    }

    #[test]
    fn source_requests_stop_visible_to_its_token() {
        let source = StopSource::new();
        let token = source.get_token();

        assert!(source.stop_possible());
        assert!(!token.stop_requested());
        assert!(source.request_stop());
        assert!(token.stop_requested());
        assert!(!source.request_stop());
    }
}
