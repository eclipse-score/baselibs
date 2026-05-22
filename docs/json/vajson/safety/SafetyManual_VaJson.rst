********************
Safety Manual VaJson
********************

Assumed Safety Requirements
===========================

This component assumes the following safety requirements:

.. rst-class:: creq2tsr_table

============================== ==============================================================================  =======================  ======
ID                             Safety Requirement                                                              Associated TSR           ASIL
============================== ==============================================================================  =======================  ======
CREQ-Json-Validation           vaJson shall provide a service to check the well-formedness of JSON data.       TSR-112401, TSR-112402,  ASIL D
                                                                                                               TSR-112404, TSR-112405,
                                                                                                               TSR-112406, TSR-112407,
                                                                                                               TSR-112408, TSR-112409
CREQ-Json-Deserialization      vaJson shall provide a service to parse JSON data according to RFC8259.         TSR-112401, TSR-112402,  ASIL D
                                                                                                               TSR-112404, TSR-112405,
                                                                                                               TSR-112406, TSR-112407,
                                                                                                               TSR-112408, TSR-112409
CREQ-Json-EventCallbacks       vaJson shall provide a service to listen to events for every parsed JSON item.  TSR-112401, TSR-112402,  ASIL D
                                                                                                               TSR-112404, TSR-112405,
                                                                                                               TSR-112406, TSR-112407,
                                                                                                               TSR-112408, TSR-112409
CREQ-Json-Unicode              vaJson shall provide support for UTF-8 encoded strings.                         TSR-112401, TSR-112402,  ASIL D
                                                                                                               TSR-112404, TSR-112405,
                                                                                                               TSR-112406, TSR-112407,
                                                                                                               TSR-112408, TSR-112409
CREQ-Json-Concurrency          vaJson shall provide support for multiple JSON documents concurrently.          TSR-112401, TSR-112402,  ASIL D
                                                                                                               TSR-112404, TSR-112405,
                                                                                                               TSR-112406, TSR-112407,
                                                                                                               TSR-112408, TSR-112409
============================== ==============================================================================  =======================  ======

This component in addition to the assumed safety requirements ensures freedom from interference with respect to memory with up to ASIL D.


Configuration Constraints
=========================

This component does not have configuration constraints.


Additional Verification Measures
================================

.. SMI:: SMI-JSON-PRIVATE-InitializeJSONWithIoIntegrityStream

   **The user of MICROSAR Adaptive Safe shall ensure that IoIntegrityStream is used when initializing vaJson from an input stream where safety requirements are assumed.**

   This SMI is stated as PRIVATE and thus it is applicable only to direct users of vaJson.

   During initialization of vaJson, a user-initialized input filestream can be passed. The Technical Reference uses std::fstream::in as an example.
   If safety requirements are to be assumed, the user shall verify that a stream opened by amsr::iointegritystream::IntegrityFileStream is passed.

   Verification can be performed e.g. by review.

.. SMI:: SMI-JSON-InitializeJSONWithBuffer

   **The user of MICROSAR Adaptive Safe shall ensure the integrity of the character buffer used to initialize vaJson where safety requirements are assumed.**

   During initialization of vaJson, a user-initialized character buffer can be passed. The Technical Reference uses ara::core::StringView and
   ara::core::Span<char> as an example.  If safety requirements are to be assumed, the user shall verify the data integrity of the provided character buffer.

   This SMI is required to ensure freedom from interference of vaJson.
   Verification can be performed e.g. by review.

.. SMI:: SMI-JSON-InitializeJSONWithFilePath

   **The user of MICROSAR Adaptive Safe shall ensure the integrity of a file whose path is used to intialize vaJson where safety requirements are assumed.**

   During initialization of vaJson, a file path can be passed. If safety requirements are to be assumed, the user shall verify the integrity of
   the file to which the path points. The file's integrity must be ensured for as long as the JsonData or ParserFile object exists.

   Verification can be performed e.g. by review, verifying that the file integrity is guaranteed by the file system and the OS.

Safety Requirements Required From Other Components
==================================================

.. SMI:: SMI-JSON-PRIVATE-LibVac

   **This component requires common types/API functionality as an assumed safety requirement (TSR-112401, TSR-112402, TSR-112404-TSR-112409) from LibVac.**

   The used amsr-vector-fs-libvac library shall provide common types/API functionality as safety requirement.
   If the amsr-vector-fs-libvac library from MICROSAR Adaptive Safe is used, this dependency  is fulfilled.

.. SMI:: SMI-JSON-PRIVATE-LibOsAbstraction

   **This component requires general OS related functionality as an assumed safety requirement (TSR-112401, TSR-112402, TSR-112404-TSR-112409) from LibOsAbstraction.**

   This includes functionality for converting integers between host and network byte order.

   The used amsr-vector-fs-libosabstraction library shall provide functionality byte order conversions as safety requirement.
   If the amsr-vector-fs-libosabstraction library from MICROSAR Adaptive Safe is used, this dependency is fulfilled.

.. SMI:: SMI-JSON-PRIVATE-CharacterConversion

   **This component requires conversions of primitive types to strings as an assumed safety requirement (TSR-112401, TSR-112402, TSR-112404-TSR-112409) from CharacterConversion.**

   The used amsr-vector-fs-characterconversion library shall provide functionality to perform conversions between primitive types and strings.
   If the amsr-vector-fs-characterconversion library from MICROSAR Adaptive Safe is used, this dependency is fulfilled.



Dependencies to Hardware
========================

This component does not use a direct hardware interface.
