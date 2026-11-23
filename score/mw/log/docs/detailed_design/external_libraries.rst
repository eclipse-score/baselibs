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

.. _component_detailed_design_log_external_libraries:

External Libraries
==================

The ``mw::log`` C++ frontend and the console backend (``//score/mw/log:log``)
use the libraries below from outside the component. Libraries marked with
``FFI`` in Bazel were assessed for freedom from interference. The list is
generated with::

   bazel query "kind('.*_library', deps(//score/mw/log:log))
     except //score/mw/log/... except @bazel_tools//..." --output=label

.. list-table::
   :header-rows: 1
   :widths: 45 15 40

   * - Package
     - Targets
     - ``FFI`` tag
   * - ``//score/bitmanipulation``
     - 1
     - yes
   * - ``//score/concurrency``
     - 2
     - yes
   * - ``//score/filesystem``
     - 9
     - yes
   * - ``//score/json``
     - 10
     - partly, missing: ``//score/json:json``
   * - ``//score/language``
     - 6
     - partly, missing: ``//score/language/futurecpp:futurecpp``, ``//score/language/futurecpp:math``
   * - ``//score/os``
     - 13
     - yes
   * - ``//score/quality``
     - 2
     - yes
   * - ``//score/result``
     - 4
     - yes
   * - ``//score/scope_exit``
     - 2
     - yes
   * - ``//score/static_reflection_with_serialization/visitor``
     - 1
     - yes
   * - ``//score/string_manipulation``
     - 3
     - yes
   * - ``//score/utils``
     - 2
     - yes

The Rust crates ``score_log``, ``score_log_fmt``, ``score_log_fmt_macro``,
``stdout_logger`` and ``stdout_logger_cpp_init`` are part of the component, see
:doc:`rust_bridge_design`. Their external dependencies are the crates
``proc_macro2``, ``quote`` and ``syn`` of ``@score_crates``.
