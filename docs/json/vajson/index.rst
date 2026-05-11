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

We propose adding support for JSON parsing via the automotive established parser VaJson within S-CORE.

This component request describes a JSON parser to be used within applications of the Eclipse S-CORE project.

This requests adds VaJson, a SAX-based JSON parser for Eclipse S-CORE applications that enables parsing of JSON data for configuration and data exchange.


Motivation
==========

VaJson provides a safety-qualified JSON parser designed for automotive environments. It has passed ASIL D compliance within Vector, offering safety artifacts that significantly reduce the effort required to adapt to S-CORE qualification processes. Its API follows automotive standards, ensuring easier integration, consistent practices, and lower risk compared to non-qualified alternatives.

The VaJson source code successfully passed Vector’s internal ASIL D compliance process, demonstrating adherence to the highest automotive safety standards. Artifacts produced during this process—such as the safety manual, safety analysis, cybersecurity analysis, and detailed design—combined with ASIL-D qualified code, provide a strong foundation for simplifying adaptation to S-CORE’s internal qualification requirements.



Rationale
=========

VaJson was chosen because it is already ASIL D qualified (within Vector processes) and comes with complete safety and cybersecurity artifacts. This reduces the effort to meet S-CORE internal processes.

The parser uses a SAX-based design, which is efficient for embedded systems and avoids high memory usage.

Its API is familiar in automotive projects, making integration easier and reducing risk compared to introducing a new or generic parser.


Specification
=============

VaJson meets the generic requirements for a JSON deserialization library.

* :need:`comp_req__json__deserialization`
* :need:`comp_req__json__user_format`

In addition, VaJson satisfies the extended requirements specified in :need:`doc__vajson_requirements`
The extended requirements are also listed here below.

- JSON Validation
	VaJson shall provide a service to check the well-formedness of JSON data. Errors must include the reason and location in the document for malformed JSON and invalid schemata.

-  JSON Deserialization RFC extensions
	VaJson shall parse JSON data according to RFC8259 from files and character buffers. The parser shall ignore trailing commas and accept hexadecimal integers.

- Unicode Support
	VaJson shall decode and encode UTF-8 strings.


Backwards Compatibility
=======================

VaJson will be integrated as an optional backend through the existing S-CORE JSON wrapper. Users can configure which JSON backend to use (e.g., VaJson or nlohmann), ensuring no breaking changes to existing functionality.


Security Impact
===============

Potential security concerns and mitigations for VaJson:

- Malformed JSON Attacks
	Threat: Supplying intentionally malformed or deeply nested JSON to cause excessive CPU or memory consumption.
	Mitigation: Handled by design within VaJson. VaJson uses a SAX-based streaming parser, which detects malformed JSON during parsing and prevents full processing of invalid input.

- Schema Abuse
	Threat: Using oversized keys/values or unexpected schema elements to bypass validation or trigger buffer overflows.
	Mitigation: Handled by design within VaJson. Input size and structure are validated during parsing.



Safety Impact
=============

A full safety analysis for VaJson exists in an external format and addresses all identified concerns with appropriate measures. Example:

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
	Clearly displays configuration options for JSON parsing. The new component introduces an additional backend solution, and users need to be aware of available configurations.


Rejected Ideas
==============

None.


Open Issues
===========

- S-CORE process compliance and Vectors role
	The task of adapting the new component to be S-CORE process compliant is currently unassigned, and Vector will not create the required S-CORE process documentation (e.g., safety and security analysis).


Footnotes
=========


.. toctree::

   architecture/index.rst
   requirements/index.rst