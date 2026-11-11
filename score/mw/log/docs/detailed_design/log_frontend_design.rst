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

.. _component_detailed_design_log_frontend:

Log Frontend Detailed Design
============================

Description
-----------

The C++ frontend of ``mw::log`` provides the logging API (``Logger``,
``LogStream`` and the ``Log*()`` free functions), the configuration handling
and the recorder creation. The backends implement the ``Recorder`` interface and
register themselves in the backend table. This page links the diagrams of the
C++ design to the component requirements. The explanation of the diagrams is in
the pages below.

- :doc:`cpp_design/README` - overview of the units and the logging flow.
- :doc:`cpp_design/configuration_design` - configuration files and the recorder
  creation.
- :doc:`cpp_design/registry_aware_recorder_factory` - backend registration.
- :doc:`cpp_design/dependency_graph` - dependencies between frontend and backends.
- :doc:`cpp_design/dynamic_backend_loading_design` - dynamic loading of backends.
- :doc:`cpp_design/design_decisions` - design decisions.

Rationale Behind Decomposition into Units
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The frontend is separated from the implementation details of the backends. The
frontend depends only on the ``Recorder`` and ``IRecorderFactory`` interfaces,
so that a backend can be added or removed at link time without changing the
frontend. The units are the folders and files of ``score/mw/log``, for example
``configuration``, ``detail`` and ``detail/text_recorder``.

Static Diagrams for Unit Interactions
-------------------------------------

.. comp_arc_sta:: Log C++ configuration class diagram
   :id: comp_arc_sta__log__cpp_configuration
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__cfg_file[version==1], comp_req__log__cfg_file_loc[version==1], comp_req__log__global_config[version==1], comp_req__log__cfg_defaults[version==1], comp_req__log__cfg_precedence[version==1], comp_req__log__cfg_syntax_err[version==1], comp_req__log__cfg_invalid[version==1], comp_req__log__cfg_log_ecu_id[version==1], comp_req__log__cfg_log_app_id[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/configuration_static.puml

.. comp_arc_sta:: Log C++ recorders class diagram
   :id: comp_arc_sta__log__cpp_recorders
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__console_backend[version==1], comp_req__log__system_logger[version==1], comp_req__log__cfg_sink_device[version==1], comp_req__log__cfg_buffer_size[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/mw_log_default_recorders.puml

.. comp_arc_sta:: Log C++ console logging class diagram
   :id: comp_arc_sta__log__cpp_console_logging
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__console_backend[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/verbose_console_logging_static.puml

.. comp_arc_sta:: Log C++ frontend dependency graph
   :id: comp_arc_sta__log__cpp_frontend_dependencies
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__console_backend[version==1], comp_req__log__system_logger[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/frontend_dependency_graph.puml

.. comp_arc_sta:: Log C++ backend registration component diagram
   :id: comp_arc_sta__log__cpp_backend_registration
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__console_backend[version==1], comp_req__log__system_logger[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/backend_registration_component_diagram.puml

Dynamic Diagrams for Unit Interactions
--------------------------------------

.. comp_arc_dyn:: Log C++ configuration sequence
   :id: comp_arc_dyn__log__cpp_configuration
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__cfg_file[version==1], comp_req__log__cfg_file_loc[version==1], comp_req__log__global_config[version==1], comp_req__log__cfg_defaults[version==1], comp_req__log__cfg_precedence[version==1], comp_req__log__cfg_syntax_err[version==1], comp_req__log__cfg_invalid[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/configuration_sequence.puml

.. comp_arc_dyn:: Log C++ backend registration and recorder creation
   :id: comp_arc_dyn__log__cpp_backend_registration
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__console_backend[version==1], comp_req__log__system_logger[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/backend_registration_sequence_diagram.puml

.. comp_arc_dyn:: Log C++ recorder factory decision flow
   :id: comp_arc_dyn__log__cpp_recorder_factory
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__console_backend[version==1], comp_req__log__system_logger[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/rarf_activity_diagram.puml

.. comp_arc_dyn:: Log C++ slot allocation
   :id: comp_arc_dyn__log__cpp_slot_allocation
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__local_allocation_strategy2[version==1], comp_req__log__cfg_buffer_size[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/circular_buffer_allocator_acquireslottowrite.puml

.. comp_arc_dyn:: Log C++ slot drainer sequence
   :id: comp_arc_dyn__log__cpp_slot_drainer_sequence
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__local_allocation_strategy2[version==1], comp_req__log__console_backend[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/slot_drainer_sequence_design.puml

.. comp_arc_dyn:: Log C++ slot drainer flush
   :id: comp_arc_dyn__log__cpp_slot_drainer_flush
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__local_allocation_strategy2[version==1], comp_req__log__console_backend[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/slot_drainer_action_diagram_design.puml

.. comp_arc_dyn:: Log C++ wait-free stack push
   :id: comp_arc_dyn__log__cpp_wait_free_stack
   :version: 1
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__local_allocation_strategy2[version==1]
   :belongs_to: comp__log[version==1]

   .. uml:: _assets/wait_free_stack_trypush.puml

.. needextend:: c.this_doc() and type != ""
   :+tags: logging
