# Result

Within our project we decided not to use exceptions, but rather the concept of [expected](https://wg21.link/p0323). For
that there is an implementation of the C++23 feature in [./details/expected](details/expected/expected.h).

In order to use this in a meaningful way, it means we have to provide respective error classes. There are multiple
use-cases like logging error messages or comparing on expected error states to trigger other countermeasures.

Instead of duplicating this logic within the code base, this library shall provide a common way to define error
types. For this purpose we re-use the ideas presented in adaptive AUTOSAR with the `ara::core::ErrorCode` class and its
related functions and classes.

## Usage

In order to provide your custom error code in a custom domain, you have to implement the following.

1. A custom error code
```c++
enum class MyErrorCode : score::result::ErrorCode
{
    ... // your error code values
};
```

2. A custom error domain
```c++
class MyErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(score::result::ErrorCode const& code) const noexcept override
    {
        switch (static_cast<MyErrorCode>(code))
        {
            case MyErrorCode::kFirstError:
                return "First Error!";
            case MyErrorCode::kSecondError:
                return "Second Error!";
            default:
                return "Unknown Error!";
        }
    }
};
```

3. A `MakeError()` function for ADL lookup
```c++
constexpr MyErrorDomain my_error_domain;

score::result::Error MakeError(MyErrorCode code, std::string_view user_message = "") noexcept
{
    return {static_cast<score::result::ErrorCode>(code), my_error_domain, user_message};
}
```

After that you are free to use the errors as follows:
```c++

score::Result<std::string> MyFancyFunction(std::int32_t number) {
    if(number == 42) {
        return score::MakeUnexpected(MyErrorCode::kFirstError, "You did it!");
    }
    return "Try Again!";
}

auto first_try = MyFancyFunction(42);
if(!first_try && first_try.error() == MyErrorCode::kFirstError) {
    ara::log::LogInfo() << first_try.error();
}

```


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
In case `T == void`, use `ResultBlank` instead of `Result<void>`.
