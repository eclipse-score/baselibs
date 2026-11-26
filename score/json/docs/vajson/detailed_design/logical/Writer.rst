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

The writer contains all logic to format JSON output.
It traces to the functional design features of the writer: Layout, Encoding, Numbers and Error reporting.

``FormattingWriter``
  Contains the logic to write the structure characters of a JSON document in a compact or a pretty-printed layout.

``EscapedJsonString``
  Marks a string that is written to the output as a JSON string literal with the escape sequences of RFC 8259.

The writer is used by the JSON-Wrapper, which traverses the data model and writes each element.
