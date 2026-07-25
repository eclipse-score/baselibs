..
   # *******************************************************************************
   # Copyright (c) 2025 Contributors to the Eclipse Foundation
   #
   # See the NOTICE file(s) distributed with this work for additional
   # information regarding copyright ownership.
   #
   # This program and the accompanying materials are made available under the
   # terms of the Apache License Version 2.0 which is available at
   # https://www.apache.org/licenses/LICENSE-2.0
   #
   # SPDX-License-Identifier: Apache-2.0
   # *******************************************************************************


.. _abi_compatible_data_types_requirements:

Requirements
############

.. document:: ABI Compatible Data Types Requirements
   :id: doc__abi_compatible_data_types_requirements
   :status: valid
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, abi_compatible_data_types

ABI Compatibility
=================

Restrictions on Native Types
----------------------------

.. comp_req:: Restrict boolean size
   :id: comp_req__abi_compatible_data_types__bool_sz
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, the implementation shall restrict boolean types to one byte (``bool`` in Rust) and to the bit patterns ``0x00`` and ``0x01``.

.. comp_req:: Fixed-width integers
   :id: comp_req__abi_compatible_data_types__int_fix
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, all integer types shall use fixed-width definitions (``uN``/``iN`` in Rust; ``std::uintN_t``/``std::intN_t`` in C++), for N ∈ {8, 16, 32, 64}.

.. comp_req:: Limit floating-point sizes
   :id: comp_req__abi_compatible_data_types__flt_sz
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, floating-point types shall be limited to 32-bit (``f32`` in Rust / ``float`` in C++) and 64-bit (``f64`` in Rust / ``double`` in C++); all floating-point representations shall be compliant with IEEE 754.

.. comp_req:: Characters
   :id: comp_req__abi_compatible_data_types__char
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, the Unicode character type shall use fixed-width definitions (``char`` in Rust; wrapper around ``std::uint32_t`` in C++), and shall restrict values to the ranges ``0x0`` to ``0xD7FF`` and ``0xE000`` to ``0x10FFFF``.

.. comp_req:: Fixed-size arrays
   :id: comp_req__abi_compatible_data_types__arr_fix
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, fixed-size arrays shall be declared as ``[T; N]`` in Rust and with a wrapper around ``T[N]`` to perform bounds-checking in C++, where T itself conforms to the ABI compatibility rules.

.. comp_req:: Struct and tuple ABI layout
   :id: comp_req__abi_compatible_data_types__st_tpl
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, tuples and structs shall preserve field order, use ``#[repr(C)]`` in Rust, and be ``standard_layout`` in C++ (no inheritance or virtuals).

.. comp_req:: Explicit enum representation
   :id: comp_req__abi_compatible_data_types__enum_udr
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, enums shall have an explicit, fixed underlying representation (e.g. ``#[repr(u8)]`` in Rust; ``enum class E : std::uint8_t`` in C++).

.. comp_req:: Disallow pointers and metadata
   :id: comp_req__abi_compatible_data_types__nop_mt
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, types shall not contain absolute pointers, references, slices, function pointers, vtables, or any language-specific metadata.

.. comp_req:: Compiler-agnostic ABI
   :id: comp_req__abi_compatible_data_types__compabi
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   For ABI compatibility, all native types shall be ABI-compatible across compilers (e.g. GCC and Clang) using the same endianness.

Custom Types
------------

Vector
^^^^^^

.. comp_req:: Provide AbiVec<T,N>
   :id: comp_req__abi_compatible_data_types__prv_abv
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   An ABI-compatible ``AbiVec<T, N>`` type shall be provided in both C++ and Rust with the specified layout.

   .. code-block:: rust

      #[repr(C)]
      pub struct AbiVec<T, const N: usize> {
         len: u32,
         elements: [T; N],
      }

   .. code-block:: cpp

      template<typename T, std::uint32_t N>
      struct AbiVec {
      private:
         std::uint32_t len;
         T elements[N];
      };

.. comp_req:: AbiVec field semantics
   :id: comp_req__abi_compatible_data_types__abv_fld
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   ``AbiVec.len`` shall report the current element count; ``AbiVec.capacity`` shall equal the compile-time size ``N``.

.. comp_req:: AbiVec API
   :id: comp_req__abi_compatible_data_types__abv_noa
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   The ``AbiVec`` API shall mirror ``std::vector`` / ``Vec<T>``, but shall not allocate or reallocate memory.

.. comp_req:: AbiVec overflow check
   :id: comp_req__abi_compatible_data_types__abv_ovf
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   Any attempt to exceed ``AbiVec.capacity`` shall result in a checked runtime error.

String
^^^^^^

.. comp_req:: Provide AbiString<N>
   :id: comp_req__abi_compatible_data_types__prv_abs
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   An ABI-compatible ``AbiString<N>`` type shall be provided in both C++ and Rust with the specified layout.

   .. code-block:: rust

      #[repr(C)]
      pub struct AbiString<const N: usize> {
         len: u32,
         bytes: [u8; N],
      }

   .. code-block:: cpp

      template<std::uint32_t N>
      struct AbiString {
      private:
         std::uint32_t len;
         std::uint8_t bytes[N];
      };

.. comp_req:: AbiString field semantics
   :id: comp_req__abi_compatible_data_types__abs_fld
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   ``AbiString.len`` shall report the current byte count; ``AbiString.capacity`` shall equal the compile-time size ``N``.

.. comp_req:: AbiString API
   :id: comp_req__abi_compatible_data_types__abs_noa
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   The ``AbiString`` API shall mirror the applicable parts of ``std::basic_string`` / ``String``, but shall not allocate or reallocate memory.

.. comp_req:: AbiString overflow check
   :id: comp_req__abi_compatible_data_types__abs_ovf
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   Any attempt to exceed ``AbiString.capacity`` shall result in a checked runtime error.

Option
^^^^^^

.. comp_req:: Provide AbiOption<T>
   :id: comp_req__abi_compatible_data_types__prv_abo
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   An ABI-compatible ``AbiOption<T>`` type shall be provided in both C++ and Rust with the specified layout.

   .. code-block:: rust

      #[repr(C)]
      pub struct AbiOption<T> {
         is_some: bool,
         value: T,
      }

   .. code-block:: cpp

      template<typename T>
      struct AbiOption {
      private:
         AbiBool is_some;
         T value;
      };

.. comp_req:: AbiOption is_some flag
   :id: comp_req__abi_compatible_data_types__abo_flg
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   ``AbiOption.is_some`` shall be ``false`` when empty and ``true`` when containing a value.

.. comp_req:: AbiOption API
   :id: comp_req__abi_compatible_data_types__abo_api
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   The ``AbiOption`` API shall mirror ``std::optional`` / ``Option<T>`` without introducing extra fields or indirections.

Result
^^^^^^

.. comp_req:: Provide AbiResult<T,E>
   :id: comp_req__abi_compatible_data_types__prv_ari
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   An ABI-compatible ``AbiResult<T, E>`` type shall be provided in both C++ and Rust with the specified layout.

   .. code-block:: rust

      #[repr(C)]
      pub struct AbiResult<T, E> {
         is_err: bool,
         value: AbiResultUnion<T, E>,
      }

      #[repr(C)]
      union AbiResultUnion<T, E> {
         ok: ManuallyDrop<T>,
         err: ManuallyDrop<E>,
      }

   .. code-block:: cpp

      template<typename T, typename E>
      struct AbiResult {
      private:
         AbiBool is_err;
         union {
            T ok;
            E err;
         } value;
      };

.. comp_req:: AbiResult is_err flag
   :id: comp_req__abi_compatible_data_types__ari_flg
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   ``AbiResult.is_err`` shall be ``false`` if ``value.ok`` is valid, and ``true`` if ``value.err`` is valid.

.. comp_req:: AbiResult API
   :id: comp_req__abi_compatible_data_types__ari_api
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__abi_containers[version==2]
   :satisfied_by: comp__baselibs_abi_compatible_data_types[version==1]
   :status: valid
   :version: 1
   :tags: baselibs, abi_compatible_data_types

   The ``AbiResult`` API shall mirror ``std::expected`` / ``Result<T, E>`` without hidden storage or pointers.
