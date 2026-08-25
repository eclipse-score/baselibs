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


FMEA (Failure Modes and Effects Analysis)
=========================================

.. document:: log_frontend FMEA
   :id: doc__log_fmea
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__sw_component_fmea


Failure Mode List
-----------------

.. list-table:: Fault Models for sequence diagrams
    :header-rows: 1
    :widths: 10,20,10,20

    * - ID
      - Failure Mode
      - Applicability
      - Rationale
    * - MF_01_01
      - message is not received (is a subset/more precise description of MF_01_05)
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - MF_01_02
      - message received too late (only relevant if delay is a realistic fault)
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - MF_01_03
      - message received too early (usually not a problem)
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - MF_01_04
      - message not received correctly by all recipients (different messages or messages partly lost). Only relevant if the same message goes to multiple recipients.
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - MF_01_05
      - message is corrupted
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - MF_01_06
      - message is not sent
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - MF_01_07
      - message is unintended sent
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - CO_01_01
      - minimum constraint boundary is violated
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - CO_01_02
      - maximum constraint boundary is violated
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - EX_01_01
      - Process calculates wrong result(s) (is a subset/more precise description of MF_01_05 or MF_01_04). This failure mode is related to the analysis if e.g. internal safety mechanisms are required (level 2 function, plausibility check of the output, …) because of the size / complexity of the feature.
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - EX_01_02
      - processing too slow (only relevant if timing is considered)
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - EX_01_03
      - processing too fast (only relevant if timing is considered)
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - EX_01_04
      - loss of execution
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - EX_01_05
      - processing changes to arbitrary process
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>
    * - EX_01_06
      - processing is not complete (infinite loop)
      - <yes | no>
      - <Rationale if not applicable, otherwise link to filled out FMEA>

FMEA
----
For all identified applicable failure initiators, the FMEA is performed in the following section.

.. code-block:: rst

    .. comp_saf_fmea:: <Title>
       :violates: <Component architecture>
       :id: comp_saf_fmea__<Component>__<Element descriptor>
       :fault_id: <ID from fault model :need:`gd_guidl__fault_models`>
       :failure_effect: "description of failure effect of the fault model on the element"
       :mitigated_by: <ID from Component Requirement | ID from AoU Component Requirement>
       :mitigation_issue: <ID from Issue Tracker>
       :sufficient: <yes|no>
       :status: <valid|invalid>

      .. note::   argument is inside the 'content'. Therefore content is mandatory

.. attention::
    The above directive must be updated according to your component FMEA.

    - The above "code-block" directive must be updated
    - Fill in all the needed information in the <brackets>
