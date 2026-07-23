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

use proc_macro2::TokenStream;
use quote::{format_ident, quote};
use syn::parse::Parse;
use syn::spanned::Spanned;
use syn::{parse_macro_input, GenericArgument, Generics, Ident, Path, PathArguments, Token, Visibility};

struct CppVariant {
    generics: Vec<GenericArgument>,
}

impl TryFrom<&'_ Path> for CppVariant {
    type Error = syn::Error;
    fn try_from(path: &Path) -> syn::Result<Self> {
        let generics = match &path.segments.last().unwrap().arguments {
            PathArguments::AngleBracketed(arguments) => arguments.args.iter().cloned().collect::<Vec<_>>(),
            PathArguments::Parenthesized(arguments) => {
                return Err(syn::Error::new(
                    arguments.span(),
                    "std::variant requires ordinary generic arguments",
                ))
            },
            PathArguments::None => {
                return Err(syn::Error::new(
                    path.span(),
                    "std::variant requires template arguments to be meaningful",
                ))
            },
        };
        Ok(Self { generics })
    }
}

enum CppRhs {
    Variant(CppVariant),
}

impl Parse for CppRhs {
    fn parse(input: syn::parse::ParseStream) -> syn::Result<Self> {
        let cpptype = input.parse::<Path>()?;
        let cpp_ident = cpptype
            .segments
            .iter()
            .map(|pathseg| pathseg.ident.to_string())
            .collect::<Vec<_>>();
        if cpp_ident == ["std", "variant"] {
            CppVariant::try_from(&cpptype).map(Self::Variant)
        } else {
            Err(syn::Error::new(cpptype.span(), "Unsupported C++ type"))
        }
    }
}

struct CppType {
    visibility: Visibility,
    _token_type: Token![type],
    name: Ident,
    generics: Generics,
    _equal_token: Token![=],
    cpptype: CppRhs,
    _semi_type: Token![;],
}

impl Parse for CppType {
    fn parse(input: syn::parse::ParseStream) -> syn::Result<Self> {
        Ok(Self {
            visibility: input.parse()?,
            _token_type: input.parse()?,
            name: input.parse()?,
            generics: input.parse()?,
            _equal_token: input.parse()?,
            cpptype: input.parse()?,
            _semi_type: input.parse()?,
        })
    }
}

/// Attribute macro to import C++ types into Rust with FFI-compatible layouts.
///
/// This macro currently supports importing C++ std::variant types by generating a corresponding
/// Rust struct that is compatible with the memory layout of a std::variant from the used C++
/// standard library implementation. The generated Rust type will have methods to construct each
/// variant and to access the stored value.
///
/// Notice that the generated structure does not implement Drop, so it is the caller's
/// responsibility to ensure that the correct destructor is called for the active variant when the
/// Rust value goes out of scope. This can be achieved by wrapping the generated struct and
/// implementing a custom Drop implementation for the wrapping type that calls the appropriate C++
/// destructor based on the active variant index.
///
/// # Generated methods for std::variant
///
/// For a C++ type defined as `std::variant<T1, T2, ...>` with 1, 2, ... denoted as \[id], the
/// following methods will be generated:
/// * `from_[id](val: T[id]) -> Self`: Constructs a new instance of the Rust struct with the
///   specified variant value.
/// * `get_[id](&self) -> &T[id]`: Returns a reference to the stored value of the specified variant.
///   This will panic if the active variant does not match the requested variant.
/// * `get_[id]_mut(&mut self) -> &mut ManuallyDrop<T[id]>`: Returns a mutable reference to the
///   stored value of the specified variant wrapped in ManuallyDrop. The wrapping is done to denote
///   that it is the caller's responsibility to ensure proper cleanup of the value when it goes out
///   of scope.
/// * `get_index(&self) -> usize`: Returns the index of the currently active variant (0 for T1, 1
///   for T2, etc.).
///
/// # Example usage
///
/// ## Create and access a C++ variant from Rust
///
/// ```rust
/// #[libcpp_derive::import]
/// pub type MyVariant<T1, T2> = std::variant<T1, T2>;
///
/// let variant = MyVariant::<i32, f64>::from_0(42);
/// assert_eq!(variant.get_index(), 0);
/// assert_eq!(*variant.get_0(), 42);
///
/// ```
///
/// ## Using a wrapper to properly clean up heap-allocated variant values
///
/// ```rust
/// use std::mem::ManuallyDrop;
///
/// #[libcpp_derive::import]
/// pub type HeapOrDirectVariant = std::variant<*mut i32, i32>;
///
/// struct HeapOrDirect {
///     variant: HeapOrDirectVariant,
/// }
///
/// impl Drop for HeapOrDirect {
///     fn drop(&mut self) {
///         match self.variant.get_index() {
///             0 => unsafe { Box::from_raw(ManuallyDrop::take(self.variant.get_0_mut())); }, // Clean up heap allocation
///             1 => (), // No cleanup needed for direct value
///             _ => unreachable!("Invalid variant index"),
///         }
///     }
/// }
///
/// let variant = HeapOrDirectVariant::from_0(Box::into_raw(Box::new(42)));
/// let managed = HeapOrDirect { variant };
/// std::mem::drop(managed); // This will correctly clean up the heap allocation
/// ```
#[proc_macro_attribute]
pub fn import(_input: proc_macro::TokenStream, item: proc_macro::TokenStream) -> proc_macro::TokenStream {
    let CppType {
        visibility,
        name,
        generics,
        cpptype,
        ..
    } = parse_macro_input!(item as CppType);

    let union_name = format_ident!("{name}VariantStorage");

    match cpptype {
        CppRhs::Variant(CppVariant { generics: cpp_generics }) => {
            let union_attributes = cpp_generics
                .iter()
                .enumerate()
                .map(|(id, generic)| {
                    let value_name = format_ident!("v{}", id);
                    quote! { #value_name: std::mem::ManuallyDrop<#generic>, }
                })
                .collect::<TokenStream>();

            let variant_methods = cpp_generics
                .iter()
                .enumerate()
                .map(|(id, generic)| {
                    let discriminant = id as u8;
                    let value_name = format_ident!("v{}", id);
                    let fn_ident = format_ident!("from_{id}");
                    let fn_get = format_ident!("get_{id}");
                    let fn_get_mut = format_ident!("get_{id}_mut");
                    quote! {
                        pub fn #fn_ident(val: #generic) -> Self {
                            Self {
                                storage: #union_name { #value_name: std::mem::ManuallyDrop::new(val) },
                                discriminant: #discriminant,
                            }
                        }

                        pub fn #fn_get(&self) -> &#generic {
                            assert_eq!(self.discriminant, #discriminant);
                            unsafe { &self.storage.#value_name }
                        }

                        pub fn #fn_get_mut(&mut self) ->&mut std::mem::ManuallyDrop<#generic> {
                            assert_eq!(self.discriminant, #discriminant);
                            unsafe { &mut self.storage.#value_name }
                        }
                    }
                })
                .collect::<TokenStream>();

            quote! {
                #[repr(C)]
                union #union_name #generics {
                    #union_attributes
                }

                #[repr(C)]
                #visibility struct #name #generics {
                    storage: #union_name #generics,
                    discriminant: u8,
                }

                impl #generics #name #generics {
                    #variant_methods

                    fn get_index(&self) -> usize {
                        self.discriminant as usize
                    }
                }
            }
            .into()
        },
    }
}
