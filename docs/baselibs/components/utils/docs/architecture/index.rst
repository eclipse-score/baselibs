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

Utils Component Architecture
**********************************

.. document:: Utils Architecture
   :id: doc__utils_architecture
   :status: valid
   :version: 1
   :security: YES
   :safety: ASIL_B
   :realizes: wp__component_arch[version==1]

Overview/Description
--------------------
see :need:`doc__utils`

Static Architecture
-------------------

.. comp:: Utils
   :id: comp__baselibs_utils
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: baselibs_utils
   :implements: logic_arc_int__baselibs__utils_base64[version==1],logic_arc_int__baselibs__utils_scoped_op[version==1]
   :belongs_to: feat__baselibs[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

.. comp_arc_sta:: Utils Static view
   :id: comp_arc_sta__baselibs__utils
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__utils__base64[version==1], comp_req__utils__scoped_operation[version==1], comp_req__utils__deterministic_behavior[version==1]
   :belongs_to: comp__baselibs_utils[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Interfaces
----------

.. logic_arc_int_op:: Encode
   :id: logic_arc_int_op__utils__base64_encode
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__utils_base64[version==1]

.. logic_arc_int_op:: Decode
   :id: logic_arc_int_op__utils__base64_decode
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__utils_base64[version==1]

.. logic_arc_int_op:: Constructor
   :id: logic_arc_int_op__utils__scoped_op_construct
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__utils_scoped_op[version==1]

.. logic_arc_int_op:: Destructor
   :id: logic_arc_int_op__utils__scoped_op_destruct
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__utils_scoped_op[version==1]
