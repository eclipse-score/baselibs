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

.. document:: log_frontend Implementation Inspection Checklist
  :id: doc__log_impl_inspection
  :status: draft
  :version: 1
  :safety: ASIL_B
  :security: YES
  :realizes: wp__sw_implementation_inspection

Implementation Inspection Checklist
===================================

Purpose
-------

The purpose of this checklist is to collect the topics to be checked during implementation,
i.e. in the detailed design and the source code of the units.

The checklist shall be agnostic to which programming language is used. Differences shall be treated
by linking to C++ or Rust specific documentation.

Conduct
-------

As described in the concept :need:`doc_concept__wp_inspections` the following "inspection roles" are expected to be filled:

- content responsible (author): <contributor/committer explicitly named here, who is the main author, as can be seen in config mgt tooling>
- reviewer: <contributor/committer explicitly named here, who is the main content reviewer, must be different from content responsible>
- moderator: <committer explicitly named here, who is is the safety manager, security manager or quality manager initiating the inspection>

Checklist
---------

It is mandatory to fill in the "passed" column with "yes" or "no" for each checklist item and additionally to add in the remarks why it is passed or not passed.
In case of "no" an issue link to the issue tracking system has to be added in the last column (if not solved in the same issue).
See also :need:`doc_concept__wp_inspections` for further information about reviews in general and inspection in particular.

.. list-table:: Implementation Checklist
   :header-rows: 1
   :widths: 10,25,30,6,80,20

   * - Review ID
     - Acceptance Criteria
     - Guidance
     - Passed
     - Remarks
     - Issue link
   * - IMPL_01_01
     - Is the design according to guidelines?
     - see :need:`gd_temp__detailed_design` and :need:`doc_concept__imp_concept`
       (e.g. are the views done with the proposed UML diagrams)
     - yes
     - Checked: the detailed design consists of :doc:`rust_bridge_design` for
       the Rust bridge and :doc:`log_frontend_design` for the C++ frontend,
       which follow the detailed design template (description, rationale behind
       the decomposition, static and dynamic diagrams). The diagrams are UML
       diagrams in PlantUML, linked to the component requirements and the
       component by ``comp_arc_sta`` and ``comp_arc_dyn`` needs. The
       explanations of the C++ design are in ``cpp_design`` and the diagrams in
       ``_assets``. They were moved from ``score/mw/log/design``, as the bundle
       ``//score/mw/log:docs`` rejects files and symbolic links to files
       outside ``score/mw/log/docs`` (``mounts.path_escape``).
     -
   * - IMPL_01_02
     - Is the implementation according to specification?
     - Check if the linked component requirements are fulfilled
       and detailed design also matches architecture description.
     - no
     - Checked: 72 existing test cases are linked to 14 of the 18 component
       requirements. The requirement-to-test traceability is shown in
       :doc:`/verification_report/requirements_statistics`.

       Following issues need to be addressed:

       - :need:`comp_req__log__cfg_file_loc` is not fulfilled.
         ``ConfigurationFileDiscoverer`` searches
         ``<binary path>/../etc/logging.json`` before ``<cwd>/etc/logging.json``
         and ``<cwd>/logging.json``, the requirement places it last.
       - :need:`comp_req__log__cfg_log_ecu_id` and
         :need:`comp_req__log__cfg_log_app_id` are not fulfilled.
         ``TargetConfigReader`` accepts any string as ECU and application ID
         and does not check for 4 uppercase ASCII characters.

       Deviations:

       - :need:`comp_req__log__system_logger` is QNX only, the slog backend is
         not part of the Linux unit tests. ``TextRecorder`` logging of a
         ``LogSlog2Message`` is the only linked test.
       - :need:`comp_req__log__compat_os` is verified by building for QNX and
         Linux in CI, :need:`comp_req__log__compat_languages` by the C++ tests
         and the Rust tests.
       - The Rust tests have no requirement links. docs-as-code provides no
         way to link Rust tests to requirements (see ``test_to_doc_links.rst``
         in ``score_docs_as_code``), and the other components have no links in
         their Rust tests either.
     - To be added
   * - IMPL_01_03
     - Are the design decisions and constraints documented?
     - Check also for plausibility of these.
     - yes
     - Design decisions in ``cpp_design/README.md`` and
       ``cpp_design/design_decisions.md`` were checked against the code. The
       constraints of the units (slot count, slot size, slots per drain cycle,
       queued slots) are expressed by the expectations of the unit tests.
     -
   * - IMPL_01_04
     - Are all external libraries used by the component specified in the detailed design?
     - Check the automated dependency analysis.
       Also make sure ASIL rated units also only use ASIL rated libraries.
     - yes
     - Dependency and FFI metadata are maintained in Bazel. The external
       libraries are listed in :doc:`external_libraries`, generated with::

         bazel query "kind('.*_library|rust_proc_macro',
           deps(//score/mw/log:log))" --output=label_kind

       Deviations:

       - ``//score/language/futurecpp:futurecpp`` and
         ``//score/language/futurecpp:math`` are third-party code without
         ``FFI`` tag and stated rating. ``score/language/futurecpp`` is a
         restricted path, which is changed only through the internal codebase. The crates ``syn``, ``quote`` and
         ``proc_macro2`` are third-party build-time dependencies of
         ``score_log_fmt_macro`` from ``@score_crates`` without ``FFI`` tag.
       - The Rust crates ``score_log``, ``score_log_fmt``, ``stdout_logger``
         and ``stdout_logger_cpp_init`` have no ``FFI`` tag. The Rust crates
         are classified by the ``safety_level`` flag in
         ``//score/log_rust`` instead, and ``stdout_logger`` is a testing and
         development backend.
       - The ``high_resolution_steady_clock`` target is deprecated and its
         replacement is not available in the open source dependencies.
     -
   * - IMPL_02_01
     - Are the static and dynamic code analysis reports verified for violations?
     - All violations in ASIL related code must be justified. This includes the checks of coding guidelines.
     - yes
     - Refer to the generated :doc:`verification report
       </verification_report/module_verification_report>` and revision-specific
       CI results.

       Deviations:

       - Static code analysis with MISRA C++:2023 checks is planned:
         `#262 <https://github.com/eclipse-score/logging/issues/262>`_
       - Dynamic code analysis using valgrind is planned:
         `#389 <https://github.com/eclipse-score/logging/issues/389>`_
       - Coverage gap: ``stdout_logger`` is tested only for its timestamp
         code, the logger itself (``lib.rs``) has no tests.
         ``stdout_logger_cpp_init`` has no tests.

         Clarification: ``stdout_logger`` is a testing and development backend
         (see its ``BUILD`` file), for production use ``score_logging``.
         Resolve the gap with unit tests or justify it by this scope.
     -
   * - IMPL_02_02
     - Do manual checks, that are derived from the coding guideline, find no safety critical error?
     - Check this for the programming language used (e.g. C++ <link_to_checks_list>, Rust <link_to_checks_list>)
     - no
     - Manual checks were done with a code review and against the `C++ guideline
       <https://eclipse-score.github.io/score/main/contribute/development/cpp/coding_guidelines.html>`_.

       Following issues need to be addressed:

       - ``SlotDrainer::current_slot_`` refers to the queue element that is
         removed by ``pop_front()``. A later ``PushBack()`` can overwrite it,
         so the wrong slot is released.
       - ``SlotDrainer`` processes one slot more than
         ``limit_slots_in_one_cycle_`` per drain cycle.
       - ``RegistryAwareRecorderFactory`` creates a second console recorder if
         an unavailable log mode triggers the console fallback before
         ``kConsole`` is processed.
       - The ``CreateFromConfiguration`` overloads behave differently. The
         overload with the config reader creates an ``EmptyRecorder`` for an
         unavailable log mode and if no log mode is configured. The other
         overload falls back to the console in both cases.

       Deviations:

       - The coding guideline asks to avoid the prefix ``I`` for interfaces. The
         ``IRecorderFactory``, ``IMessageBuilder``, ``ITargetConfigReader``,
         ``IConfigurationFileDiscoverer`` and ``INvConfig`` use it.
     - To be added
   * - IMPL_03_01
     - Do the UID of the interface in component documentation match the implemented interface names of the unit?
     - Compare interface UIDs (which contains the interface name) in component architecture/detailed design documentation
       with public interfaces in source code (e.g. API headers, traits, public types/functions).
     - no
     - The component architecture defines no interface, the ``real_arc_int``
       entry is the template placeholder.

       Following issues need to be addressed:

       - Add the interfaces to the component architecture and map them to the
         code in the detailed design.
       - ``logic_arc_int__log_cpp__logging`` is implemented by ``LogFatal()`` to
         ``LogVerbose()``, ``Logger`` and ``LogStream``,
         ``logic_arc_int__log_rust__logging_rust`` by the macros ``log!``,
         ``fatal!`` to ``trace!`` and the trait ``Log``. Both are defined in the
         feature architecture of the ``score`` repository, the names differ from
         the UIDs.
     - To be added
   * - IMPL_03_02
     - Are detailed design and source code consistent and is the respective traceability established ?
     - Check if available static and dynamic design diagrams and the textual descriptions match the code
       (e.g. naming of interfaces, units, functions/operations/messages, data types).
       Check if the folder/file names of the units and its source code matches the intended functionality.
       For example if a unit is named "communication" it should not contain code for "data processing".
     - yes
     - The C++ design in ``cpp_design`` and :doc:`rust_bridge_design` were
       compared with the code and corrected. The folder and file names of the units match their
       function.
     -
