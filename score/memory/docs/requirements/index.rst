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

.. document:: Memory Library Requirements
   :id: doc__memory_lib_requirements
   :status: draft
   :version: 2
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, memory_library

Functional Requirements
=======================

.. comp_req:: Endianness Utilities
   :id: comp_req__memory__endianness
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__memory_library[version==2]
   :status: valid
   :version: 2
   :satisfied_by: comp__baselibs_memory[version==1]

   The Memory library shall identify the host byte order as little endian or big endian and report whether it differs from a requested byte order.
   The library shall reverse the byte order of 32-bit and 64-bit unsigned integers and convert big-endian values of those types to host byte order.

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: baselibs

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: memory
