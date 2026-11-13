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

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("score/language/rust/stop_token/cpp/stop_token_consumer.h");

        #[namespace = "score::cpp"]
        #[cxx_name = "stop_token"]
        type StopToken = stop_token::RawStopToken;

        #[namespace = "score::language::rust::stop_token::test"]
        #[cxx_name = "BorrowedStopTokenStopRequested"]
        fn borrowed_stop_token_stop_requested(token: &StopToken) -> bool;
    }
}

#[test]
#[cfg(not(miri))]
fn borrowed_token_is_usable_by_another_cxx_bridge() {
    let source = stop_token::StopSource::new();
    let token = source.get_token();

    assert!(!ffi::borrowed_stop_token_stop_requested(token.as_raw()));
    assert!(source.request_stop());
    assert!(ffi::borrowed_stop_token_stop_requested(token.as_raw()));
}
