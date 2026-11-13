# Passing a token to another CXX bridge

A downstream CXX bridge can bind `RawStopToken` to the `CxxStopToken` alias and pass a borrowed token to a C++ function that accepts `const score::cpp::stop_token&`:

```rust
#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("score/language/rust/stop_token/cpp/stop_token_adapter.h");

        #[namespace = "score::language::rust::stop_token"]
        #[cxx_name = "CxxStopToken"]
        type StopToken = stop_token::RawStopToken;

        fn wait(self: &Foo, token: &StopToken) -> bool;
    }
}

foo.wait(token.as_raw());
```

The downstream Bazel target must depend on `@score_baselibs//score/language/rust/stop_token:stop_token_adapter`.
The downstream crate must use the same `cxx` crate instance as this crate because opaque types from separate `cxx` crate instances are incompatible.

The returned borrow cannot outlive its owning `StopToken`.
C++ must not retain the reference beyond the call.
Copy the token in C++ when it must be retained.

`RawStopToken` exposes read-only observation and is `Sync`.
It cannot be constructed from Rust.
