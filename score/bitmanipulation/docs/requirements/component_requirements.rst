..
   # *******************************************************************************
   # Copyright (c) 2025-2026 Contributors to the Eclipse Foundation
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

Component Requirements
======================

Functional Requirements
-----------------------

.. comp_req:: Support for Bit Operations
   :id: comp_req__bitmanipulation__bit_operations
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__bitmanipulation[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_bit_manipulation[version==1]
   :tags: inspected

   The bit manipulation component shall provide an API for setting, clearing, toggling, and checking individual bits for any integral type up to 64 bits, returning boolean success status.

.. comp_req:: Support for Byte and Half-Byte Types
   :id: comp_req__bitmanipulation__byte_operations
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__bitmanipulation[version==2]
   :status: valid
   :version: 2
   :satisfied_by: comp__baselibs_bit_manipulation[version==1]

   The bit manipulation component shall provide byte and half-byte types, with a byte composable from and decomposable into two half-bytes.

.. comp_req:: Support for Byte Extraction
   :id: comp_req__bitmanipulation__byte_extraction
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__bitmanipulation[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_bit_manipulation[version==1]

   The bit manipulation component shall provide an API to extract the byte at a given position from any integral type up to 64 bits.

.. comp_req:: Support for Bitmask Operators for Enum Classes
   :id: comp_req__bitmanipulation__bitmask_operators
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__bitmanipulation[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_bit_manipulation[version==1]
   :tags: inspected

   The bit manipulation library shall provide type-safe bitmask operations for scoped enumeration types.

.. comp_req:: Bounds and Safety Checks
   :id: comp_req__bitmanipulation__bounds_safety
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__bitmanipulation[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_bit_manipulation[version==1]
   :tags: inspected

   The bit manipulation functions shall validate input parameters against bounds and, on out-of-bounds access, shall leave the target value unmodified and return false.

Non-Functional Requirements
---------------------------

.. comp_req:: Header-only API
   :id: comp_req__bitmanipulation__header_only
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__bitmanipulation[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_bit_manipulation[version==1]
   :tags: inspected

   The bit manipulation API shall be header-only and not require external dependencies.

.. needextend:: c.this_doc() and (type == "comp_req")
   :+tags: baselibs, bitmanipulation
