.. _structural-index:

Structural
**********

*Vector Adaptive Json* (vaJson) is a library that provides utilities to create streaming JSON serializer & deserializer.

The ``Reader`` subcomponent provides utilities to deserialize/read JSON files.
The ``Writer`` subcomponent provides utilities to serialize/write JSON files.
The ``Util`` subcomponent provides generic utilities.

.. uml::

    @startuml

    [libCharConv] <<library>>
    [libIoStream] <<library>>
    [libOsAbstraction Utils] <<library>>
    [libVac] <<library>>

    package vaJson <<library>> {
    [Reader]
    [Writer]
    [Util]
    }

    vaJson ..> libCharConv : uses
    vaJson ..> libIoStream : uses
    vaJson ..> [libOsAbstraction Utils] : uses
    vaJson ..> libVac : uses

    Reader ..> Util : uses
    Writer ..> Util : uses

    @enduml
