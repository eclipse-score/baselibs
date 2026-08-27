# Result

Within our project we decided not to use exceptions, but rather the concept of [expected](https://wg21.link/p0323). For
that there is an implementation of the C++23 feature in [./details/expected](details/expected/expected.h).

In order to use this in a meaningful way, it means we have to provide respective error classes. There are multiple
use-cases like logging error messages or comparing on expected error states to trigger other countermeasures.

Instead of duplicating this logic within the code base, this library shall provide a common way to define error
types. For this purpose we re-use the ideas presented in adaptive AUTOSAR with the `ara::core::ErrorCode` class and its
related functions and classes.

## Usage

For a complete usage example covering custom error codes, error domains, `Result<T>`, and `Result<void>`,
see [examples/integration/result/result_demo_test.cpp](../../examples/integration/result/result_demo_test.cpp).


## Design

<img alt="Static Design" src="https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/eclipse-score/baselibs/refs/heads/main/score/result/static_design.puml">

As you can see in the static architecture above, the main idea is to provide an `ErrorDomain` which users can inherit
from. In this case, they will need to implement two things. First they will need to implement an `enum` of type
`CodeType` and second a function that translates the error codes into human-readable strings (like `strerror()`). At
last, they shall provide a `MakeError()` function, which will create a respective `Error` class by providing an instance
of the ErrorDomain. This instance can be globally instantiated via a `constexpr`.

The `Error` class will be placed in the `score::details::expected` and provides functionality for logging the error
message and comparing the `Error` with the user defined error codes.

To let users of this library interact with the `score::details::expected` without them using API of implementation
details, we provide the type `Result<T>` which is a type alias of `score::details::expected<T, Error>`.
In case `T == void`, use `Result<void>` instead of `Result<void>`.

## Rust integration

There's an interface that allows one to exchange Result objects between C++ and Rust. Check the `rust` subfolder
for details.
