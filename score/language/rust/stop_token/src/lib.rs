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

/// Opaque view of C++ `score::cpp::stop_token`.
pub use ffi::stop_token as StopToken;

/// Opaque view of C++ `score::cpp::stop_source`.
pub use ffi::stop_source as StopSource;

/// SAFETY: StopToken is meant to be used on multi-threaded use-cases as is described at:
/// score/language/futurecpp/include/score/private/stop_token/stop_token.hpp
/// Therefore it is safe to move and share these objects between threads.
unsafe impl Send for StopToken {}
unsafe impl Sync for StopToken {}

/// SAFETY: StopSource is a means to share a `stop_state` and that class uses atomics to guarantee
/// that the state is consistent between threads, as described at:
/// score/language/futurecpp/include/score/private/stop_token/stop_state.hpp
/// Therefore it is safe to move and share these objects between threads.
unsafe impl Send for StopSource {}
unsafe impl Sync for StopSource {}

impl StopToken {
    /// Creates an owned, default-constructed token without associated stop state.
    pub fn make() -> cxx::UniquePtr<Self> {
        ffi::make_stop_token()
    }

    /// Returns whether a stop request has been made on this token's stop state.
    pub fn stop_requested(&self) -> bool {
        ffi::stop_token_stop_requested(self)
    }

    /// Returns whether this token has associated stop state that can be stopped.
    pub fn stop_possible(&self) -> bool {
        ffi::stop_token_stop_possible(self)
    }
}

impl PartialEq for StopToken {
    fn eq(&self, other: &Self) -> bool {
        ffi::stop_token_equal(self, other)
    }
}

impl Eq for StopToken {}

impl StopSource {
    /// Creates an owned source with a new associated stop state.
    pub fn make() -> cxx::UniquePtr<Self> {
        ffi::make_stop_source()
    }

    /// Returns whether a stop request has been made on this source's stop state.
    pub fn stop_requested(&self) -> bool {
        ffi::stop_source_stop_requested(self)
    }

    /// Returns whether this source has associated stop state.
    pub fn stop_possible(&self) -> bool {
        ffi::stop_source_stop_possible(self)
    }

    /// Requests stop on the associated stop state.
    pub fn request_stop(&self) -> bool {
        ffi::stop_source_request_stop(self)
    }

    /// Returns an owned token associated with this source's stop state.
    pub fn get_token(&self) -> cxx::UniquePtr<StopToken> {
        ffi::stop_source_get_token(self)
    }
}

impl PartialEq for StopSource {
    fn eq(&self, other: &Self) -> bool {
        ffi::stop_source_equal(self, other)
    }
}

impl Eq for StopSource {}

#[cfg(test)]
mod tests {
    use super::{StopSource, StopToken};

    #[test]
    fn default_token_has_no_stop_state() {
        let token = StopToken::make();
        let token = token.as_ref().expect("default token must be allocated");

        assert!(!token.stop_requested());
        assert!(!token.stop_possible());
    }

    #[test]
    fn source_requests_stop_visible_to_its_token() {
        let source = StopSource::make();
        let token = source.as_ref().expect("stop source must be allocated").get_token();
        let token = token.as_ref().expect("source token must be allocated");

        assert!(source.as_ref().expect("stop source must be allocated").stop_possible());
        assert!(!token.stop_requested());
        assert!(source.as_ref().expect("stop source must be allocated").request_stop());
        assert!(token.stop_requested());
        assert!(!source.as_ref().expect("stop source must be allocated").request_stop());
    }
}
