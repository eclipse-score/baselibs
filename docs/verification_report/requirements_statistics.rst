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
   :class: compact-needtable
