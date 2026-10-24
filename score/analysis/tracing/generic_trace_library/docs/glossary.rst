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

Generic Trace Library Glossary
===============================

.. glossary::
    ShmDataChunkList
      A list of Chunks in which the memory location of the chunk is given as a tuple of a shared memory handle and an offset. The data is then located in some shared memory area (not on the stack or heap).

    LocalDataChunkList
      A list of Chunks in which the memory location of the chunk is just a pointer to the data (data can be located on the stack or heap)

    Client
      Client refers to tuple of an application/process and a binding type that want to use the Tracing feature. This means if an application registerers with two binding types then it's considered two different clients.

    Tracing Backend
      Refers to the component responsible for packaging the raw data and metadata and sending it on the wire.
