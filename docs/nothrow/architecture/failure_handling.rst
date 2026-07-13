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

.. document:: Nothrow Failure Handling Model
   :id: doc__nothrow_failure_handling
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch

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
  no error-handling path to implement or test. The ``OrAbort`` variants and the
  bounds-checked accessors (``at()``, ``front()``, ``back()``) implement this
  category.

The category is chosen by the response mechanism and by who is responsible for
handling the failure -- not by how severe the failure seems. The same
underlying condition, a failed allocation, is an *error* when the available
memory is not known in the scope of use, and a *violation* when the capacity is
guaranteed by design, so that a failure could only mean that guarantee was
broken.

Choosing between the two APIs
-----------------------------

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
