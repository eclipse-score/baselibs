..
   # *******************************************************************************
   # Copyright (c) 2026 Contributors to the Eclipse Foundation
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

Requirements
############

.. document:: Scope Exit Requirements
   :id: doc__scope_exit_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, scope_exit

Functional Requirements
=======================

.. comp_req:: Scope-Based Cleanup Ownership
   :id: comp_req__scope_exit__cleanup_ownership
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :tags: inspected
   :satisfied_by: comp__baselibs_scope_exit[version==1]

   The Scope Exit component shall invoke an owned cleanup callback at scope exit at most once, and shall transfer or release callback ownership according to move construction, move assignment, and explicit release operations.

.. comp_req:: Move-Only Flag Ownership
   :id: comp_req__scope_exit__flag_ownership
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :tags: inspected
   :satisfied_by: comp__baselibs_scope_exit[version==1]

   The Scope Exit component shall provide a move-only flag owner that transfers its flag value to the destination and clears the source during move operations, while preserving the value during self-move assignment.