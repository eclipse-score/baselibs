..
   # ******************************************************************************
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
   # ******************************************************************************

Hash
####

.. document:: Hash Library
   :id: doc__hash
   :status: draft
   :version: 1
   :safety: ASIL_B
   :tags: baselibs_hash
   :realizes: wp__cmpt_request[version==1]
   :security: NO

The Hash library lets C++ application developers calculate and compare checksums and hash values for data-integrity checks.
This overview covers supported algorithms, SHA-256 variants, CRC-32 customization for S-CORE integrators, input handling, and usage constraints.

.. toctree::
   :hidden:

   requirements/index.rst

Supported algorithms
====================

The library provides the following algorithms and digest sizes:

.. list-table::
   :header-rows: 1

   * - Algorithm
     - Digest size (bytes)
   * - CRC-32
     - 4
   * - SHA-256
     - 32
   * - SHA-384
     - 48
   * - SHA-512
     - 64

By default, the general-purpose factory provides all four algorithms.
The safety-oriented factory provides CRC-32 and SHA-256 without an OpenSSL dependency.

SHA-256 variants
================

The library provides a native SHA-256 implementation for safety-oriented use and an OpenSSL-based SHA-256 implementation for general-purpose use.
Both default factories select the native implementation for SHA-256.
The general-purpose factory selects OpenSSL for SHA-384 and SHA-512.
To use OpenSSL-based SHA-256, create a calculator through the OpenSSL-specific interface instead of either default factory.

CRC-32 customization
====================

The default CRC-32 calculator uses the IEEE 802.3 polynomial, ``0x04C11DB7`` (reflected form: ``0xEDB88320``).
S-CORE integrators can supply a CRC-32 calculator with a different polynomial and select it at build time.
The general-purpose and safety-oriented variants have separate Bazel build settings for this selection.

Each setting selects a complete factory implementation, not an individual CRC-32 calculator or polynomial value.
The replacement must provide the corresponding factory, including the algorithm selection required by its consumers.
The polynomial is an integration-time choice rather than a runtime parameter.

Input and results
=================

You can calculate a digest from a byte buffer or an input stream, either in one operation or incrementally across multiple blocks.
You can also limit stream processing to a specified number of bytes.

Each hash value retains its algorithm and digest bytes.
You can parse hexadecimal text into a hash value or format a digest as lowercase hexadecimal text.
Two hash values compare equal only when both the algorithm and digest bytes match.

Usage constraints
=================

The native SHA-256 implementation supports little-endian platforms.
For native SHA-256, byte-buffer updates must contain at least one byte.

.. caution::

   Use the native SHA-256 implementation only for non-cryptographic purposes, such as data-integrity checks.
   Do not use it for security-sensitive applications, including password hashing, digital signatures, or key derivation.
