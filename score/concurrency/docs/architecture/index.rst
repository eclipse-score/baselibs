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

Concurrency Component Architecture
***********************************

.. document:: Concurrency Architecture
   :id: doc__concurrency_architecture
   :status: valid
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__component_arch[version==1]

Overview/Description
--------------------

see :need:`doc__concurrency`

Static Architecture
-------------------

.. comp:: Concurrency
   :id: comp__baselibs_concurrency
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: baselibs_concurrency
   :implements: logic_arc_int__baselibs__promise[version==1], logic_arc_int__baselibs__future[version==1], logic_arc_int__baselibs__shared_future[version==1], logic_arc_int__baselibs__executor[version==1], logic_arc_int__baselibs__task[version==1], logic_arc_int__baselibs__task_result[version==1], logic_arc_int__baselibs__synchronized_queue[version==1], logic_arc_int__baselibs__condition_variable[version==1]
   :belongs_to: feat__baselibs[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

.. comp_arc_sta:: Concurrency Static view
   :id: comp_arc_sta__baselibs__concurrency
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__concurrency__task_interface[version==1], comp_req__concurrency__task_cancellation[version==1], comp_req__concurrency__simple_task[version==1], comp_req__concurrency__task_result[version==1], comp_req__concurrency__periodic_task[version==1], comp_req__concurrency__delayed_task[version==1], comp_req__concurrency__executor_interface[version==1], comp_req__concurrency__thread_pool[version==1], comp_req__concurrency__condition_variable[version==1], comp_req__concurrency__interruptible_wait[version==1], comp_req__concurrency__notification[version==1], comp_req__concurrency__synchronized_queue[version==1], comp_req__concurrency__long_running_threads[version==1], comp_req__concurrency__memory_usage_control[version==1], comp_req__concurrency__memory_reservation[version==1], comp_req__concurrency__thread_count_reporting[version==1], comp_req__concurrency__operation_timeout[version==1], comp_req__concurrency__error_handling[version==1]
   :belongs_to: comp__baselibs_concurrency[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Interfaces
----------

.. logic_arc_int_op:: Set Value
   :id: logic_arc_int_op__conc__promise_setval
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__promise[version==1]

.. logic_arc_int_op:: Set Error
   :id: logic_arc_int_op__conc__promise_set_error
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__promise[version==1]

.. logic_arc_int_op:: Get Future
   :id: logic_arc_int_op__conc__promise_get_future
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__promise[version==1]

.. logic_arc_int_op:: On Abort
   :id: logic_arc_int_op__conc__promise_on_abort
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__promise[version==1]

.. logic_arc_int_op:: Get
   :id: logic_arc_int_op__conc__future_get
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__future[version==1]

.. logic_arc_int_op:: Wait
   :id: logic_arc_int_op__conc__future_wait
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__future[version==1]

.. logic_arc_int_op:: Wait For
   :id: logic_arc_int_op__conc__future_wait_for
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__future[version==1]

.. logic_arc_int_op:: Wait Until
   :id: logic_arc_int_op__conc__future_wait_until
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__future[version==1]

.. logic_arc_int_op:: Valid
   :id: logic_arc_int_op__conc__future_valid
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__future[version==1]

.. logic_arc_int_op:: Share
   :id: logic_arc_int_op__conc__future_share
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__future[version==1]

.. logic_arc_int_op:: Then
   :id: logic_arc_int_op__conc__future_then
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__future[version==1]

.. logic_arc_int_op:: Get Shared
   :id: logic_arc_int_op__conc__shared_future_get
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__shared_future[version==1]

.. logic_arc_int_op:: Copy
   :id: logic_arc_int_op__conc__shared_future_copy
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__shared_future[version==1]

.. logic_arc_int_op:: Enqueue
   :id: logic_arc_int_op__conc__executor_enqueue
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__executor[version==1]

.. logic_arc_int_op:: Post
   :id: logic_arc_int_op__conc__executor_post
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__executor[version==1]

.. logic_arc_int_op:: Submit
   :id: logic_arc_int_op__conc__executor_submit
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__executor[version==1]

.. logic_arc_int_op:: Shutdown
   :id: logic_arc_int_op__conc__executor_shutdown
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__executor[version==1]

.. logic_arc_int_op:: Max Concurrency Level
   :id: logic_arc_int_op__conc__executor_max_conc
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__executor[version==1]

.. logic_arc_int_op:: Execute
   :id: logic_arc_int_op__conc__task_execute
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__task[version==1]

.. logic_arc_int_op:: Get Stop Source
   :id: logic_arc_int_op__conc__task_get_stop_source
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__task[version==1]

.. logic_arc_int_op:: Abort
   :id: logic_arc_int_op__conc__task_result_abort
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__task_result[version==1]

.. logic_arc_int_op:: Aborted
   :id: logic_arc_int_op__conc__task_result_aborted
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__task_result[version==1]

.. logic_arc_int_op:: Get Result
   :id: logic_arc_int_op__conc__task_result_get
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__task_result[version==1]

.. logic_arc_int_op:: Push
   :id: logic_arc_int_op__conc__sync_queue_push
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__synchronized_queue[version==1]

.. logic_arc_int_op:: Pop
   :id: logic_arc_int_op__conc__sync_queue_pop
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__synchronized_queue[version==1]

.. logic_arc_int_op:: Try Push
   :id: logic_arc_int_op__conc__sync_queue_try_push
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__synchronized_queue[version==1]

.. logic_arc_int_op:: Try Pop
   :id: logic_arc_int_op__conc__sync_queue_try_pop
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__synchronized_queue[version==1]

.. logic_arc_int_op:: Wait
   :id: logic_arc_int_op__conc__cv_wait
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__condition_variable[version==1]

.. logic_arc_int_op:: Wait For
   :id: logic_arc_int_op__conc__cv_wait_for
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__condition_variable[version==1]

.. logic_arc_int_op:: Notify One
   :id: logic_arc_int_op__conc__cv_notify_one
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__condition_variable[version==1]

.. logic_arc_int_op:: Notify All
   :id: logic_arc_int_op__conc__cv_notify_all
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__condition_variable[version==1]
