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

JSON Component Architecture
===========================

.. document:: JSON Architecture
   :id: doc__json_architecture
   :status: draft
   :version: 3
   :safety: ASIL_B
   :security: YES
   :realizes: wp__component_arch[version==1]

Overview
--------
The component JSON implements functionality for parsing JSON data into a data model and for writing a data model as JSON data.
It provides only a C++ API.

Terminology
-----------

.. list-table::
   :header-rows: 1
   :widths: 20,80

   * - Term
     - Meaning
   * - Parser
     - Converts JSON data from a file or a buffer into the data model (deserialization in the requirements).
   * - Writer
     - Converts the data model into JSON data in a file or a buffer, either compact or pretty-printed (serialization in the requirements).
   * - Data model
     - The tree of JSON values (object, list, string, number, boolean, null) that the parser returns and the writer consumes.
   * - Declarative conversion
     - Conversion between a user-defined type and the data model by static reflection.
       It is a separate, optional layer on top of the data model.

Requirements Linked to Component Architecture
---------------------------------------------

See the "fulfils" links in static and dynamic architecture below.

Description
-----------

JSON consists of a "Wrapper" and a "vaJson" lower-level component.
The Wrapper provides the public API: the parser and writer interfaces together with their implementations and the data model.
vaJson provides the parsing engine and the output formatting and string escaping that the Wrapper uses.
The Wrapper depends on vaJson; vaJson does not depend on the Wrapper.
There is exactly one parser implementation and exactly one writer implementation.
Neither is selectable at build time or at run time.

Design Decisions:

The vaJson parser from Vector (see :need:`doc__vajson`) is used for parsing, because it comes with ASIL D qualification evidence.

JSON writing is built on the vaJson output formatting and string escaping.
The writer produces compact and pretty-printed output from the same implementation.

The public API consists of interfaces (parser and writer) so that users can replace them by test doubles.

Design Constraints:

The public API and the observable behavior of the parser and the writer do not depend on the internal structure of vaJson.

Rationale Behind Architecture Decomposition
*******************************************

The decomposition in a wrapper and a vaJson component separates the stable public API and data model from the imported parsing engine.
This keeps the imported code, and the evidence that comes with it, isolated from the API that users depend on.

Static Architecture
-------------------

.. comp:: JSON
   :id: comp__baselibs_json
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 2
   :implements: logic_arc_int__baselibs__json[version==1]
   :consists_of: comp__baselibs_json_wrapper[version==1], comp__baselibs_vajson[version==1]
   :belongs_to: feat__baselibs[version==1]

.. comp_arc_sta:: JSON Static view
   :id: comp_arc_sta__baselibs__json
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 6
   :fulfils: comp_req__json__deserialization[version==1], comp_req__json__serialization[version==1], comp_req__json__user_format[version==1], comp_req__json__duplicate_member_names[version==1], comp_req__json__parsing_error_reason[version==1], comp_req__json__io_error[version==1], comp_req__json__regular_file[version==1], comp_req__json__max_file_size[version==1], comp_req__json__deserialization_error_field[version==1], comp_req__json__lang_idioms[version==1], comp_req__json__lang_infra[version==1], comp_req__json__type_compatibility[version==1], comp_req__json__out_of_range_error[version==1], comp_req__json__full_testability[version==1], comp_req__json__asil[version==1]
   :belongs_to: comp__baselibs_json[version==2]

   .. needarch::
      :scale: 50
      :align: center

      {{ draw_component(need(), needs) }}


Dynamic Architecture
--------------------

.. comp_arc_dyn:: JSON parse dynamic view
   :id: comp_arc_dyn__baselibs__json_dynamic_view
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 7
   :fulfils: comp_req__json__deserialization[version==1], comp_req__json__user_format[version==1], comp_req__json__duplicate_member_names[version==1], comp_req__json__parsing_error_reason[version==1], comp_req__json__io_error[version==1], comp_req__json__regular_file[version==1], comp_req__json__max_file_size[version==1]
   :belongs_to: comp__baselibs_json[version==2]

   .. uml::

      @startuml
      hide footbox

      actor User
      participant "JSON-Wrapper\n(JsonParser)" as Wrapper
      participant "vaJson\n(JsonData, StructureParser)" as VaJson

      User -> Wrapper : FromFile(path) / FromBuffer(buffer)
      Wrapper -> VaJson : load document
      alt file cannot be read, is not a regular file or is larger than 64 MiB
        VaJson --> Wrapper : error
        Wrapper --> User : I/O error
      else document loaded
        Wrapper -> VaJson : Parse()
        loop for each JSON element
          VaJson -> Wrapper : event (value, key, start/end of object or list)
          Wrapper -> Wrapper : add element to the data model
        end
        alt malformed JSON or duplicate member name
          Wrapper --> User : parsing error with reason
        else valid
          Wrapper --> User : data model
        end
      end
      @enduml

.. comp_arc_dyn:: JSON write dynamic view
   :id: comp_arc_dyn__baselibs__json_write_view
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :fulfils: comp_req__json__serialization[version==1], comp_req__json__io_error[version==1]
   :belongs_to: comp__baselibs_json[version==2]

   .. uml::

      @startuml
      hide footbox

      actor User
      participant "JSON-Wrapper\n(JsonWriter)" as Wrapper
      participant "vaJson\n(output formatting and escaping)" as VaJson

      User -> Wrapper : ToFile(data, path) / ToBuffer(data)
      opt ToFile
        Wrapper -> Wrapper : open file
        alt file cannot be opened
          Wrapper --> User : I/O error
        end
      end
      loop for each element of the data model
        Wrapper -> VaJson : write element (compact or pretty-printed)
      end
      alt stream failure
        Wrapper --> User : I/O error
      else number without JSON representation
        Wrapper --> User : invalid value error
      else success
        Wrapper --> User : success / JSON string
      end
      @enduml


Interfaces
----------

.. logic_arc_int_op:: Parse
   :id: logic_arc_int_op__baselibs__parse
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__json[version==1]

.. logic_arc_int_op:: Write
   :id: logic_arc_int_op__baselibs__write
   :security: YES
   :safety: ASIL_B
   :status: valid
   :version: 1
   :included_by: logic_arc_int__baselibs__json[version==1]

Lower Level Components
----------------------

.. comp:: JSON-Wrapper
   :id: comp__baselibs_json_wrapper
   :security: YES
   :safety:  ASIL_B
   :status: valid
   :version: 1
   :implements: logic_arc_int__baselibs__json[version==1]
   :belongs_to: feat__baselibs[version==1]

See :need:`comp__baselibs_vajson` in :need:`doc__vajson_architecture` for the vaJson lower-level component.
