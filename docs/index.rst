..
    *******************************************************************************
    Copyright (c) 2026 Contributors to the Eclipse Foundation

    See the NOTICE file(s) distributed with this work for additional
    information regarding copyright ownership.

    This program and the accompanying materials are made available under the
    terms of the Apache License Version 2.0 which is available at
    https://www.apache.org/licenses/LICENSE-2.0

    SPDX-License-Identifier: Apache-2.0
    *******************************************************************************

Base Libraries
==============

This repository is the `Eclipse S-CORE <https://eclipse.dev/score/>`_ implementation of the `Base Libraries feature <https://eclipse-score.github.io/score/main/features/baselibs/index.html>`_.
It provides foundational libraries for use across the S-CORE platform, offering common functionality that ensures consistent implementations,
reduces code duplication, and promotes interoperability between components.

Libraries are developed following the `Eclipse S-CORE Process Description <https://github.com/eclipse-score/process_description>`_
at ISO 26262 integrity levels from QM up to ASIL-B, depending on their intended use cases.

For build instructions, usage examples, and configuration options, refer to the `README <https://github.com/eclipse-score/baselibs/blob/main/README.md>`_ in the `eclipse-score/baselibs <https://github.com/eclipse-score/baselibs>`_ repository.

.. contents:: Table of Contents
   :depth: 2
   :local:

.. toctree::
   :titlesonly:
   :hidden:
   :glob:

   baselibs/feature/index
   baselibs/module/index
   baselibs/components/index

Documentation Structure
-----------------------

Feature
~~~~~~~

The :doc:`Feature documentation <baselibs/feature/index>` covers the feature-level definition of Base Libraries,
including architecture and safety planning artifacts.

Module
~~~~~~

The :doc:`Module documentation <baselibs/module/index>` covers the module-level view of Base Libraries,
including architecture, safety management documents, and the user manual.

Components
~~~~~~~~~~

The :doc:`Components documentation <baselibs/components/index>` provides detailed documentation for each
individual library component, including requirements, architecture, and design decisions.
