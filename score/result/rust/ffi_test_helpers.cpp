/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

#include "score/result/result.h"
#include <cstring>
#include <string_view>

namespace
{

// Test ErrorDomain for FFI testing
class FFITestErrorDomain : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (code)
        {
            case 12345:
                return "FFI test success turned error";
            case 54321:
                return "Test error from C++";
            case 98765:
                return "Rust created error";
            case 111:
                return "Bool test error";
            case 123:
                return "Discriminant test error";
            default:
                return "Unknown FFI test error";
        }
    }

    // Add virtual destructor to fix the warning
    virtual ~FFITestErrorDomain() = default;
};

static const FFITestErrorDomain ffi_test_domain{};

}  // anonymous namespace

// C functions for Rust FFI testing - these work with actual score::Result
extern "C" {

// Create a score::Result<int32_t> with a success value
void* create_bmw_result_success_int32(int32_t value)
{
    auto* result = new score::Result<int32_t>(value);
    return static_cast<void*>(result);
}

// Create a score::Result<int32_t> with an error
void* create_bmw_result_error_int32(int32_t error_code, const char* message)
{
    auto error = score::result::Error(error_code, ffi_test_domain, message ? message : "");
    auto* result = new score::Result<int32_t>(score::MakeUnexpected<int32_t>(error));
    return static_cast<void*>(result);
}

// Create a score::Result<bool> with a success value
void* create_bmw_result_success_bool(bool value)
{
    auto* result = new score::Result<bool>(value);
    return static_cast<void*>(result);
}

// Create a score::Result<bool> with an error
void* create_bmw_result_error_bool(int32_t error_code, const char* message)
{
    auto error = score::result::Error(error_code, ffi_test_domain, message ? message : "");
    auto* result = new score::Result<bool>(score::MakeUnexpected<bool>(error));
    return static_cast<void*>(result);
}

// Clean up allocated results
void destroy_bmw_result_int32(void* result)
{
    if (result)
    {
        delete static_cast<score::Result<int32_t>*>(result);
    }
}

void destroy_bmw_result_bool(void* result)
{
    if (result)
    {
        delete static_cast<score::Result<bool>*>(result);
    }
}

// Get sizes for comparison
size_t get_bmw_result_int32_size()
{
    return sizeof(score::Result<int32_t>);
}

size_t get_bmw_result_bool_size()
{
    return sizeof(score::Result<bool>);
}

size_t get_bmw_error_size()
{
    return sizeof(score::result::Error);
}

// Verify a Rust-created result can be interpreted as a C++ result
bool verify_rust_result_as_cpp_int32(const void* rust_result,
                                     bool should_have_value,
                                     int32_t expected_value_or_error_code)
{
    if (!rust_result)
    {
        return false;
    }

    const auto* cpp_result = static_cast<const score::Result<int32_t>*>(rust_result);

    try
    {
        if (should_have_value)
        {
            if (!cpp_result->has_value())
            {
                return false;
            }
            return cpp_result->value() == expected_value_or_error_code;
        }
        else
        {
            if (cpp_result->has_value())
            {
                return false;
            }
            // For error case, just check if we can access the error and it matches
            return *cpp_result->error() == expected_value_or_error_code;
        }
    }
    catch (...)
    {
        // If any exception occurs during C++ access, the layout is incompatible
        return false;
    }
}

// Print memory layout for debugging
void print_bmw_result_memory_layout()
{
    printf("\n=== BMW Result Memory Layout Analysis ===\n");

    // Create instances to analyze
    score::Result<int32_t> success_result(42);
    auto error = score::result::Error(123, ffi_test_domain, "Test error message");
    score::Result<int32_t> error_result = score::MakeUnexpected<int32_t>(error);

    printf("bmw::Result<int32_t> analysis:\n");
    printf("  Size: %zu bytes\n", sizeof(score::Result<int32_t>));
    printf("  Alignment: %zu bytes\n", alignof(score::Result<int32_t>));
    printf("  Success result has_value: %s\n", success_result.has_value() ? "true" : "false");
    if (success_result.has_value())
    {
        printf("  Success result value: %d\n", success_result.value());
    }
    printf("  Error result has_value: %s\n", error_result.has_value() ? "true" : "false");
    if (!error_result.has_value())
    {
        printf("  Error result error code: %d\n", *error_result.error());
    }

    printf("\nbmw::result::Error analysis:\n");
    printf("  Size: %zu bytes\n", sizeof(score::result::Error));
    printf("  Alignment: %zu bytes\n", alignof(score::result::Error));
    printf("  Error code: %d\n", *error);
    printf("  Error message: '%.*s'\n", (int)error.UserMessage().size(), error.UserMessage().data());

    // Dump raw bytes for both success and error results
    printf("\nRaw memory dump (first 64 bytes):\n");

    printf("  Success result: ");
    const uint8_t* success_bytes = reinterpret_cast<const uint8_t*>(&success_result);
    size_t success_size = std::min(sizeof(success_result), size_t(64));
    for (size_t i = 0; i < success_size; ++i)
    {
        printf("%02x ", success_bytes[i]);
        if ((i + 1) % 16 == 0)
            printf("\n                   ");
    }
    printf("\n");

    printf("  Error result:   ");
    const uint8_t* error_bytes = reinterpret_cast<const uint8_t*>(&error_result);
    size_t error_size = std::min(sizeof(error_result), size_t(64));
    for (size_t i = 0; i < error_size; ++i)
    {
        printf("%02x ", error_bytes[i]);
        if ((i + 1) % 16 == 0)
            printf("\n                   ");
    }
    printf("\n");

    // Also dump the Error object itself
    printf("  Error object:   ");
    const uint8_t* error_obj_bytes = reinterpret_cast<const uint8_t*>(&error);
    size_t error_obj_size = std::min(sizeof(error), size_t(64));
    for (size_t i = 0; i < error_obj_size; ++i)
    {
        printf("%02x ", error_obj_bytes[i]);
        if ((i + 1) % 16 == 0)
            printf("\n                   ");
    }
    printf("\n");

    // Additional analysis for different value types
    printf("\nSize analysis for different types:\n");
    printf(
        "  bmw::Result<bool>: %zu bytes, alignment: %zu\n", sizeof(score::Result<bool>), alignof(score::Result<bool>));
    printf("  bmw::Result<uint64_t>: %zu bytes, alignment: %zu\n",
           sizeof(score::Result<uint64_t>),
           alignof(score::Result<uint64_t>));

    // Test bool results
    score::Result<bool> bool_success(true);
    score::Result<bool> bool_error = score::MakeUnexpected<bool>(error);

    printf("\n  Bool success result: ");
    const uint8_t* bool_success_bytes = reinterpret_cast<const uint8_t*>(&bool_success);
    size_t bool_success_size = std::min(sizeof(bool_success), size_t(32));
    for (size_t i = 0; i < bool_success_size; ++i)
    {
        printf("%02x ", bool_success_bytes[i]);
    }
    printf("\n");

    printf("  Bool error result:   ");
    const uint8_t* bool_error_bytes = reinterpret_cast<const uint8_t*>(&bool_error);
    size_t bool_error_size = std::min(sizeof(bool_error), size_t(32));
    for (size_t i = 0; i < bool_error_size; ++i)
    {
        printf("%02x ", bool_error_bytes[i]);
    }
    printf("\n");
}

// Structure to pass result bytes and size to Rust for verification
struct ResultBytes
{
    const uint8_t* data;
    size_t size;
};

// Create a Result<int32_t> with a known test value and return its bytes
bool create_result_int32_success_for_layout_test(int32_t test_value,
                                                 score::Result<std::int32_t>* out_result,
                                                 std::size_t bufsize)
{
    if (bufsize == sizeof(score::Result<std::int32_t>))
    {
        new (out_result) score::Result<int32_t>(test_value);
        return true;
    }
    else
    {
        return false;
    }
}

// Create a Result<int32_t> with an error and return its bytes
std::size_t create_result_int32_error_for_layout_test(int32_t error_code,
                                                      score::Result<std::int32_t>* out_result,
                                                      std::size_t bufsize)
{
    if (bufsize == sizeof(score::Result<std::int32_t>))
    {
        auto error = score::result::Error(error_code, ffi_test_domain, "Test error");
        new (out_result) score::Result<std::int32_t>(score::MakeUnexpected<int32_t>(error));
        return true;
    }
    else
    {
        return false;
    }
}

}  // extern "C"
