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

Reader
======

Parser
------

The parser contains all logic of the parser.
It traces to the functional design features Deserialization, Encoding, Data Items and Error Reporting.

``StructureParser``
  Contains the logic for a SAX-style JSON parser based on dynamic polymorphism.

``StructureValidator``
  Contains the logic to check the order of the tokens of a JSON document.

Util
----

The utilities trace to the functional design features Deserialization, Encoding and Number Formatting.

``JsonData``
  Contains the logic to implement stateful JSON documents that are loaded from a file or a buffer.

``JsonOps``
  Contains the logic for operations on a JSON document.

``OptChar``
  Contains a union over end of file and a character.

``JsonNumber``
  Contains the logic to handle JSON numbers.

``NumberParser``
  Contains the logic to parse strings as numbers.

``JsonErrc``
  Contains the vaJson error codes.

``JsonErrorDomain``
  Contains the vaJson error domain.

.. uml:: diagrams/Util.puml

..
    The following text is the upstream text of the logical design, with the ASIL D units of the upstream product.

    LogicalUnit:: Parser
    :trace: DSGN-JSON-Reader-Deserialization, DSGN-JSON-Reader-Encoding, DSGN-JSON-Reader-Data-Items,
            DSGN-JSON-Reader-Error-Reporting

    Contains all logic of the Parser.

    .. Unit:: StructureParser
      :safety: ASIL_D

      Contains the logic for a SAX-style JSON parser based on dynamic polymorphism.

    .. Unit:: StructureValidator
      :safety: ASIL_D

      Contains the logic to check the order of the tokens of a JSON document.

..
    LogicalUnit:: Util
    :trace: DSGN-JSON-Reader-Deserialization, DSGN-JSON-Reader-Encoding, DSGN-JSON-Reader-Number-Formatting

    .. Unit:: JsonData
      :safety: ASIL_D

      Contains the logic to implement stateful JSON files.

    .. Unit:: JsonOps
      :safety: ASIL_D

      Contains the logic for operations on a JSON file.

    .. Unit:: OptChar
      :safety: ASIL_D

      Contains a union over end of file and a character.

    .. Unit:: JsonNumber
      :safety: ASIL_D

      Contains the logic to handle JSON numbers.

    .. Unit:: NumberParser
      :safety: ASIL_D

      Contains the logic to parse strings as numbers.

    .. Unit:: JsonErrc
      :safety: ASIL_D

      Contains the vaJson error codes.

    .. Unit:: JsonErrorDomain
      :safety: ASIL_D

      Contains the vaJson error domain.

    .. Unit:: JsonException
      :safety: ASIL_D

      Contains the vaJson exception type.

    .. uml:: diagrams/Util.puml


Diagrams
--------

StructureParser
^^^^^^^^^^^^^^^

.. uml::

    @startuml
    hide empty members

    package "score" {
      package "json" {
        package "vajson" {
          abstract class StructureParser {
            + StructureParser(JsonData&)
            + auto Parse() -> Result<void>

            - {abstract} virtual auto OnNull() -> ParserResult
            - {abstract} virtual auto OnBool(bool) -> ParserResult
            - {abstract} virtual auto OnNumber(JsonNumber) -> ParserResult
            - {abstract} virtual auto OnString(CStringView) -> ParserResult
            - {abstract} virtual auto OnKey(CStringView) -> ParserResult
            - {abstract} virtual auto OnStartObject() -> ParserResult
            - {abstract} virtual auto OnEndObject() -> ParserResult
            - {abstract} virtual auto OnStartArray() -> ParserResult
            - {abstract} virtual auto OnEndArray() -> ParserResult
          }

          class OptChar {}

          class JsonOps {}

          class JsonData {}

          class StructureValidator {}
        }
      }
    }

    StructureParser --> JsonOps

    StructureParser --> StructureValidator

    StructureParser --> JsonData

    JsonOps --> JsonData
    JsonOps --> OptChar
    @enduml

JsonData
^^^^^^^^

.. uml::

    @startuml
    hide empty members

    package "score" {
      package "json" {
        class JsonOps {
          + JsonOps(JsonData& json_data)
        }

        class JsonData {
          + {static} auto FromFile(StringView path, std::size_t max_size) -> Result<JsonData>
          + {static} auto FromBuffer(StringView buffer) -> Result<JsonData>
        }
      }
    }

    JsonOps --> JsonData

    @enduml

Util
^^^^

.. uml::

    @startuml
    hide empty members

      package "score" {
        package "json" {
          package "vajson" {
            enum JsonErrc {
              kInvalidJson,
              kStreamFailure
            }

            class JsonErrorDomain {}

            class NumberParser {}

            class JsonNumber {
              + {static} auto New(StringView view) -> Result<JsonNumber>
              + auto As()<T> const -> Optional<T>
            }

          package Reader {
          }

          package Writer {
          }
          }
        }
      }

      JsonNumber --> NumberParser

      Reader -down-> JsonNumber
      Reader -down-> JsonErrorDomain
      Reader -down-> JsonErrc

    @enduml
