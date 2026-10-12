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

Scope Exit Architecture
#######################

.. document:: Scope Exit Architecture
   :id: doc__scope_exit_architecture
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__component_arch[version==1]

.. comp:: Scope Exit
   :id: comp__baselibs_scope_exit
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :implements: logic_arc_int__baselibs__utils_scoped_op[version==1]
   :belongs_to: feat__baselibs[version==1]

   The Scope Exit component contains RAII scope guards that invoke a callable on scope exit unless released.

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

.. comp_arc_sta:: Scope Exit Static view
   :id: comp_arc_sta__baselibs__scope_exit
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__scope_exit__invoke_on_destruction[version==1], comp_req__scope_exit__release[version==1], comp_req__scope_exit__move_transfer[version==1], comp_req__scope_exit__replace_on_assignment[version==1]
   :belongs_to: comp__baselibs_scope_exit[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Interfaces
----------

.. logic_arc_int_op:: Constructor
   :id: logic_arc_int_op__scope_exit__op_construct
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__utils_scoped_op[version==1]

.. needextend:: c.this_doc() and type == "logic_arc_int_op"
   :+tags: baselibs, scope_exit

.. logic_arc_int_op:: Destructor
   :id: logic_arc_int_op__scope_exit__op_destruct
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__utils_scoped_op[version==1]
