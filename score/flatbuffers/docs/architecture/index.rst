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

FlatBuffers Component Architecture
##################################

.. document:: FlatBuffers Architecture
   :id: doc__flatbuffers_architecture
   :status: valid
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__component_arch

Overview
********

The component wraps the upstream Google FlatBuffers library. Code and binary buffers are generated from
``.fbs`` schemas via the bundled ``flatc`` Bazel rules.
Primary use case: loading module configuration, see :need:`doc__flatbuffers` for the full component description.


Requirements Linked to Component Architecture
*********************************************

.. needtable:: Overview of Component Requirements
   :style: table
   :columns: title;id
   :filter: search("comp__baselibs_flatbuffers", str(satisfied_by))
   :colwidths: 70,30

Description
***********

FlatBuffers provides three entry points — :need:`logic_arc_int_op__flatbuffers__loadbuffer`,
:need:`logic_arc_int_op__flatbuffers__versionreader`, and
:need:`logic_arc_int_op__flatbuffers__generatedcode` per supported language.
C++ and Rust are (targeting) ASIL-B implementations. Python follows the same three-entry-point architecture
as a supportive (QM) implementation, other languages may be added on demand.

Design Decisions:
=================

C++ Interface - Upstream Implementation Exposure
-------------------------------------------------

.. dec_rec:: C++ Interface - Upstream Implementation Exposure
   :id: dec_rec__flatbuffers__upstream_impl_exposure
   :context: C++ interface - upstream flatbuffers types exposed to consumers
   :decision: use flatc
   :status: accepted
   :version: 1

Use ``flatc`` for C++ code generation and the ``flatbufferscpp`` Bazel target as the
sanctioned C++ interface for FlatBuffers consumers, with direct upstream primitives discouraged.

Context
^^^^^^^
The component must expose FlatBuffers to C++ consumers in an ASIL-B-capable manner.
The ``flatc`` compiler generates builder, verifier, and accessor header code from ``.fbs`` schemas.
Generated code depends on runtime types (``flatbuffers::Table``, ``flatbuffers::Verifier``,
``flatbuffers::Vector<>``, etc.) from the upstream library, making that dependency unavoidable.

Consequences
^^^^^^^^^^^^
-  ``flatbufferscpp`` is a mandatory, visible compile-time dependency for all generated-code consumers.
-  The sanctioned usage path is: verify via ``VerifyXxxBuffer()``; access data via generated accessors,
   ``GetRoot<>()``, and keyed table-vector methods such as ``LookupByKey()``; construct via ``FlatBufferBuilder`` 
   and ``CreateXxx()`` / ``XxxBuilder`` helpers.
-  Bypassing generated helpers (``StartTable()``, ``AddElement()``, ``EndTable()``, custom
   allocators) is discouraged — incorrect sequencing leads to invalid buffers or undefined behavior.

Alternatives Considered
^^^^^^^^^^^^^^^^^^^^^^^

Custom flatc-like code generator
""""""""""""""""""""""""""""""""
Develop a bespoke code generator that wraps FlatBuffers internals and exposes only a
project-internal API, hiding upstream ``flatbuffers::*`` types from consumers.

Advantages:

-  Cleaner API boundary: Consumers not exposed to upstream types.
-  Controlled surface: Only explicitly approved operations visible to integrators.

Disadvantages:

-  Correctness risk: A custom generator introduces an untested translation layer between schema intent and generated code.
-  Schema coverage uncertainty: The upstream schema language evolves, a custom generator may
   silently mishandle schema constructs it does not support, producing incorrect or incomplete code
   without any diagnostic.
-  Loss of community support: ``flatc`` benefits from broad industrial adoption
   and a community test suite that directly validates generated-code correctness.

Justification for the Decision
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``flatc`` generator carries established qualification evidence through broad industrial
adoption and a comprehensive test suite that validates generated-code correctness. Treating
direct usage of upstream primitives as a documented discouraged pattern achieves equivalent API discipline
through process controls.

Design Constraints:
===================

``Verify*`` functions validate structural well-formedness only — payload integrity is outside scope :need:`aou_req__flatbuffers__data_integrity`. 
The same applies to file access control :need:`aou_req__flatbuffers__access_control`.

Rationale Behind Architecture Decomposition
*******************************************

The component is not split into sub-components. The three key interface entry points are:

- :need:`logic_arc_int_op__flatbuffers__loadbuffer`: raw file I/O, decoupled from any FlatBuffers specifics
- :need:`logic_arc_int_op__flatbuffers__versionreader`: lightweight opt-in version check, without requiring the full application schema
- :need:`logic_arc_int_op__flatbuffers__generatedcode`: schema-typed access layer (reader, verifier, builder)

Static Architecture
*******************

.. comp:: FlatBuffers
   :id: comp__baselibs_flatbuffers
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :implements: logic_arc_int__baselibs__flatbuffers[version==1]
   :belongs_to: feat__baselibs[version==1]

.. comp_arc_sta:: FlatBuffers Static View
   :id: comp_arc_sta__baselibs__flatbuffers
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :belongs_to: comp__baselibs_flatbuffers
   :fulfils:
      comp_req__flatbuffers__serialization[version==1],
      comp_req__flatbuffers__access[version==1],
      comp_req__flatbuffers__verification[version==1],
      comp_req__flatbuffers__buffer_identification[version==1],
      comp_req__flatbuffers__version_check[version==1],
      comp_req__flatbuffers__asil[version==1]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}

Dynamic Architecture
********************

.. comp_arc_dyn:: FlatBuffers Dynamic View
   :id: comp_arc_dyn__baselibs__flatbuffers
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :belongs_to: comp__baselibs_flatbuffers
   :fulfils:
      comp_req__flatbuffers__serialization[version==1],
      comp_req__flatbuffers__access[version==1],
      comp_req__flatbuffers__verification[version==1],
      comp_req__flatbuffers__buffer_identification[version==1],
      comp_req__flatbuffers__version_check[version==1],
      comp_req__flatbuffers__asil[version==1]

   Put here a sequence diagram

Interfaces
**********

.. logic_arc_int_op:: LoadBuffer
   :id: logic_arc_int_op__flatbuffers__loadbuffer
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__flatbuffers[version==1]

   | C++: see source_code_link ``*.hpp``
   | Rust: not yet available
   | Python (QM): not yet available

.. logic_arc_int_op:: VersionReader
   :id: logic_arc_int_op__flatbuffers__versionreader
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__flatbuffers[version==1]

   | C++: see source_code_link ``*.hpp``
   | Rust: not yet available
   | Python (QM): not yet available

.. logic_arc_int_op:: GeneratedCode
   :id: logic_arc_int_op__flatbuffers__generatedcode
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__flatbuffers[version==1]

   | C++: see source_code_link starlark rule ``generate_cpp``
   | Rust: not yet available
   | Python (QM): not yet available
