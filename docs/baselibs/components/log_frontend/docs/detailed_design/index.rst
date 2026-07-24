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

.. _component_detailed_design_log:

Detailed Design
###############

.. document:: Log Detailed Design
   :id: doc__log_detailed_design
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__sw_implementation
   :tags: log


Detailed Design for Component: Log
==================================

Description
-----------

Log component consists of three units:

- `score_log` - modelled after `log` Rust library.
- `score_log_fmt` - replacement for `core::fmt` provided by Rust core library.
- `score_log_fmt_macro` - replacement for macros provided by Rust compiler:
  - `score_log_format_args!` - replacement for `format_args!`
  - `ScoreDebug` - replacement for `Debug`

Most common approach in Rust is that formatting always results in a string.
This means that the `log` library always receives a pre-formatted string.

Such approach is incompatible with the expectation that log sink is not always text-based.
Log component design is no longer string-based, and data frames can consist of multiple types.
Value is passed along with formatting options to the backend.

Rationale Behind Decomposition into Units
******************************************

All units provide an interface or an implementation to a well defined functionality.
Those units are not described in architecture, as they all form a monolithic logging frontend.

Such frontend shall remain transparent replacement to common functionalities provided by Rust.

Static Diagrams for Unit Interactions
-------------------------------------

.. comp_arc_sta:: Log class diagram
   :id: comp_arc_sta__log__class_diagram
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__log__placeholder
   :belongs_to: comp__logging

   .. uml:: _assets/class_diagram.puml

Dynamic Diagrams for Unit Interactions
--------------------------------------

.. comp_arc_dyn:: Log operation
   :id: comp_arc_dyn__log__log_op
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__log__placeholder
   :belongs_to: comp__logging

   .. uml:: _assets/log_op.puml

.. comp_arc_dyn:: Log to level
   :id: comp_arc_dyn__log__log_to_level
   :security: NO
   :safety: ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__log__placeholder
   :belongs_to: comp__logging

   .. uml:: _assets/log_to_level.puml
