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

Requirements
############

.. document:: Utils Library Requirements
   :id: doc__utils_lib_requirements
   :status: valid
   :version: 2
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, utils_library

Functional Requirements
=======================

.. comp_req:: Base64 Encoding
   :id: comp_req__utils__base64_encoding
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid_inspected
   :version: 1
   :satisfied_by: comp__baselibs_utils[version==1]

   The Utils component shall provide functions for encoding data to Base64 format.

.. comp_req:: Base64 Decoding
   :id: comp_req__utils__base64_decoding
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid_inspected
   :version: 1
   :satisfied_by: comp__baselibs_utils[version==1]

   The Utils component shall provide functions for decoding Base64 data back to its original form.

.. comp_req:: Thread-Safe Singleton Instance Creation
   :id: comp_req__utils__threadsafe_singleton
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid_inspected
   :version: 1
   :satisfied_by: comp__baselibs_utils[version==1]

   The Utils component shall provide a thread-safe singleton whose instance is created exactly once when accessed concurrently by multiple threads.

   .. comp_req:: String Registry
   :id: comp_req__utils__string_registry__string_registry
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__containers_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_containers[version==2]

   The Utils component shall provide a string registry that deduplicates strings into a single backing allocation and guarantees stable memory addresses across the registry's lifetime.

.. comp_req:: Registry String View
   :id: comp_req__utils__string_registry__registry_string_view
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__containers_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_containers[version==2]

   The Utils component shall provide a Registry_string_view that uses pointer equality and pointer-based hashing to enable O(1) identity comparisons between views originating from the same String_registry.

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: baselibs, utils
