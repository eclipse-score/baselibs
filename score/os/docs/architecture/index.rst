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

OSAL Component Architecture
***************************

.. document:: OSAL Architecture
   :id: doc__osal_architecture
   :status: valid
   :version: 3
   :security: YES
   :safety: ASIL_B
   :realizes: wp__component_arch[version==1]

Overview/Description
--------------------
see :need:`doc__osal`

.. note::

   TODO: Document which OSAL functions can provide ASIL-B services based on their operating system dependencies.
   See `issue #663 <https://github.com/eclipse-score/baselibs/issues/663>`_.

Static Architecture
-------------------

.. comp:: OSAL
   :id: comp__baselibs_osal
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 2
   :tags: baselibs_os
   :implements: logic_arc_int__baselibs__osal_general[version==1], logic_arc_int__baselibs__osal_qnx[version==1]
   :consists_of: comp__baselibs_osal_qm[version==1]
   :uses: logic_arc_int__os__fcntl[version==1], logic_arc_int__os__stat[version==1], logic_arc_int__os__mman[version==1], logic_arc_int__os__unistd[version==1], logic_arc_int__os__libcpp[version==1], logic_arc_int__os__message_passing[version==1]
   :belongs_to: feat__baselibs[version==1]

.. comp:: OSAL QM
   :id: comp__baselibs_osal_qm
   :security: YES
   :safety: QM
   :status: valid
   :version: 1
   :tags: baselibs_os
   :implements: logic_arc_int__baselibs__osal_linux[version==1]
   :belongs_to: feat__baselibs[version==1]

   Contains the QM libraries of the OSAL, including Linux-specific services.
   QM functions within the general and QNX-specific service families retain their QM allocation.

.. comp_arc_sta:: OSAL Static view
   :id: comp_arc_sta__baselibs__osal
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 2
   :fulfils: comp_req__osal__asil_b_operation_delegation[version==1], comp_req__osal__qm_operation_delegation[version==1], comp_req__osal__error_information[version==1]
   :belongs_to: comp__baselibs_osal[version==2]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Dynamic Architecture
--------------------

No need for sequence diagram. Simple caller callee flow.
