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

Nothrow
#######

.. document:: nothrow
   :id: doc__nothrow
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__cmpt_request
   :tags: component_request, nothrow


Abstract
========

This component request captures ``score::nothrow``, a baselibs component that
provides nothrow memory resources and container building blocks for C++ code
that must not rely on exceptions for allocation-failure handling.

The component currently contains memory-resource abstractions, pointer-slot
policies, and container implementations such as ``Vector`` and ``Map``.
Together, these building blocks provide a std-like programming model while
making out-of-memory behavior explicit in the API.


Motivation
==========

The standard C++ container and ``std::pmr`` interfaces rely on exceptions to
report allocation failure. That model does not fit code that must remain robust
in builds or environments where exceptions are not available or are not an
acceptable error-handling mechanism.

``score::nothrow`` exists to provide a predictable alternative for these cases:
allocation failures are surfaced explicitly through ``score::Result``-based
interfaces or, where appropriate, through clearly named aborting convenience
wrappers.


Rationale
=========

The component follows standard-library interfaces closely where that can be
done without reintroducing exception-based failure handling. This minimizes the
learning curve and supports reuse of established container idioms.

For operations that would ordinarily fail with ``std::bad_alloc``, the
component deliberately exposes explicit error-propagating APIs. This design
makes failure handling visible at the call site and allows higher-level code to
choose between propagation and deliberate abort semantics.


Scope and Relation to Communication Modelling
=============================================

``score::nothrow`` is a **library component**: its types are linked into an
executable and used within that executable's address space. The component is
not a platform service and does not define a communication stack.

The only interprocess contract the component specifies is the **binary
representation of container state in shared memory**: standard-layout types,
internal pointers persisted through the offset pointer-slot policy, and
bounded objects with a fixed footprint but dynamic content. Memory
*management* does not cross that boundary: the polymorphic allocator and
memory resource are not part of the shared representation. Memory provisioning
stays with the memory-aware side, which creates the container in the shared
segment (``CreateWithBuffer``); in that scenario the allocator parameter is
type-compatibility decoration only.

The proposal therefore addresses two separable aspects:

1. **Binary representation** -- the stable ABI that shared-memory participants
   agree on: standard-layout types and offset-encoded pointers, independent of
   language bindings and rebuild-free across storage locations and container
   sizes.

2. **API** -- error propagation on memory exhaustion, with memory provisioning
   hidden behind a generic allocator as in the C++ standard library. Container
   references pointing into shared memory can be passed to component
   interfaces that are unaware of the IPC layer and of memory provisioning,
   without payload copying.

Relation to communication modelling:

* score has deliberately decided not to model executables and components,
  unlike communication-modelling approaches such as AUTOSAR Adaptive. Nothing
  in this component changes or depends on that decision. ``score::nothrow``
  does not introduce, require, or substitute communication modelling.

* Where a communication model and generator exist, the types can serve as
  **elements in the generator's output** -- for example as typed event
  payloads placed in shared memory by a communication stack. They complement
  such a stack; they do not compete with it.

* The types work equally well in **pure API-driven configurations** without
  any model or generator involved.

Usage scenarios discussed in the component review illustrate this scope:

* **Event communication (one-to-many):** the sender is memory-aware, maintains
  an object pool in shared memory, and retains ownership; receivers access
  payloads by reference without ownership transfer.

* **Method communication (one-to-one):** for intra-process API communication,
  ownership passing is possible, including across C++/Rust API boundaries,
  using opaque references to the binary-stable memory-resource interfaces.

* **Gateway serialization:** dynamic datatypes in shared memory remain
  interpretable by a gateway process (for example for SOME/IP serialization),
  because the shared representation is self-contained.

Both aspects are demonstrated by runnable examples in ``example/``. The
shared-memory demo (``shm_parent``/``shm_child`` with
``bounded_containers.h``) exercises the **binary representation**: two
processes map the same segment at different base addresses, and the child
reads, mutates, and inserts into containers the parent created.
``resource_scopes.cpp`` exercises the **API** aspect: the same allocation is
handled with ``OrAbort`` where the budget is co-located with its use, and as
a propagated ``score::Result`` error where memory is provisioned centrally --
including the rejection of an oversized request without process termination.


Failure Handling Model
======================

score does not yet define a common software-error concept. To make the intent
of the ``score::nothrow`` interfaces unambiguous, the component adopts a
simplified categorization of operation failures and maps each category to a
specific API shape. Failures are either **errors** -- the request could not be
fulfilled, program state remains valid, and the failure is returned to the
caller as ``score::Result`` -- or **violations** -- a caller precondition was
broken, which indicates a code defect and aborts at the point of detection
(the ``OrAbort`` variants and bounds-checked accessors).

The category is chosen by the response mechanism and by who owns the handling,
not by severity: the same failed allocation is an *error* when the memory
budget is unknown at the call site, and a *violation* when the budget is
guaranteed by design. In a modularized architecture, memory *need* and memory
*budget* are usually not co-located -- the budget is sized by configuration at
integration time while the need arises at run time in independently evolving,
resource-sharing components -- so an up-front guarantee holds only in tightly
bounded constellations. The result-returning APIs are therefore the generic
choice and the ``OrAbort`` variants the special case. Process-level
termination in response to a propagated allocation error is deliberately left
to higher layers that have the context to decide it.

The full model -- category definitions, the need/budget separation argument,
the rule for choosing between the two API shapes, and the rationale for
keeping process termination out of the containers -- is defined in the
component architecture: :need:`doc__nothrow_failure_handling`.


Specification
=============

The component requirements are collected in :need:`doc__nothrow_requirements`.
At a high level, ``score::nothrow`` provides the following capabilities:

.. feat_req:: Explicit no-exception container utilities
   :id: feat_req__nothrow__container_utilities
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: invalid
   :satisfies: stkh_req__functional_req__base_libraries

   Baselibs shall provide container and allocation utilities that make
   allocation failure explicit without relying on exception-based error
   signaling.

* nothrow memory-resource APIs that return ``nullptr`` instead of throwing on
  allocation failure
* fallible construction and mutation APIs for containers using
  ``score::Result`` and ``score::ResultBlank``
* explicit ``OrAbort`` convenience variants for cases where failure is treated
  as a programming error
* configurable pointer-slot storage policies for persisted container state


Backward Compatibility
======================

``score::nothrow`` is additive within baselibs. It introduces new component
APIs and does not require existing baselibs users to change their current code
unless they opt into these types.


Security Impact
===============

The component does not introduce a network-facing or parser-facing attack
surface on its own. Its primary security relevance is operational: callers can
use explicit error-propagating APIs to prevent memory pressure from turning
into uncontrolled termination.

Resource exhaustion triggered by attacker-controlled input is a classic
denial-of-service vector, and the separation of memory need from memory budget
in a modularized architecture makes it easy to reach and hard to rule out by
testing; :need:`doc__nothrow_failure_handling` develops this argument. When
inputs are influenced by untrusted sources, users should therefore prefer the
``score::Result``-returning APIs over ``OrAbort`` variants so that resource
exhaustion can be handled as a contained error path.


Safety Impact
=============

The component is intended to support code bases that require explicit handling
of allocation failure without exceptions. This can reduce ambiguity in safety
relevant control flow by making failure paths part of the interface contract.

**Expected ASIL Level:**

ASIL B.

**Classification:**

Safety-relevant component.

The detailed assumptions of use resulting from this classification still need
to be captured as part of component qualification.


License Impact
==============

None identified. The component is implemented within baselibs and follows the
repository's existing licensing model.


How to Teach This
=================

Users should be taught to approach ``score::nothrow`` as a std-like API with
explicit failure handling. The `Failure Handling Model`_ section describes the
error and violation categories and the rule for choosing between the
result-returning operations and their ``OrAbort`` variants.

Runnable teaching material lives in ``example/``: ``resource_scopes.cpp`` is
the decision rule in code -- a co-located budget handled with ``OrAbort``
next to centrally provisioned memory handled with ``score::Result`` -- and
the shared-memory demo shows bounded containers used across processes.


Rejected Ideas
==============

No rejected alternatives are documented yet in this initial component-request
draft.


Open Issues
===========

* complete component qualification artifacts, including architecture detail and
  the security classification
* decide whether additional ``score::nothrow`` containers or adapters belong in
  the component scope


Footnotes
=========

.. toctree::

   architecture/index.rst
   requirements/index.rst
