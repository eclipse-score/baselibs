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

Utils
#####

.. document:: Utils Library
   :id: doc__utils
   :status: draft
   :version: 2
   :safety: ASIL_B
   :security: YES
   :tags: baselibs_utils
   :realizes: wp__cmpt_request[version==1]

.. toctree::
   :hidden:

   requirements/index.rst
   requirements/chklst_req_inspection.rst
   architecture/index.rst
   architecture/chklst_arc_inspection.rst
   safety_analysis/fmea.rst
   safety_analysis/dfa.rst
   detailed_design/chklst_impl_inspection.rst

The Utils component provides general-purpose C++ utilities for S-CORE platform and application developers.
It covers Base64 encoding and decoding, thread-safe singletons, static destruction ordering guards, and string deduplication with fast view comparisons.
