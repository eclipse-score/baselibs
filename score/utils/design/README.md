# Detailed Design

`lib/utils` imposes a grouping function on small functionalities that can be reused in other parts of our code, but does
not really match into one of our common libraries.

Overall we would like to get rid of `lib/utils` since it is quite unspecific and it is unclear from the name what is
behind. But there will be always the need for a "last resort" collection facility.
That's why the detailed design does not really cover a static or dynamic design that illustrates the dependency between
the single classes of `lib/utils`, but rather how single classes operate.

## PayloadValidation

Only one function that is standalone as such.

## ScopedOperation

Is a small helper class that follows the ideas of https://en.cppreference.com/w/cpp/experimental/scope_exit where you
the class just stores a callback and executes that when the class gets destructed.

## Base64

The `Base64` utility provides functions for encoding and decoding data using the Base64 scheme, which is commonly used for representing binary data in an ASCII string format. This is useful for tasks such as embedding binary data in text files (e.g., JSON, XML) or transmitting data over protocols that are not binary-safe.

### Features

- Implements both encoding and decoding of Base64 according to the standard.

### Usage

- `EncodeBase64(const std::vector<std::uint8_t>& buffer)`: Encodes a byte buffer into a Base64 string.
- `DecodeBase64(const std::string& encoded_string)`: Decodes a Base64 string back into a byte buffer.
