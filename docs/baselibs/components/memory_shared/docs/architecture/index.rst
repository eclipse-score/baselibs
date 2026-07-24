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

Memory Shared Component Architecture
************************************

.. comp:: Memory Shared
   :id: comp__baselibs_memory_shared
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :implements: logic_arc_int__baselibs__memory_shared[version==1]
   :uses: logic_arc_int__os__fcntl[version==1], logic_arc_int__os__stat[version==1], logic_arc_int__os__mman[version==1]
   :belongs_to: feat__baselibs[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

.. comp_arc_sta:: Memory Shared Static view
   :id: comp_arc_sta__baselibs__memory_shared
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__memory__shared_memory[version==1], comp_req__memory__offset_ptr[version==1], comp_req__memory__shared_container[version==1], comp_req__memory__inter_process_sync[version==1], comp_req__memory__bounds_check[version==1], comp_req__memory__endianness[version==1], comp_req__memory__sealed_shm[version==1], comp_req__memory__typed_memory[version==1], comp_req__memory__resource_registry[version==1], comp_req__memory__string_utils[version==1], comp_req__memory__atomic_ops[version==1], comp_req__memory__deterministic_alloc[version==1], comp_req__memory__address_independence[version==1]
   :belongs_to: comp__baselibs_memory_shared[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

.. logic_arc_int_op:: Open
   :id: logic_arc_int_op__baselibs__open
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__memory_shared[version==1]

.. logic_arc_int_op:: Update
   :id: logic_arc_int_op__baselibs__update
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__memory_shared[version==1]

.. logic_arc_int_op:: Lock
   :id: logic_arc_int_op__baselibs__lock
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__memory_shared[version==1]

.. logic_arc_int_op:: Set Permissions
   :id: logic_arc_int_op__baselibs__set_perm
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__memory_shared[version==1]
