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

.. document:: Analysis Tracing Requirements
   :id: doc__analysis_tracing_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, analysis_tracing

Functional Requirements
=======================

.. comp_req:: Canary-Protected Data Integrity
   :id: comp_req__analysis_tracing__canary_integrity
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__com__data_corruption[version==1]
   :status: valid
   :version: 1
   :tags: inspected
   :satisfied_by: comp__baselibs_analysis_tracing[version==1]

   The Analysis Tracing component shall wrap arbitrary data with configurable start and end canary values, detect corruption of either canary during data access, and report corrupted data as unavailable.