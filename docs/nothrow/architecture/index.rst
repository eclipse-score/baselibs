..
   *******************************************************************************
   Copyright (c) 2026 Contributors to the Eclipse Foundation

   See the NOTICE file(s) distributed with this work for additional
   information regarding copyright ownership.

   This program and the accompanying materials are made available under the
   terms of the Apache License Version 2.0 which is available at
   https://www.apache.org/licenses/LICENSE-2.0

   SPDX-License-Identifier: Apache-2.0
   *******************************************************************************

.. document:: Nothrow Architecture
   :id: doc__nothrow_architecture
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch

Nothrow Component Architecture
==============================

.. comp:: score::nothrow
   :id: comp__baselibs_nothrow
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :belongs_to: feat__baselibs

Overview
--------

``score::nothrow`` is organized into three main areas:

* memory resources and allocators, including ``MemoryResource``,
  ``PolymorphicAllocator``, and ``MonotonicBufferResource``
* pointer-slot abstractions that control how persisted container pointers are
  stored
* container implementations such as ``Vector`` and ``Map`` that use the
  nothrow allocation model

Design Principles
-----------------

The component preserves standard-library naming and behavior where possible,
but replaces exception-based allocation failure with explicit result-returning
or aborting APIs. Which failures are reported which way is governed by the
component's failure handling model, defined in
:need:`doc__nothrow_failure_handling`: failures are categorized as *errors*
(returned to the caller as ``score::Result``) or *violations* (caller contract
breaches that abort at the point of detection).

Persistent container state is parameterized through pointer-slot policies so
that storage of internal links and buffer pointers can be adapted without
changing container algorithms.

A side-by-side comparison with the standard containers, one failure scenario
per row, is provided in :need:`doc__nothrow_comparison_std`. It serves as an
input to the planned "when to use which container" guideline.

Dependencies
------------

The component depends on existing baselibs building blocks, most notably
``score::Result`` for error propagation. It also follows repository-wide
toolchain and quality settings defined in the local Bazel targets.

Detailed design notes remain in ``score/nothrow/README.md`` and the component's
source files while this architecture document is being expanded.

.. toctree::

   failure_handling.rst
   comparison_std.rst
