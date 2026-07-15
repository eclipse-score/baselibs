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


.. document:: Log Requirements
   :id: doc__log_requirements
   :status: valid
   :safety: ASIL_B
   :security: NO
   :realizes: wp__requirements_comp
   :tags: log


Requirements
############


Configuration
=============


.. comp_req:: Logging Configuration File
   :id: comp_req__logging_frontend__config_file
   :status: valid
   :derived_from: feat_req__logging__config_log_level
   :reqtype: Functional
   :security: NO
   :safety: QM
   :satisfied_by: comp__logging
   :belongs_to: comp__logging

    ``mw::log`` shall be uniformly configured via an application-specific
    configuration file.


.. comp_req:: Logging Configuration Fields
   :id: comp_req__logging_frontend__config_fields
   :status: valid
   :derived_from: feat_req__logging__entity_identifier
   :reqtype: Functional
   :security: NO
   :safety: QM
   :satisfied_by: comp__logging
   :belongs_to: comp__logging

    Application log configuration shall provide information necessary for log
    message generation and filtering.

    The provided parameter set includes, but is not limited to the following:
    application ID (appId), log mode for mw::log, default log level, log level
    assignments for individual context IDs.


.. comp_req:: Logging Global Configuration File
   :id: comp_req__logging_frontend__global_config
   :status: valid
   :derived_from: feat_req__logging__config_fallback
   :reqtype: Functional
   :security: NO
   :safety: QM
   :satisfied_by: comp__logging
   :belongs_to: comp__logging

    Global configuration shall be used as basis for all applications. 

    Global configuration shall remain in the file
    ``/etc/ecu_logging_config.json``. Application-specific configuration shall
    override the global defaults.


.. comp_req:: Logging Configuration File Defaults
   :id: comp_req__logging_frontend__config_defaults
   :status: valid
   :derived_from: feat_req__logging__config_fallback
   :reqtype: Functional
   :security: NO
   :safety: QM
   :satisfied_by: comp__logging
   :belongs_to: comp__logging

    ``mw::`` log shall fall back to a default value for a configuration item that is
    not available from the configuration files.


.. comp_req:: Logging Configuration Syntax Errors Handling
   :id: comp_req__logging_frontend__config_syntax_error
   :status: valid
   :derived_from: feat_req__logging__error_handling_recoverable
   :reqtype: Functional
   :security: NO
   :safety: QM
   :satisfied_by: comp__logging
   :belongs_to: comp__logging

    ``mw::log`` shall discard configuration files with syntax errors. 


.. comp_req:: Logging Configuration Not Found Handling
   :id: comp_req__logging_frontend__config_not_found
   :status: valid
   :derived_from: feat_req__logging__error_handling_recoverable
   :reqtype: Functional
   :security: NO
   :safety: QM
   :satisfied_by: comp__logging
   :belongs_to: comp__logging

    ``mw::log`` shall discard configuration files that are not found.


.. comp_req:: Logging Configuration Invalid Settings Handling
   :id: comp_req__logging_frontend__config_invalid_settings
   :status: valid
   :derived_from: feat_req__logging__error_handling_recoverable
   :reqtype: Functional
   :security: NO
   :safety: QM
   :satisfied_by: comp__logging
   :belongs_to: comp__logging

    ``mw::log`` shall discard any invalid configuration settings.
