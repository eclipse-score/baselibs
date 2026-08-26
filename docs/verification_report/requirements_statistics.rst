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

.. _requirements_statistics:

Component Requirements Statistics
=================================

This page gives an overview of the component requirements (``comp_req``) defined for
baselibs: how many are valid, how many are covered by tests, and the current test
results. Use the tables below to drill down into individual requirements.

.. raw:: html

   <style>
     /* Smaller font keeps the wide requirement tables below readable without
        excessive scrolling. */
     table.compact-needtable {
       font-size: 0.75em;
     }
     /* needpie renders a fixed 768x384px SVG; keep it from overflowing on
        narrow viewports (e.g. mobile) instead of causing horizontal scroll. */
     img[id^="needpie-"] {
       max-width: 100%;
       height: auto;
     }
     /* The "Tests" column lists every test linked to a requirement as a single
        "; "-separated inline string. In a narrow column that renders as an
        unreadable wall of text. Render each linked test on its own line
        instead, like a plain list, and drop the "; " separators. */
     table.compact-needtable td.needs_testlink p {
       margin: 0;
     }
     table.compact-needtable td.needs_testlink a {
       display: block;
       padding: 0.15em 0;
       word-break: break-word;
     }
     table.compact-needtable td.needs_testlink a:not(:last-child) {
       border-bottom: 1px solid #eee;
     }
     table.compact-needtable td.needs_testlink em {
       display: none;
     }
   </style>

Overview
--------

.. needpie:: Requirements Status
   :labels: invalid, valid and not tested, valid and tested
   :colors: red,yellow, green

   type == 'comp_req' and status == 'invalid'
   type == 'comp_req' and testlink == '' and (status == 'valid' or status == 'invalid')
   type == 'comp_req' and testlink != '' and (status == 'valid' or status == 'invalid')

.. needpie:: Test Results
   :labels: passed, failed, skipped
   :colors: green, red, orange

   type == 'testcase' and result == 'passed'
   type == 'testcase' and result == 'failed'
   type == 'testcase' and result == 'skipped'

Requirements Without Tests
--------------------------

Valid requirements that are not yet covered by any test.

.. needtable:: REQUIREMENTS WITHOUT TESTS
   :filter: type == "comp_req" and status == "valid" and testlink == ""
   :columns: id;title;satisfied_by as "Satisfied by";safety as "Safety"
   :class: compact-needtable

Requirements With Tests
-----------------------

Valid requirements that are covered by at least one test, along with the tests
verifying them.

.. needtable:: REQUIREMENTS WITH TESTS
   :filter: type == "comp_req" and status == "valid" and testlink != ""
   :columns: id;title;satisfied_by as "Satisfied by";safety as "Safety";testlink as "Tests"
   :colwidths: 10,20,10,8,52
   :class: compact-needtable
