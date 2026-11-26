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

Writer
======

These are the functionalities of the JSON writer.

Layout
------

The writer writes a JSON document to an output stream in one of two layouts.
The compact layout omits all insignificant whitespace.
The pretty-printed layout starts each element on a new line, indents nested elements by four spaces per level and
writes empty objects and arrays as ``{}`` and ``[]``.
The two layouts differ only in whitespace.

Encoding
--------

The writer writes strings and object member names as JSON string literals.
It escapes the quotation mark, the reverse solidus and the control characters U+0000 to U+001F.
Control characters without a two-character escape sequence are written as ``\uXXXX`` escape sequences.
All other characters are written unchanged, so that UTF-8 encoded strings stay UTF-8 encoded.

Numbers
-------

The writer writes integers and floating-point numbers in decimal notation.
A floating-point number that is infinity or not a number has no representation in RFC 8259.
The writer reports such a number as an error.

Error reporting
---------------

The writer reports a failure of the output stream as an error.
It reports a number that has no JSON representation as an error of a different kind.
