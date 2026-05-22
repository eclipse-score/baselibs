Templates and Polymorphism
==========================

.. Concept:: DSGN-Concept-Polymorphism

  There are two parser interfaces available, one using static polymorphism and another one using dynamic polymorphism.

  Interface with Static Polymorphism
  ----------------------------------

  A child parser instantiates the parent parser it is derived from using its own type as a template argument.
  This is known as the "Curiously Recurring Template Pattern" (CRTP) or "F-bound polymorphism" and is a form of static
  polymorphism.
  This way, whenever the parent parser accesses the child parser, e.g. to call one of the callbacks, it issues a static
  cast for the child parser's type.
  The child parser does not necessarily need to implement all callbacks.
  If a callback is not implemented by the child parser, i.e. it does not override the corresponding method of the parent
  parser, the method of the parent parser is called instead.

  Advantages:
  - Allows for better optimization

  Disadvantages:
  - Provides a compile-time interface, i.e. it overrides functions

  Interface with Dynamic Polymorphism
  -----------------------------------

  The interface using dynamic polymorphism is a run-time interface, i.e. the correct callbacks are assigned during
  run-time.

  Advantages:
  - No overloaded functions and templates
  - Reduced AUTOSAR code violations

  Disadvantages:
  - There may be a performance impact due to virtual method table (VMU) organization

Fluent Interface
================

  .. Concept:: DSGN-Concept-Fluent-Interface

  The interface of the class JsonParser is fluent as it can be used by method chaining:

  .. code-block:: c++

    JsonParser parser{data};
    parser
      .StartObject()
      .Key("key1")
      .Number<std::uint8_t>(...)
      ...

  Each method returns a reference to itself, allowing the next method call to be appended.
  The class holds an internal state machine to keep track of its error state.
  Subsequent method calls are only executed if no error occurred before.

Parser Mixins
=============

  .. Concept:: DSGN-Concept-Parser-Mixins

  The class CompositionParser is intended to provide pre-defined parser functions to an existing class for commonly
  occurring JSON elements (single values and compounds).
  These pre-defined functions can be called by the user at any time to parse the following JSON element.
