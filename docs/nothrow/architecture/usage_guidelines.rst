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

.. document:: Nothrow Usage Guidelines
   :id: doc__nothrow_usage_guidelines
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch

Usage Guidelines
================

This document answers the two questions a user of ``score::nothrow`` has to
decide, in order:

1. **Container choice** -- standard container or ``score::nothrow`` container?
2. **Pointer policy choice** -- ``OffsetSlotPolicy`` (default) or
   ``RawSlotPolicy``?

The two decisions are orthogonal: the first is driven by failure semantics,
the second by where the container's memory lives and which interfaces it must
pass through. Supporting analyses: :need:`doc__nothrow_failure_handling`
(failure semantics), :need:`doc__nothrow_comparison_std` (side-by-side
comparison with the standard containers), and ``score/nothrow/BENCHMARK.md``
(pointer-policy runtime costs).

Container choice: standard or nothrow
-------------------------------------

The standard library remains the default where its failure semantics are
acceptable, and standard types remain required at boundaries to interfaces
that consume them -- a third-party API expecting ``std::vector`` cannot be
handed a ``score::nothrow`` type. Adoption of ``score::nothrow`` is therefore
deliberately targeted at the code that needs its guarantees rather than a
blanket replacement: a statement about where the types can be deployed today,
not about their generality.

Use a ``score::nothrow`` container when at least one of the following holds:

* **Allocation failure must be a contained, testable error.** The memory
  budget is not co-located with the consuming code -- it is sized by
  configuration at integration time, shared between consumers, or the
  allocation amount is influenced by input data. Standard containers can only
  terminate in this situation; the failure path is not even testable without
  exceptions (:need:`doc__nothrow_comparison_std`, scenarios 1--4).

* **The process must survive memory pressure.** Data crossing a trust
  boundary can be attacker-sized; exhaustion must be rejected per request
  rather than terminate the process (scenario 7).

* **A fixed budget contract should trap at the defect site.** Where capacity
  is guaranteed by design, the ``OrAbort`` variants convert a broken guarantee
  into an immediate, death-testable abort instead of a masked reallocation
  (scenario 5).

* **Container state is placed in shared memory** (scenario 8, and the pointer
  policy section below).

Keep the standard container when none of these applies -- in particular for
interoperation with interfaces that consume standard types, and for local
containers whose allocation behavior is trivially bounded and whose failure
would be acceptable as termination anyway.

Within ``score::nothrow``, choose between the error-returning and the
aborting API per operation, following the failure handling model: the
result-returning operations are the generic choice; ``OrAbort`` is legitimate
only where the memory budget is established, owned, and verifiable in the
same scope as the consumption (:need:`doc__nothrow_failure_handling`).

Pointer policy choice: offset or raw
------------------------------------

The pointer policy decides how a container persists its internal pointers
(tree links, buffer bases, pool free lists). It is part of the container
*type*, so the choice propagates through every interface the container passes
through -- which is exactly why the default is the generic one.

Use ``OffsetSlotPolicy`` (the default) when a dynamic container is passed by
reference through generic interfaces
...............................................................................

Application code that operates on a container received by reference should
not have to care where that container's memory lives. That placement is an
integration decision, and it legitimately differs between deployments of the
same logic:

* **Shared-memory IPC:** the container lives in a shared-memory object and is
  accessed zero-copy by several processes, each mapping it at a different
  base address.
* **Process-local IPC:** the container lives in ordinary process-local
  memory -- for example populated from an OS-provided buffer backing
  socket-based communication -- and never leaves the process address space.

With ``OffsetSlotPolicy``, both cases are the *same container type*. A
function taking ``Map<int, int>&`` serves both deployments unchanged; the
application logic does not fork per IPC mechanism, and the transport decision
stays where it belongs -- with the integrator provisioning the memory.

This genericity is deliberately bought at the cheapest available price. The
offset encoding is inline and fully visible to the optimizer; the measured
overhead over raw pointers is in the single-digit to low-double-digit percent
range for worst-case random access patterns (~1.1x nested-vector random
access, ~1.5x map iteration, ~2x pointer-chasing map lookup microbenchmarks;
insert/rebalance at or below standard-container cost -- see
``score/nothrow/BENCHMARK.md``). The alternatives for placement-independent
application code all cost more: instantiating the logic for both policies
duplicates types and interfaces, type erasure adds dispatch on every access,
and barrier-based fancy-pointer implementations measured in the same
benchmark run one to two orders of magnitude slower.

Use ``RawSlotPolicy`` for closed, single-address-space subsystems with a
profiled need
...............................................................................

``RawSlotPolicy`` stores plain pointers and benchmarks at parity with the
standard containers. Choose it only when *both* hold:

* the container is confined to one process address space by design, with no
  generic interface anywhere in its reach that must also accept shared-memory
  instances, and
* profiling shows the offset arithmetic matters on a hot path of this
  subsystem.

The cost of choosing raw is architectural, not runtime: a raw-policy
container is a different type, so it cannot be handed to the generic
interfaces above, and the fork propagates through every signature it touches.
Confine ``RawSlotPolicy`` to subsystem-internal types that never cross a
module boundary.

.. note::

   The pointer policy makes container *state* position-independent; it does
   not by itself make a container shared-memory ready. For cross-process use,
   the backing storage must also reside in the shared segment
   (``CreateWithBuffer``), and synchronization between processes remains the
   integrator's responsibility.

Decision summary
----------------

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - Situation
     - Choice
   * - Interop with third-party or standard-library interfaces; trivially
       bounded local container
     - Standard container
   * - Budget not co-located with consumption; input-influenced allocation;
       failure must be contained and testable
     - ``score::nothrow``, result-returning APIs
   * - Untrusted input can drive allocation size
     - ``score::nothrow``, result-returning APIs; reject the request on error
   * - Budget established and verifiable in the same scope
     - ``score::nothrow``, ``OrAbort`` variants
   * - Container passed by reference through generic interfaces; placement
       (SHM vs process-local) is an integration decision
     - ``OffsetSlotPolicy`` (default)
   * - Container state in shared memory
     - ``OffsetSlotPolicy`` + ``CreateWithBuffer``
   * - Closed subsystem, single address space, profiled hot path
     - ``RawSlotPolicy``
