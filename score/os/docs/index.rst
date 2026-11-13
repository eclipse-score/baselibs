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

Operating System Abstraction Layer (OSAL)
#########################################

.. document:: OSAL
   :id: doc__osal
   :status: draft
   :version: 2
   :safety: ASIL_B
   :security: YES
   :tags: baselibs_os
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

Abstract
========

The Operating System Abstraction Layer (OSAL) provides C++ interfaces to operating system and C standard library services for S-CORE components.
It supports Linux and QNX through general interfaces and platform-specific extensions.

Motivation and Rationale
========================

Components use OSAL instead of calling supported native APIs directly, separating operating system access from component logic.
Test doubles can replace service implementations to control results and exercise failure handling without exhausting real resources or altering system configuration.

Scope and Use
=============

Use OSAL when a component needs:

- File and directory access, metadata queries, or memory mapping.
- Process and thread control, synchronization, signals, or timers.
- Socket communication, message queues, or other interprocess communication.
- C standard library services such as environment-variable access.

General interfaces cover supported POSIX and C standard library services shared by Linux and QNX.
Platform-specific interfaces expose services of the selected operating system; they do not provide cross-platform emulation.

Safety and Specification
========================

OSAL contains ASIL-B and QM functionality.
Suitability for safety-related use depends on the selected function and its underlying operating system service, not on the interface family alone.
