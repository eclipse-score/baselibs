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


Log Frontend Requirements
*************************


Configuration
=============


.. comp_req:: Logging Configuration File
   :id: comp_req__log_frontend__cfg_file
   :status: valid
   :derived_from:
      feat_req__logging__config_log_level,
      feat_req__logging__sink_device,
      feat_req__logging__sink_multiple_device,
      feat_req__logging__sink_strategy,
      feat_req__logging__config_buffer_size,
      feat_req__logging__config_storage_size,
      feat_req__logging__config_permissions,
      feat_req__logging__config_log_filter,
      feat_req__logging__config_entity_id,
      feat_req__logging__config_on_demand,
      feat_req__logging__config_fallback
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall load configurations from an application-specific
    configuration file.


.. comp_req:: Logging Configuration File Location
   :id: comp_req__log_frontend__cfg_file_loc
   :status: valid
   :derived_from: feat_req__logging__config_fallback
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    The application specific file shall be the first valid configuration file
    from the following locations.

    1. location pointed to by the environmental variable ``MW_LOG_CONFIG_FILE``
    2. ``<cwd>/etc/logging.json``
    3. ``<cwd>/logging.json``
    4. ``<binary path>/../etc/logging.json``


.. comp_req:: Logging Global Configuration File
   :id: comp_req__log_frontend__global_config
   :status: valid
   :derived_from: feat_req__logging__config_fallback
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall load configurations from a global configuration file.

    The global configuration shall be loaded from
    ``/etc/ecu_logging_config.json``


.. comp_req:: Logging Configuration File Defaults
   :id: comp_req__log_frontend__cfg_defaults
   :status: valid
   :derived_from: feat_req__logging__config_fallback
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall fall back to a default value for a configuration item
    that is not available from the configuration files.

    These are hard-coded in the library.


.. comp_req:: Logging Configuration Precedence 
   :id: comp_req__log_frontend__cfg_precedence
   :status: valid
   :derived_from: feat_req__logging__config_fallback
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    For each configuration field, ``mw::log`` shall resolve values using the
    following precedence order:

    1. Application configuration file
    2. Global configuration file
    3. Defaults


.. comp_req:: Logging Configuration Syntax Errors Handling
   :id: comp_req__log_frontend__cfg_syntax_err
   :status: valid
   :derived_from: feat_req__logging__error_handling_recoverable
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall discard any configuration files with syntax errors and
    continue using other sources of configuration.


.. comp_req:: Logging Configuration Invalid Settings Handling
   :id: comp_req__log_frontend__cfg_invalid
   :status: valid
   :derived_from: feat_req__logging__error_handling_recoverable
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall discard any invalid configuration fields.

    Invalid meaning that the value is not in the expected range or format.


Configuration Fields
--------------------

.. comp_req:: Logging Configuration Log Storage Device
   :id: comp_req__log_frontend__cfg_sink_device
   :status: valid
   :derived_from: feat_req__logging__sink_device
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall support configuration of a file path specifying where log
    are written.


.. comp_req:: Logging Configuration Multiple Log Storage Devices
   :id: comp_req__log_frontend__cfg_sink_multi_dev
   :status: valid
   :derived_from: feat_req__logging__sink_multiple_device
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall support configuration of multiple log storage devices.


.. comp_req:: Logging Configuration Log Storage Strategy
   :id: comp_req__log_frontend__cfg_sink_strategy
   :status: valid
   :derived_from: feat_req__logging__sink_strategy
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall support configuration of the log storage strategy.


.. comp_req:: Logging Configuration Internal Buffer Size
   :id: comp_req__log_frontend__cfg_buffer_size
   :status: valid
   :derived_from: feat_req__logging__config_buffer_size
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall allow configuration of the size of the internal buffer.


.. comp_req:: Logging Configuration Log Storage Size
   :id: comp_req__log_frontend__cfg_storage_size
   :status: valid
   :derived_from: feat_req__logging__config_storage_size
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall allow configuration of the maximum size of all defined
    log files.


.. comp_req:: Logging Configuration Log ECU Identifier
   :id: comp_req__log_frontend__cfg_log_ecu_id
   :status: valid
   :derived_from: feat_req__logging__entity_identifier, feat_req__logging__config_entity_id
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

   ``mw::log`` shall support configuration of an ECU log identifier, which shall
   be a DLT-style ASCII uppercase 4 character string.


.. comp_req:: Logging Configuration Log Application Identifier
   :id: comp_req__log_frontend__cfg_log_app_id
   :status: valid
   :derived_from: feat_req__logging__entity_identifier, feat_req__logging__config_entity_id
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

   ``mw::log`` shall support configuration of an application log identifier,
   which shall be a DLT-style ASCII uppercase 4 character string.


Log Handling
============

.. comp_req:: Log Prioritization
   :id: comp_req__log_frontend__log_prior
   :status: valid
   :derived_from: feat_req__logging__prioritization
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall prioritize log messages in case of resource conflicts to
    ensure critical logs are not lost.

    The order of prioritization shall follow the log level in the following
    way:

    1. ``Fatal``
    2. ``Error``
    3. ``Warn``
    4. ``Info``
    5. ``Debug``
    6. ``Verbose``


.. comp_req:: Early Startup Logging
   :id: comp_req__log_frontend__early_startup
   :status: valid
   :derived_from: feat_req__logging__early_startup
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall support logging of early startup events to capture
    critical initialization information.


.. comp_req:: Message Loss Detection
   :id: comp_req__log_frontend__log_loss_detect
   :status: valid
   :derived_from: feat_req__logging__message_loss_detection
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall detect and report any message loss.


.. comp_req:: Context-Specific Log Level
   :id: comp_req__log_frontend__context_log_level
   :status: valid
   :derived_from: feat_req__logging__context_log_level
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall allow context-specific log level activation at runtime to
    enable fine-grained control over logging behaviour.


.. comp_req:: Boot Logging
   :id: comp_req__log_frontend__boot_logging
   :status: valid
   :derived_from: feat_req__logging__boot_logging
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall support logging of data to memory which survives a reboot
    cycle.


.. comp_req:: Custom Log Types
   :id: comp_req__log_frontend__cfg_custom_types
   :status: valid
   :derived_from: feat_req__logging__config_custom_types
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall allow extensions for custom log types.


.. comp_req:: Non-Recoverable Error Handling Silent
   :id: comp_req__log_frontend__err_nonrec_silent
   :status: valid
   :derived_from: feat_req__log__err_handling_nonrec
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    In case of a non-recoverable error, ``mw::log`` shall deactivate without
    affecting the user application.
    
    
.. comp_req:: Non-Recoverable Error Handling Reporting
   :id: comp_req__log_frontend__err_nonrec_report
   :status: valid
   :derived_from: feat_req__log__err_handling_nonrec
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    In case of a non-recoverable error, ``mw::log`` shall set an error state
    reported on shutdown.


Log Message Format
==================

.. comp_req:: Local Timestamp
   :id: comp_req__log_frontend__timestamp_local
   :status: valid
   :derived_from: feat_req__logging__timestamping_local
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall attach a local timestamp to each log entry.


.. comp_req:: Original Timestamp
   :id: comp_req__log_frontend__timestamp_orig
   :status: valid
   :derived_from: feat_req__logging__timestamping_original
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall preserve the original timestamp for log entries that have
    been routed.


.. comp_req:: Timestamp Synchronization
   :id: comp_req__log_frontend__timestamp_sync
   :status: valid
   :derived_from: feat_req__logging__timestamping_sync
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall support timestamp synchronization for log entries coming
    from different logging nodes.


Console Backend
===============

.. comp_req:: Console Backend Stdout Sink
   :id: comp_req__log_frontend__console_backend
   :status: valid
   :derived_from: feat_req__logging__log_sinks_stdout
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall output log messages to stdout when running unit tests.


Compatibility
=============

.. comp_req:: Compatibility Supported Operating Systems
   :id: comp_req__log_frontend__compat_os
   :status: valid
   :derived_from: feat_req__logging__compat_os
   :reqtype: Interface
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall support QNX and Linux operating systems, encapsulated via OSAL.


.. comp_req:: Compatibility Supported Programming Languages
   :id: comp_req__log_frontend__compat_languages
   :status: valid
   :derived_from: feat_req__logging__compat_languages
   :reqtype: Interface
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall provide a logging API for C++ and Rust.


Non-Functional Requirements
===========================

.. comp_req:: Storage Resource Consumption
   :id: comp_req__log_frontend__resource_storage
   :status: valid
   :derived_from: feat_req__logging__resource_storage
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall minimize storage resource consumption.


.. comp_req:: Safety System Classification
   :id: comp_req__log_frontend__system_class
   :status: valid
   :derived_from: feat_req__logging__system_class
   :reqtype: Non-Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging
   :belongs_to: comp__logging
   :version: 1

    ``mw::log`` shall be classified according to the overall system's safety concept if
    logging information is part of the verification strategy.
