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

os Component Architecture
**********************************

.. document:: OS Architecture
   :id: doc__os_architecture
   :status: valid
   :version: 1
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
   :implements: logic_arc_int__baselibs__os_env[version==1]
   :belongs_to: feat__baselibs[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

.. comp_arc_sta:: OS Static view
   :id: comp_arc_sta__baselibs__os
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__os__env_set[version==1], comp_req__os__env_unset[version==1]
   :belongs_to: comp__baselibs_os[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Interfaces
----------

.. logic_arc_int_op:: Set environment variable
   :id: logic_arc_int_op__os__setenv
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__os_env[version==1]

.. logic_arc_int_op:: Unset environment variable
   :id: logic_arc_int_op__os__unsetenv
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__os_env[version==1]

.. needextend:: c.this_doc() and type == "logic_arc_int_op"
   :+tags: baselibs, os
