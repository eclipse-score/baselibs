# Low Level Design of the Configuration and Initialization for `mw::log`

- [Low Level Design of the Configuration and Initialization for `mw::log`](#low-level-design-of-the-configuration-and-initialization-for-mwlog)
  - [Introduction](#introduction)
  - [Use Cases](#use-cases)
  - [Class Diagram](#class-diagram)
  - [Initialization Sequence Diagram](#initialization-sequence-diagram)

## Introduction

Many aspects in the logging library shall be configurable for the user in static
configuration files. For instance, the user wants to change the minimum logging
level without recompiling the code just by modifying a configuration file on
target.

For Verbose Logging, an adaptive application on target loads two configuration files:

1. The ECU-wide configuration file under `/etc/ecu_logging_config.json`.
2. The environmental config file from the path saved under `MW_LOG_CONFIG_FILE` environmental variable
      or
   The application-specific configuration file under `/opt/<app>/etc/logging.json` or `<cwd>/etc/logging.json` or `<cwd>/logging.json`.

In the ECU-wide config file, an attribute called ECUID is made available that shall used
in the DLT header. The environmental configuration then shall override the ECU-wide configuration. If not defined, the application-specific configuration then shall be applied with the settings from the application developer. In particular, an application identifier (APPID) shall be provided.
Often also the default log level is overridden from the ECU-wide configuration, and
context-specific log levels may be provided.

Note that for non-verbose logging there is a third configuration file:

3. The non-verbose message configuration: `/opt/datarouter/etc/class-id.json`

`class-id.json` provides the logging level that a non-verbose message shall use
when sent using the `TRACE()` macro. This design document focuses on the
the verbose logging configuration and omit the non-verbose aspect.

## Use Cases

The use case diagram below shows three typical use cases supported by the design:

![Use Case Diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/mw/log/design/configuration_use_cases.uxf)

## Class Diagram

This design is based on the concept of having multiple backends and recorders
from the original `mw::log` [design document](README.md). We omit the details
for the `Recorder` and `Runtime` classes, and focus on the new classes
introduced in this design:

![Class diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/mw/log/design/configuration_static.uxf)

The design is centered around the `RecorderFactory` class, which contains three
static methods for each major use case. By default for on-target logging the
`Runtime` will initialize its recorder instance using the `CreateForTarget()`
static method. Internally, this will instantiate the `TargetConfigReader` to
obtain the logging configuration. Depending on the configuration, multiple
recorders might be needed. For instance, a user might select console logging and
remote logging, i.e. DLT logging. Hence both the `DataRouterRecorder` and the
`TextRecorder` are required in parallel.

The responsibility of the `RecorderComposite` is to facilitate multiple active
recorders in parallel. As the name implies the composite design
pattern is used here. In the constructor, the class receives a list of Recorder
instances. The class implements the `Recorder` interface methods by calling all
the corresponding method on each instance in its list.

The `Configuration` is kept simple as an aggregate class and passed by value in
the constructor in each `Recorder` instance that needs it. Since this shall be
done only once during initialization on target, this is not on a hot path. Thus
a simpler design is chosen by accepting the extra copies as a tradeoff.

On target, the configuration shall be loaded by the `TargetConfigReader` class.
As this class has several external dependencies we also introduce the
corresponding interfaces and mock class to unit-test the `RecorderFactory`
methods. `TargetConfigReader` uses `ConfigurationFileDiscoverer` to obtain the
file paths for the configuration. After that it uses `lib::json` library to read
the JSON documents and extract the configuration information from there.

For unit-testing, we introduce the `IConfigurationFileDiscoverer` interface and
corresponding mock. This enables dependency-injection of
`MockConfigurationFileDiscoverer` into the `TargetConfigReader` for testing.

On lowest layer, `ConfigurationFileDiscoverer` makes use of `OSAL` for
general purpose file and directory operations, as well as finding the path of
the current executable. We reuse the mocking facilities of OSAL to unit test the
file discoverer.

## Initialization Sequence Diagram

The sequence diagram below depicts the initialization that takes place once when
the user makes the first invokes `mw::log`:

![Class diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/swh/safe-posix-platform/score/mw/log/design/configuration_sequence.uxf)

In the diagram, the user calls `mw::log::Error()` to send the first log message
of the program. The `LogStreamFactory` then calls `GetRecorder()` on the
`Runtime` class to obtain the handler of the log message. Since this is the
first invocation, the instance of the `Runtime` shall be empty and thus the
`Runtime` will call `RecorderFactory::CreateForTarget()` to create the recorder
according to user configuration. Thus we invoke the `ReadConfig()` method of the
local instance of `ITargetConfigReader`.

The concrete `TargetConfigReader` then will use the
`ConfigurationFileDiscoverer` to check if the global, environmental xor application-specific
config files exist. It will first load the JSON document of the global
configuration, and then override the values with the environmental xor application-specific
configuration. Finally, the obtained instance of `Configuration` is returned.

Back in the method `RecorderFactory::CreateForTarget()` we instantiate the
configured recorder types. In the example, `kConsole` and `kRemote` are given.
Thus the method creates a vector that contains `DatarouterRecorder` and
`TextRecorder`. From that list, we construct the `CompositeRecorder` that
will forward the messages to both recorders.

The created instance of `CompositeRecorder` is returned by
`RecorderFactory::CreateForTarget()` and stored in the `Runtime`. Henceforth,
this recorder will be used for all following `mw::log` invocations by the user.

If a user wants to avoid the latency on the first log message, we provide
`mw::log::Initialize()` that shall take care of initializing the runtime. If
this method is called, the first log message will directly use the initialized
recorders.
