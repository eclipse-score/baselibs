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
     - no
     - The detailed design consists of :doc:`rust_bridge_design` and the stub
       :doc:`log_frontend_design`. The C++ design is in ``score/mw/log/design``
       and is not part of the documentation build, the bundle
       ``//score/mw/log:docs`` rejects files outside ``score/mw/log/docs``
       (``mounts.path_escape``).

       Following issues need to be addressed:

       - Move the C++ design into the documentation and restructure it after the
         template.
       - Add an ``architectural_design`` target that validates the diagrams.
       - :doc:`rust_bridge_design` uses the architecture need types
         ``comp_arc_sta`` and ``comp_arc_dyn``. ``log_to_level.puml`` refers to
         ``dd_dyn__log__log_op``, which does not exist.
     - Design location: `#187 <https://github.com/eclipse-score/logging/issues/187>`_.
       Other points: To be added.
   * - IMPL_01_02
     - Is the implementation according to specification?
     - Check if the linked component requirements are fulfilled
       and detailed design also matches architecture description.
     - no
     - 29 existing tests were linked to 13 of the 18 component requirements. The
       requirement-to-test traceability is shown in
       :doc:`/verification_report/requirements_statistics`.

       Following issues need to be addressed:

       - No test for :need:`comp_req__log__log_loss_detect`,
         :need:`comp_req__log__timestamp_local`,
         :need:`comp_req__log__console_backend` and
         :need:`comp_req__log__system_logger`. :need:`comp_req__log__compat_os`
         and :need:`comp_req__log__compat_languages` need another verification
         method.
       - :need:`comp_req__log__cfg_file_loc` defines another search order than
         ``configuration/configuration_file_discoverer.cpp``.
       - :need:`comp_req__log__cfg_log_ecu_id` and
         :need:`comp_req__log__cfg_log_app_id` require uppercase, the code does
         not check it.
       - The Rust tests have no requirement links.
     - To be added
   * - IMPL_01_03
     - Are the design decisions and constraints documented?
     - Check also for plausibility of these.
     - no
     - Design decisions in ``design/README.md`` and
       ``design/design_decisions.md`` were checked against the code.

       Following issues need to be addressed:

       - ``design/dependency_graph.md`` states that the frontend does not depend
         on the details, it uses ``detail:empty_recorder``,
         ``detail:thread_local_guard``, ``detail:logging_identifier`` and
         ``detail/wait_free_stack``.
       - The constraints of the units (slot count, slot size, slots per drain
         cycle, queued slots) are not documented.
     - To be added
   * - IMPL_01_04
     - Are all external libraries used by the component specified in the detailed design?
     - Check the automated dependency analysis.
       Also make sure ASIL rated units also only use ASIL rated libraries.
     - no
     - Dependency and FFI metadata are maintained in Bazel. The direct
       dependencies outside of the component are listed with::

         bazel query "let l = kind('.*_library|rust_proc_macro',
           //score/mw/log/... + //score/log_rust/...)
           except attr(testonly, 1, //score/mw/log/... + //score/log_rust/...)
           in labels(deps, \$l) + labels(implementation_deps, \$l)
           + labels(proc_macro_deps, \$l)
           except (//score/mw/log/... + //score/log_rust/...)" \
           --output=label_kind

       Following issues need to be addressed:

       - ``//score/json``, ``//score/language/futurecpp`` and the crates
         ``syn``, ``quote`` and ``proc_macro2`` have no ``FFI`` tag and no
         stated rating.
       - The crates ``score_log``, ``score_log_fmt``, ``stdout_logger`` and
         ``stdout_logger_cpp_init`` have no ``FFI`` tag.
       - The external libraries are not listed in the detailed design.
       - The ``high_resolution_steady_clock`` target is deprecated and its
         replacement is not available in the open source dependencies.
     - To be added
   * - IMPL_02_01
     - Are the static and dynamic code analysis reports verified for violations?
     - All violations in ASIL related code must be justified. This includes the checks of coding guidelines.
     - no
     - Refer to the generated :doc:`verification report
       </verification_report/module_verification_report>` and revision-specific
       CI results.

       Following issues need to be addressed:

       - Resolve or justify coverage gaps, ``stdout_logger`` and
         ``stdout_logger_cpp_init`` are mostly not tested.
       - 69 of 232 suppressions have no justification and use AUTOSAR C++14 ids.
       - Memcheck (valgrind 3.22.0) is not part of CI. No errors and no definite
         leaks in the unit tests found in manual runs.
     - To be added
   * - IMPL_02_02
     - Do manual checks, that are derived from the coding guideline, find no safety critical error?
     - Check this for the programming language used (e.g. C++ <link_to_checks_list>, Rust <link_to_checks_list>)
     - no
     - The MISRA C++:2023 and Safety-Critical Rust checks are not done

       Following issues found in the code review need to be addressed:

       - ``SlotDrainer`` keeps a reference to a removed element in
         ``current_slot_`` (``detail/text_recorder/slot_drainer.cpp``).
       - ``RegistryAwareRecorderFactory`` can add the console recorder twice for
         ``kRemote|kConsole``.
       - The two ``CreateFromConfiguration`` overloads behave differently, the
         tests use the overload not used in production.
       - ``SlotDrainer`` processes 33 slots per cycle with a limit of 32.

       Following issues found in `C++ guideline
       <https://eclipse-score.github.io/score/main/contribute/development/cpp/coding_guidelines.html>`_:

       - The coding guideline asks to avoid the prefix ``I`` for interfaces. The
         ``IRecorderFactory``, ``IMessageBuilder``, ``ITargetConfigReader``,
         ``IConfigurationFileDiscoverer`` and ``INvConfig``.
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
     - no
     - The C++ design in ``score/mw/log/design`` and :doc:`rust_bridge_design`
       were compared with the code. The folder and file names of the units match
       their function.

       Following issues need to be addressed when the design is moved into the
       documentation (see IMPL_01_01):

       - Classes that do not exist (``RecorderFactory``, ``ConsoleRecorder``,
         ``RecorderComposite``, namespace ``bmw::``).
       - Sequences that differ from the code (recorder creation, fallback
         recorder, console output path).
       - The configuration design uses outdated names, a wrong ``class-id.json``
         path and a wrong discovery order.
       - The slot drainer diagrams miss the mutex, the slot allocation shows one
         atomic operation as two steps.
       - :doc:`rust_bridge_design` lists 3 of 5 crates, ``class_diagram.puml``
         shows the crate ``score_log_subscriber`` that does not exist.
     - Design location: `#187 <https://github.com/eclipse-score/logging/issues/187>`_.
