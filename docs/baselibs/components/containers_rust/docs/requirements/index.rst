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

.. document:: Rust Containers Library Requirements
   :id: doc__containers_rust_lib_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, containers_rust_library

.. comp:: Rust Containers
   :id: comp__baselibs_containers_rust
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: baselibs_rust_containers_rust
   :belongs_to: feat__baselibs[version==1]

Functional Requirements
=======================

.. comp_req:: Fixed-Capacity Vector
   :id: comp_req__containers_rust__fixed_vector
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a heap-allocated vector container with construction-time capacity specification.

.. comp_req:: Inline-Storage Vector
   :id: comp_req__containers_rust__inline_vector
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a vector container with a capacity determined at compile-time,
   which stores the elements within the data structure itself.
   This container shall have a stable, precisely defined memory layout, capable of serving as the basis for an ABI-compatible vector.

.. comp_req:: Fixed-Capacity String
   :id: comp_req__containers_rust__fixed_string
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a heap-allocated, UTF-8 encoded string data type with construction-time capacity specification.

.. comp_req:: Inline-Storage String
   :id: comp_req__containers_rust__inline_string
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a UTF-8 encoded string data type with a capacity determined at compile-time,
   which stores the codepoints within the data structure itself.
   This container shall have a stable, precisely defined memory layout, capable of serving as the basis for an ABI-compatible string.

.. comp_req:: Fixed-Capacity Queue
   :id: comp_req__containers_rust__fixed_queue
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a heap-allocated double-ended queue container with construction-time capacity specification.

.. comp_req:: Inline-Storage Queue
   :id: comp_req__containers_rust__inline_queue
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a queue container with a capacity determined at compile-time,
   which stores the elements within the data structure itself.
   This container shall have a stable, precisely defined memory layout, capable of serving as the basis for an ABI-compatible queue.

.. comp_req:: Fixed-Capacity Hashmap
   :id: comp_req__containers_rust__fixed_hashmap
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a heap-allocated hashmap container with construction-time capacity specification.

.. comp_req:: Inline-Storage Hashmap
   :id: comp_req__containers_rust__inline_hashmap
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a hashmap container with a capacity determined at compile-time,
   which stores the elements within the data structure itself.
   This container shall have a stable, precisely defined memory layout, capable of serving as the basis for an ABI-compatible hashmap.

.. comp_req:: Fixed-Capacity Hashset
   :id: comp_req__containers_rust__fixed_hashset
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a heap-allocated hashset container with construction-time capacity specification.

.. comp_req:: Inline-Storage Hashset
   :id: comp_req__containers_rust__inline_hashset
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a hashset container with a capacity determined at compile-time,
   which stores the elements within the data structure itself.
   This container shall have a stable, precisely defined memory layout, capable of serving as the basis for an ABI-compatible hashset.

.. comp_req:: Fixed-Capacity B-Tree
   :id: comp_req__containers_rust__fixed_btree
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a heap-allocated B-tree container with construction-time capacity specification.

.. comp_req:: Inline-Storage B-Tree
   :id: comp_req__containers_rust__inline_btree
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall provide a B-tree container with a capacity determined at compile-time,
   which stores the elements within the data structure itself.
   This container shall have a stable, precisely defined memory layout, capable of serving as the basis for an ABI-compatible B-tree.

.. comp_req:: Type Safety
   :id: comp_req__containers_rust__type_safety
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust Containers library shall enforce compile-time type safety for all container operations.

Non-Functional Requirements
===========================

.. comp_req:: Deterministic Behavior
   :id: comp_req__containers_rust__det_behavior
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   The Rust containers library shall provide deterministic behavior.

.. comp_req:: No Heap Allocation
   :id: comp_req__containers_rust__inline_no_heap
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   Inline-storage containers shall never allocate heap memory.

.. comp_req:: No Reallocation
   :id: comp_req__containers_rust__fixed_no_realloc
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__baselibs_containers_rust[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]
   :status: valid
   :version: 1

   Fixed-capacity containers shall never allocate or re-allocate memory after their initial construction.
