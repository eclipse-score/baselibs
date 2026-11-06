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

OS Component Architecture
*************************

.. document:: OS Architecture
   :id: doc__os_architecture
   :status: valid
   :version: 2
   :security: YES
   :safety: ASIL_B
   :realizes: wp__component_arch[version==1]

Overview/Description
--------------------
see :need:`doc__os`

Static Architecture
-------------------

.. comp:: OS
   :id: comp__baselibs_os
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: baselibs_os
   :implements: logic_arc_int__baselibs__osal_general[version==1], logic_arc_int__baselibs__osal_qnx[version==1]
   :belongs_to: feat__baselibs[version==1]


.. comp_arc_sta:: OS Static view
   :id: comp_arc_sta__baselibs__os
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__os__env_get[version==1], comp_req__os__env_set[version==1], comp_req__os__env_unset[version==1], comp_req__os__sem_close[version==1], comp_req__os__sem_destroy[version==1], comp_req__os__sem_getvalue[version==1], comp_req__os__sem_init[version==1], comp_req__os__sem_instance[version==1], comp_req__os__sem_open[version==1], comp_req__os__sem_post[version==1], comp_req__os__sem_timedwait[version==1], comp_req__os__sem_timedwait_monotonic[version==1], comp_req__os__sem_trywait[version==1], comp_req__os__sem_unlink[version==1], comp_req__os__sem_wait[version==1], comp_req__os__sem_waittimeout[version==1]
   :belongs_to: comp__baselibs_os[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Dynamic Architecture
--------------------

No need for sequence diagram. Simple caller callee flow.

Interfaces
----------

.. logic_arc_int_op:: Get environment variable
   :id: logic_arc_int_op__os__env_get
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Set environment variable
   :id: logic_arc_int_op__os__env_set
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Unset environment variable
   :id: logic_arc_int_op__os__env_unset
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Named semaphore closure
   :id: logic_arc_int_op__os__sem_close
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Unnamed semaphore destruction
   :id: logic_arc_int_op__os__sem_destroy
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Semaphore value reading
   :id: logic_arc_int_op__os__sem_getvalue
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Unnamed semaphore initialization
   :id: logic_arc_int_op__os__sem_init
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Semaphore instance
   :id: logic_arc_int_op__os__sem_instance
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Named semaphore creation
   :id: logic_arc_int_op__os__sem_open
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Semaphore value increment
   :id: logic_arc_int_op__os__sem_post
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Semaphore blocking wait with absolute timeout
   :id: logic_arc_int_op__os__sem_timedwait
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Semaphore blocking wait with absolute monotonic timeout
   :id: logic_arc_int_op__os__sem_timedwait_monotonic
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_qnx[version==1], logic_arc_int__baselibs__osal_linux[version==1]

.. logic_arc_int_op:: Semaphore non-blocking wait
   :id: logic_arc_int_op__os__sem_trywait
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Named semaphore destruction
   :id: logic_arc_int_op__os__sem_unlink
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Semaphore blocking wait
   :id: logic_arc_int_op__os__sem_wait
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_general[version==1]

.. logic_arc_int_op:: Semaphore blocking wait with relative timeout
   :id: logic_arc_int_op__os__sem_waittimeout
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__osal_qnx[version==1], logic_arc_int__baselibs__osal_linux[version==1]

.. needextend:: c.this_doc() and type == "logic_arc_int_op"
   :+tags: baselibs, os
