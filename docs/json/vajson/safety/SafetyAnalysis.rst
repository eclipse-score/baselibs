Safety Analysis of vajson
=========================

The following TSRs are allocated to this component:

- TSR-112401 **The embedded software shall provide a mechanism to initialize itself and its controlled hardware.**
- TSR-112402 **The embedded software shall provide a mechanism to store and retrieve information to/from non-volatile memory by means of a key-value storage.**
- TSR-112404 **The embedded software shall provide mechanisms to protect communication between ECUs.**
- TSR-112405 **The embedded software shall provide mechanisms to communicate between its applications.**
- TSR-112406 **The embedded software shall provide a mechanism to detect faults in program flow.**
- TSR-112407 **The embedded software shall provide a mechanism to detect stuck software.**
- TSR-112408 **The embedded software shall provide a mechanism to detect deadline violations.**
- TSR-112409 **The embedded software shall provide a mechanism to switch between operating modes.**

The component uses the following exit criteria:

.. meta::
  :exitcriterion: LowComplexity


Safety Analysis for TSR-112401, TSR-112402, TSR-112404, TSR-112405, TSR-112406, TSR-112407, TSR-112408, TSR-112409
------------------------------------------------------------------------------------------------------------------

All TSRs can be analyzed together, since for vajson the effect on those TSRs is always equivalent.


- CREQ-Json-Validation *vaJson shall provide a service to check the well-formedness of JSON data.*

    .. fm:: FM-VaJson-InvalidJson Invalid JSON is not recognized or valid JSON is rejected.
        :effect:  Invalid data is deserialized and, thus, safety requirements are violated.
                  Valid JSON is rejected as invalid and, thus, safety requirements are violated.
        :measure: Check for invalid JSON during deserialization.
                  This is verified by the following test cases:
                  - UT__Parser__Invalid__Static
                  - UT__Parser__Invalid__Dynamic

    .. fm:: FM-LibVac-Usage LibVac Common types and API functionality may not be safe.
        :effect: Safety requirement of this component will be compromised.
        :measure: LibVac must adhere to the safety requirements of vaJson, see SMI-JSON-PRIVATE-LibVac.
                    Applicable for all other CREQs and TSRs.

    .. fm:: FM-LibOsAbstraction-Usage LibOsAbstraction functionality may not be safe.
        :effect: Safety requirement of this component will be compromised.
        :measure: LibOsAbstraction must adhere to the safety requirements of vaJson, see SMI-JSON-PRIVATE-LibOsAbstraction.
                    Applicable for all other CREQs and TSRs.

    .. fm:: FM-CharacterConversion-Usage CharacterConversion functionality may not be safe.
        :effect: Safety requirement of this component will be compromised.
        :measure: CharacterConversion must adhere to the safety requirements of vaJson, see SMI-JSON-PRIVATE-CharacterConversion.
                    Applicable for all other CREQs and TSRs.

    - DSGN-JSON-Reader-Error-Reporting
        - JsonParser
        - Parser
        - Cpp
        - Vac

        Analysis can be stopped on this level, since all units are considered low complex.

    Dependencies between all listed units have been checked and all failure modes have been documented.
    There is no interference from any unlisted units since there are no functional dependencies between the unlisted
    units and the analyzed element.

    Functionality is not interfered by other functionality, because there are no dependencies to other functionality.


- CREQ-Json-Deserialization *vaJson shall provide a service to parse JSON data according to RFC8259.*

    .. fm:: FM-VaJson-CorruptedInput Input byte stream is corrupted.
        :details: Not all corruption can be detected by a well-formedness check.
        :effect:  Invalid data is deserialized and, thus, safety requirements are violated.
        :measure: User must provide uncorrupted input stream, e.g., via protected IoIntegrityStream.
                  See SMI-JSON-PRIVATE-InitializeJSONWithIoIntegrityStream.
                  User must provide uncorrupted input character buffer.
                  See SMI-JSON-InitializeJSONWithBuffer.
                  User must not use the file API, since IoIntegrityStream is *not* used there.

    .. fm:: FM-VaJson-InvalidInput Input is grammatically or syntactically invalid.
        :effect:  Invalid input is deserialized and, thus, safety requirements are violated.
        :measure: Check input for well-formedness during deserialization.
                  See CREQ-Json-Validation and related tests.

    .. fm:: FM-VaJson-TrailingCommas: Trailing commas are treated as invalid JSON.
        :effect:  Valid JSON as demanded by the CREQ is rejected as invalid and, thus, safety requirements are violated.
        :measure: Check commas for validity during deserialization.
                  This is verified by the following test cases:
                  - UT__JsonParser__Parsing__Parse

    .. fm:: FM-VaJson-HexIntegers: Hexadecimal integers are treated as invalid JSON.
        :effect:  Valid JSON as demanded by the CREQ is rejected as invalid and, thus, safety requirements are violated.
        :measure: Add support for deserialization of hexadecimal integers.
                  This is verified by the following test cases:
                  - UT__Parser__Number__Static
                  - UT__Parser__Number__Dynamic

    .. fm:: FM-VaJson-NumberConversion: Number conversion changes value.
        :details: Number conversion from textual JSON representation to a numerical value changes the value.
        :effect:  Number reported to the user has a different numerical value than the JSON number and, thus, safety
                  requirements are violated.
        :measure: Use only well-known standard library functions for number conversion.
                  This is verified by the following test cases:
                  - UT__Parser__Number__Static
                  - UT__Parser__Number__Dynamic

    .. fm:: FM-VaJson-NumberLength: JSON numbers can consist of an unlimited number of characters.
        :details: The numerical value of a sufficiently long JSON number cannot be represented by any standard C++ type.
        :effect:  Equivalent to FM-VaJson-NumberConversion.
        :measure: Equivalent to FM-VaJson-NumberConversion.

    .. fm:: FM-VaJson-InvalidCharacters Input contains invalid characters.
        :details: The input contains characters that are not valid, i.e. characters in the range 0x0 to 0x20
                  (inclusive).
        :effect:  Invalid data is deserialized and, thus, safety requirements are violated.
        :measure: Check for invalid characters in the input data during deserialization.
                  This is verified by the following test cases:
                  - UT__Parser__Invalid__Static
                  - UT__Parser__Invalid__Dynamic

    .. fm:: FM-VaJson-InvalidEscapeSequence Input contains invalid escape sequences.
        :details: The input contains invalid escape sequences not specified in the JSON specification.
        :effect:  Equivalent to FM-VaJson-InvalidCharacters.
        :measure: Equivalent to FM-VaJson-InvalidCharacters.

    - DSGN-JSON-Reader-Deserialization
    - DSGN-JSON-Reader-Number-Formatting
        - JsonParser
        - Parser
        - Cpp
        - Vac

        Analysis can be stopped on this level, since all units are considered low complex.

    Dependencies between all listed units have been checked and all failure modes have been documented.
    There is no interference from any unlisted units since there are no functional dependencies between the unlisted
    units and the analyzed element.

    Functionality is not interfered by other functionality, because there are no dependencies to other functionality.


- CREQ-Json-EventCallbacks *vaJson shall provide a service to listen to events for every parsed JSON item.*

    .. fm:: FM-VaJson-CallbackNotCalled User callbacks are not called.
        :details: The parser does not call the user callback for a JSON event.
        :effect:  Deserialized data is lost and, thus, safety requirements are violated.
        :measure: Correct behaviour is verified by the following test cases:
                  - UT__JsonParser__Parsing__Parse

    .. fm:: FM-VaJson-CallbackMismatch Wrong callback is called or called with wrong data.
        :details: The parser calls the wrong callback for the JSON event using the correct data or it calls the correct
                  callback using incorrect data.
        :effect:  User receives incorrect data and, thus, safety requirements are violated.
        :measure: Correct behaviour is verified by the following test cases:
                  - UT__JsonParser__Parsing__Parse

    - DSGN-JSON-Reader-Data-Items
        - JsonParser
        - Parser
        - Cpp
        - Vac

        Analysis can be stopped on this level, since all units are considered low complex.

    Dependencies between all listed units have been checked and all failure modes have been documented.
    There is no interference from any unlisted units since there are no functional dependencies between the unlisted
    units and the analyzed element.

    Functionality is not interfered by other functionality, because there are no dependencies to other functionality.


- CREQ-Json-Unicode *vaJson shall provide support for UTF-8 encoded strings.*

    .. fm:: FM-VaJson-UnicodeStrings Unicode strings are misinterpreted.
        :details: Unicode code points, e.g. control characters, could be misinterpreted when parsed.
        :effect:  User receives incorrect data and, thus, safety requirements are violated.
        :measure: VaJson passes strings to the user as-is by design, no transformation or interpretation takes place.
                  Thus, no misinterpretation can happen and behaviour is not safety relevant.

    .. fm:: FM-VaJson-ByteOrderMark Byte order marks can appear inside strings.
        :details: VaJson cannot report the presence of a byte order mark inside of a string.
        :effect:  User code cannot handle such situations and, thus, safety requirements are violated.
        :measure: Behaviour is not safety relevant and documented in the Technical Reference.

    - DSGN-JSON-Reader-Encoding
        - JsonParser
        - Parser
        - Writer
        - Cpp
        - Vac

        Analysis can be stopped on this level, since all units are considered low complex.

    Dependencies between all listed units have been checked and all failure modes have been documented.

    Functionality is not interfered by other functionality, because there are no dependencies to other functionality.


- CREQ-Json-Concurrency *vaJson shall provide support for multiple JSON documents concurrently.*

    .. fm:: FM-VaJson-Concurrency Concurrent instances interfere with each other.
        :effect:  Interference causes unexpected or undefined behaviour and, thus, safety requirements are violated.
        :measure: VaJson does not have a shared state by design and instances cannot interfere with each other.
                  Behaviour is not safety relevant and documented in the Technical Reference.

    - DSGN-JSON-Reader-Deserialization
        - JsonParser
        - Parser
        - Cpp
        - Vac

        Analysis can be stopped on this level, since all units are considered low complex.

    Dependencies between all listed units have been checked and all failure modes have been documented.
    There is no interference from any unlisted units since there are no functional dependencies between the unlisted
    units and the analyzed element.

    Functionality is not interfered by other functionality, because there are no dependencies to other functionality.
