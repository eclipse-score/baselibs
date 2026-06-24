Reader
======

.. LogicalUnit:: JsonParser
    :trace: DSGN-JSON-Reader-Deserialization, DSGN-JSON-Reader-Composition-Parser, DSGN-JSON-Reader-Method-Based-Parser

    Contains all logic of the JsonParser.

    .. Unit:: JsonParser
      :safety: ASIL_D

      Contains the logic for a parser based on methods.

    .. Unit:: StartObjectParser
      :safety: ASIL_D

      Contains the logic to parse the start of a JSON object.

    .. Unit:: EndObjectParser
      :safety: ASIL_D

      Contains the logic to parse the end of a JSON object.

    .. Unit:: StartArrayParser
      :safety: ASIL_D

      Contains the logic to parse the start of a JSON array.

    .. Unit:: EndArrayParser
      :safety: ASIL_D

      Contains the logic to parse the end of a JSON array.

    .. Unit:: CompositionParser
      :safety: ASIL_D

      Contains the logic for a parser based on composition.

    .. Unit:: ArrayParser
      :safety: ASIL_D

      Contains the logic to parse a single JSON array.

    .. Unit:: BinaryParser
      :safety: ASIL_D

      Contains the logic to parse a single binary value.

    .. Unit:: BoolParser
      :safety: ASIL_D

      Contains the logic to parse a single bool value.

    .. Unit:: KeyParser
      :safety: ASIL_D

      Contains the logic to parse a single key.

    .. Unit:: NumberParser
      :safety: ASIL_D

      Contains the logic to parse a single number value.

    .. Unit:: ObjectParser
      :safety: ASIL_D

      Contains the logic to parse a single JSON object.

    .. Unit:: StringParser
      :safety: ASIL_D

      Contains the logic to parse a single string value.

    .. uml:: diagrams/JsonParser.puml

.. LogicalUnit:: Parser
    :trace: DSGN-JSON-Reader-Deserialization, DSGN-JSON-Reader-Encoding, DSGN-JSON-Reader-Data-Items,
            DSGN-JSON-Reader-Error-Reporting

    Contains all logic of the Parser.

    .. Unit:: Parser
      :safety: ASIL_D

      Contains the logic for a parser based on callbacks.

    .. Unit:: StrictParser
      :safety: ASIL_D

      Contains the logic for a SAX-style JSON parser based on static polymorphism.

    .. Unit:: VirtualParser
      :safety: ASIL_D

      Contains the logic for a SAX-style JSON parser based on dynamic polymorphism.

    .. Unit:: StructureParser
      :safety: ASIL_D

      Contains the logic for an SAX-style JSON parser.

    .. Unit:: SingleArrayParser
      :safety: ASIL_D

      Contains the logic to parse an array.

    .. Unit:: SingleObjectParser
      :safety: ASIL_D

      Contains the logic to parse an object.

    .. Unit:: LevelValidator
      :safety: ASIL_D

      Contains the logic to check the level of a single element parser.

    .. uml:: diagrams/parser/Parser_v1.puml
    .. uml:: diagrams/parser/Parser_v2.puml

.. LogicalUnit:: Util
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

JsonParser
^^^^^^^^^^

.. uml::

    @startuml
    hide empty members

    package "score" {
      package "json" {

        class JsonParser {
          + JsonParser(JsonData&)
          + auto Validate() const -> Result<void>
          + auto Key<Fn>(Fn const&) -> JsonParser&
          + auto Key(StringView) -> JsonParser&
          + auto Key(CStringView) -> JsonParser&
          + auto Key(ara::core::String) -> JsonParser&
          + auto StartObject() -> JsonParser&
          + auto EndObject() -> JsonParser&
          + auto StartArray() -> JsonParser&
          + auto EndArray() -> JsonParser&
          + auto Bool<Fn>(Fn const&) -> JsonParser&
          + auto String<Fn>(Fn const&) -> JsonParser&
          + auto String(StringView) -> JsonParser&
          + auto String(CStringView) -> JsonParser&
          + auto String(ara::core::String const&) -> JsonParser&
          + auto Number<Fn>(Fn const&) -> JsonParser&
          + auto Binary<Fn>(Fn const&) -> JsonParser&
          + auto Array<Fn>(Fn const&) -> JsonParser&
          + auto StringArray<Fn>(Fn const&) -> JsonParser&
          + auto NumberArray<Fn>(Fn const&) -> JsonParser&
          + auto BoolArray<Fn>(Fn const&) -> JsonParser&
          + auto Object<Fn>(Fn const&, bool) -> JsonParser&
          + auto AddErrorInfo(CStr) -> JsonParser&
          + auto AddErrorInfo(CStringView) -> JsonParser&
          + auto AddErrorInfo<Args>(Args...) -> JsonParser&
        }

        class CompositionParser<VirtualParser> {}

        class VirtualParser {}

        class StructureParser<VirtualParser> {}

        class StartObjectParser {}

        class EndObjectParser {}

        class StartArrayParser {}

        class EndArrayParser {}

        class VirtualParser {}

        class JsonData {}

        class CompositionParser {}
      }
    }

    JsonParser --> CompositionParser
    JsonParser --> StartObjectParser
    JsonParser --> EndObjectParser
    JsonParser --> StartArrayParser
    JsonParser --> EndArrayParser
    JsonParser --> JsonData

    StartObjectParser --|> VirtualParser
    EndObjectParser --|> VirtualParser
    StartArrayParser --|> VirtualParser
    EndArrayParser --|> VirtualParser

    CompositionParser --|> VirtualParser
    CompositionParser --> JsonData

    VirtualParser --> StructureParser

    StructureParser --> JsonData
    StructureParser --|> StructureParserBase

    StructureParser --> VirtualParser

    @enduml

Parser
^^^^^^

.. uml::

    @startuml
    hide empty members

    package score {
      package json {
        package vajson {
        package v2 {

          class Parser {}

        }

        class CompositionParser<VirtualParser> {}

        class VirtualParser {}

        class StructureParser<VirtualParser> {}

        class JsonData {}

        Parser --|> CompositionParser
        Parser --> JsonData

        CompositionParser --|> VirtualParser
        CompositionParser --> JsonData

        VirtualParser --> StructureParser

        StructureParser --> JsonData
        StructureParser --|> StructureParserBase
        StructureParser --> VirtualParser
        }
      }
    }

    @enduml

StrictParser
^^^^^^^^^^^^

.. uml::

    @startuml
    hide empty members

    package "score" {
      package "json" {
        package "vajson" {
          class StrictParser<Child> {
            + StrictParser(JsonData&)
            + auto Parse() -> Result<void>
            + auto SubParse() -> ParserResult
            + auto OnNull() -> ParserResult
            + auto OnBool(bool) -> ParserResult
            + auto OnNumber(util::JsonNumber) -> ParserResult
            + auto OnString(CStringView) -> ParserResult
            + {static} auto OnBinaryString(StringView) -> ParserResult
            + auto OnKey(CStringView) -> ParserResult
            + {static} auto OnBinaryKey(StringView) -> ParserResult
            + auto OnStartObject() -> ParserResult
            + auto OnEndObject(std::size_t) -> ParserResult
            + auto OnStartArray() -> ParserResult
            + auto OnEndArray(std::size_t) -> ParserResult
            + {static} auto OnBinary(Bytes) -> ParserResult
            + {static} auto OnUnexpectedEvent() -> ParserResult

            # auto GetCurrentKey() const -> CStringView
          }

        class StructureParser<StrictParser>
        }
      }
    }

    StrictParser --> StructureParser
    @enduml

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
          + auto SubParse() -> ParserResult

          # auto GetCurrentKey() const -> CStringView
          # auto GetJsonDocument() -> JsonData&
          # auto GetJsonDocument() const -> JsonData const&

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
            + auto SubParse() -> ParserResult
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

CompositionParser
^^^^^^^^^^^^^^^^^

.. uml::

    @startuml
    hide empty members

    package "score" {
      package "json" {
        package "vajson" {
        class CompositionParser<Mixin> {
          + CompositionParser(JsonData&)
          + auto Key<Fn>(Fn) -> CallableReturnsNoResult<Fn, StringView, R>
          + auto Key<Fn>(Fn) -> CallableReturnsResult<Fn, StringView, R>
          + auto Key(StringView) -> R
          + auto Bool<Fn>(Fn) -> CallableReturnsNoResult<Fn, bool, R>
          + auto Bool<Fn>(Fn) -> CallableReturnsResult<Fn, bool, R>
          + auto Number<Fn>(Fn) -> CallableReturnsNoResult<Fn, T, R>
          + auto Number<Fn>(Fn) -> CallableReturnsResult<Fn, T, R>
          + auto String<Fn>(Fn) -> CallableReturnsNoResult<Fn, StringView, R>
          + auto String<Fn>(Fn) -> CallableReturnsResult<Fn, StringView, R>
          + auto String(StringView key) -> R
          + auto NumberArray<Fn>(Fn) -> ArrayCallableReturnsNoResult<Fn, T, R>
          + auto NumberArray<Fn>(Fn) -> ArrayCallableReturnsResult<Fn, T, R>
          + auto StringArray<Fn>(Fn) -> ArrayCallableReturnsNoResult<Fn, StringView, R>
          + auto StringArray<Fn>(Fn) -> ArrayCallableReturnsResult<Fn, StringView, R>
          + auto BoolArray<Fn>(Fn) -> ArrayCallableReturnsNoResult<Fn, bool, R>
          + auto BoolArray<Fn>(Fn) -> ArrayCallableReturnsResult<Fn, bool, R>
          + auto Binary<Fn>(Fn) -> CallableReturnsNoResult<Fn, Bytes, R>
          + auto Binary<Fn>(Fn) -> CallableReturnsResult<Fn, Bytes, R>
          + auto Array<Fn>(Fn) -> CallableReturnsNoResult<Fn, std::size_t, R>
          + auto Array<Fn>(Fn) -> CallableReturnsResult<Fn, std::size_t, R>
          + auto Object<Fn>(Fn, bool) -> CallableReturnsNoResult<Fn, StringView, R>
          + auto Object<Fn>(Fn, bool) -> CallableReturnsResult<Fn, StringView, R>
        }

        class ArrayParser {}

        class ObjectParser {}

        class SingleArrayParser {}

        class SingleObjectParser {}

        class BinaryParser {}

        class BoolParser {}

        class KeyParser {}

        class NumberParser<Fn> {}

        class StringParser {}

        class VirtualParser {}

        class JsonData{}
        }
      }
    }

    ArrayParser --|> SingleArrayParser

    ObjectParser --|> SingleObjectParser

    CompositionParser --> ArrayParser
    CompositionParser --> ObjectParser
    CompositionParser --> BinaryParser
    CompositionParser --> BoolParser
    CompositionParser --> KeyParser
    CompositionParser --> NumberParser
    CompositionParser --> StringParser
    CompositionParser --> JsonData

    BinaryParser --|> VirtualParser
    BoolParser --|> VirtualParser
    KeyParser --|> VirtualParser
    NumberParser --|> VirtualParser
    StringParser --|> VirtualParser

    @enduml

Single Level Parser
^^^^^^^^^^^^^^^^^^^
.. uml::

    @startuml
    hide empty members

    namespace score {
      namespace json {
        namespace vajson {

        namespace v2 {
          class SingleArrayParser {
            + SingleArrayParser(JsonData&)
            + auto OnStartArray() -> ParserResult
            + auto OnEndArray(std::size_t) -> ParserResult
            + auto OnUnexpectedEvent() -> ParserResult
            + auto GetIndex() const -> std::size_t

            # virtual auto OnElement() -> ParserResult
            # virtual auto Finalize() -> ara::core::Result<void>
          }

          class SingleObjectParser {
            + SingleObjectParser(JsonData&, bool)
            + auto OnStartObject() -> ParserResult
            + auto OnEndObject(std::size_t) -> ParserResult
            + auto OnUnexpectedEvent() -> ParserResult

            # virtual auto Finalize() -> ara::core::Result<void>
          }

          class Parser {}

          SingleArrayParser --|> Parser
          SingleArrayParser --> score.json.vajson.LevelValidator

          SingleObjectParser --|> Parser
          SingleObjectParser --> score.json.vajson.LevelValidator
        }

        class LevelValidator {}

        }
      }
    }

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
          + {static} auto FromFile(StringView path) -> Result<JsonData>
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
      Writer -down-> EncodingType

    @enduml
