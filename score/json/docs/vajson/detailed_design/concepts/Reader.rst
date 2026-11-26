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

Polymorphism
============

The parser interface uses dynamic polymorphism.
The event handlers are virtual functions of the abstract class ``StructureParser``.
A user of the parser derives from this class and overrides all event handlers.
The correct event handler is selected at run time.

Advantages:

* No overloaded functions and templates.
* Fewer violations of the coding guidelines.

Disadvantages:

* There may be a performance impact due to the organization of the virtual method table.

..
  The following text is the upstream text of the concept.
  It describes a second interface with static polymorphism that is not part of the S-CORE implementation.

  Concept:: DSGN-Concept-Polymorphism

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
