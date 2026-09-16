..
   # ******************************************************************************
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
   # ******************************************************************************

hash
####

.. document:: Hash Library
   :id: doc__hash
   :status: draft
   :version: 1
   :safety: ASIL_B
   :tags: baselibs_hash
   :realizes: wp__cmpt_request[version==1]
   :security: NO

This component provides IEEE CRC-32 and temporarily retains the native SHA-256
implementation for existing safety-integrity consumers. Cryptographic SHA-256,
SHA-384, and SHA-512 operations are owned by the Security Crypto feature. See
`inc_security_crypto issue #125
<https://github.com/eclipse-score/inc_security_crypto/issues/125>`_ for the
migration context.

.. toctree::
   :hidden:

   requirements/index.rst
