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

Initialization
==============

vaJson intentionally does not provide any initialization or deinitialization methods.
As there is neither a global state nor any global variables to be initialized, it is not necessary to provide such
methods.

Because this design decision is self-contained and there are no elements it could be traced to, it is not marked as a
design feature.
