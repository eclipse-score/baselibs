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

.. document:: VaJson Architecture
    :id: doc__vajson_architecture
    :status: draft
    :version: 2
    :safety: ASIL_B
    :security: YES
    :realizes: wp__component_arch[version==1]

VaJson Component Architecture
=============================

VaJson is the lower-level component of the JSON component, see :need:`doc__json_architecture`.
It consists of a parser part and a writer part:

* The parser reads JSON data from a file or a buffer and reports each JSON element as an event to the user of VaJson.
  The JSON-Wrapper uses these events to build the data model.
* The writer formats JSON data for output in a compact or a pretty-printed layout and escapes strings.

The parser part and the writer part do not depend on each other.
VaJson does not depend on the JSON-Wrapper.
The structure of the parts is described in :need:`doc__vajson_detailed_design`.

.. comp:: VaJson
   :id: comp__baselibs_vajson
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :belongs_to: feat__baselibs[version==1]
