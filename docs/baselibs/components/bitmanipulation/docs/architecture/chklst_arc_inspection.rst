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


.. document:: Bitmanipulation Architecture Inspection Checklist
   :id: doc__bitmanipulation_arc_inspection
   :status: valid
   :safety: ASIL_B
   :security: YES
   :realizes: wp__sw_arch_verification

.. mod_insp:: Bitmanipulation Component Architecture Inspection Record
   :id: mod_insp__bitmanipulation__comp_arc
   :status: valid
   :safety: ASIL_B
   :security: YES
   :inspection_type: architecture
   :inspection_state: approved
   :checklist_template: gd_chklst__arch_inspection_checklist
   :reviewers: aschemmel-tech
   :findings_total: 0
   :findings_open: 0
   :belongs_to: mod__baselibs
   :inspects: feat_arc_sta__baselibs__static_view_arch
   :checklist: doc__bitmanipulation_arc_inspection
   :sha256: c5d79e0571769b1e032434fc1c97c39e2f8edb3d531524b6d00fc494f5b430bf

   Machine-readable inspection record for the bitmanipulation component
   architecture. There is no dedicated component architecture; the inspection is
   covered by the baselibs feature architecture
   (:need:`feat_arc_sta__baselibs__static_view_arch`). Build
   ``//:bitmanipulation_arch_checklist`` to validate that the architecture has
   not changed since the inspection documented in
   :need:`doc__bitmanipulation_arc_inspection`.

Architecture Inspection Checklist
=================================

No need for component architecture inspection, as there is no additional information as in the
feature architecture :need:`doc__baselibs_architecture` and this is to be inspected.

The following static views in "valid" state and with "inspected" tag set are in the scope of this inspection:

.. needtable::
   :filter: docname is not None and "bitmanipulation" in docname and "architecture" in docname and status == "valid"
   :style: table
   :types: comp_arc_sta
   :columns: id;status;tags
   :colwidths: 25,25,25
   :sort: title

and the following dynamic views:

.. needtable::
   :filter: docname is not None and "bitmanipulation" in docname and "architecture" in docname and status == "valid"
   :style: table
   :types: comp_arc_dyn
   :columns: id;status;tags
   :colwidths: 25,25,25
   :sort: title
