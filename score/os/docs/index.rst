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

Overview
========

Use the Operating System Abstraction Layer (OSAL) when your component needs C++ access to supported POSIX, C standard library, Linux, or QNX services.
OSAL separates native service calls from component logic and supports controlled testing of service results and failures.

This page introduces the component's service scope, platform behavior, testability, and safety classification.
For normative behavior and architecture allocation, see :need:`doc__osal_lib_requirements` and :need:`doc__osal_architecture`.

Service scope
=============

OSAL provides interfaces for the following service groups:

- File and directory access, metadata queries, and memory mapping.
- Process and thread control, synchronization, signals, and timers.
- Socket communication, message queues, and other interprocess communication.
- C standard library services, including environment-variable access.

Platform behavior
=================

General interfaces expose services supported on Linux and QNX.
Platform-specific interfaces expose services available only on the selected operating system; OSAL does not emulate those services on other platforms.

Testability
===========

Selected interfaces provide replaceable implementations and mocks.
Tests can use these seams to control service results and failures.

Safety classification
=====================

OSAL includes ASIL-B and QM functionality.
Safety-related use depends on the selected function and its underlying operating system service.
The general, Linux, or QNX interface family alone does not determine the safety classification.
