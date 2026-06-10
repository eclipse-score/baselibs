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

use proc_macro::TokenStream;
use quote::quote;
use syn::{parse_macro_input, ItemFn, ItemMod};

/// Attribute macro to create a test function with stdout logger initialized.
#[proc_macro_attribute]
pub fn test_with_log(_attr: TokenStream, item: TokenStream) -> TokenStream {
    let input = parse_macro_input!(item as ItemFn);

    let vis = &input.vis;
    let sig = &input.sig;
    let block = &input.block;
    let attrs = &input.attrs;

    TokenStream::from(quote! {
        #(#attrs)*
        #vis #sig {
            let _ = stdout_logger::StdoutLoggerBuilder::new()
                .context("TEST")
                .log_level(score_log::LevelFilter::Trace)
                .try_set_as_default_logger();
            #block
            score_log::global_logger().flush();
        }
    })
}

/// Attribute macro that modifies a test module to initialize stdout logger for each test.
#[proc_macro_attribute]
pub fn test_mod_with_log(_attr: TokenStream, item: TokenStream) -> TokenStream {
    let mut input = parse_macro_input!(item as ItemMod);

    if let Some((_, ref mut items)) = input.content {
        // Wrap each #[test] function to call test_hook automatically
        for item in items.iter_mut() {
            if let syn::Item::Fn(ref mut f) = item {
                let is_test = f.attrs.iter().any(|a| a.path().is_ident("test"));
                if is_test {
                    let block = &f.block;
                    f.block = syn::parse_quote!({

                        let _ = stdout_logger::StdoutLoggerBuilder::new()
                            .context("TEST")
                            .log_level(score_log::LevelFilter::Trace)
                            .try_set_as_default_logger();
                        #block

                        score_log::global_logger().flush();
                    });
                }
            }
        }
    }

    TokenStream::from(quote! {
        #input
    })
}
