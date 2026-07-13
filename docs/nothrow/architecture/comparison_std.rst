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

.. document:: Nothrow Comparison with Standard Containers
   :id: doc__nothrow_comparison_std
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch

Comparison with Standard Containers
===================================

This document compares ``score::nothrow`` containers with the C++ standard
containers in side-by-side code tables, one scenario per row, in the style used
by C++ standardization papers. Each scenario maps onto the failure handling
model defined in :need:`doc__nothrow_failure_handling`; the comparison
illustrates that model rather than arguing independently of it.

Ground rules, to keep the comparison honest:

* The standard-container cell always shows the **strongest available standard
  attempt**, including ``std::pmr`` where it helps, not a strawman.
* All rows assume the score execution environment: **exceptions are not
  available as an error-handling mechanism** (builds may disable them, and
  coding guidelines ban ``try``/``catch`` recovery paths).
* Where the standard cell cannot be written at all, the cell says so -- an
  empty column is the finding, not a gap in the table.

.. note::

   This comparison covers **failure semantics only**. It serves as *one input*
   to the "when to use which container" guideline requested in the component
   review -- it is not that guideline. The guideline,
   :need:`doc__nothrow_usage_guidelines`, additionally weighs the pointer-slot
   policy selection (offset versus raw pointer storage) and the boundaries
   where standard types remain required because existing and third-party
   interfaces consume them.

Scenario 1: Fallible construction, budget unknown
-------------------------------------------------

Constructing a container with initial capacity requires allocation. The
standard expresses allocation failure in constructors only via exceptions.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - .. code-block:: cpp

          std::pmr::vector<int> values{&resource};
          values.reserve(count);
          // std::bad_alloc -> std::terminate()
          // (even std::pmr::null_memory_resource
          //  signals failure by throwing)

       Verdict: terminates. There is no non-throwing spelling of "construct
       with capacity, tell me if it failed".
     - .. code-block:: cpp

          auto values = Vector<int>::CreateWithCapacity(
              count, allocator);
          if (!values.has_value())
          {
              return values.error();
          }

       Construction failure is an *error* returned to the caller
       (:need:`doc__nothrow_failure_handling`).

Scenario 2: Fallible growth in business logic
---------------------------------------------

The memory budget is dimensioned elsewhere in the architecture; the code
mutating the container cannot know whether growth fits.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - .. code-block:: cpp

          values.push_back(x);
          // std::bad_alloc -> std::terminate()
          // (-fno-exceptions: abort inside
          //  operator new)

       Verdict: terminates. The caller has no error path to implement -- or to
       test.
     - .. code-block:: cpp

          if (auto grown = values.PushBack(x);
              !grown.has_value())
          {
              return grown.error();
          }

       The failure is contained: reject this one request, keep the process
       alive.

Scenario 3: Pre-flight check before mutation
--------------------------------------------

Could the standard-container caller avoid the failure by checking first?

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - **Not expressible.** The standard provides no API to query remaining
       allocatable memory, no way to ask a container whether the next growth
       step will allocate (the growth strategy is an implementation detail),
       and no atomic test-and-allocate operation. Any check-then-act sequence
       a caller improvises races against other consumers of a shared memory
       resource.
     - .. code-block:: cpp

          // no separate check needed: the
          // operation is the atomic
          // test-and-allocate
          auto grown = values.PushBack(x);

       The mutation itself reports whether memory sufficed, atomically with
       respect to the shared resource.

Scenario 4: Testing the failure path
------------------------------------

Error handling that cannot be exercised in a test does not count as robust.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - **Untestable in this environment.** Forcing the failure requires a test
       allocator that throws ``std::bad_alloc`` and a ``try``/``catch`` in the
       test to observe it -- the banned mechanism. With exceptions disabled the
       allocator cannot even throw, so the path cannot be reached at all.
     - .. code-block:: cpp

          PolymorphicAllocator<int> failing{
              GetNullMemoryResource()};
          auto values = Vector<int>::CreateWithCapacity(
              1U, failing);
          EXPECT_FALSE(values.has_value());

       Failure paths are exercised with an undersized or always-failing
       resource; abort paths are exercised with death tests.

Scenario 5: Budget guaranteed by design (violation leg)
-------------------------------------------------------

The capacity was established in the same scope; exceeding it can only be a
bug. The failure handling model classifies this as a *violation*.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - .. code-block:: cpp

          values.reserve(kMaxEntries);
          // ... later, a defect adds one
          // element too many:
          values.push_back(extra);
          // silently reallocates: the broken
          // capacity assumption is absorbed,
          // unbounded growth continues until
          // memory pressure terminates the
          // process far from the defect

       Verdict: the defect is masked at the defect site and surfaces as a
       termination elsewhere, later.
     - .. code-block:: cpp

          auto values = Vector<int>::CreateWithBuffer(
              buffer, sizeof(buffer));
          // ... the same defect:
          values.PushBackOrAbort(extra);
          // aborts here, at the defect site

       The broken guarantee traps immediately where it is broken, and the
       contract is death-testable.

Scenario 6: Bounds-checked element access
-----------------------------------------

In a no-exception environment both libraries end the process on a bounds
violation -- but not with the same clarity.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - .. code-block:: cpp

          const int value = values.at(index);
          // std::out_of_range ->
          // std::terminate()

       Verdict: terminates via an exception no caller is allowed to catch --
       an error-reporting channel that pretends to be recoverable but is not.
     - .. code-block:: cpp

          const int value = values.at(index);
          // aborts on out-of-range access,
          // by documented contract

       The same outcome is a defined *violation* response: a precondition
       breach traps at the defect site and is death-testable.

Scenario 7: Attacker-sized input (denial of service)
----------------------------------------------------

A request field controls how much memory an operation allocates. The
need/budget separation developed in :need:`doc__nothrow_failure_handling`
makes this reachable in practice.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - .. code-block:: cpp

          // count originates from the request
          std::vector<Item> items(count);
          // attacker chooses count large
          // enough -> std::bad_alloc ->
          // std::terminate()

       Verdict: memory pressure becomes process termination -- the attacker's
       denial of service succeeds by design.
     - .. code-block:: cpp

          auto items = Vector<Item>::Create(count);
          if (!items.has_value())
          {
              return Reject(request);
          }

       Exhaustion is a contained error: reject the oversized request, release
       its partial allocations, serve the next one.

Scenario 8: Shared-memory placement
-----------------------------------

Container state placed in a shared-memory object is mapped at different base
addresses in different processes.

.. list-table::
   :header-rows: 1
   :widths: 50 50

   * - Standard containers
     - ``score::nothrow``
   * - **Not usable.** Standard containers persist raw pointers in their
       internal state; a second process mapping the segment at a different
       address reads dangling addresses (undefined behavior). The honest
       non-standard alternative, ``boost::interprocess`` with ``offset_ptr``,
       solves relocation -- but still reports exhaustion of the shared segment
       by throwing, so the failure-semantics gap of the other scenarios
       remains.
     - .. code-block:: cpp

          // node links, buffer base, and pool
          // free list are offsets/indices
          auto map = Map<int, int>::CreateWithBuffer(
              shm_buffer, sizeof(shm_buffer));

       With the default ``OffsetSlotPolicy`` the container state is
       position-independent, and allocation failure remains a ``Result``
       error in every mapping process.

Relation to the usage guideline
-------------------------------

The scenarios above compare **failure semantics**, where the standard
containers range from fragile (scenarios 5, 6) through terminating (1, 2, 7)
to inexpressible (3, 4, 8). That comparison is necessary but not sufficient
for deciding *when to use which container*: a usage guideline must also weigh
the pointer-slot policy choice (offset storage costs indirection and constrains
object placement; raw storage forbids relocation) and the boundaries where
standard types remain required because third-party and standard-library
interfaces consume them -- a deployment constraint, not a statement about the
types' generality. This document is an input to that guideline, not the
guideline itself; the guideline is :need:`doc__nothrow_usage_guidelines`.
