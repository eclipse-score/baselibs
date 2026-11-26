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

JSON-Library
############

.. document:: JSON-Library
   :id: doc__json
   :status: valid
   :version: 2
   :safety: ASIL_B
   :security: YES
   :tags: component_request
   :realizes: wp__cmpt_request[version==1]


.. toctree::
   :hidden:

   requirements/index.rst
   requirements/chklst_req_inspection.rst
   architecture/index.rst
   architecture/chklst_arc_inspection.rst
   json_wrapper/index.rst
   vajson/index.rst
   safety_analysis/fmea.rst
   safety_analysis/dfa.rst
   detailed_design/chklst_impl_inspection.rst


Abstract
========

This component request describes a safe JSON-Library that parses JSON data into a data model and writes a data model as JSON data.
The library has one parser implementation and one writer implementation.


Basic Functionality
----------------------
The component supports parsing JSON data and converting that information to a compatible user format.
The component supports writing the user format as compact or pretty-printed JSON data.

Motivation
==========

S-CORE is targeting high-performance automotive systems with safety impact.
Some applications, like the IPC communication solution, require a safe JSON-Library with parsing functionality to enable loading of configuration information at runtime.
There is currently no solution for this inside S-CORE.

Rationale
=========

A JSON-Library with parsing and writing functionality is part of the S-CORE software platform due to specific functional dependencies of different features (ex: IPC).

Further details are available in the following section:

.. _Specification:

Specification
=============

The following details and requirements describe the aspects of current feature in the context of S-CORE.

General considerations
----------------------

The JSON-Library should provide parsing, writing and data conversion capabilities:

* :need:`comp_req__json__deserialization`
* :need:`comp_req__json__serialization`
* :need:`comp_req__json__user_format`
* :need:`comp_req__json__duplicate_member_names`
* :need:`comp_req__json__parsing_error_reason`
* :need:`comp_req__json__io_error`
* :need:`comp_req__json__regular_file`
* :need:`comp_req__json__max_file_size`
* :need:`comp_req__json__deserialization_error_field`

User friendly API
-----------------

Programming languages have their own feature set and idioms.
It is crucial for any library that it seamlessly integrates into both.
This means, wherever possible and meaningful, infrastructure of the programming language and accompanying standard
libraries shall be reused.
Further, a developer used to the programming language shall have no problems understanding the API.
It should feel natural to use.
This includes error handling, which shall follow one of the error handling concepts of the programming language.

* :need:`comp_req__json__lang_idioms`
* :need:`comp_req__json__lang_infra`
* :need:`comp_req__json__type_compatibility`
* :need:`comp_req__json__out_of_range_error`

Full testability for the public API
----------------------------------------

Our users will be required to proof certain coverage metrics, like line coverage or branch coverage.
For them to reach full coverage, they need to be easily able to mock or fake the public API of the JSON-Library in their unit tests.

* :need:`comp_req__json__full_testability`

Backwards Compatibility
=======================

The public API and the observable behavior of the parser and the writer are unchanged by the consolidation on one parser and one writer implementation.
The Bazel flag ``base_library`` is a deprecated no-op that is kept until its planned removal.

Security Impact
===============

The parser processes data that may come from an untrusted source.
It rejects malformed JSON, files that are not regular files and files larger than 64 MiB.
The integrity and the access control of the JSON data are Assumptions of Use:

* :need:`aou_req__json__data_integrity`
* :need:`aou_req__json__access_control`

Safety Impact
=============

Overall, the component shall support use cases up to ASIL-B:

* :need:`comp_req__json__asil`

The failure modes of the component and their mitigations are analyzed in :need:`doc__json_fmea` and :need:`doc__json_dfa`.

License Impact
==============

The vaJson parser is licensed under the Apache License Version 2.0, see :need:`doc__vajson`.
No other third-party JSON library is used.

How to Teach This
=================

The usage of the parser, the writer and the data model is described in the README of the component.
