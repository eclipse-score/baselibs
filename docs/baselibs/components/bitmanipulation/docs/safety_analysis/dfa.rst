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


DFA (Dependent Failure Analysis)
================================

.. document:: bitmanipulation DFA
   :id: doc__bitmanipulation_dfa
   :status: draft
   :safety: ASIL_B
   :security: YES
   :realizes: wp__sw_component_dfa

.. mod_insp:: Bitmanipulation Component DFA Inspection Record
   :id: mod_insp__bitmanipulation__comp_dfa
   :status: valid
   :safety: ASIL_B
   :security: YES
   :inspection_type: safety_analysis
   :inspection_state: rework_required
   :checklist_template: gd_chklst__safety_analysis
   :reviewers: aschemmel-tech
   :belongs_to: mod__baselibs
   :inspects: doc__bitmanipulation_dfa
   :checklist: doc__bitmanipulation_dfa
   :sha256: c5d79e0571769b1e032434fc1c97c39e2f8edb3d531524b6d00fc494f5b430bf

   Machine-readable inspection record for the bitmanipulation component DFA. It
   validates the extracted DFA elements against the reviewed safety analysis.
   The build fails and reports the SHA256 to pin once the analysis has been
   reviewed.

.. note:: Use the content of the document to describe e.g. why a fault model is not applicable for the diagram.


Dependent Failure Initiators
----------------------------

.. code-block:: rst

    .. comp_saf_dfa:: <Title>
       :violates: <Component architecture>
       :id: comp_saf_dfa__<Component>__<Element descriptor>
       :failure_id: <ID from DFA failure initiators :need:`gd_guidl__dfa_failure_initiators`>
       :failure_effect: "description of failure effect of the failure initiator on the element"
       :mitigated_by: <ID from Component Requirement | ID from AoU Component Requirement>
       :mitigation_issue: <ID from Issue Tracker>
       :sufficient: <yes|no>
       :status: <valid|invalid>

.. note::   argument is inside the 'content'. Therefore content is mandatory

.. attention::
    The above directive must be updated according to your component DFA.

    - The above "code-block" directive must be updated
    - Fill in all the needed information in the <brackets>
