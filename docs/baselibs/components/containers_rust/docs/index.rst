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

.. _component_containers_rust:

containers_rust
###############

.. document:: Rust Containers Library
   :id: doc__containers_rust
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__cmpt_request
   :tags: baselibs_rust_containers_rust


Abstract
========

This component provides a library of Rust containers with fixed capacity.
Each container is available in two variations: one which stores its elements within the data structure itself, and one which allocates memory on the heap.
The inline-storage containers have a stable, well-defined memory layout and can serve as the basis for implementing *ABI-compatible data structures*.
The heap-allocated containers only perform a single allocation when they are created, and reject operations that would exceed their capacity.


Motivation
==========

Software based on the S-CORE platform requires a rich set of data structures, including vectors, maps, and sets.
These data structures should exhibit deterministic behavior; in particular, operations on them should not fail due to memory allocation errors.
One way to achieve this would be to reserve a fixed amount of memory in the form of static variables.
However, this approach requires selecting the maximum capacity already at compile time.
A more flexible solution, which allows defering this decision until runtime, is to allocate memory only during the startup phase of the program, so that any out-of-memory errors occur immediately and not during the main operational phase.
After the creation of such a *fixed-capacity container*, any operation that would exceed the allocated capacity should fail with an explicit error return value, instead of panicking or aborting the program.

Another use case requiring custom container implementations are *ABI-compatible data structures*.
These data structures guarantee that they can be safely sent to other processes via shared memory.
They have different requirements compared to the aforementioned fixed-capacity containers;
specifically, they need to store their elements *inline*, i.e., within the type instance itself instead of on the heap, and therefore require that their capacity to be chosen at compile-time.
The reason for including these inline-storage containers in this component is that they offer a similar interface to fixed-capacity containers, and share nearly all of their implementation logic.


Rationale
=========

Minimum Capacity on Inline Storage
----------------------------------

The inline storage implementation requires a minimum capacity of 1.
The reason for this is that the containers which use this inline storage are intended as the basis for ABI-compatible data structures, and C++ doesn't allow zero-sized types.
Although it would be possible (through template-specialization for the zero-capacity case) to implement C++ data structures which circumvent this restriction, this would make the C++ code more complex.
Since containers with a statically-determined capacity of zero are not very useful, they are forbidden for now;
this constraint may be relaxed in the future.

Fixed-capacity, heap-allocated containers are not affected by this minimum:
When a capacity of zero is requested, no memory is allocated and the container behaves accordingly.


Maximum Capacity
----------------

The capacity field in containers is encoded as a 32-bit unsigned integer (``u32``), so the maximum number of elements per instance is 4,294,967,295.
This makes the computation of indices within the container's logic more efficient.
Larger containers are expected to be an extremely rare case in the context of the S-CORE platform:
Even single-byte elements would involve reserving more than 4 GiB of memory, a prohibitively large commitment of resources on typical embedded systems.
In case an application really *does* need a larger container, it can implement a custom data structure according to its demands, or it can distribute the elements over several standard containers.


Specification
=============

[Describe the requirements, architecture of any new component.] or
[Describe the change to requirements, architecture, implementation, documentation of any change request.]

   .. note::
      A CR shall specify the component requirements as part of our platform/project.
      Thereby the :need:`rl__project_lead` will approve these requirements as part of accepting the CR (e.g. merging the PR with the CR).


Security Impact
===============

The implementation of the container data structures requires low-level memory management and `unsafe` operations.
These code sections are thoroughly tested and strictly encapsulated behind a safe interface.
The API provided by the data structures doesn't contain any `unsafe` methods beyond those available through the Rust standard library.
Therefore, no security impact is expected.


Safety Impact
=============

[How could the safety be impacted by the new/modified component?]

   .. note::
      If there are safety concerns in relation to the CR, those concerns should be explicitly written out to make sure reviewers of the CR are aware of them.

Which safety requirements are affected or has to be changed?
Could the new/modified component be a potential common cause or cascading failure initiator?
If applicable, which additional safety measures must be implemented to mitigate the risk?

    .. note::
     Use Dependency Failure Analysis and/or Safety Software Critically Analysis.
     [Methods will be defined later in Process area Safety Analysis]

For new feature/component contributions:

[What is the expected ASIL level?]
[What is the expected classification of the contribution?]

   .. note::
      Use the component classification method here to classify your component, if it shall to be used in a safety context: :need:`gd_temp__component_classification`.


License Impact
==============

No license impact expected.


How to Teach This
=================

The container types provide an interface which is very similar to the corresponding containers in the Rust standard library, except that capacity-related operations return ``Result`` or ``Option``.
The API should therefore be quick and easy to learn for any Rust developer.


.. toctree::
   :hidden:

   requirements/index.rst
   architecture/index.rst
