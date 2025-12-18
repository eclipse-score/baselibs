
# Custom Recorder Support

This document describes the custom recorder extension that allows external integrators to route logs to non-S-CORE backends, providing an alternative to the default S-CORE provided recorders.

The custom recorder feature enables developers to implement their own logging backends by providing a standardized interface for custom log handling. This extension point allows for flexible log routing while maintaining compatibility with the existing logging framework.

## Implementation

A user who wants to use the custom recorder, has to provide implementation of "CustomRecorder" and "CustomRecorderFactory" classes.

### custom recorder classes

- `CustomRecorder` - The main recorder implementation
- `CustomRecorderFactory` - Factory class for creating recorder instance

### feature configuration change

Implement `user\specific\repo\folder\custom_recorder.h` to include your custom implementation:

```c++

#include "custom_recorder_impl.h"
#include "custom_recorder_factory_impl.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
using CustomRecorderType = user::specific::impl::detail::CustomRecorderImpl;
using CustomRecorderFactoryType = user::specific::impl::detail::CustomRecorderFactoryImpl;
}
}
}
}

```

## Build with Custom Recorder Implementation

The label flag `custom_recorder_impl` is defined in "score/mw/log/flags/BUILD". To link with custom implementation, the user should inject the library with build label.

```bash
bazel build --config=<platform_config> --//score/mw/log/flags:KCustom_Logging=True --//score/mw/log/flags:custom_recorder_impl=//score/mw/log/custom_recorder_example --cxxopt=-I<user/specific/repo/folder> <build_target>
```

Therein,

- `--//score/mw/log/flags:KCustom_Logging=True` - feature flag, enable custom logging feature, False by default and recorder will fallback to `EmptyRecorder`
- `--//score/mw/log/flags:custom_recorder_impl=<impl_target>` - label flag, path to your implementation target
- `--cxxopt=-I<user/specific/repo/folder>` - specify include path to header file `custom_recorder.h`

## Configuration

To route the log messages to custom recorder, the application specific configuration file `logging.json` with "logMode" = "kCustom" should provided. For example:

```json
{
    "appId": "TEST",
    "appDesc": "test custom recorder",
    "logMode": "kCustom",
    "logLevel": "kVerbose",
}
```

## Example

A reference implementation is available in "score/mw/log/custom_recorder_example".
