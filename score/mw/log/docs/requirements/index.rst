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
   :version: 1
   :status: valid
   :safety: ASIL_B
   :security: NO
   :realizes: wp__requirements_comp[version==1]
   :tags: log


Log Frontend Requirements
*************************


Configuration
=============


.. comp_req:: Logging Configuration File
   :id: comp_req__log__cfg_file
   :status: valid
   :derived_from:
      feat_req__logging__config_log_level[version==1],
      feat_req__logging__sink_device[version==1],
      feat_req__logging__sink_multiple_device[version==1],
      feat_req__logging__sink_strategy[version==1],
      feat_req__logging__config_buffer_size[version==1],
      feat_req__logging__config_storage_size[version==1],
      feat_req__logging__config_permissions[version==1],
      feat_req__logging__config_log_filter[version==1],
      feat_req__logging__config_entity_id[version==1],
      feat_req__logging__config_on_demand[version==1],
      feat_req__logging__config_fallback[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall load configurations from an application-specific
    configuration file.


.. comp_req:: Logging Configuration File Location
   :id: comp_req__log__cfg_file_loc
   :status: valid
   :derived_from: feat_req__logging__config_fallback[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    The application specific file shall be the first valid configuration file
    from the following locations.

    1. location pointed to by the environmental variable ``MW_LOG_CONFIG_FILE``
    2. ``<cwd>/etc/logging.json``
    3. ``<cwd>/logging.json``
    4. ``<binary path>/../etc/logging.json``


.. comp_req:: Logging Global Configuration File
   :id: comp_req__log__global_config
   :status: valid
   :derived_from: feat_req__logging__config_fallback[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall load configurations from a global configuration file.

    The global configuration shall be loaded from
    ``/etc/ecu_logging_config.json``


.. comp_req:: Logging Configuration File Defaults
   :id: comp_req__log__cfg_defaults
   :status: valid
   :derived_from: feat_req__logging__config_fallback[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall fall back to a default value for a configuration item
    that is not available from the configuration files.

    These are hard-coded in the library.


.. comp_req:: Logging Configuration Precedence
   :id: comp_req__log__cfg_precedence
   :status: valid
   :derived_from: feat_req__logging__config_fallback[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    For each configuration field, ``mw::log`` shall resolve values using the
    following precedence order:

    1. Application configuration file
    2. Global configuration file
    3. Defaults


.. comp_req:: Logging Configuration Syntax Errors Handling
   :id: comp_req__log__cfg_syntax_err
   :status: valid
   :derived_from: feat_req__logging__error_handling_recoverable[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall discard any configuration files with syntax errors and
    continue using other sources of configuration.


.. comp_req:: Logging Configuration Invalid Settings Handling
   :id: comp_req__log__cfg_invalid
   :status: valid
   :derived_from: feat_req__logging__error_handling_recoverable[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall discard any invalid configuration fields.

    Invalid meaning that the value is not in the expected range or format.


Configuration Fields
--------------------

.. comp_req:: Logging Configuration Log Storage Device
   :id: comp_req__log__cfg_sink_device
   :status: valid
   :derived_from: feat_req__logging__sink_device[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall support configuration of a file path specifying where log
    are written.


.. comp_req:: Logging Configuration Internal Buffer Size
   :id: comp_req__log__cfg_buffer_size
   :status: valid
   :derived_from: feat_req__logging__config_buffer_size[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall allow configuration of the size of the internal buffer.


.. comp_req:: Logging Configuration Log ECU Identifier
   :id: comp_req__log__cfg_log_ecu_id
   :status: valid
   :derived_from: feat_req__logging__entity_identifier[version==1], feat_req__logging__config_entity_id
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

   ``mw::log`` shall support configuration of an ECU log identifier, which shall
   be a DLT-style ASCII uppercase 4 character string.


.. comp_req:: Logging Configuration Log Application Identifier
   :id: comp_req__log__cfg_log_app_id
   :status: valid
   :derived_from: feat_req__logging__entity_identifier[version==1], feat_req__logging__config_entity_id
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

   ``mw::log`` shall support configuration of an application log identifier,
   which shall be a DLT-style ASCII uppercase 4 character string.


Log Handling
============

.. comp_req:: Message Loss Detection
   :id: comp_req__log__log_loss_detect
   :status: valid
   :derived_from: feat_req__logging__message_loss_detection[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall detect and report any message loss.


.. comp_req:: Custom Log Types
   :id: comp_req__log__cfg_custom_types
   :status: valid
   :derived_from: feat_req__logging__config_custom_types[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall allow extensions for custom log types.


Log Message Format
==================

.. comp_req:: Local Timestamp
   :id: comp_req__log__timestamp_local
   :status: valid
   :derived_from: feat_req__logging__timestamping_local[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall attach a local timestamp to each log entry.


System Backend
===============

.. comp_req:: System Logger Backend
   :id: comp_req__log__system_logger
   :status: valid
   :derived_from: feat_req__logging__early_startup[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall support logging of slog backed for QNX deployments.


Console Backend
===============

.. comp_req:: Console Backend Stdout Sink
   :id: comp_req__log__console_backend
   :status: valid
   :derived_from: feat_req__logging__log_sinks_stdout[version==1]
   :reqtype: Functional
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall support output log messages to stdout when running unit tests.


Compatibility
=============

.. comp_req:: Compatibility Supported Operating Systems
   :id: comp_req__log__compat_os
   :status: valid
   :derived_from: feat_req__logging__compat_os[version==1]
   :reqtype: Interface
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall support QNX and Linux operating systems, encapsulated via OSAL.


.. comp_req:: Compatibility Supported Programming Languages
   :id: comp_req__log__compat_languages
   :status: valid
   :derived_from: feat_req__logging__compat_languages[version==1]
   :reqtype: Interface
   :security: NO
   :safety: ASIL_B
   :satisfied_by: comp__logging[version==1]
   :version: 1

    ``mw::log`` shall provide a logging API for C++ and Rust.

.. toctree::
   :maxdepth: 1

   chklst_req_inspection
