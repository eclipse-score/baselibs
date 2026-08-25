..
    ******************************************************************************
    Copyright (c) 2026 Contributors to the Eclipse Foundation

    See the NOTICE file(s) distributed with this work for additional
    information regarding copyright ownership.

    This program and the accompanying materials are made available under the
    terms of the Apache License Version 2.0 which is available at
    https://www.apache.org/licenses/LICENSE-2.0

    SPDX-License-Identifier: Apache-2.0
    ******************************************************************************

Module Verification Report
==========================

.. document:: Module Verification Report
   :id: doc__baselibs_module_verification_report
   :status: valid
   :safety: QM
   :security: YES
   :version: 1
   :realizes: wp__feature_arch[version==1]

This document provides a verification report of the Baselibs feature and its
associated components.

.. module-verification-report::
   :module-id: mod__baselibs
   :feature-id: feat__baselibs
   :safety: ASIL_B
   :security: YES
   :status: valid
   :verification-method: test_and_inspection
   :components: comp__baselibs_json,
               comp__baselibs_memory_shared,
               comp__baselibs_result,
               comp__baselibs_bit_manipulation,
               comp__baselibs_containers,
               comp__baselibs_filesystem,
               comp__baselibs_concurrency,
               comp__baselibs_safecpp,
               comp__baselibs_static_reflection,
               comp__baselibs_hash,
               comp__baselibs_flatbuffers,
               comp__baselibs_abi_compatible_data_types
