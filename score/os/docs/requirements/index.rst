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

Requirements
############

.. document:: OS Library Requirements
   :id: doc__os_lib_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, os_library

Functional Requirements
=======================

.. comp_req:: Environment variable getting
   :id: comp_req__os__env_get
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for getting an environment variable.

.. comp_req:: Environment variable setting
   :id: comp_req__os__env_set
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for setting environment variable.

.. comp_req:: Environment variable unsetting
   :id: comp_req__os__env_unset
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for unsetting environment variable.

.. comp_req:: Semaphore instance
   :id: comp_req__os__sem_instance
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for getting the value of a named or unnamed semaphore.

.. comp_req:: Unnamed semaphore initialization
   :id: comp_req__os__sem_init
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for initializing an unnamed semaphore.

.. comp_req:: Named semaphore creation
   :id: comp_req__os__sem_open
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for creating a named semaphore.

.. comp_req:: Unnamed semaphore destruction
   :id: comp_req__os__sem_destroy
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for destroying an unnamed semaphore.

.. comp_req:: Semaphore non-blocking wait
   :id: comp_req__os__sem_trywait
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for waiting for a named or unnamed semaphore without blocking.

.. comp_req:: Semaphore blocking wait
   :id: comp_req__os__sem_wait
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for waiting for a named or unnamed semaphore.

.. comp_req:: Semaphore value increment
   :id: comp_req__os__sem_post
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for waiting for incrementing a named or unnamed semaphore.

.. comp_req:: Named semaphore closure
   :id: comp_req__os__sem_close
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for closing a named semaphore.

.. comp_req:: Named semaphore destruction
   :id: comp_req__os__sem_unlink
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for destroying a named semaphore.

.. comp_req:: Semaphore blocking wait with relative timeout
   :id: comp_req__os__sem_waittimeout
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for waiting for a named or unnamed semaphore with a relative timeout.

.. comp_req:: Semaphore blocking wait with absolute timeout
   :id: comp_req__os__sem_timedwait
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for waiting for a named or unnamed semaphore with an absolute timeout.

.. comp_req:: Semaphore blocking wait with absolute monotonic timeout
   :id: comp_req__os__sem_timedwait_monotonic
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for waiting for a named or unnamed semaphore with an absolute timeout based on monotonic clock.

.. comp_req:: Semaphore value reading
   :id: comp_req__os__sem_getvalue
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_os[version==1]

   The OS component shall provide functions for getting the value of a named or unnamed semaphore.

.. needextend:: c.this_doc() and (type == "comp_req" or type == "aou_req")
   :+tags: baselibs, os
