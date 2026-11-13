# Passing a token to another CXX bridge

A downstream CXX bridge can bind `RawStopToken` to `score::cpp::stop_token` and pass a borrowed token to a C++ function that accepts `const score::cpp::stop_token&`:

```rust
#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        #[namespace = "score::cpp"]
        #[cxx_name = "stop_token"]
        type StopToken = stop_token::RawStopToken;

        fn wait(self: &Foo, token: &StopToken) -> bool;
    }
}

foo.wait(token.as_raw());
```

The downstream crate must use the same `cxx` crate instance as this crate because opaque types from separate `cxx` crate instances are incompatible.

The returned borrow cannot outlive its owning `StopToken`.
C++ must not retain the reference beyond the call.
Copy the token in C++ when it must be retained.

`RawStopToken` exposes read-only observation and is `Sync`.
It cannot be constructed from Rust.
