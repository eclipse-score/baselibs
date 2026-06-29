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

Module Verification Report
==========================

.. document:: Baselibs Module Verification Report
   :id: doc__baselibs_module_ver_report
   :status: valid
   :safety: ASIL_B
   :security: YES
   :realizes: wp__verification_module_ver_report

.. mod_ver_report:: Baselibs Module Verification Report
   :id: mod_vrep__baselibs__module
   :status: valid
   :safety: ASIL_B
   :security: YES
   :verification_method: Test, Analysis, Inspection
   :requirements_coverage_percent: 100
   :structural_coverage_percent: 92
   :branch_coverage_percent: 60
   :verdict: open
   :belongs_to: mod__baselibs
   :realizes: wp__verification_module_ver_report
   :sha256:

   Module Verification Report and SW Component Qualification Verification Report
   for the contained pre-existing components of the ``baselibs`` module. The data
   is based on the S-CORE v1 reference integration overall status and covers the
   components bitmanipulation, concurrency, containers, filesystem, json, result,
   safecpp, static_reflection_with_serialization and utils.

   Requirements verification: component requirements 84/84 and Assumptions of Use
   32/32 complete (Test/Analysis), architecture verification 165/167 (98%, in
   progress), ABI compatible data types 37/37 complete. Structural coverage from
   host unit testing: C/C++ C0 92.3% / C1 60.3%, Rust line coverage 74.35%.
   Static code analysis and coding guideline checks: 0 findings. Test execution:
   5376 unit/module tests and 13 component integration tests passed.

   Formal evidence about the performed Dependent Failure Analysis and FMEA is
   provided by :need:`doc__baselibs_dfa` and :need:`doc__baselibs_fmea`.

   Verdict: module verification in progress - requirements, AoU and unit tests
   complete; architecture verification at 98% and component integration testing
   pending.
