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

.. comp_req:: Scoped Execution
   :id: comp_req__scope_exit__scoped_execution
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :tags: inspected
   :satisfied_by: comp__baselibs_scope_exit[version==1]

   The Scope Exit component shall provide a scope-bound callable wrapper that invokes its stored callback exactly once when the wrapper is destroyed, unless the wrapper has been released or moved from, and that transfers callback ownership on move construction and move assignment.
