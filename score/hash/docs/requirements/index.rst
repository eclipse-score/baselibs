..
   # ******************************************************************************
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
   # ******************************************************************************

Requirements
############

.. document:: Hash Requirements
   :id: doc__hash_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, hash

.. comp:: Hash Library
   :id: comp__baselibs_hash
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: baselibs_hash
   :belongs_to: feat__baselibs[version==1]

Functional Requirements
=======================

.. comp_req:: Hash Computation from Byte Data
   :id: comp_req__hash__calculation_interface
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__hash_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_hash[version==1]

   The hash library shall compute a hash value over a contiguous byte sequence
   supplied by the caller, accumulating state across one or more data inputs
   and producing the final digest upon finalization.

.. comp_req:: Hash Value Retrieval as Raw Bytes
   :id: comp_req__hash__value_retrieval_bytes
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__hash_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_hash[version==1]

   The hash library shall expose the computed hash value as a raw byte sequence.

.. comp_req:: Hash Value Retrieval as Hexadecimal String
   :id: comp_req__hash__value_retrieval_hex
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__hash_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_hash[version==1]

   The hash library shall expose the computed hash value as a hexadecimal string
   representation.

.. comp_req:: Algorithm Selection via Factory
   :id: comp_req__hash__factory_interface
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__hash_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_hash[version==1]

   The hash library shall allow the hash algorithm to be selected and a
   calculator instance to be created independently of the code that performs
   the computation, so that the calling code is not coupled to any specific
   algorithm implementation.

.. comp_req:: Cryptographic Hash Algorithms
   :id: comp_req__hash__sha_algorithms
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__hash_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_hash[version==1]

   The hash library shall support SHA-1, SHA-256, SHA-384, and SHA-512 as
   cryptographic hash algorithms.

.. comp_req:: CRC-32 Checksum Algorithm
   :id: comp_req__hash__crc32_algorithm
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__hash_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_hash[version==1]

   The hash library shall support IEEE CRC-32 as a checksum algorithm.

.. comp_req:: Exception-Free Error Propagation
   :id: comp_req__hash__safe_computation
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__hash_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_hash[version==1]

   The hash library shall propagate errors from hash calculator instantiation
   via return-value types, enabling use in safety-critical contexts where
   exception handling is not available.
