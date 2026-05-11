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

.. document:: VaJson Requirements
    :id: doc__vajson_requirements
    :status: draft
    :safety: ASIL_B
    :security: YES
    :realizes: wp__requirements_comp

Requirements
============

VaJson needs to implement some of the general requirements defined for the JSON module, particularly:
* :need:`comp_req__json__deserialization`
* :need:`comp_req__json__user_format`

In addition, the following component-specific requirements apply to VaJson:

.. comp_req:: JSON Validation
   :id: comp_req__vajson__validation
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfies: feat_req__baselibs__json_library
   :status: valid
   :belongs_to: comp__baselibs_vajson

   VaJson checks the well-formedness of JSON data.
   Errors shall be reported including the error reason and the location in the JSON document for malformed JSON.

.. comp_req:: JSON Deserialization RFC extension trailing commas
   :id: comp_req__vajson__trailing_commas
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfies: feat_req__baselibs__json_library
   :status: valid
   :belongs_to: comp__baselibs_vajson

   VaJson shall ignore trailing commas. This is an extension to RFC 8259.

.. comp_req:: JSON Deserialization RFC extension hexadecimal integers
   :id: comp_req__vajson__hex_integers
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfies: feat_req__baselibs__json_library
   :status: valid
   :belongs_to: comp__baselibs_vajson

   VaJson shall accept hexadecimal integers. This is an extension to RFC 8259.

.. comp_req:: Unicode Support
   :id: comp_req__vajson__unicode
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfies: feat_req__baselibs__json_library
   :status: valid
   :belongs_to: comp__baselibs_vajson

   VaJson shall encode and decode UTF-8 encoded strings
