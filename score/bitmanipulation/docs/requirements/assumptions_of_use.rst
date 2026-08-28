..
   # *******************************************************************************
   # Copyright (c) 2025-2026 Contributors to the Eclipse Foundation
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

Assumptions of Use (AoU)
========================

.. aou_req:: Integral Type Constraints
   :id: aou_req__bitmanipulation__type_constraints
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: inspected

   The user shall use bit manipulation functions only with integral types (integers, enumerations) as specified in the library's type constraints.

   Note: Operations on floating-point or non-integral types are not supported.

.. aou_req:: Bitmask Enum Value Constraints
   :id: aou_req__bitmanipulation__enum_constraints
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: inspected

   The user shall use scoped enumeration types (enum class) whose enumerators are defined as non-zero power-of-two values.

.. aou_req:: External Synchronization Required for Concurrent Access
   :id: aou_req__bitmanipulation__concurrent_access
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: inspected

   The user shall implement external synchronization mechanisms (e.g., mutexes, atomic operations, or locks) when accessing or modifying the same integral value from multiple threads concurrently.

   Note: The library provides no internal thread safety guarantees.

.. needextend:: c.this_doc() and (type == "aou_req")
   :+tags: baselibs, bitmanipulation
