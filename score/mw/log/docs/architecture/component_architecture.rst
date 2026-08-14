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


Component Architecture Documentation
====================================

.. document:: Log Frontend Architecture
   :id: doc__log_architecture
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch

Overview
--------

<Brief summary of the architecture.>

Requirements Linked to Component Architecture
---------------------------------------------

see "fulfills" links of architecture needs

Description
-----------

<General Description>

<Design Decisions - For the documentation of the decision the :need:`gd_temp__change_decision_record` can be used.>

<Design Constraints>

Rationale Behind Architecture Decomposition
*******************************************

due to low complexity, no need for further decomposition

Static Architecture
-------------------

The components are designed to cover the expectations from the feature architecture
(i.e. if already exists a definition it should be taken over and enriched).

A component can optional also consist of lower level components to further structure the architecture. The component and its static views can also optionally use interfaces provided by other components.

.. comp:: Log Frontend
   :id: comp__log
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :belongs_to: feat__logging

.. comp_arc_sta:: Log Frontend (Static View)
   :id: comp_arc_sta__log__static_view
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :belongs_to: comp__log
   :fulfils: comp_req__log__compat_languages

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Dynamic Architecture
--------------------

not needed, just routing of commands and answers

Interfaces
----------

.. code-block:: rst

   .. real_arc_int:: <Title>
      :id: real_arc_int__<component>__<Title>
      :security: <YES|NO>
      :safety: <QM|ASIL_B>
      :fulfils: <link to component requirement id>
      :language: cpp
