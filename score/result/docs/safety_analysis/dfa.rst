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

.. document:: result DFA
   :id: doc__result_dfa
   :status: valid
   :version: 2
   :safety: ASIL_B
   :security: NO
   :realizes: wp__sw_component_dfa[version==1]

.. note:: Use the content of the document to describe e.g. why a fault model is not applicable for the diagram.

Dependent Failure Initiators
----------------------------

Shared resources
^^^^^^^^^^^^^^^^

The dependent failure initiators related to shared resources are not applicable for the component. The shared resources
are considered in the platform and feature DFA.

Communication between the two elements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Receiving function is affected by information that is false, lost, sent multiple times, or in the wrong order etc. from the sender.

.. list-table:: DFA communication between elements
  :header-rows: 1
  :widths: 10,20,10,20

  * - ID
    - Violation cause communication between elements
    - Applicability
    - Rationale
  * - CO_01_01
    - Information passed via argument through a function call, or via writing/reading a variable being global to the two software functions (data flow)
    - Yes
    - Call to used libcpp can return wrong values, :need:`comp_saf_dfa__result__ffi`
  * - CO_01_02
    - Data or message corruption / repetition / loss / delay / masquerading or incorrect addressing of information
    - no
    - No messages between result and used Libcpp functions.
  * - CO_01_03
    - Insertion / sequence of information
    - no
    - No messages between result and used Libcpp functions.
  * - CO_01_04
    - Corruption of information, inconsistent data
    - no
    - No messages between result and used Libcpp functions.
  * - CO_01_05
    - Asymmetric information sent from a sender to multiple receivers, so that not all defined receivers have the same information
    - no
    - No messages between result and used Libcpp functions.
  * - CO_01_06
    - Information from a sender received by only a subset of the receivers
    - no
    - No messages between result and used Libcpp functions.
  * - CO_01_07
    - Blocking access to a communication channel
    - no
    - No communication channel shared between result and used Libcpp functions.

Shared information inputs
^^^^^^^^^^^^^^^^^^^^^^^^^

Same information input used by multiple functions.

.. list-table:: DFA shared information inputs
  :header-rows: 1
  :widths: 10,20,10,20

  * - ID
    - Violation cause shared information inputs
    - Applicability
    - Rationale
  * - SI_01_02
    - Configuration data
    - no
    - No shared configuration data between result and used Libcpp functions.
  * - SI_01_03
    - Constants, or variables, being global to the two software functions
    - no
    - No global data used by result and used Libcpp functions.
  * - SI_01_04
    - Basic software passes data (read from hardware register and converted into logical information) to two applications software functions
    - no
    - result and used Libcpp functions are not sharing HW related data.
  * - SI_01_05
    - Data / function parameter arguments / messages delivered by software function to more than one other function
    - no
    - Not expected for result and used Libcpp functions.

Unintended impact
^^^^^^^^^^^^^^^^^

Unintended impacts to function due to various failures.

.. list-table:: DFA unintended impact
  :header-rows: 1
  :widths: 10,20,10,20

  * - ID
    - Violation cause unintended impact
    - Applicability
    - Rationale
  * - UI_01_01
    - Memory miss-allocation and leaks
    - no
    - Not a specific bitmnipulation topic, therefore covered at platform DFA.
  * - UI_01_02
    - Read/Write access to memory allocated to another software element
    - yes
    - result and used Libcpp functions are in same memory space so the memory could be corrupted, :need:`comp_saf_dfa__result__ffi`
  * - UI_01_03
    - Stack/Buffer under-/overflow
    - no
    - Not a specific result topic, therefore covered at platform DFA.
  * - UI_01_04
    - Deadlocks
    - yes
    - Not a specific result topic, therefore covered at feature level.
  * - UI_01_05
    - Livelocks
    - no
    - Not a specific result topic, therefore covered at feature level.
  * - UI_01_06
    - Blocking of execution
    - yes
    - Used Libcpp functions may block result, :need:`comp_saf_dfa__result__ffi`
  * - UI_01_07
    - Incorrect allocation of execution time
    - no
    - Execution time allocated by (external) OS on platform level, should be covered centrally at platform level.
  * - UI_01_08
    - Incorrect execution flow
    - no
    - Execution flow controlled by (external) OS on platform level, should be covered centrally at platform level.
  * - UI_01_09
    - Incorrect synchronization between software elements
    - no
    - result and used Libcpp functions have no synchronization needs.
  * - UI_01_10
    - CPU time depletion
    - yes
    - Used Libcpp functions may deplete result's CPU time, :need:`comp_saf_dfa__result__ffi`
  * - UI_01_11
    - Memory depletion
    - no
    - Not a specific result topic, therefore covered at platform DFA.
  * - UI_01_12
    - Other HW unavailability
    - no
    - No special HW used for baselibs.


DFA
===

For all identified applicable failure initiators, the DFA is performed in the following section.


.. comp_saf_dfa:: Result component FFI
   :violates: comp_arc_sta__baselibs__result
   :id: comp_saf_dfa__result__ffi
   :failure_id: CO_01_01,UI_01_02,UI_01_06,UI_01_10
   :failure_effect: result and used Libcpp functions influence each other and cause wrong read or write of error/result data
   :mitigated_by: aou_req__platform__os_safety_functions
   :sufficient: yes
   :status: valid
   :version: 1

   Used C++ standard library (libcpp) functions are required to be provided at least as ASIL_B.
