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

//! Tests for `score_log_format_args`.
//!
//! Only positive paths can be checked with regular unit tests.
//! This is due to error paths resulting in compilation errors (as expected with proc macros).
//!
//! Results are compared with Rust built-in `format_args` macro.

mod utils;

use crate::utils::StringWriter;
use score_log_fmt::{write, Alignment, DebugAsHex, DisplayHint, Fragment, Sign};
use score_log_fmt_macro::score_log_format_args;

#[track_caller]
fn common_format_args_test(
    score_log_args: score_log_fmt::Arguments,
    std_args: core::fmt::Arguments,
    expected_num_fragments: usize,
    expected_output: &str,
) {
    // Write data to string.
    let mut w = StringWriter::new();
    let _ = write(&mut w, score_log_args).map_err(|_| panic!("write failed"));

    // Check `score_log` args.
    assert_eq!(score_log_args.0.len(), expected_num_fragments);
    assert_eq!(w.get(), expected_output);

    // Compare with Rust built-in format args.
    let expected = std::fmt::format(std_args);
    assert_eq!(w.get(), expected);
}

#[test]
fn test_single_literal() {
    let score_log_args = score_log_format_args!("test_string");
    let core_fmt_args = format_args!("test_string");
    common_format_args_test(score_log_args, core_fmt_args, 1, "test_string");
}

#[test]
fn test_escaped_braces() {
    let score_log_args = score_log_format_args!("{{}}}}{{");
    let core_fmt_args = format_args!("{{}}}}{{");
    common_format_args_test(score_log_args, core_fmt_args, 1, "{}}{");
}

#[test]
fn test_single_placeholder() {
    let score_log_args = score_log_format_args!("{}", 123);
    let core_fmt_args = format_args!("{}", 123);
    common_format_args_test(score_log_args, core_fmt_args, 1, "123");
}

#[test]
fn test_mixed_literals_and_placeholders() {
    let score_log_args = score_log_format_args!("test_{}_string", 321);
    let core_fmt_args = format_args!("test_{}_string", 321);
    common_format_args_test(score_log_args, core_fmt_args, 3, "test_321_string");
}

#[test]
fn test_arg_index() {
    let score_log_args = score_log_format_args!("test_{2}_{1}_{0}", 123, 234, 345);
    let core_fmt_args = format_args!("test_{2}_{1}_{0}", 123, 234, 345);
    common_format_args_test(score_log_args, core_fmt_args, 6, "test_345_234_123");
}

#[test]
fn test_arg_pos_and_index() {
    let score_log_args = score_log_format_args!("test_{2}_{}_{1}_{}_{0}", 123, 234, 345);
    let core_fmt_args = format_args!("test_{2}_{}_{1}_{}_{0}", 123, 234, 345);
    common_format_args_test(score_log_args, core_fmt_args, 10, "test_345_123_234_234_123");
}

#[test]
fn test_arg_name() {
    let x1 = 123;
    let x2 = 234;
    let x3 = 345;
    let score_log_args = score_log_format_args!("test_{x3}_{x2}_{x1}", x1, x2, x3);
    // NOTE: known misalignment.
    // It is not allowed to have redundant arguments in Rust (`("{x1}", x1)`).
    // This is currently not possible to do using `score_log_format_args`.
    let core_fmt_args = format_args!("test_{x3}_{x2}_{x1}");
    common_format_args_test(score_log_args, core_fmt_args, 6, "test_345_234_123");
}

#[test]
fn test_arg_name_alias() {
    let x1 = 123;
    let x2 = 234;
    let x3 = 345;
    let score_log_args = score_log_format_args!("test_{a3}_{a2}_{a1}", a1 = x1, a2 = x2, a3 = x3);
    let core_fmt_args = format_args!("test_{a3}_{a2}_{a1}", a1 = x1, a2 = x2, a3 = x3);
    common_format_args_test(score_log_args, core_fmt_args, 6, "test_345_234_123");
}

#[test]
fn test_arg_pos_and_name() {
    let x1 = 123;
    let x2 = 234;
    let x3 = 345;
    let score_log_args = score_log_format_args!("test_{x3}_{}_{x2}_{}_{x1}", x1, x2, x3);
    // NOTE: known misalignment.
    // It is not allowed to have redundant arguments in Rust (`("{x1}", x1)`).
    // This is currently not possible to do using `score_log_format_args`.
    let core_fmt_args = format_args!("test_{x3}_{}_{x2}_{}_{x1}", x1, x2);
    common_format_args_test(score_log_args, core_fmt_args, 10, "test_345_123_234_234_123");
}

#[test]
fn test_arg_mixed() {
    let x1 = 111;
    let x2 = 222;
    let score_log_args = score_log_format_args!("test_{x1}_{1}_{}", x1, x2);
    let core_fmt_args = format_args!("test_{x1}_{1}_{}", x1, x2);
    common_format_args_test(score_log_args, core_fmt_args, 6, "test_111_222_111");
}

#[test]
fn test_format_spec_empty() {
    let args = score_log_format_args!("{:}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::NoHint);
    assert_eq!(format_spec.get_fill(), ' ');
    assert!(format_spec.get_align().is_none());
    assert!(format_spec.get_sign().is_none());
    assert!(!format_spec.get_alternate());
    assert!(!format_spec.get_zero_pad());
    assert!(format_spec.get_debug_as_hex().is_none());
    assert_eq!(format_spec.get_width(), None);
    assert_eq!(format_spec.get_precision(), None);
}

#[test]
fn test_format_spec_all() {
    let args = score_log_format_args!("{:c<-#0333.555x}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::LowerHex);
    assert_eq!(format_spec.get_fill(), 'c');
    assert!(format_spec.get_align() == Some(Alignment::Left));
    assert!(format_spec.get_sign() == Some(Sign::Minus));
    assert!(format_spec.get_alternate());
    assert!(format_spec.get_zero_pad());
    assert!(format_spec.get_debug_as_hex().is_none());
    assert_eq!(format_spec.get_width(), Some(333));
    assert_eq!(format_spec.get_precision(), Some(555));
}

#[test]
fn test_format_spec_debug() {
    let args = score_log_format_args!("{:#X?}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::Debug);
    assert_eq!(format_spec.get_fill(), ' ');
    assert!(format_spec.get_align().is_none());
    assert!(format_spec.get_sign().is_none());
    assert!(format_spec.get_alternate());
    assert!(!format_spec.get_zero_pad());
    assert!(format_spec.get_debug_as_hex() == Some(DebugAsHex::Upper));
    assert_eq!(format_spec.get_width(), None);
    assert_eq!(format_spec.get_precision(), None);
}

#[test]
fn test_format_spec_display_hint_octal() {
    let args = score_log_format_args!("{:o}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::Octal);
}

#[test]
fn test_format_spec_display_hint_lower_hex() {
    let args = score_log_format_args!("{:x}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::LowerHex);
}

#[test]
fn test_format_spec_display_hint_upper_hex() {
    let args = score_log_format_args!("{:X}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::UpperHex);
}

#[test]
fn test_format_spec_display_hint_pointer() {
    let args = score_log_format_args!("{:p}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::Pointer);
}

#[test]
fn test_format_spec_display_hint_binary() {
    let args = score_log_format_args!("{:b}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::Binary);
}

#[test]
fn test_format_spec_display_hint_lower_exp() {
    let args = score_log_format_args!("{:e}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::LowerExp);
}

#[test]
fn test_format_spec_display_hint_upper_exp() {
    let args = score_log_format_args!("{:E}", 123);

    let placeholder = match args.0.first().unwrap() {
        Fragment::Literal(_) => panic!("invalid variant"),
        Fragment::Placeholder(placeholder) => placeholder,
    };

    let format_spec = placeholder.format_spec();
    assert!(format_spec.get_display_hint() == DisplayHint::UpperExp);
}
