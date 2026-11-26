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

.. _structural-index:

Structural
**********

vaJson is a library that provides a streaming JSON parser and the formatting of JSON output.

The ``Reader`` subcomponent provides the parser that reads JSON data from a file or a buffer and reports each JSON
element as an event.
The ``Writer`` subcomponent provides the formatting of JSON output in a compact or a pretty-printed layout and the
escaping of strings.
The ``Util`` subcomponent provides generic utilities that the ``Reader`` uses.
