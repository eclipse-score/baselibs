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

.. comp_req:: Environment variable setting
   :id: comp_req__os__env_set
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__os_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]
   :tags: inspected

   The OS component shall provide functions for setting environment variable.

.. comp_req:: Environment variable unsetting
   :id: comp_req__os__env_unset
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__os_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]
   :tags: inspected

   The OS component shall provide functions for unsetting environment variable.

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: baselibs, os
