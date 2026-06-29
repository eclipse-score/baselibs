..
   # *******************************************************************************
   # Copyright (c) 2025 Contributors to the Eclipse Foundation
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


FMEA (Failure Modes and Effects Analysis)
=========================================

.. document:: Baselibs FMEA
   :id: doc__baselibs_fmea
   :status: valid
   :safety: ASIL_B
   :security: YES
   :realizes: wp__feature_fmea

.. mod_insp:: Baselibs Feature FMEA Inspection Record
   :id: mod_insp__baselibs__feat_fmea
   :status: valid
   :safety: ASIL_B
   :security: YES
   :inspection_type: safety_analysis
   :inspection_state: rework_required
   :checklist_template: gd_chklst__safety_analysis
   :reviewers: aschemmel-tech
   :belongs_to: mod__baselibs
   :inspects: doc__baselibs_fmea
   :checklist: doc__baselibs_fmea
   :sha256: 03f4c66a817d110adbdd1227e9624352741c8a8f3013ed6c7ae7ba0d67f14dd0

   Machine-readable inspection record for the baselibs feature FMEA. It validates
   the extracted FMEA elements against the reviewed safety analysis. The build
   fails and reports the SHA256 to pin once the analysis has been reviewed.


The feature baselibs consists of multiple components which provide very different functionality.
They are also low-complex (i.e. no component architecture is documented, so that the feature architecture is the only one to analyze,
with one exception which is the Json component).

For a better usability and readability the FMEA will be documented on component level and for every
component individually, but using the feature architecture views (plus additional component architecture if any decompositon was done).

Consequently there is no AoU detected and documented on feature level.
