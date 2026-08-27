/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

// Copy from google/flatbuffers, src/idl_parser.cpp (commit 57fdd4f9951b16411bdd823eff08f5d8dea34fde)
// Licensed under the Apache License, Version 2.0.
// Copyright 2014 Google Inc.
// Modifications: Only retain the FLATBUFFERS_VERSION

// below is a copy of the implementation of the FlatBuffer function, which is only declared in the library header.
// clang-format off

#include "flatbuffers/base.h"

namespace flatbuffers {

// Reflects the version at the compiling time of binary(lib/dll/so).
const char* FLATBUFFERS_VERSION() {
  // clang-format off
  return
      FLATBUFFERS_STRING(FLATBUFFERS_VERSION_MAJOR) "."
      FLATBUFFERS_STRING(FLATBUFFERS_VERSION_MINOR) "."
      FLATBUFFERS_STRING(FLATBUFFERS_VERSION_REVISION);
    // clang-format on
}
}  // namespace flatbuffers
// clang-format on
