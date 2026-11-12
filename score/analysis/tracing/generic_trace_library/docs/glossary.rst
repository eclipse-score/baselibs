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
    Chunk
      A contiguous block of memory that holds a portion of the trace data.

    Client
      Tuple of an application/process and a binding type that want to use the Tracing feature. This means if an application registers with two binding types then it's considered two different clients.

    LocalDataChunkList
      A list of :term:`Chunk` in which the memory location of the :term:`Chunk` is just a pointer to the data (data can be located on the stack or heap)

    ShmDataChunkList
      A list of :term:`Chunk` in which the memory location of the :term:`Chunk` is given as a tuple of a shared memory handle and an offset. The data is then located in some shared memory area (not on the stack or heap).

    Tracing Backend
      The component responsible for packaging the raw data and metadata and sending it on the wire.
