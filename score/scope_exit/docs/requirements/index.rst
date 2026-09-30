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

.. document:: Scope Exit Requirements
   :id: doc__scope_exit_requirements
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__requirements_comp[version==1]
   :tags: requirements, scope_exit

Functional Requirements
=======================

.. comp_req:: Invoke Callback On Destruction
   :id: comp_req__scope_exit__invoke_on_destruction
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_scope_exit[version==1]

   The Scope Exit component shall invoke its stored callback exactly once when the wrapper is destroyed, unless the wrapper has been released or moved from.

.. comp_req:: Release Suppresses Callback
   :id: comp_req__scope_exit__release
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_scope_exit[version==1]

   The Scope Exit component shall provide a release operation that, once invoked on a wrapper, suppresses invocation of that wrapper's stored callback when it is subsequently destroyed.

.. comp_req:: Move Transfers Callback Ownership
   :id: comp_req__scope_exit__move_transfer
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_scope_exit[version==1]

   The Scope Exit component shall transfer ownership of the stored callback to the destination wrapper on move construction and move assignment, without invoking the callback on the moved-from wrapper during the transfer.

.. comp_req:: Move Assignment Invokes Previous Destination Callback
   :id: comp_req__scope_exit__replace_on_assignment
   :reqtype: Functional
   :security: YES
   :safety: ASIL_B
   :derived_from: feat_req__baselibs__utils_library[version==2]
   :status: valid
   :version: 1
   :satisfied_by: comp__baselibs_scope_exit[version==1]

   When move-assigned and the destination wrapper already owns an active callback, the Scope Exit component shall invoke the destination's previous callback before adopting the source wrapper's callback.
