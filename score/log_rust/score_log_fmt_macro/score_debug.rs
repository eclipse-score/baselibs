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

use quote::{format_ident, quote};
use syn::{
    parse_macro_input, Data, DataEnum, DataStruct, DeriveInput, Error, Fields, Ident, ImplGenerics, Index, TypeGenerics,
};

/// Generate `ScoreDebug` implementation for struct.
fn generate_for_struct(
    ident: Ident,
    data_struct: DataStruct,
    impl_generics: ImplGenerics,
    ty_generics: TypeGenerics,
) -> Result<proc_macro2::TokenStream, Error> {
    // Generate `.fmt` implementations for struct types.
    let struct_name = ident.to_string();
    let fmt_impl = match data_struct.fields {
        // Regular struct - contains named fields.
        Fields::Named(fields) => {
            // Generate `.field` method calls for named fields.
            let mut field_methods = Vec::new();
            for field in fields.named.into_iter() {
                let ident = match field.ident {
                    Some(ident) => ident,
                    None => return Err(Error::new_spanned(field, "identifier not found")),
                };
                let name = ident.to_string();
                field_methods.push(quote! { .field(#name, &self.#ident) });
            }

            // Generate `.fmt` implementation using named struct helper.
            quote! {
                score_log::fmt::DebugStruct::new(f, spec, #struct_name)
                    #(#field_methods)*
                    .finish()
            }
        },

        // Tuple struct - contains unnamed fields.
        Fields::Unnamed(fields) => {
            // Generate `.field` method calls for unnamed fields.
            let mut field_methods = Vec::new();
            for index in 0..fields.unnamed.len() {
                let syn_index = Index::from(index);
                field_methods.push(quote! { .field(&self.#syn_index) });
            }

            // Generate `.fmt` implementation using named tuple helper.
            quote! {
                score_log::fmt::DebugTuple::new(f, spec, #struct_name)
                    #(#field_methods)*
                    .finish()
            }
        },

        // Unit struct - no fields.
        Fields::Unit => {
            quote! {
                score_log::fmt::DebugStruct::new(f, spec, #struct_name).finish()
            }
        },
    };

    // Generate `ScoreDebug` implementation for provided struct.
    Ok(quote! {
        #[automatically_derived]
        impl #impl_generics score_log::fmt::ScoreDebug for #ident #ty_generics {
            fn fmt(&self, f: score_log::fmt::Writer, spec: &score_log::fmt::FormatSpec) -> score_log::fmt::Result {
                #fmt_impl
            }
        }
    })
}

/// Generate `ScoreDebug` implementation for enum.
fn generate_for_enum(
    ident: Ident,
    data_enum: DataEnum,
    impl_generics: ImplGenerics,
    ty_generics: TypeGenerics,
) -> Result<proc_macro2::TokenStream, Error> {
    // Handle technically legal empty enum definition.
    if data_enum.variants.is_empty() {
        return Ok(quote! {
            #[automatically_derived]
            impl #impl_generics score_log::fmt::ScoreDebug for #ident #ty_generics {
                fn fmt(&self, f: score_log::fmt::Writer, spec: &score_log::fmt::FormatSpec) -> score_log::fmt::Result {
                    Ok(())
                }
            }
        });
    }

    // Generate implementations for each variant.
    let mut variants = Vec::new();
    for variant in data_enum.variants {
        let variant_ident = variant.ident;
        let variant_name = variant_ident.to_string();

        let variant_impl = match variant.fields {
            Fields::Named(fields) => {
                // Generate arg names and `.field` method calls for named fields.
                let mut arg_names = Vec::new();
                let mut field_methods = Vec::new();
                for field in fields.named {
                    let ident = match field.ident {
                        Some(ident) => ident,
                        None => return Err(Error::new_spanned(field, "identifier not found")),
                    };
                    let name = ident.to_string();
                    arg_names.push(quote! { #ident });
                    field_methods.push(quote! { .field(#name, #ident) });
                }

                // Generate variant match implementation.
                quote! {
                    Self::#variant_ident { #(#arg_names),* } => {
                        score_log::fmt::DebugStruct::new(f, spec, #variant_name)
                            #(#field_methods)*
                            .finish()
                    },
                }
            },
            Fields::Unnamed(fields) => {
                // Generate arg names and `.field` method calls for unnamed fields.
                let mut arg_names = Vec::new();
                let mut field_methods = Vec::new();
                for index in 0..fields.unnamed.len() {
                    let arg_name = format_ident!("arg{}", index);
                    arg_names.push(quote! { #arg_name });
                    field_methods.push(quote! { .field(#arg_name) });
                }

                // Generate variant match implementation.
                quote! {
                    Self::#variant_ident (#(#arg_names),*) => {
                        score_log::fmt::DebugTuple::new(f, spec, #variant_name)
                            #(#field_methods)*
                            .finish()
                    },
                }
            },
            Fields::Unit => {
                quote! {
                    Self::#variant_ident => f.write_str(#variant_name, spec),
                }
            },
        };

        variants.push(variant_impl)
    }

    // Generate `ScoreDebug` implementation for provided enum.
    Ok(quote! {
        #[automatically_derived]
        impl #impl_generics score_log::fmt::ScoreDebug for #ident #ty_generics {
            fn fmt(&self, f: score_log::fmt::Writer, spec: &score_log::fmt::FormatSpec) -> score_log::fmt::Result {
                match self {
                    #(#variants)*
                }
            }
        }
    })
}

/// Generate `ScoreDebug` implementation.
fn generate_score_debug(derive_input: DeriveInput) -> Result<proc_macro2::TokenStream, Error> {
    let DeriveInput {
        attrs: _,
        vis: _,
        ident,
        generics,
        data,
    } = derive_input;

    // Split generics.
    let (impl_generics, ty_generics, _) = generics.split_for_impl();

    match data {
        Data::Struct(data_struct) => generate_for_struct(ident, data_struct, impl_generics, ty_generics),
        Data::Enum(data_enum) => generate_for_enum(ident, data_enum, impl_generics, ty_generics),
        Data::Union(_) => Err(Error::new(
            proc_macro2::Span::call_site(),
            "`#[derive(ScoreDebug)] does not support unions`",
        )),
    }
}

pub(crate) fn expand(input: proc_macro::TokenStream) -> proc_macro::TokenStream {
    let derive_input = parse_macro_input!(input as DeriveInput);
    match generate_score_debug(derive_input) {
        Ok(token_stream) => token_stream,
        Err(e) => e.into_compile_error(),
    }
    .into()
}
