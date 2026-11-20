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
****************************

.. document:: Utils Architecture
   :id: doc__utils_architecture
   :status: valid
   :version: 3
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
   :implements: logic_arc_int__baselibs__base64_encoding[version==1], logic_arc_int__baselibs__base64_decoding[version==1], logic_arc_int__baselibs__threadsafe_singleton[version==1], logic_arc_int__baselibs__static_guard[version==1], logic_arc_int__baselibs__string_registry[version==1], logic_arc_int__baselibs__registry_string_view[version==1]
   :belongs_to: feat__baselibs[version==1]


.. comp_arc_sta:: Utils Static view
   :id: comp_arc_sta__baselibs__utils
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__utils__base64_encoding[version==1], comp_req__utils__base64_decoding[version==1], comp_req__utils__threadsafe_singleton[version==1], comp_req__utils__string_registry[version==1], comp_req__utils__registry_string_view[version==1], comp_req__utils__static_destruction_guard[version==1]
   :belongs_to: comp__baselibs_utils[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Dynamic Architecture
--------------------

No need for sequence diagram. Simple caller callee flow.

Interfaces
----------

.. logic_arc_int_op:: Encode Base64
   :id: logic_arc_int_op__utils__base64_encode
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__base64_encoding[version==1]

   Encodes byte data as Base64 text.

.. logic_arc_int_op:: Decode Base64
   :id: logic_arc_int_op__utils__base64_decode
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__base64_decoding[version==1]

   Decodes Base64 text into byte data.

.. logic_arc_int_op:: Get Singleton Instance
   :id: logic_arc_int_op__utils__singleton_get
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__threadsafe_singleton[version==1]

   Returns the singleton instance, initializing it with the supplied constructor arguments on first access.

.. logic_arc_int_op:: Initialize Singleton With Callable
   :id: logic_arc_int_op__utils__singleton_init
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__threadsafe_singleton[version==1]

   Returns the singleton instance, invoking the supplied initialization callable on first access.

.. logic_arc_int_op:: Construct Static Destruction Guard
   :id: logic_arc_int_op__utils__static_guard_create
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__static_guard[version==1]

   Constructs the guarded static object when the first guard is created.

.. logic_arc_int_op:: Access Guarded Static Storage
   :id: logic_arc_int_op__utils__static_guard_get
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__static_guard[version==1]

   Returns the guarded static object while at least one guard remains alive.

.. logic_arc_int_op:: Destroy Static Destruction Guard
   :id: logic_arc_int_op__utils__static_guard_destroy
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__static_guard[version==1]

   Destroys the guarded static object when the final guard is destroyed.

.. logic_arc_int_op:: Insert String
   :id: logic_arc_int_op__utils__registry_insert
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__string_registry[version==1]

   Registers a string and returns its stable registry view and insertion status.

.. logic_arc_int_op:: Compare Registry String Views
   :id: logic_arc_int_op__utils__registry_view_cmp
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__registry_string_view[version==1]

   Compares registry string views by identity for equality and by content for ordering.

.. logic_arc_int_op:: Hash Registry String View
   :id: logic_arc_int_op__utils__registry_view_hash
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__registry_string_view[version==1]

   Provides a hash value that is consistent with registry string view equality.

.. needextend:: c.this_doc() and type == "logic_arc_int_op"
   :+tags: baselibs, utils
