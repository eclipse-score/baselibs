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

Scope Exit
##########

.. document:: Scope Exit
   :id: doc__scope_exit
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__cmpt_request[version==1]

.. toctree::
   :hidden:

   requirements/index.rst
   architecture/index.rst

The Scope Exit component provides move-only ownership utilities for reliable
scope-based cleanup and flag transfer.