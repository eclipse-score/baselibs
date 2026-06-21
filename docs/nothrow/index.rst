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


Failure Handling Model
======================

score does not yet define a common software-error concept. To make the intent
of the ``score::nothrow`` interfaces unambiguous, the component adopts a
simplified categorization of operation failures and maps each category to a
specific API shape. Two categories are relevant at the container boundary:

* **Error** -- the requested operation could not be completed, but the program
  state remains valid and execution can proceed. The failure is handed back to
  the caller as a value, and the caller decides whether to recover from it or
  to propagate it further. Allocation failure is reported this way: a memory
  resource returns ``nullptr``, and container operations return
  ``score::Result`` or ``score::ResultBlank``.

* **Violation** -- a precondition of the operation was not met. This indicates
  a defect in the calling code, not a runtime condition that the caller could
  legitimately encounter. The operation aborts at the point of detection
  instead of returning, because a bug-free caller cannot trigger it and so has
  no error-handling path to implement or test. The ``OrAbort`` variants
  implement this category.

The category is chosen by the response mechanism and by who is responsible for
handling the failure -- not by how severe the failure seems. The same
underlying condition, a failed allocation, is an *error* when the available
memory is not known in the scope of use, and a *violation* when the capacity is
guaranteed by design, so that a failure could only mean that guarantee was
broken.

Choosing between the two APIs:

* Use the result-returning operations (``Create``, ``PushBack``, ``Insert``,
  and similar) when the memory budget is **not** known at the call site. The
  failure path is real and must be handled by the caller, and it can be tested
  by deliberately providing an undersized memory resource.

* Use the ``OrAbort`` variants when the memory budget **is** known at the call
  site, so that an allocation failure could only mean a broken design
  assumption. The abort is part of the component's contract and is tested
  within the component, for example with a death test, rather than by the
  caller.

Out of scope: process-level termination
---------------------------------------

A third failure category -- an unrecoverable corruption of the runtime
environment that justifies terminating the process -- exists at the system
level but is deliberately **not** a concern of these containers. A container is
not the right place to decide that a process must exit: that decision needs
context about whether any caller higher up can still recover and whether an
orderly shutdown is still safe. ``score::nothrow`` therefore never terminates
the process on its own. It either returns the failure as an error, so that a
layer with the necessary context can decide how to react, or it aborts on a
contract violation.

Memory exhaustion is frequently driven by the size or shape of incoming data
rather than by a defect, and that data may be attacker-controlled -- a request
crafted to allocate as much as possible is a classic denial-of-service vector.
Recovery is therefore the common case, not the exception: a service that hits
its allocation limit while assembling a response can reject that one request,
release the memory the partially built response had reserved, and proceed to
serve the next request. Only the layer handling the request has the context to
draw that line -- which request to reject, what to release, and when to keep
running. A container several levels below it cannot.

For that reason the result-returning APIs are the generic choice and the
``OrAbort`` variants the special case. The premise behind ``OrAbort`` -- that
the integrated environment guarantees enough memory for every possible
operation up front -- holds only in tightly bounded, statically sized
constellations. It is not a generic safe bet, so a guaranteed budget should be
treated as something a specific design has deliberately established, not as the
normal state of affairs.

Converting a propagated allocation error into a controlled process shutdown,
where that genuinely is the right response, remains the responsibility of a
higher layer, at the point where continuing is no longer safe.


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

When inputs are influenced by untrusted sources, users should prefer the
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
