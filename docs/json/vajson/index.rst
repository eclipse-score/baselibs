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

VaJson
######

.. document:: vajson
   :id: doc__vajson
   :status: valid
   :safety: ASIL_B
   :security: NO
   :realizes: wp__cmpt_request
   :tags: component_request


Abstract
========

This component request proposes adding VaJson as a JSON parser backend for Eclipse S-CORE.

This component request describes a JSON parser to be used within applications of the Eclipse S-CORE project.

This requests adds VaJson, a SAX-based JSON parser for Eclipse S-CORE applications that enables parsing of JSON data for configuration and data exchange.


Motivation
==========

VaJson provides a safety-qualified JSON parser designed for automotive environments.
It has passed Vector's internal ASIL D qualification process and comes with supporting safety artifacts.

Artifacts such as the safety manual, safety analysis, cybersecurity analysis, and detailed design significantly reduce the effort needed to adapt the component to S-CORE qualification processes.
Its API follows established automotive patterns, which improves integration consistency and reduces project risk.



Rationale
=========

VaJson was selected because it already has ASIL D qualification evidence (within Vector processes) and includes safety and cybersecurity artifacts.
This reduces the effort to satisfy S-CORE internal process requirements.

The SAX-based design is efficient for embedded systems and avoids the memory overhead of fully materialized document models.

Its API style is familiar in automotive projects, making adoption easier and less risky than introducing a new or generic parser.


Specification
=============

VaJson meets the generic requirements for a JSON deserialization library.

* :need:`comp_req__json__deserialization`
* :need:`comp_req__json__user_format`

In addition, VaJson satisfies the extended requirements defined in :need:`doc__vajson_requirements`:

* :need:`comp_req__vajson__validation`
* :need:`comp_req__vajson__trailing_commas`
* :need:`comp_req__vajson__hex_integers`
* :need:`comp_req__vajson__unicode`

Backward Compatibility
======================

VaJson is integrated as an optional backend through the existing S-CORE JSON wrapper.
Users can select the JSON backend (for example, VaJson or nlohmann), so existing functionality remains backward compatible.


Security Impact
===============

Potential security concerns and mitigations for VaJson:

- Malformed JSON Attacks

  Threat: Supplying intentionally malformed or deeply nested JSON to cause excessive CPU or memory consumption.
  Mitigation: Handled by design in VaJson. The SAX-based streaming parser detects malformed input during parsing and stops processing invalid data early.

- Schema Abuse

  Threat: Using oversized keys/values or unexpected schema elements to bypass validation or trigger buffer overflows.
  Mitigation: Handled by design in VaJson. Input size and structure are validated during parsing.



Safety Impact
=============

A full safety analysis for VaJson exists in an external format and addresses all identified concerns with appropriate measures.
Example:

FM-VaJson-NumberConversion

Details: Number conversion from textual JSON representation to a numerical value could change the value.
Effect: Incorrect numerical values may violate safety requirements.
Measure: Use only well-known standard library functions for number conversion, verified through unit testing.

All concerns listed in the external safety analysis have been implemented and verified in the source code and tests.

**Expected ASIL Level:**

ASIL B.

**Classification:**

Safety-relevant component.


**Safety Related Functionality Required From Other Components**

The Assumptions of Use which apply to the JSON modules public interface also apply to VaJson:

* :need:`aou_req__json__data_integrity`
* :need:`aou_req__json__access_control`

License Impact
==============

None. VaJson was previously released under a commercial license, but is now open sourced under the Apache License Version 2.0.


How to Teach This
=================

- Update baselibs/README.md

  Clearly document the available JSON parser configuration options.
  This component introduces an additional backend, so users must understand the selectable configurations.


Rejected Ideas
==============

None.


Open Issues
===========

- S-CORE process compliance and Vector's role

  The task of adapting the new component to be S-CORE-process-compliant is currently unassigned.
  Vector will not create the required S-CORE process documentation (for example, safety and security analysis).


Footnotes
=========


.. toctree::

   architecture/index.rst
   requirements/index.rst