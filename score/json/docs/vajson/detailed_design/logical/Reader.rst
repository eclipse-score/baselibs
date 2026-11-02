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

..
    LogicalUnit:: Parser
    :trace: DSGN-JSON-Reader-Deserialization, DSGN-JSON-Reader-Encoding, DSGN-JSON-Reader-Data-Items,
            DSGN-JSON-Reader-Error-Reporting

    Contains all logic of the Parser.

    .. Unit:: VirtualParser
      :safety: ASIL_D

      Contains the logic for a SAX-style JSON parser based on dynamic polymorphism.

    .. Unit:: StructureParser
      :safety: ASIL_D

      Contains the logic for an SAX-style JSON parser.

..
    LogicalUnit:: Util
    :trace: DSGN-JSON-Reader-Deserialization, DSGN-JSON-Reader-Encoding, DSGN-JSON-Reader-Number-Formatting

    .. Unit:: JsonData
      :safety: ASIL_D

      Contains the logic to implement stateful JSON files.

    .. Unit:: JsonOps
      :safety: ASIL_D

      Contains the logic for operations on a JSON file.

    .. Unit:: DepthCounter
      :safety: ASIL_D

      Contains the logic to track the nesting depth.

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

VirtualParser
^^^^^^^^^^^^^

.. uml::

    @startuml
    hide empty members

    package "score" {
      package "json" {
        package "vajson" {
        class VirtualParser {
          + VirtualParser(JsonData&)
          + virtual auto Parse() -> Result<void>

          - virtual auto OnNull() -> ParserResult
          - virtual auto OnBool(bool) -> ParserResult
          - virtual auto OnNumber(util::JsonNumber) -> ParserResult
          - virtual auto OnString(StringView) -> ParserResult
          - virtual auto OnKey(StringView) -> ParserResult
          - virtual auto OnStartObject() -> ParserResult
          - virtual auto OnEndObject(std::size_t) -> ParserResult
          - virtual auto OnStartArray() -> ParserResult
          - virtual auto OnEndArray(std::size_t) -> ParserResult
          - virtual auto OnBinary(Bytes) -> ParserResult
          - virtual auto OnUnexpectedEvent() -> ParserResult

          - auto OnString(CStringView) -> ParserResult
          - auto OnBinaryString(StringView) -> ParserResult
          - auto OnKey(CStringView) -> ParserResult
          - auto OnBinaryKey(StringView) -> ParserResult
        }

        class StructureParser<VirtualParser>
        }
      }
    }

    VirtualParser --> StructureParser
    @enduml

StructureParser
^^^^^^^^^^^^^^^

.. uml::

    @startuml
    hide empty members

    package "score" {
      package "json" {
        package "vajson" {
          class StructureParser<Implementer> {
            + StructureParser(Implementer&, JsonData&)
          }
          class StructureParserBase<logic> {
          + auto Parse() -> Result<void>
          }

          class OptChar {}

          class JsonOps {}

          class JsonData {}
        }
      }
    }

    StructureParser --> JsonOps
    StructureParser --|> StructureParserBase

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
      package stream {
        class InputStream {}
      }
      package "json" {
        class JsonOps {
          + JsonOps(JsonData& json_data)
        }

        class JsonData {
          + {static} auto FromBuffer(StringView buffer) -> Result<JsonData>
          + {static} auto FromBuffer(ara::core::Span<char const> buffer) -> Result<JsonData>
          + auto GetEncoding() noexcept -> EncodingType&
        }

        class DepthCounter {}
      }
    }

    JsonOps --> JsonData

    JsonData --> InputStream
    JsonData --> DepthCounter

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
              kNotInitialized
              kInvalidJson,
              kUserValidationFailed,
              kStreamFailure
            }

            class JsonErrorDomain {}

            class NumberParser {}

            class JsonNumber {
              + {static} auto New(StringView view) -> Result<JsonNumber>
              + auto TryAs<T>() const -> ara::core::Result<T>
              + auto As()<bool> const -> Optional<bool>
              + auto As()<vac::language::byte> const -> Optional<vac::language::byte>
              + auto As()<T> const -> Optional<T>
              + auto As()<JsonNumber> const -> Optional<JsonNumber>
              + auto Convert<Fn>(Fn&& parser) const -> decltype(parser(std::declval<StringView>()))
            }

            enum EncodingType {
              kNone
              kUtf8
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

      Writer -down-> JsonErrorDomain

    @enduml
