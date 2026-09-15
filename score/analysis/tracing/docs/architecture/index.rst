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

Analysis Tracing Architecture
##############################

.. document:: Analysis Tracing Architecture
   :id: doc__analysis_tracing_architecture
   :status: draft
   :version: 1
   :safety: ASIL_B
   :security: YES
   :realizes: wp__component_arch[version==1]

.. comp:: Analysis Tracing
   :id: comp__baselibs_analysis_tracing
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :belongs_to: feat__baselibs[version==1]

   The Analysis Tracing component contains tracing utilities for detecting corruption of wrapped data.