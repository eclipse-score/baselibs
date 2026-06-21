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

.. document:: Nothrow Requirements
   :id: doc__nothrow_requirements
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__requirements_comp

Requirements
============

.. comp_req:: Nothrow allocation failure signaling
   :id: comp_req__nothrow__alloc_fail
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :satisfies: feat_req__nothrow__container_utilities
   :belongs_to: comp__baselibs_nothrow

   The ``score::nothrow`` memory-resource allocation interfaces shall not throw
   on allocation failure and shall instead signal failure by returning
   ``nullptr``.

.. comp_req:: Nothrow fallible construction
   :id: comp_req__nothrow__create
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :satisfies: feat_req__nothrow__container_utilities
   :belongs_to: comp__baselibs_nothrow

   Containers in ``score::nothrow`` shall provide explicit fallible factory
   functions that return ``score::Result<Container>`` for construction paths
   that may require allocation.

.. comp_req:: Nothrow explicit mutation error propagation
   :id: comp_req__nothrow__mutate_result
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :satisfies: feat_req__nothrow__container_utilities
   :belongs_to: comp__baselibs_nothrow

   Container operations that may allocate during mutation shall provide
   explicit error-propagating APIs using ``score::Result`` or
   ``score::ResultBlank`` instead of relying on exception-based failure
   signaling.

.. comp_req:: Nothrow aborting convenience operations
   :id: comp_req__nothrow__or_abort
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :satisfies: feat_req__nothrow__container_utilities
   :belongs_to: comp__baselibs_nothrow

   For operations with explicit error-propagating APIs, ``score::nothrow``
   shall provide clearly named ``OrAbort`` convenience variants for cases where
   allocation failure is considered a programming error.

.. comp_req:: Nothrow pointer-slot policy abstraction
   :id: comp_req__nothrow__pointer_policy
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :satisfies: feat_req__nothrow__container_utilities
   :belongs_to: comp__baselibs_nothrow

   ``score::nothrow`` containers shall store persisted internal pointers
   through a selectable pointer-slot policy abstraction so that storage
   semantics can be changed without rewriting the container algorithms.

Safety Impact
=============

.. comp_req:: Nothrow library ASIL level
   :id: comp_req__nothrow__asil
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :satisfies: feat_req__nothrow__container_utilities
   :belongs_to: comp__baselibs_nothrow

   The ``score::nothrow`` library shall be ASIL-B compliant.
