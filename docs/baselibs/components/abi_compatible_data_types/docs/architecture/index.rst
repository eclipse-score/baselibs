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

ABI Compatible Data Types Component Architecture
************************************************

.. document:: ABI Compatible Data Types Architecture
   :id: doc__abi_compatible_data_types_architecture
   :status: valid
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__component_arch

Overview/Description
--------------------

see :need:`doc__abi_compatible_data_types`

Static Architecture
-------------------

.. comp:: ABI Compatible Data Types
   :id: comp__baselibs_abi_compatible_data_types
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: baselibs_abi_compatible_data_types
   :belongs_to: feat__baselibs

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}
