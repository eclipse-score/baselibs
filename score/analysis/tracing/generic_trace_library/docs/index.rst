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

Generic Trace Library
######################

.. document:: Generic Trace Library
   :id: doc__generic_trace_library
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: NO
   :realizes: wp__cmpt_request[version==1]

.. toctree::
   :hidden:

   requirements/index.rst
   architecture/index.rst
   glossary.rst

The Generic Trace Library (GTL) provides a binding-agnostic API that allows application and middleware
code to register clients and shared-memory areas with, and submit IPC data to, the IPC tracing backend.
