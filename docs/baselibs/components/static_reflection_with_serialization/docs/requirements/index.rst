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

Requirements
############

.. document:: Static Reflection with Serialization Library Requirements
   :id: doc__static_reflect_serial_lib_req
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, static_reflection_serialization_library

.. comp_req:: Static Reflection Support
   :id: comp_req__static_reflect_serial__reflect
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__static_reflection_library[version==1]
   :satisfied_by: comp__baselibs_static_reflection[version==1]
   :status: valid
   :version: 1
   :tags: baselibs

   The library shall provide compile-time reflection-based serialization and deserialization for C++ Data Structures.

.. comp_req:: Generic Visitor Pattern
   :id: comp_req__static_reflect_serial__visitor
   :reqtype: Interface
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__static_reflection_library[version==1]
   :satisfied_by: comp__baselibs_static_reflection[version==1]
   :status: valid
   :version: 1
   :tags: baselibs

   The library shall provide a visitor pattern for traversal of C++ data structures.

.. comp_req:: Automatic Container Iteration
   :id: comp_req__static_reflect_serial__container
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__static_reflection_library[version==1]
   :satisfied_by: comp__baselibs_static_reflection[version==1]
   :status: valid
   :version: 1
   :tags: baselibs

   The library shall automatically traverse containers using appropriate iteration.

.. comp_req:: Nested Type Support
   :id: comp_req__static_reflect_serial__nested
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__static_reflection_library[version==1]
   :satisfied_by: comp__baselibs_static_reflection[version==1]
   :status: valid
   :version: 1
   :tags: baselibs

   The library shall support serialization and visitation of nested data structures.

.. comp_req:: Header-Only Implementation
   :id: comp_req__static_reflect_serial__header_only
   :reqtype: Non-Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__static_reflection_library[version==1]
   :satisfied_by: comp__baselibs_static_reflection[version==1]
   :status: valid
   :version: 1
   :tags: baselibs

   The library shall be implemented as a header-only library to enable compile-time optimizations and avoid runtime dependencies.

.. comp_req:: Compile-Time Efficiency
   :id: comp_req__static_reflect_serial__compile_eff
   :reqtype: Non-Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__static_reflection_library[version==1]
   :satisfied_by: comp__baselibs_static_reflection[version==1]
   :status: valid
   :version: 1
   :tags: baselibs

   The library shall provide compile-time safety and efficiency of serialization operations with minimal runtime overhead.

.. needextend:: "__static_reflect_serial__" in id
   :+tags: static_reflect
