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

.. _comp_doc_memory:

Memory
######

.. document:: Memory Library
   :id: doc__memory
   :status: draft
   :version: 2
   :safety: ASIL_B
   :tags: baselibs_memory
   :realizes: wp__cmpt_request[version==1]
   :security: YES

.. toctree::
   :hidden:

   architecture/index
   requirements/index.rst

Abstract
========

The Memory library provides byte-order utilities, size and alignment information, and a ring buffer with construction-time capacity.

Motivation and Rationale
========================

Callers use these utilities to interpret byte order, represent allocation size and alignment, and store a bounded sequence using a caller-selected allocation resource.

Specification
=============

The Memory library provides the following capabilities.

Endianness
----------

The endianness utilities identify the host byte order as little endian or big endian and determine whether a requested byte order differs from the host.
They reverse the bytes of 32-bit and 64-bit unsigned integers and convert big-endian values of those types to host byte order.
The endianness unit tests partially verify :need:`comp_req__memory__endianness` through host detection, byte-order comparison, byte reversal, and big-endian-to-host conversion.
The current test environment assumes a little-endian host.

Size and Alignment Information
------------------------------

Size and alignment information describes an allocation's size and required alignment and supports comparing two size and alignment pairs for equality.
Alignment must be a nonzero power of two within the supported range, and size must be a multiple of alignment.

Fixed-Capacity Ring Buffer
--------------------------

The ring buffer stores a bounded sequence with capacity selected at construction.
Callers supply an allocation resource and a capacity greater than zero.
Inserting into a full buffer removes the oldest element before adding the new element.
The oldest element can be accessed or removed, the buffer can be cleared, and its size and capacity can be queried.
