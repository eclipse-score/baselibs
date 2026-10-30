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

.. document:: Generic Trace Library Requirements
   :id: doc__generic_trace_library_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, generic_trace_library, tracing

Public API
==========

.. comp_req:: Use of Feature flag
   :id: comp_req__trace__feature_flag
   :reqtype: Interface
   :security: NO
   :safety: ASIL_B
   :derived_from: feat_req__tracing__support[version==1],
   :satisfied_by: comp__baselibs_generic_trace_library[version==1]
   :status: valid
   :version: 1

   The "implementation" label_flag shall be used to link the concrete tracing implementation with its Public API's.



Assumptions of Use (AoU)
========================

.. aou_req:: Read-Only Shared Memory Objects
   :id: aou_req__trace__shm_readonly
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1

   Shared-memory objects handed to the trace API must be read-only.

.. aou_req:: Trace API Return Values Not for ASIL B Decisions
   :id: aou_req__trace__no_asil_b_decision
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1

   Callers must not use trace API return values to drive any ASIL B logic or decision.

.. aou_req:: Trace Done Callback Thread Safety
   :id: aou_req__trace__cb_thread_safety
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1

   Callbacks registered with the trace API shall be thread-safe and lightweight.

.. aou_req:: Client Registration as First API Call
   :id: aou_req__trace__client_reg_first
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1

   The client registration API shall be treated as the first entry point to be called by any trace client.

.. aou_req:: Caller Responsible for Data Contiguity
   :id: aou_req__trace__data_contiguity
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1

   It is the responsibility of the trace client to maintain data contiguity.

.. aou_req:: No Trace API Usage During Static Initialization / Destruction
   :id: aou_req__trace__no_static_init_use
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1

   Safety-related platform software and safety-related application software shall not use the trace API during
   C++ static storage initialization and destruction.

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: baselibs, generic_trace_library, tracing
