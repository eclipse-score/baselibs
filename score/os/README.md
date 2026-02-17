# OS Abstraction Layer (OSAL)

The OS Abstraction Layer (OSAL) is an adapter library to interface with different POSIX-like operating systems like
Linux and QNX. It improves the portability of code by providing an OS-independent interface to the operating system.

Software developers shouldn't be concerned how the OSAL implements a given functionality. For example, if a developer
wants to open a file, she doesn't need to know if her code runs on QNX or Linux. She uses the platform-independent OSAL
API and doesn't care if OSAL implements the API with a POSIX API or with a QNX/Linux-specific API (which may be more
performant).

> An operating system abstraction layer (OSAL) provides an application programming interface (API) to an abstract
> operating system making it easier and quicker to develop code for multiple software or hardware platforms.

Source: https://en.wikipedia.org/wiki/Operating_system_abstraction_layer

## Usage

To include OSAL in your package, depend on individual OSAL modules. This allows for fine-grained control to only include
what is needed. To determine whether a module is ASIL-B qualified, please refer to the safety qualification of the
underlying system and the corresponding POSIX header. Please be aware, that the qualification of a function does not
mean that using this function in your context is allowed by the safety manual. It is the responsibility of the users to
verify the assumptions of use and **adhere to the safety manual**.
See [QM and ASIL B qualification](#qm-and-asil-b-certification) for more details.

We intentionally restrict the visibility of the individual OSAL modules so that new use cases are first reviewed by an
OSAL maintainer. If your library needs for example `//platform/aas/lib/os:unistd` the visibility could be added if
agreed with OSAL maintainers.

## Architecture

The following diagram shows the main Bazel targets of OSAL and how applications depend on it. The modules of OSAL are
grouped by portability.

<img alt="OSAL package" src="https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/eclipse-score/baselibs/refs/heads/main/score/os/diagrams/osal_arch.puml">

* **`//platform/aas/lib/os`**
    * Contains OS-agnostic low-level APIs that should do nothing more than wrap the underlying OS call
* **`//score/os/utils`**
    * Contains OS-agnostic high-level APIs that may contain some commonly used sequences for the purpose of reuse
* **`//score/os/linux`**
    * Provides Linux-specific APIs for applications that have to use a Linux-specific functionality (
      e.g., [epoll](https://man7.org/linux/man-pages/man7/epoll.7.html)).
* **`//score/os/qnx`**
    * Provides QNX-specific APIs for applications that have to use a QNX-specific functionality (
      e.g. [channels](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.sys_arch/topic/ipc_Channels.html), [pulses](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.sys_arch/topic/ipc_Pulses.html)).

### Modularization

We cannot place all code into one header. This header would get huge. It would also be a single dependency and
everything would need to be rebuilt whenever it is modified. Additionally, separate modules lower qualification effort
for users, since they can exclude modules missing the necessary qualification.

Thus, we modularize the code similar to how POSIX, Linux, and QNX are doing it. We create the Bazel target `fcntl` with
the files `fcntl.h` and `fcntl.cpp` to implement support
for [`fcntl(3p)`](https://www.man7.org/linux/man-pages/man3/fcntl.3p.html).

### Public API

OSAL provides a low-level and high-level public API. Since OSAL doesn't leak any POSIX/QNX/Linux APIs, the OSAL APIs
often return [`score::cpp::expected`](broken_link_g/swh/amp/blob/master/include/score/expected.hpp), which is
similar to `ara::core::Result` and `score::Result` and stores either the actual return value or an error code.
See [Error Handling](#error-handling) for more details.

#### Low-Level API

The low-level public API is a thin wrapper around the legacy C APIs provided by POSIX-like operating systems (
e.g., `mmap()`, `stat()`).

```cpp
#include "score/os/pthread.h"

std::thread thread{[]{
    // Do some work
}};

// Set thread name
score::os::Pthread::instance().setname_np(thread.native_handle(), "foobar");  // calls pthread_setname_np() internally

// Get thread name
constexpr std::size_t kMaxLength{16U};
std::array<char, kMaxLength> name;
score::os::Pthread::instance().getname_np(thread.native_handle(), name.data(), kMaxLength);  // calls pthread_getname_np() internally
```

##### Usage of Singletons

OSAL currently uses singletons (score::os::Something::instance) in many places, but this is not an ideal design
pattern. (
See: [Design Guidelines: Singletons](broken_link_g/swh/xpad_documentation/blob/master/enhancement_proposals/proposals/14_software_design_guidelines.md#singleton))
It has provided historical benefit by allowing easier migration from free functions in the POSIX API to use of the OSAL,
but we should aim to take the next step in migration by properly instantiating implementations in our applications. This
means:

1. Do not add `instance` methods to new classes when you add them to OSAL
1. Do not use `instance` methods when you start to use OSAL functions in your applications for the first time
1. Remove existing uses of `instance` method calls and replace them with properly instantiated objects when it makes
   sense to do so in your applications. This often has a lower priority than our daily business, so we will need to find
   the appropriate moments to do these migrations at our own discretion.

#### High-Level API

The high-level API is located in the package `//score/os/utils`. It provides a modern, type-safe, and elegant
C++14 API. It uses the low-level public API in its implementation. Users are highly encouraged to use the high-level
public API if possible. The intention of those APIs is to facilitate the most common use cases and to decouple customer
applications from OS implementation details as much as possible.

```cpp
#include "score/os/utils/thread.h"

std::thread thread{[]{
    // Do some work
}};

// Set thread name
score::os::set_thread_name(thread, "foobar");

// Get thread name
score::cpp::expected<std::string, score::os::Error> result{score::os::get_thread_name(thread)};
if (result.has_value()) {
    std::string name{result.value()};
}
else {
    score::os::Error error{result.error()};
}
```

### OS-specific implementations

OSAL modules do not leak POSIX, QNX, or Linux specific implementation details. The public API is purely ISO C++14.
However, sometimes OS vendors extend POSIX APIs like [`mmap()`](https://www.man7.org/linux/man-pages/man3/mmap.3p.html)
with OS-specifics. The implementation can use OS-specific extensions of POSIX APIs or OS-specific APIs
like [`timerfd_create()`](https://man7.org/linux/man-pages/man2/timerfd_create.2.html) (Linux-specific)
or [`spawn()`](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/spawn.html) (QNX-specific)
as long as they are only used in the implementation (e.g., within
an [`#if defined(__QNX__)`](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.prog/topic/devel_Neutrino_specific.html)
or `#if defined(__linux__)` block). These OS-specifics shall not be exposed in the API of OSAL!

An application that depends on a library in the packages `//score/os/linux` or `//score/os/qnx` is
not portable! Visibility to the Bazel targets in these packages is therefore approved only on a case-by-case basis and
has to be justified.

[Making your code more portable](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.prog/topic/devel_PortableCode.html)
and [What's in a Function Description? - Classification](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/summary.html#summary__CLASSIFICATION)
provides more information on how to distinguish POSIX and QNX-specific APIs of QNX.

### Requirements

The requirements of OSAL are maintained at [#4976935](broken_link_cp/issue/4976935).

### QM and ASIL B certification

ASIL B applications (and thus the libraries they depend on) are not allowed to use all system APIs, but only the ones
certified to ASIL B (or higher). Only QM applications are allowed to use all system APIs.

Even safety-certified operating systems like QNX contain C/C++ headers and APIs that are not covered by the
certification. The scope of certification for QNX can be found in
the [QNX OS for Safety (QOS) certificate details](https://fs-products.tuvasi.com/certificates?filter_prod=1&filter_apps=1&keywords=QNX&productcategory_id=1&x=0&y=0#prodid_6694) (
i.e., which headers are certified to which ASIL). The safety manual also contains restrictions about system API usage in
safety-critical applications (e.g., regarding [slog2](broken_link_cp/issue/4752500)).

Independent on whether an API is certified to ASIL B or higher, it is up to the user to determine whether his specific
use case is adhering to the safety manual. This is not something that OSAL can take care of. ASIL qualification of an
API solely means that there are no unintended side effects that would violate the safety manual when the API is used
according to the safety manual. Thus, OSAL does not exempt users from a safety analysis of their application and the
therein contained usage of the POSIX API. For this reason, we also decided against providing "ASIL-B" and "QM" targets,
as this would give the wrong impression of a safety qualification for all API use cases.

## Error Handling

The standard way of POSIX is to report errors through the global `errno` variable. A user can then compare this variable
against symbolic values like `EOF`. This approach cannot be reused by our OS-Abstraction, since Linux and QNX will fill
defined OS specific errors in addition to the ones defined by POSIX.

Instead of using another global variable, we directly use the approach of `score::cpp::expected` to either return the result or
error codes. Since we don't want to operate on plain integers and rather use symbolic values. We define a custom `Error`
class which will act as OS abstraction and contain the set of all supported errors. Each operating system specific
implementation then needs to implement some translation from the OS-specific error, to the OS-agnostic one.

In more detail this approach can enforce that nobody will use `errno` directly and thus `cerror.h`. This way it will
also be possible in future to ensure that a certain function only tests a defined sets of errors (see the AoU for
SafeLinux for more explanation). Since the OS might also set errors that are not described anywhere (see description
of `errno` in QNX) we define an `unknown` error type and provide a logging output that explains the OS-specific
error. This feature shall really only be used for logging purposes and not for error reaction (since this would defeat
portability of code).

## Why Use an OSAL?

The software platform has a lot of dependencies to the lower layers of the system, one of them is the operating system.
Having a well-defined boundary between the software platform and the underlying operating system has multiple benefits.
The following sections explain them in more detail.

### Testability

Writing testable code is important. Unit tests shouldn't cause side effects (i.e., changing the operating system state)
or invoke filesystem or networking APIs. Unit tests should also be independent of the underlying host, because the
host might not be configured as expected.

To get code under test, there are 3 seam types:

- preprocessor seams
- link seams
- object seams

From the
article [Testing Effectively With Legacy Code](https://www.informit.com/articles/article.aspx?p=359417&seqNum=3):

> It is important to choose the right type of seam when you want to get pieces of code under test. In general, object
> seams are the best choice in object-oriented languages. Preprocessing seams and link seams can be useful at times but
> they are not as explicit as object seams. In addition, tests that depend upon them can be hard to maintain. I like to
> reserve preprocessing seams and link seams for cases where dependencies are pervasive and there are no better
> alternatives.

OSAL uses object seams. The
book [Working Effectively with Legacy Code](https://www.oreilly.com/library/view/working-effectively-with/0131177052/)
shows on page 239ff. (chapter 19 "My Project Is Not Object Oriented. How Do I Make Safe Changes?", section "Taking
Advantage of Object Orientation") how to use object seams to get legacy C code under test.

#### Why Not Use Link Seams?

* Link seams work on the build system level. Link seams require that headers and implementations are in distinct
  libraries. A unit test uses a test double by linking the test implementation instead of the production implementation.
  With object seams, this is not necessary. Object seams work on the code level. A unit test creates an instance of a
  mock and uses the mock instead of the production code via dependency injection.
* `--wrap` is a GNU-specific possibility to exchange specific function calls to replace function calls with a custom
  function. You can read more about it in the [`ld(1)`](https://www.man7.org/linux/man-pages/man1/ld.1.html) man page.
  This approach is not portable.

#### Example

The library provides access to the system APIs through interface classes. This allows creating different implementations
for production and testing, i.e. wrappers that perform actual POSIX calls and mock versions that can be easily used to
simulate different outputs and error conditions.

For example `pthread.h` has an abstract class `Pthread` that has (pure) virtual methods `setname_np()`
and `getname_np()`. The class method `instance()` returns the singleton instance that the production code uses
(see [Usage of Singletons](#usage-of-singletons) regarding the usage of singletons). OSAL provides OS-specific
subclasses for production and a mock `MockPthread` (for testing).

To use a `Pthread` mock in a test:

```cpp
MockPthread pthread;
score::os::Pthread::set_testing_instance(pthread);  // subsequent calls to score::os::Pthread::instance() return the mock object
```

To restore the production instance:

```cpp
score::os::Pthread::restore_instance();  // subsequent calls to score::os::Pthread::instance() return the production object
```

For modules that no longer use singletons (see [Low-Level API](#low-level-api) regarding the usage of singletons) you
can use the normal dependency injection pattern for object-seams. E.g. depend only on the interface and inject an
instance of the production or testing implementation depending on the use case.

### Portability

Switching the underlying operating system causes big impacts in the code. Even though POSIX is a standard, sometimes it
is necessary to use some APIs specific to a particular OS (e.g., Linux, QNX). But also when it comes to POSIX, some
operating systems are more compliant than others. Whenever we are forced to switch or even support multiple operating
systems, we would have a hard time to port our code to new OSes without an OSAL.

Contributions to the high-level API of OSAL need to take care of adapting the functionality, so it is compliant to POSIX
as much as possible and that it works for all supported operating systems in a consistent manner.

### Safety

#### Enforce Assumptions Of Use

Safety-critical operating systems document assumptions of use (AoU) in their safety manuals. The OS vendor describes
which use cases of the software are certified and which are not. Users of the OS (developers, architects, integrators,
etc.) have to ensure that the AoUs are fulfilled. OSAL is a central component in the overall architecture where OS AoUs
can be implemented.

This does not mean, that all usage of OSAL is in accordance with the AoUs to your component. This is something the users
of OSAL are required to check whenever they use an API. A good example is the use of the system call `open` with the
open flag `O_EXCL`. This system call is qualified as ASIL B on QNX, but it can still be used in a manner that will
conflict with freedom of interference (FFI). Simply by opening a configuration file of a different application will keep
that application from being able to read it. Hence, do not blindly use system calls with the assumption that your AoUs
are covered by OSAL.

#### Testing of OSAL

Unit testing a wrapper around the POSIX API is difficult. The same reason why OSAL exists ([Testability](#testability))
is the bane of testing this library. Since there is no way to inject behavior, tests must trigger edge cases of the API
that largely depend on implementation details of the underlying system. In some cases possible return codes are no
longer relevant in newer implementations or were never used in the system where the unit tests are executed. All this is
to say, that OSAL has full test coverage, but that part of it comes from unit tests and the other part from manual code
analysis. The following checklist is used for manual code analysis:

- General questions:
    - [ ] There is no complex logic involved (e.g. mapping of values, transformations, ...)
- For all calls to the Posix API:
    - [ ] The call is prepended with the global namespace (`::`)
    - [ ] The types of all parameters are consistent with the definition in
      the [man pages](https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.lib_ref/topic/about.html) (
      no implicit casts)
    - [ ] OSAL does error checking
    - [ ] The check for an error (return value,...) is consistent with
      the [man pages](https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.lib_ref/topic/about.html)
    - [ ] If errors are reported via `errno`, OSAL reads `errno` with no other direct or indirect call to POSIX API
      inbetween

To create a PR using this checklist, compare your own branch to the target branch on GitHub and
append `?quick_pull=1&template=osal.md` to the URL.

##### Test case derivation method and metadata

FFI components does not have requirements, so to derive test case [Analysis of equivalence classes and boundary values](broken_link_cf/display/ESDF/Test+Method%3A+Analysis+of+equivalence+classes+and+boundary+values) is used. Main equivalence class, applicable form most of tests, is the division between error output and no error output, data for it is taken directly from QNX or POSIX documentation of APIs.

<details><summary>Discarded Alternatives</summary>
The following other options were considered, but ultimately discarded for below reasons:

- Linker option `-Wl,--wrap` (https://drewdevault.com/2016/07/19/Using-Wl-wrap-for-mocking-in-C.html)
    - Drawbacks:
        - Hard to split mocks between different unit tests
        - Not all points from above checklist can be ensured with this approach
- Runtime linker option `LD_PRELOAD`
    - Drawbacks:
        - Only works with symbols linked at runtime
        - Requires one library per unit test to inject appropriate mock
        - Not all points from above checklist can be ensured with this approach
- Preprocessor macros or variadic function to wrap POSIX API calls
    - Drawbacks:
        - Forwarding of parameters error-prone and does not prohibit implicit casts
        - Not all points from above checklist can be ensured with this approach

</details>

## Detailed design

The lib os documentation can be found under: [Lib os documentation](./libos.md)

## FAQ

### How do we ensure everybody uses OSAL?

We don't. The problem is that currently we cannot enforce that every developer is using our OS abstraction. They could
just include any POSIX API header and use them (even without declaring a dependency). This is because right now the
POSIX headers are in the global namespace that are included by default for every compiler invocation (part of the Bazel
toolchains). The only possibility to prevent misuse, would be to find a way to enable or disable the POSIX headers
within this global include path. Best would be a Bazel toolchain `feature` that would only be enabled within our
abstraction.

OSAL is not a single component that enforces compliant use of OS APIs. It might be necessary to run automatic checks in
the CI system (e.g., via a custom clang-tidy checker) to ensure only approved components like OSAL invoke operating
system APIs.

### This is crazy, this will cost us a lot of effort to abstract all POSIX/QNX/Linux calls!

Yes, that is true! If you have a better proposal on how to enforce the correct usage and easy testing of these function
calls, please contact the architecture team.
