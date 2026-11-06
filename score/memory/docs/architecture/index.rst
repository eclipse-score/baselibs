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

Memory Component Architecture
*****************************

.. document:: Memory Architecture
   :id: doc__memory_architecture
   :status: valid
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__component_arch[version==1]

Overview/Description
--------------------

The Memory component contains independent utilities for byte order, size and alignment information, and fixed-capacity buffering.
See :need:`doc__memory` for the capability descriptions.

Static Architecture
-------------------

.. comp:: Memory
   :id: comp__baselibs_memory
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :belongs_to: feat__baselibs[version==1]

.. comp_arc_sta:: Memory Static View
   :id: comp_arc_sta__baselibs__memory
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__memory__endianness[version==2]
   :belongs_to: comp__baselibs_memory[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Dynamic Architecture
--------------------

The utilities are invoked directly by their callers.

Interfaces
----------

The interfaces cover byte-order detection and conversion, size and alignment information, and fixed-capacity buffering.
The ring buffer accepts a capacity and an allocation resource at construction.
