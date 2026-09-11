..
   # *******************************************************************************
   # Copyright (c) 2026 Contributors to the Eclipse Foundation
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

Requirements
############

.. document:: String Manipulation Requirements
   :id: doc__string_manipulation_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, string_manipulation

Functional Requirements
=======================

.. comp_req:: Argument Conversion
   :id: comp_req__string_manipulation__args_conv
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :tags: inspected
   :satisfied_by: comp__baselibs_string_manipulation[version==1]

   The String Manipulation component shall provide an operation that converts a command-line argument array into an ordered collection of null-terminated string views.

.. comp_req:: Lazy String Splitting
   :id: comp_req__string_manipulation__lazy_split
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :tags: inspected
   :satisfied_by: comp__baselibs_string_manipulation[version==1]

   The String Manipulation component shall provide a forward-iterable operation that lazily splits a string view on a delimiter, preserves empty substrings required by delimiter placement, and performs no dynamic memory allocation.

.. comp_req:: String-Like Comparison and Hashing
   :id: comp_req__string_manipulation__cmp_hash
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :tags: inspected
   :satisfied_by: comp__baselibs_string_manipulation[version==1]

   The String Manipulation component shall provide a string-like adaptor that accepts supported string representations, exposes their content as a string view, compares by content, and produces equal hashes for equal content.