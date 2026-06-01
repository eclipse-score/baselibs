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

Test Traceability Report
========================

This report shows test case needs parsed from Bazel JUnit XML files (test.xml)
by the SCORE docs-as-code toolchain.

All Parsed Test Cases
---------------------

.. needtable::
   :types: testcase

Unlinked Test Cases
-------------------

.. needtable::
   :types: testcase
   :filter: not partially_verifies and not fully_verifies

JUnit XML Upload
-----------------

This workflow now uploads JUnit XML test reports as artifacts for traceability analysis.
