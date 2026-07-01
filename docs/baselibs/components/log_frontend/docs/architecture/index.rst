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

.. _component_architecture_log:

Component Architecture
======================

.. document:: Log Architecture
   :id: doc__log_architecture
   :status: draft
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch
   :tags: log


Overview
--------

Document describes Log component architecture.


Description
-----------

Log component is modelled after the `log` library, which is ubiquitous in Rust ecosystem.
This provides familiar APIs and syntax - provided APIs can be replaced at compile time with `log`.

Component provides new formatting functionality (replacement to `core::fmt`) to ensure improved flexibility in formatting on backend side.
E.g., numeric types are formatted by the backend, and not by the core library.

Even though design is similar - existing `log` implementations are not compatible.


Rationale Behind Architecture Decomposition
*******************************************

Architecture is not decomposed.
Log component is a monolithic frontend.


Static Architecture
-------------------

.. comp_arc_sta:: Log (Static View)
   :id: comp_arc_sta__log__static_view
   :security: NO
   :safety: ASIL_B
   :status: valid
   :belongs_to: comp__logging
   :fulfils: comp_req__log__placeholder
   :includes:

   .. uml:: _assets/static_view.puml


Dynamic Architecture
--------------------

.. comp_arc_dyn:: Register global logger
   :id: comp_arc_dyn__log__register_global_logger
   :security: NO
   :safety: ASIL_B
   :status: valid
   :belongs_to: comp__logging
   :fulfils: comp_req__log__placeholder

   .. uml:: _assets/register_global_logger.puml

.. comp_arc_dyn:: Log with global logger instance
   :id: comp_arc_dyn__log__log_with_global_logger
   :security: NO
   :safety: ASIL_B
   :status: valid
   :belongs_to: comp__logging
   :fulfils: comp_req__log__placeholder

   .. uml:: _assets/log_with_global_logger.puml

.. comp_arc_dyn:: Log with local logger instance
   :id: comp_arc_dyn__log__log_with_local_logger
   :security: NO
   :safety: ASIL_B
   :status: valid
   :belongs_to: comp__logging
   :fulfils: comp_req__log__placeholder

   .. uml:: _assets/log_with_local_logger.puml


Interfaces
----------

.. real_arc_int:: Log interface
   :id: real_arc_int__log__interface
   :security: NO
   :safety: ASIL_B
   :status: valid
   :fulfils: comp_req__log__placeholder
   :language: rust

   .. uml:: _assets/interface.puml
