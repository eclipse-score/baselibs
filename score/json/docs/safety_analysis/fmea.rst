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



FMEA (Failure Modes and Effects Analysis)
=========================================

.. document:: JSON FMEA
   :id: doc__json_fmea
   :status: draft
   :version: 2
   :safety: ASIL_B
   :security: NO
   :realizes: wp__sw_component_fmea[version==1]

The analysis covers the single JSON design: the parser and the writer of the JSON-Wrapper on top of vaJson, see
:need:`comp_arc_dyn__baselibs__json_dynamic_view` and :need:`comp_arc_dyn__baselibs__json_write_view`.
The upstream vaJson analysis is retained as evidence in :doc:`../vajson/safety/SafetyAnalysis`.

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
      - yes
      - A parse or write request is not processed and no result is returned. See :need:`comp_saf_fmea__json__no_return`
    * - MF_01_02
      - message received too late (only relevant if delay is a realistic fault)
      - yes
      - Delayed processing delays the result. See :need:`comp_saf_fmea__json__late_return`
    * - MF_01_03
      - message received too early (usually not a problem)
      - no
      - Do not see where this is a problem.
    * - MF_01_04
      - message not received correctly by all recipients (different messages or messages partly lost). Only relevant if the same message goes to multiple recipients.
      - no
      - No multiple recipients, one call has one caller.
    * - MF_01_05
      - message is corrupted
      - yes
      - Corrupted JSON input is not detectable in general. See :need:`comp_saf_fmea__json__corrupted_input`
    * - MF_01_06
      - message is not sent
      - yes
      - The parser or writer returns no result. See :need:`comp_saf_fmea__json__no_return`
    * - MF_01_07
      - message is unintended sent
      - yes
      - Malformed input is accepted as valid. See :need:`comp_saf_fmea__json__invalid_input_accepted`
    * - CO_01_01
      - minimum constraint boundary is violated.
      - yes
      - Numbers below the range of the requested type. See :need:`comp_saf_fmea__json__wrong_value`
    * - CO_01_02
      - maximum constraint boundary is violated,
      - yes
      - Numbers above the range of the requested type and files larger than 64 MiB. See :need:`comp_saf_fmea__json__wrong_value`
    * - EX_01_01
      - Process calculates wrong result(s)
      - yes
      - Wrong parse or write result. See :need:`comp_saf_fmea__json__wrong_value`
    * - EX_01_02
      - processing too slow (only relevant if timing is considered)
      - yes
      - See :need:`comp_saf_fmea__json__late_return`
    * - EX_01_03
      - processing too fast (only relevant if timing is considered)
      - no
      - No problem seen.
    * - EX_01_04
      - loss of execution
      - yes
      - See :need:`comp_saf_fmea__json__no_return`
    * - EX_01_05
      - processing changes to arbitrary process
      - no
      - Not a JSON problem as it is a library and not a process.
    * - EX_01_06
      - processing is not complete (infinite loop)
      - yes
      - See :need:`comp_saf_fmea__json__late_return`

FMEA
----
For all identified applicable failure initiators, the FMEA is performed in the following section.

.. comp_saf_fmea:: JSON No Return
   :violates: comp_arc_sta__baselibs__json[version==6]
   :id: comp_saf_fmea__json__no_return
   :fault_id: MF_01_01,MF_01_06,EX_01_04
   :failure_effect: A parse or write request returns no result, so a safety function relying on it may violate its safety requirements.
   :mitigated_by: aou_req__platform__error_reaction[version==1]
   :sufficient: yes
   :status: draft
   :version: 1

   Users are required on platform level to evaluate return values.

.. comp_saf_fmea:: JSON Late Return
   :violates: comp_arc_sta__baselibs__json[version==6]
   :id: comp_saf_fmea__json__late_return
   :fault_id: MF_01_02,EX_01_02,EX_01_06
   :failure_effect: A parse or write request completes too late or stalls, so there may be a violation of safety requirements.
   :mitigated_by: aou_req__platform__flow_monitoring[version==1]
   :sufficient: yes
   :status: draft
   :version: 1

   Program flow monitoring on platform level covers late or stalled execution.

.. comp_saf_fmea:: JSON Corrupted Input
   :violates: comp_arc_sta__baselibs__json[version==6]
   :id: comp_saf_fmea__json__corrupted_input
   :fault_id: MF_01_05
   :failure_effect: Corrupted JSON data that is still well-formed is parsed and used, so safety requirements may be violated.
   :mitigated_by: aou_req__json__data_integrity[version==1], aou_req__json__access_control[version==1]
   :sufficient: yes
   :status: draft
   :version: 1

   A well-formedness check cannot detect all corruption, so the user protects the integrity of the input.

.. comp_saf_fmea:: JSON Invalid Input Accepted
   :violates: comp_arc_sta__baselibs__json[version==6]
   :id: comp_saf_fmea__json__invalid_input_accepted
   :fault_id: MF_01_07
   :failure_effect: Malformed JSON is accepted and deserialized, so safety requirements may be violated.
   :mitigated_by: comp_req__vajson__validation[version==2], comp_req__vajson__number_grammar[version==1], comp_req__json__duplicate_member_names[version==1]
   :sufficient: yes
   :status: draft
   :version: 1

   The parser rejects malformed input, numbers outside the accepted grammar and duplicate member names.

.. comp_saf_fmea:: JSON Wrong Value
   :violates: comp_arc_sta__baselibs__json[version==6]
   :id: comp_saf_fmea__json__wrong_value
   :fault_id: CO_01_01,CO_01_02,EX_01_01
   :failure_effect: The user receives or writes a value that differs from the JSON data, so safety requirements may be violated.
   :mitigated_by: comp_req__json__type_compatibility[version==1], comp_req__json__out_of_range_error[version==1], comp_req__vajson__unicode[version==2], comp_req__json__max_file_size[version==1]
   :sufficient: yes
   :status: draft
   :version: 1

   Type and range are checked on access, strings are decoded and encoded as UTF-8 and oversized files are rejected.

.. needextend:: c.this_doc() and type == "comp_saf_fmea"
   :+tags: baselibs, json
