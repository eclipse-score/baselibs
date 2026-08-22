# Global replacement of exceptions for std::abort()

Two motivations exist for preventing the use of C++ exceptions:
- Safety-relevant applications should avoid use of exceptions, further details
  can be found under the safety concept
  and the [AEP-002: C++ Embedded Guidelines](broken_link_cf/pages/viewpage.action?pageId=193061641)
- C++ exceptions can cause performance issues since uncaught exceptions may
  cause a thread to hang with elevated processor use, rather than terminate.
  Further details in [Ticket-115538](broken_link_j/Ticket-115538)

Therefore, we require an approach that satisfies the following use cases:
- Allow the possibility of replacing exceptions both globally and per
  application.
- Support this behaviour on both target toolchains and host toolchains,
  to allow users to approximate the behavior of target and host code.

Exception allocation to be overridden with a call to `std::abort()` is accomplished
through a custom library equivalent to the `aborts_upon_exception` target
provided in this package. This is implemented at toolchain level, by taking
advantage of link order. Therefore, for a toolchain to support this feature,
it must ensure such custom library is linked before the standard library.

## Toolchain feature approach

We use [C++ toolchain features](https://bazel.build/docs/cc-toolchain-config-reference#features)
to provide the ability to enable this feature globally, or per unit, and
establish a standard interface for toggling it across different toolchains.

The feature must be named `aborts_upon_exception` and toolchains must support
this feature individually, since they require a custom pre-built library.
Various unit tests are also contained in this package to ensure that our
toolchains correctly support this feature.

Note that for global activation, the feature's `enabled` field (part of the
toolchain configuration) should not be set to `True`. Instead, specify the
feature via flag on the project `.bazelrc`, i.e., `--features=aborts_upon_exception`.
This will ensure toolchains behave consistently and exception usage is still
allowed for non-target code, such as build/test tools.

In case a project has restricted the use of exceptions globally, but still needs
to support some edge cases (as it is typical during migrations to guarantee no
new exceptions are introduced, while giving time to resolve existing ones),
Bazel toolchains can support this via [Feature Relationships](https://bazel.build/docs/cc-toolchain-config-reference#feature-relationships).
Feature relationships allows one feature to not be expanded to the build command
if another feature is set. In this sense, we provide another feature called
`throws_upon_exception` which sole purpose is to prevent linking to the custom
library, thus allowing exceptions on a particular target. In other words, 
when `throws_upon_exception` feature is set, `aborts_upon_exception` feature
is deactivated. Note that feature negation could result in configuration conflicts
and shall not be used.

### How to enable a toolchain to support aborts\_upon\_exception

This assumes as pre-requisite that a C++ toolchain is already configured in Bazel.

Steps to extend a toolchain to support `aborts_upon_exception`:
1. Build the `aborts_upon_exception` library with the desired toolchain. e.g.
   `bazel build --config=spp_host_gcc //score/language/safecpp/aborts_upon_exception:abortsuponexception`
2. Add the library either to your toolchain `sysroot`, or as an extra package
   that's exposed into compilation actions sandbox.
3. Add the respective linking flag `-labortsuponexception`. **Ensure that this
   flag is added before C++ standard library flag, `-lc++` or `-lstdc++`**. If
   you added an extra package, ensure that the library search directory is also
   configured, typically using the `-L` flag.
4. Test your toolchain.
   e.g. `bazel test --config=spp_host_gcc //score/language/safecpp/aborts_upon_exception:abortsuponexception_toolchain_test`
