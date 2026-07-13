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

Memory need and memory budget are separated
-------------------------------------------

Treating allocation failure as an error by default is not a stylistic choice.
It follows from how memory is provisioned in a modularized architecture: the
*budget* and the *need* are established in different places, by different
roles, at different times, and several independent separations compound.

* **Separation in time and role.** The budget is fixed at integration time
  through configuration -- for example by sizing preallocated memory resources
  or shared-memory buffers. The need arises at run time inside business logic
  operating on containers. The integrator sizes without seeing every code path;
  the component consumes without seeing the configuration.

* **Separation across components.** Data crossing a trust boundary is validated
  in a different component than the one that later allocates container elements
  from it. The allocating code cannot assume that input validation bounded the
  data to fit *its* budget, because the validator does not know that budget.

* **Sharing couples consumers.** Memory resources are deliberately shared
  between container objects to keep the number of configuration parameters
  manageable. As a consequence, one component's consumption changes another
  component's headroom -- invisibly to both.

* **Independent evolution.** The logic that determines actual consumption lives
  in reusable parts that change under their own maintenance cadence. A budget
  that was sufficient at integration can silently stop being sufficient after a
  routine update of a library nobody re-sized the configuration for.

These separations produce an asymmetry: in such an architecture it is
extraordinarily hard to ensure that all values fit, and practically impossible
to *prove* budget sizing correct by testing -- while it is easy for an attacker
to trigger resource exhaustion deliberately. A request crafted to allocate as
much as possible is a classic denial-of-service vector: the defender must be
right about every path, the attacker needs one oversized request.

Allocation failure must therefore be treated as a reachable runtime condition
-- an **error** -- by default. The payoff is a contained failure path: a
service that hits its allocation limit while assembling a response can reject
that one request, release the memory the partially built response had reserved,
and proceed to serve the next request. Only the layer handling the request has
the context to draw that line -- which request to reject, what to release, and
when to keep running.

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

A co-located budget -- established, owned, and verifiable in the same scope as
the consumption -- is exactly the special constellation in which the
separations described above have been deliberately eliminated. Only then is
classifying allocation failure as a violation legitimate. Because that holds
solely in tightly bounded, statically sized constellations, the
result-returning APIs are the generic choice and the ``OrAbort`` variants the
special case: a guaranteed budget is something a specific design has
deliberately established, not the normal state of affairs.

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

Converting a propagated allocation error into a controlled process shutdown,
where that genuinely is the right response, remains the responsibility of a
higher layer, at the point where continuing is no longer safe.
