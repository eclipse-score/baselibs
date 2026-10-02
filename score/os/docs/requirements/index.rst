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

.. document:: OS Library Requirements
   :id: doc__os_lib_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, os_library

Functional Requirements
=======================

.. comp_req:: ASIL-B OS Operation Delegation
   :id: comp_req__os__asil_b_operation_delegation
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__os_library[version==1]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==2]

   For each operation declared by an ASIL-B-classified public wrapper interface, the OS component shall invoke the
   corresponding operating system operation with argument values equivalent to those supplied by the caller and shall
   provide its successful return value and output-parameter values through the declared C++ API.

.. comp_req:: QM OS Operation Delegation
   :id: comp_req__os__qm_operation_delegation
   :reqtype: Functional
   :security: YES
   :safety: QM
   :derived_from: feat_req__baselibs__os_library[version==1]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==2]

   For each operation declared by a QM-classified public wrapper interface, the OS component shall invoke the
   corresponding operating system operation with argument values equivalent to those supplied by the caller and shall
   provide its successful return value and output-parameter values through the declared C++ API.

.. comp_req:: OS Error Information
   :id: comp_req__os__error_information
   :reqtype: Interface
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__os_library[version==1]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==2]

   When an operating system operation exposed by an ASIL-B-classified wrapper fails and the wrapper declares an OS
   error result, the OS component shall make the operating system error code reported for the failed operation
   available to the caller.

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: baselibs, os
