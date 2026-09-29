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

Requirements
############

.. document:: OSAL Requirements
   :id: doc__osal_lib_requirements
   :status: valid
   :version: 2
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, os_library

Functional Requirements
=======================

.. comp_req:: ASIL-B OS Operation Delegation
   :id: comp_req__osal__asil_b_operation_delegation
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__osal[version==1]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_osal[version==2]

   For each function allocated to ASIL-B, the OSAL component shall invoke the corresponding operating system or C standard library service with argument values equivalent to those supplied by the caller.
   It shall return the operation's result and output values and indicate success or failure.

.. comp_req:: QM OS Operation Delegation
   :id: comp_req__osal__qm_operation_delegation
   :reqtype: Functional
   :security: YES
   :safety: QM
   :derived_from: feat_req__baselibs__osal[version==1]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_osal_qm[version==1]

   For each function allocated to QM, the OSAL component shall invoke the corresponding operating system or C standard library service with argument values equivalent to those supplied by the caller.
   It shall return the operation's result and output values and indicate success or failure.

.. comp_req:: OS Error Information
   :id: comp_req__osal__error_information
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__osal[version==1]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_osal[version==2]

   For each OSAL function with an error-reporting interface, the OSAL component shall return the error code reported by a failed service invocation as part of the function's failure result.

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: baselibs, os
