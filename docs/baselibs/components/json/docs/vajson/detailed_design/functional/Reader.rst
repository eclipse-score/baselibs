Reader
======

These are the functionalities of the JSON reader.

Deserialization
---------------
.. DesignFeature:: DSGN-JSON-Reader-Deserialization
    :safety: ASIL_D
    :trace: CREQ-Json-Deserialization, CREQ-Json-Concurrency

    Reading of JSON documents is done by the class score::json::vajson::internal::StructureParser by implementing a SAX-style
    JSON parser.
    The JSON data must be provided to the parser as
    * a path to a file
    * a character buffer
    * a stream, e.g. a file or string stream.

    The JSON document size and the number of concurrently parsed documents is not restricted by vaJson.

Sequences
^^^^^^^^^

.. uml::

    @startuml
    !include diagrams/sequences.puml
    @enduml

Encoding
--------
.. DesignFeature:: DSGN-JSON-Reader-Encoding
    :safety: ASIL_D
    :trace: CREQ-Json-Unicode

    Since all JSON characters are ASCII compliant and the score::json::vajson::internal::StructureParser just passes the data
    from the input stream to the event handler the user must handle the encoding.

    The method ``score::json::vajson::JsonData::GetEncoding()`` returns the detected encoding if the input stream contains a byte
    order mark (BOM).


Data Items
----------
.. DesignFeature:: DSGN-JSON-Reader-Data-Items
    :safety: ASIL_D
    :trace:  CREQ-Json-EventCallbacks, DSGN-Concept-Polymorphism, DSGN-Concept-ProprietaryJson

    The parser creates events for each detected JSON element of the input data and reports them to an event handler that
    must be implemented by the application.
    The event handler must inherit from score::json::vajson::internal::StructureParser and can implement the following functions:

    * "null" is reported to the applications event handler by calling ``OnNull()``
    * "true" is reported to the applications event handler by calling ``OnBool(bool)``
    * "false" is reported to the applications event handler by calling ``OnBool(bool)``
    * A number is reported to the applications event handler by calling ``OnNumber(score::json::vajson::JsonNumber)``
    * A string is reported to the applications event handler by calling ``OnString(ara::core::StringView)``
    * A key is reported to the applications event handler by calling ``OnKey(ara::core::StringView)``
    * The start of an object is reported to the applications event handler by calling ``OnStartObject()``
    * The end of an object is reported to the applications event handler by calling ``OnEndObject(std::size_t)``
    * The start of an array is reported to the applications event handler by calling ``OnStartArray()``
    * The end of an object is reported to the applications event handler by calling ``OnEndArray(std::size_t)``
    * A binary value is reported to the applications event handler by calling ``OnBinary(ara::core::Span<char>)``
    * The default action for any of the above methods is to call ``OnUnexpectedEvent()``
      The default implementation returns an error.

    Each function returns an ``ara::core::Result<score::json::vajson::ParserState>`` to report errors and to indicate whether the
    parser shall go on (``score::json::vajson::ParserState::kRunning``) or stop (``score::json::vajson::ParserState::kFinished``).

    A hierarchical parser can be implemented by the application by providing event handlers spread over several classes
    that are instantiated by themselves depending on the last event.


Number formatting
-----------------
.. DesignFeature:: DSGN-JSON-Reader-Number-Formatting
    :safety: ASIL_D
    :trace: CREQ-Json-Deserialization

    A Number of the input stream is reported to the applications event handler by calling OnNumber().
    The parameter of the function (score::json::vajson::JsonNumber) contains the number and can provide it in different formats
    to the application:

    * boolean
    * unsigned int (any unsigned integer smaller or equal to unsigned long long int)
    * signed int (any signed integer smaller or equal to long long int)
    * float
    * double
    * score::json::vajson::JsonNumber (for further processing by the application)

    The application can also try to convert the number to a user defined format by calling
    score::json::vajson::JsonNumber::Convert.


Error reporting
---------------
.. DesignFeature:: DSGN-JSON-Reader-Error-Reporting
    :safety: ASIL_D
    :trace: CREQ-Json-Validation

    Upon detection of an error, the parser returns from Parse() with an ara::core::Result<void> and one of the errors
    defined in score::json::vajson::JsonErrc.
    Also, the position in the stream where the error has occurred is reported.


Composition Parser
------------------

.. DesignFeature:: DSGN-JSON-Reader-Composition-Parser
    :trace: DSGN-Concept-Parser-Mixins

    The class CompositionParser takes a template parameter from which it inherits.
    In order to use the class and the provided convenience methods, a user defined class must inherit from
    CompositionParser and provide its own type as the template argument.
    This way, the public methods of CompositionParser are available in the child class.


Method Based Parser
-------------------

.. DesignFeature:: DSGN-JSON-Reader-Method-Based-Parser
    :trace: DSGN-Concept-Fluent-Interface

    The class JsonParser provides an interface that allows to parse JSON documents by calling specific methods for each
    expected element.
    It provides methods for every possible JSON element as well as convenience methods for JSON arrays of a known type,
    e.g. an array of strings.
    The fluent interface allows for method chaining.
