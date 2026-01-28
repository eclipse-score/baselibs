# Table of Contents

[Introduction](#introduction)

[Requirements](#requirements)

[Motivation](#motivation)

[Detailed Design](#detailed-design)

[Assumptions of Use](#assumptions-of-use-aous)

[Limitations of Use](#limitations-of-use)

[Configuration](#configuration)

[Logging modes](#logging-modes)

[Usage](#usage)

[LogHex and LogBin](#loghex-and-logbin)

[Overload mw::log::LogStream with custom user types](#overload-mwloglogstream-with-custom-user-types)

[Feature flags](#Feature-Flags)

[Remote Logging](#Remote-Logging)

## Introduction

The `mw::log` library enables logging for the users. Throughout its lifetime,
any application may log messages by calling the `mw::log` API. For example:

```c++
score::mw::log::LogFatal() << "Hello World!!!";
score::mw::log::LogError() << score::mw::log::LogHex32{1470258};
```

Depending on the selected logging mode, the log traces are written into the
terminal or a respective `.dlt` file. See the [details below](#logging-modes).

## Requirements

The requirements are maintained in [Codebeamer](broken_link_c/issue/861469), and the high-level architecture is illustrated in the [Magic Draw diagram](broken_link_md:27112/collaborator/document/4ff2a028-8ac7-4f7a-b42e-dd90816fec81?viewId=a0683e3f-a47e-4269-9000-255abd14110a&viewType=model&sectionId=977f79a2-d84f-4932-9190-1e520cc3696b)

## Motivation

With `mw::log` deterministic memory management is ensured in the logging context.
I.e. memory is only allocated within the initialization (phase). This is achieved
by usage of pre-allocated buffers.

The benefits of using `mw::log` are:

* Gives developers the full control over the logging framework.
* Resolves dependencies to the AUTOSAR `ara::log` interfaces.
* Reduces memory demands of logging (considerably) as compared to its predecessor
    `ara::log`.

## Detailed Design

The Detailed Design for the `mw::log` implementation can be found under the [design](design/README.md) directory.

## Assumptions of use (AoUs)

The AoUs are maintained in [Codebeamer](broken_link_c/share/f55e57bf89054000a975502375922db5)

## Limitations of use

Logging is a best effort operation that shall ensure freedom from interference at all times.
Log messages may be dropped if the available resources or the limits of the implementation are exceeded.

## Configuration

The configuration of `mw::log` is done via `.json` files.

* **ecu_logging_config.json** - one file per ECU,
* the environment config file indicated by the `MW_LOG_CONFIG_FILE` environmental variable.
* **logging.json** - one file per application. Besides that there is the one default
logging.json which is used when the particular file for the given app is missing.

When the configuration can be stored only under the user-defined path, this path can be indicated in
the `MW_LOG_CONFIG_FILE` environmental variable. If it is defined, the application specific configuration
will be omitted.

The user shall consider configurable logging parameters in `ecu_logging_config.json`
file and check whether those values need to be overridden by `logging.json` for
their specific app needs.

The default path for the config file should be: `/opt/$APP_NAME/etc/logging.json`.
Where $APP_NAME is a placeholder for the respective application's name.
If the syntax or semantics of `logging.json` is incorrect, the default are read
and applied.

The following attributes are read from configuration files:

* **ecuId** -- 4-character identifier for the ECU
* **appId** -- 4-character identifier for the application
* **appDesc** -- description, not very important
* **logMode** -- default value: `LogMode::kRemote`; note: multiple modes can be
defined simultaneously. Hence, e.g. logging to the file and console is possible
in parallel
* **logFilePath** -- used for file logging, if ```logMode``` includes
```kFile```, this is the directory, where the logfile ```appId.dlt``` will be
put
* **logFileSizePolicy** -- an object that defines the rules for log file rotation.
    * **enabled** (`true`/`false`): Enables or disables the file rotation/sizing feature.
    * **maxFileSizeInBytes** (integer): The maximum size in bytes that a single log file can reach before rotation is triggered.
    * **numberOfFiles** (integer): The total number of log files to use in the rotation cycle (e.g., `appId_1.dlt`, `appId_2.dlt`).
    * **overwriteExistingFiles** (`true`/`false`): If `true`, allows the logger to overwrite an existing log file when it rotates back to it. If `false`, rotation will stop if the next file in the cycle already exists.
    * **truncateOnRotation** (`true`/`false`): If `true`, the contents of the next log file in the cycle will be deleted (truncated) before new logs are written to it.
* **logLevel** -- default value: LogLevel::kWarn, global log level threshold
for the application
* **logLevelThresholdConsole** -- if ```logMode``` includes ```kConsole```,
console message will be filtered by this log level. Note, the global
setting/configuration, however, has priority
* **contextConfigs** -- it's a map which may contain different log levels for
different contexts. This overrides the global log level threshold defined above
for the specified context.
* **stackBufferSize** -- size of the linear buffer used for storing type
information (metadata). If your application sends a lot of different datatypes,
you might consider increasing this limit
* **ringBufferSize** -- size of ringbuffer for data sent to the DLT deamon. All
verbose and non-verbose DLT messages along with CDC land in the ringbuffer, so
please consider increasing this one if you lose messages
* **overwriteOnFull** -- defines the ringbuffer write strategy, default
(=`true`) is to overwrite when the buffer is full; alternatively
(=`false`) the messages would just be dropped, the oldest unread messages
are left intact (useful to identify the scenarios of ringbuffer overflow, for
example)
* **numberOfSlots** -- default value: 8, the name refers to the implementation
detail of `mw::log`, which is based on CircularBuffer with the given amount of
slots. Thus the amount of the memory used for logs storage is dependent on this
number. Less slots require fewer memory, but the likelihood of logs being
dropped increases.
NOTE: Although this is configurable, the maximum number of slots is limited
based on the underlying recorder/backend implmentation.
* **slotSizeBytes** -- default value: 2048, the size of a single slot, which
may affect memory usage. If the slot is too short, the message is truncated.

## Logging modes

```c++
enum class LogMode : uint8_t
{
    kRemote = 0x01,   ///< Sent remotely
    kFile = 0x02,     ///< Save to file
    kConsole = 0x04,  ///< Forward to console,
    kSystem = 0x08,   ///< QNX: forward to slog,
    kCustom = 0x10,   ///< Custom log mode,
    kInvalid = 0xff   ///< Invalid log mode,
};
```

The above code presents the modes of the logging:

* **kRemote** -- the logs are sent remotely via network by DLT protocol.
* **kFile** -- the logs are written to the file `<file_name>.dlt` located
    on a target.
* **kConsole** -- the logs are written into the terminal.
* **kSystem** -- the logs are written into the QNX slogger2.
* **kInvalid** -- self-explanatory - no logging then.

## Usage

If one wants to use `mw::log`, the following include shall be necessary:

```c++
#include "score/mw/log/logging.h"
```

And the following bazel dependency as well:

```bazel
deps = [
    "@score_logging//score/mw/log:log",
],
```

Note: This target bundles all supported logging backends, introducing additional dependencies determined by [feature flags](#feature-flags) configuration. For projects or targets requiring minimal footprint, depend exclusively on the logging frontend to avoid including unused backends and their transitive dependencies.

```bazel
deps = [
    "@score_baselibs//score/mw/log:frontend",
],
```

This approach ensures that only required dependencies are included per target, avoiding bloat from unused logging backends.

Note: Test binaries that depend on frontend only library targets require an explicit backend implementation. For this you can provide a basic console backend:

```bazel
deps = [
    "@score_baselibs//score/mw/log:backend_stub_testutil",
],
```

[Baselibs](broken_link_g/swh/safe-posix-platform/tree/master/platform/aas/lib) follow this approach by using only the mw::log frontend in their library targets, maintaining self-containment and avoiding unnecessary dependencies. However users of those libraries could see linker errors while building binaries (cc_binary) if a backend is not provided. For this you can use the `@score_logging//score/mw/log:log` as a dependency that includes all the backends depending on the [feature flags](#feature-flags) configuration.

### How to log something the most easy way?

```
#include "score/mw/log/logging.h"

score::mw::LogError() << "Your custom message";
```

### How to log something under a specific context?

```
#include "score/mw/log/Logger.h"

score::mw::Logger my_context{"MCTX"};
my_context.LogError() << "My custom message";
```

or

```
#include "score/mw/log/logging.h"

score::mw::LogError("MCTX") << "My Message";
```

### How to log my custom type?

Refer section [logging custom type](#overload-mwloglogstream-with-custom-user-types).

### How to ignore logging in Unit-Testing

You can easily inject your custom recorder implementation. For most unit tests you just want to ignore the log messages.
For this we provide an empty recorder.

```
score::mw::log::EmptyRecorder my_recorder{};
score::mw::test::SetLogRecorder(my_recorder);
```

For more advanced usage like logging custom type, go to the section of
[custom types](#overload-mwloglogstream-with-custom-user-types).

Before listing the possible log calls, here are possible log levels listed:

```c++
enum class LogLevel : std::uint8_t
{
    kOff = 0x00,
    kFatal = 0x01,
    kError = 0x02,
    kWarn = 0x03,
    kInfo = 0x04,
    kDebug = 0x05,
    kVerbose = 0x06
};
```

Above log levels are easily matched to the direct log calls:

```c++
LogStream LogFatal() noexcept;
LogStream LogError() noexcept;
LogStream LogWarn() noexcept;
LogStream LogInfo() noexcept;
LogStream LogDebug() noexcept;
LogStream LogVerbose() noexcept;
LogStream LogFatal(const std::string_view context_id) noexcept;
LogStream LogError(const std::string_view context_id) noexcept;
LogStream LogWarn(const std::string_view context_id) noexcept;
LogStream LogInfo(const std::string_view context_id) noexcept;
LogStream LogDebug(const std::string_view context_id) noexcept;
LogStream LogVerbose(const std::string_view context_id) noexcept;
```

When `context_id` is not passed, the default value is: `"DFLT"`.

There are plenty of types which can be logged with `mw::log` using `operator <<`
(to preserve the similarity to standard C++ ostream usage):

```c++
LogStream& operator<<(const bool) const noexcept;
LogStream& operator<<(const std::int8_t) const noexcept;
LogStream& operator<<(const std::int16_t) const noexcept;
LogStream& operator<<(const std::int32_t) const noexcept;
LogStream& operator<<(const std::int64_t) const noexcept;
LogStream& operator<<(const std::uint8_t) const noexcept;
LogStream& operator<<(const std::uint16_t) const noexcept;
LogStream& operator<<(const std::uint32_t) const noexcept;
LogStream& operator<<(const std::uint64_t) const noexcept;
LogStream& operator<<(const float) const noexcept;
LogStream& operator<<(const double) const noexcept;
LogStream& operator<<(const std::string_view) const noexcept;
LogStream& operator<<(const std::string&) const noexcept;
LogStream& operator<<(const score::StringLiteral) const noexcept;
LogStream& operator<<(const LogHex8& value) const noexcept;
LogStream& operator<<(const LogHex16& value) const noexcept;
LogStream& operator<<(const LogHex32& value) const noexcept;
LogStream& operator<<(const LogHex64& value) const noexcept;
LogStream& operator<<(const LogBin8& value) const noexcept;
LogStream& operator<<(const LogBin16& value) const noexcept;
LogStream& operator<<(const LogBin32& value) const noexcept;
LogStream& operator<<(const LogBin64& value) const noexcept;
LogStream& operator<<(const LogRawBuffer& value) const noexcept;
LogStream& operator<<(const LogSlog2Message& value) noexcept;
```

`LogHex*` family enables logging numbers in hexadecimal form, while `LogBin*`
family enables logging number in binary form.
`LogRawBuffer` is a span over characters.
`LogSlog2Message` provides means to forward qnx specific slog2 code to the slog backend. This is advantageous for clients that intend to take additional actions based on the code. E.g.: Security clients like secpolllog can use this code to indicate severity/type of security failures and take necessary actions (setting DTCs) accordingly.
NOTE: By default the slog2 code is set to zero if not set explicitly.

User shall register an empty recodrder in case of log messages wanted to be
ignored in unit tests.

```c++
score::mw::log::EmptyRecorder my_recorder{};
score::mw::test::SetLogRecorder(my_recorder);
```

It's preferred to link your app directly with `mw/log` no need for fakes.

### LogHex and LogBin

It is very useful feature to log numbers in hexadecimal or binary representation.
What is important is that `LogHex*` and `LogBin*` structures stores **unsigned**
integers with the appropriate size, thus `LogHex8` stores `uint8_t`, `LogHex16`
stores `uint16_t` and so on.
The examples of usage from the code:

```c++
const score::mw::log::LogHex8 log_hex_8{10};
score::mw::log::LogInfo() << "log_hex_8: " << log_hex_8;
```

And it produces the output:

```bash
log_hex_8: 0x0a
```

```c++
const score::mw::log::LogBin16 log_bin_16{9'012};
score::mw::log::LogInfo() << "log_bin_16: " << log_bin_16;
```

And it produces the output:

```bash
log_bin_16: 0b0010 0011 0011 0100
```

## mw::log::LogStream overloads

The goal is to enable users to define their own types and to log them the same
way as all types listed with `operator<<` in section [Usage](#usage).

The main assumption is to limit the dependencies to the minimum amount. Thus
it's not required to depend on the whole `mw::log`, since the only needed library
is `log_stream`.

**Important:** It is mandatory to add unit tests for each custom overload.

### Overload mw::log::LogStream with custom user types

The example implementation can be viewed [here](test/my_custom_lib).

The key sections of this implementation are going to be described in this document.

The primary assumption is to **have separate library** for each custom user type.
Thus the first step is to create separate directory. For the purpose of presenting
an example there was a directory created inside `mw/log` test directory, e.g.:
`mw/log/test/my_custom_lib` and create `BUILD` file there, e.g.:

```bazel
cc_library(
    name = "my_custom_type_mw_log",
    ...
    srcs = [
        "my_custom_type_mw_log.cpp",
    ],
    hdrs = [
        "my_custom_type.h",
        "my_custom_type_mw_log.h",
    ],
    ...
    deps = [
        "@score_logging//score/mw/log:log_stream",
    ],
)
```

## Note: If you want to run (using cc_binary) or test (using cc_test) the custom type, you should depend directly on mw/log target to avoid any issues during linkage stage.

```bazel
cc_test(
    ...
    deps = [
        "@score_logging//score/mw/log:log",
    ],
)
```


In `my_custome_type.h` there is user type defined:

```c++
#include <string>

namespace my
{
namespace custom
{
namespace type
{

struct MyCustomType
{
    std::int32_t int_field;
    std::string string_field;
};

}  // namespace type
}  // namespace custom
}  // namespace my
```

The content of C++ files are declaration and definition of `operator<<` needed
for logging, e.g. header `my_custom_type_mw_log.h`:

```c++
#include "mw/log/log_stream.h"
#include "mw/log/test/my_custom_lib/my_custom_type.h"

namespace my
{
namespace custom
{
namespace type
{

score::mw::log::LogStream& operator<<(score::mw::log::LogStream& log_stream,
                                    const my::custom::type::MyCustomType& my_custom_type) noexcept;

}  // namespace type
}  // namespace custom
}  // namespace my
```

and source file `my_custom_type_mw_log.cpp`:

```c++
#include "my_custom_type_mw_log.h"

namespace my
{
namespace custom
{
namespace type
{

score::mw::log::LogStream& operator<<(score::mw::log::LogStream& log_stream,
                                    const my::custom::type::MyCustomType& my_custom_type) noexcept
{
    log_stream << "my_custom_type: int_field : " << my_custom_type.int_field
               << " , string_field : " << my_custom_type.string_field;
    return log_stream;
}

}  // namespace type
}  // namespace custom
}  // namespace my
```

For `LogStream` usage there is the dependency in `BUILD` file necessary:

```bazel
deps = [
    "@score_logging//score/mw/log:log_stream",
],
```

And also `visibility` for `log_stream` needs to be added:

```bazel
cc_library(
    name = "log_stream",
    ...
    visibility = [
        ...
        "//mw/log/my_custom_lib:__pkg__",
    ],
```

It's not a big deal to extend `ara::log::LogStream` with the custom user type
the same way as `score::mw::log::LogStream`. What is needed:

**1.** Create the library for `ara::log` in `BUILD` file:

```bazel
cc_library(
    name = "my_custom_type_ara_log",
    ...
    srcs = [
        "my_custom_type_ara_log.cpp",
    ],
    hdrs = [
        "my_custom_type.h",
        "my_custom_type_ara_log.h",
    ],
    ...
    deps = [
        "//platform/aas/ara/log",
    ],
)
],
```

**2.** Add the overload in header file:

```c++
#include "mw/log/logstream.h"
#include "mw/log/test/my_custom_lib/my_custom_type.h"

namespace my
{
namespace custom
{
namespace type
{

ara::log::LogStream& operator<<(ara::log::LogStream& log_stream,
                                const my::custom::type::MyCustomType& my_custom_type) noexcept;

}  // namespace type
}  // namespace custom
}  // namespace my
```

**3.** Add the implementation to the source file:

```c++
#include "my_custom_type_ara_log.h"

namespace my
{
namespace custom
{
namespace type
{

ara::log::LogStream& operator<<(ara::log::LogStream& log_stream,
                                const my::custom::type::MyCustomType& my_custom_type) noexcept
{
    log_stream << "my_custom_type: int_field : " << my_custom_type.int_field
               << " , string_field : " << my_custom_type.string_field;
    return log_stream;
}

}  // namespace type
}  // namespace custom
}  // namespace my
```

#### mw::log::LogStream overloading for ara::core::Result

The example implementation can be viewed [here](test/sample_ara_core_logging).

The main point to consider is **ADL (Argument-dependent lookup)**. This rule requires that operator overloads and the data types used in the overload are declared in the same namespace.

To log `ara::core` types, a (new) directory containing a bazel file `/path/to/directory/BUILD` should be created. Then the following steps have to be considered.```

**1.** In order to use `score::mw::log::LogStream` in the new bazel target the `visibility` parameter in `//score/mw/log/BUILD` has to updated:

```c++
cc_library(
    name = "log_stream",
    ...
    visibility = [
        ...
        "//path/to/sample:__pkg__",
    ],
```

Normally, similar visibility updates would be needed within the target of the type to be logged. Since `ara::core` types have public visibility, however, no further action is required.

**2.** Update the dependencies of the new `BUILD` file to include `log_stream` and `ara::core` such as:

```c++
cc_library(
    name = "sample_ara_core_logging",
    testonly = True,
    hdrs = [
        "sample_ara_core_logging.h",
    ],
    features = [
        ...
    ],
    visibility = [
        ...
    ],
    deps = [
        "//platform/aas/ara/core",
        "@score_logging//score/mw/log:log_stream",
    ],
)
```

**3.** Provide a custom meaningful implementation, e.g. in `sample_ara_core_logging.h`

```c++
#include "score/mw/log/log_stream.h"
#include <ara/core/result.h>

namespace ara
{
namespace core
{

template <typename T, typename E>
score::mw::log::LogStream& operator<<(score::mw::log::LogStream& log_stream, const ara::core::Result<T, E>& result) noexcept
{
    if (result.HasValue())
    {
        log_stream << "Result value: " << result.Value();
    }
    else
    {
        log_stream << "Error message: " << result.Error().UserMessage();
    }

    return log_stream;
}

}  // namespace core
}  // namespace ara
```

In case the `Result` holds a value, the `operator<<` is applied to this value. This requires an appropriate overload is provided for the given type of `T`.

Overloads of `mw::log::LogStream` for other `ara::core` could be provided congruently.

## Feature Flags

- **KFile_Logging** : for enabling/disabling KFile log mode, by default is enabled.
- **KConsole_Logging** : for enabling/disabling KConsole log mode, by default is enabled.
- **KRemote_Logging** : for enabling/disabling KRemote log mode, but this additionally requires a daemon process that gathers log mesages and forwards them to external systems for Analysis. E.g.: datarouter (DLT daemon)

### Building mw::log with/out feature flags.

  * KFile_Logging & KConsole_Logging are enabled by default.

  ```bash
  bazel build --config=spp_host_gcc //score/mw/log/...
  ```

  * Disable file logging

  ```bash
  bazel build --config=spp_host_gcc //score/mw/log/... --//score/mw/log/detail/flags:KFile_Logging=False
  ```

  * Disable console logging

  ```bash
  bazel build --config=spp_host_gcc //score/mw/log/... --//score/mw/log/detail/flags:KConsole_Logging=False
  ```

For build or test you can enable/disable log mode.

## Remote Logging

The following component diagram shows an high level overview of Remote Logging that includes the DLT remote backend. E.g.: Datarouter.
![Remote Logging component diagram](./design/remote_logging.plantuml)

### DLT-formatted payload

Verbose DLT-Logging is _not_ just a sequence of characters. In fact, different types are encoded differently.
This is described in the [DLT-Protocol (PRS_Dlt_00625)](https://www.autosar.org/fileadmin/user_upload/standards/foundation/1-0/AUTOSAR_PRS_DiagnosticLogAndTraceProtocol.pdf).

To give an example:

```
Logging() << "ABC" << 0xFF
```

would end up as something like

```
StringIdentifier|0x41|0x42|0x43|Uint8Identifier|0xFF
```

This format reduces memory usage especially for long-numbers.

### DLT-Daemon backend configuration guidelines
For this particular configuration all logging clients are relying on daemon. Messages are passed to particular system process with a use of shared memory file. This architectural decision is impacting characteristic of communication and messages loss. To avoid problems with logs being dropped consider adjusting **ringBufferSize** to be twice as much as the highest amount of bytes logging application may produce in any 100ms period or limit logging content and frequency.

The rule of thumb for calculating buffer size is:
```
ring_buffer_size >=  2 * logging_volume_100ms
```
where **logging_volume_100ms** is size in bytes of the logging volume produced by application at any given period of 100ms.
Note, however, that other limitation like total bandwidth of network or processing power availability apply.

Dropped log messages can be monitored and reported by the DLT-Daemon backend.

### DLT-Daemon backend (Datarouter) security policy (secpol) configuration
For the [message exchange between Logging Clients and the Datarouter](broken_link_g/swh/safe-posix-platform/tree/master/score/mw/log/design/datarouter_backend#message-exchange-between-logging-clients-and-datarouter), it is crucial that the secpol for the Datarouter (datarouter.secpol) contains the secpol type of the client process in its channel connect configuration. Otherwise, the Datarouter will be unable to establish a connection with the client, and consequently, it cannot forward the logs to remote.

```
allow datarouter_t {
    ClientApp_t    // Example secpol type for ClientApp
}:channel connect;
```
