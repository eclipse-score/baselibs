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

Rust Containers Architecture
============================

.. document:: Rust Containers Architecture
   :id: doc__containers_rust_architecture
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch[version==1]
   :tags: baselibs_rust_containers_rust


Overview
--------

The implementation of the containers library comprises two main parts:

- The generic storage abstraction for elements, with two concrete implementations:
  heap-allocated and inline storage
- The generic logic for each container type, each of which has two specializations:
  one using heap-allocated storage, and one using inline storage

Static Architecture
-------------------

.. comp_arc_sta:: Rust Containers
   :id: comp_arc_sta__baselibs_rust__containers_rust
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :tags: baselibs_rust_containers_rust
   :fulfils: comp_req__containers_rust__fixed_vector[version==1], comp_req__containers_rust__inline_vector[version==1], comp_req__containers_rust__fixed_queue[version==1], comp_req__containers_rust__inline_queue[version==1]
   :belongs_to: comp__baselibs_containers_rust[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}


Interfaces
----------

.. logic_arc_int:: Fixed-Capacity Vector
   :id: logic_arc_int__b_r__fixvec
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1

.. logic_arc_int_op:: Push
   :id: logic_arc_int_op__cont__fixvec_push
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixvec[version==1]

.. logic_arc_int_op:: Pop
   :id: logic_arc_int_op__cont__fixvec_pop
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixvec[version==1]

.. logic_arc_int_op:: Clear
   :id: logic_arc_int_op__cont__fixvec_clear
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixvec[version==1]

.. logic_arc_int_op:: Index
   :id: logic_arc_int_op__cont__fixvec_index
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixvec[version==1]

.. logic_arc_int_op:: Iterate
   :id: logic_arc_int_op__cont__fixvec_iterate
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixvec[version==1]


.. logic_arc_int:: Inline-Storage Vector
   :id: logic_arc_int__b_r__inlinevec
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1

.. logic_arc_int_op:: Push
   :id: logic_arc_int_op__cont__inlinevec_push
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlinevec[version==1]

.. logic_arc_int_op:: Pop
   :id: logic_arc_int_op__cont__inlinevec_pop
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlinevec[version==1]

.. logic_arc_int_op:: Clear
   :id: logic_arc_int_op__cont__inlinevec_clear
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlinevec[version==1]

.. logic_arc_int_op:: Index
   :id: logic_arc_int_op__cont__inlinevec_index
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlinevec[version==1]

.. logic_arc_int_op:: Iterate
   :id: logic_arc_int_op__cont__inlinevec_iterate
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlinevec[version==1]


.. logic_arc_int:: Fixed-Capacity Queue
   :id: logic_arc_int__b_r__fixqueue
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1

.. logic_arc_int_op:: Push Front
   :id: logic_arc_int_op__cont__fixqueue_pushfront
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixqueue[version==1]

.. logic_arc_int_op:: Push Back
   :id: logic_arc_int_op__cont__fixqueue_pushback
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixqueue[version==1]

.. logic_arc_int_op:: Pop Front
   :id: logic_arc_int_op__cont__fixqueue_popfront
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixqueue[version==1]

.. logic_arc_int_op:: Pop Back
   :id: logic_arc_int_op__cont__fixqueue_popback
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixqueue[version==1]

.. logic_arc_int_op:: Clear
   :id: logic_arc_int_op__cont__fixqueue_clear
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixqueue[version==1]

.. logic_arc_int_op:: Iterate
   :id: logic_arc_int_op__cont__fixqueue_iterate
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__fixqueue[version==1]


.. logic_arc_int:: Inline-Storage Queue
   :id: logic_arc_int__b_r__inlqueue
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1

.. logic_arc_int_op:: Push Front
   :id: logic_arc_int_op__cont__inlqueue_pushfront
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlqueue[version==1]

.. logic_arc_int_op:: Push Back
   :id: logic_arc_int_op__cont__inlqueue_pushback
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlqueue[version==1]

.. logic_arc_int_op:: Pop Front
   :id: logic_arc_int_op__cont__inlqueue_popfront
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlqueue[version==1]

.. logic_arc_int_op:: Pop Back
   :id: logic_arc_int_op__cont__inlqueue_popback
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlqueue[version==1]

.. logic_arc_int_op:: Clear
   :id: logic_arc_int_op__cont__inlqueue_clear
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlqueue[version==1]

.. logic_arc_int_op:: Iterate
   :id: logic_arc_int_op__cont__inlqueue_iterate
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__b_r__inlqueue[version==1]
