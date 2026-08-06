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

#include <cstdint>
#include <cstring>
#include <string_view>

// C functions for Rust FFI testing - these work with actual bmw::Result
extern "C" {

// String view FFI functions for layout compatibility testing

// Create a C++ std::string_view and return it as void*
void* create_cpp_string_view(const char* data, std::size_t len)
{
    auto* sv = new std::string_view(data, len);
    return static_cast<void*>(sv);
}

void* create_empty_cpp_string_view()
{
    auto* sv = new std::string_view();
    return static_cast<void*>(sv);
}

// Destroy a C++ std::string_view
void destroy_cpp_string_view(void* sv)
{
    if (sv)
    {
        delete static_cast<std::string_view*>(sv);
    }
}

// Get the size of std::string_view
std::size_t get_cpp_string_view_size()
{
    return sizeof(std::string_view);
}

// Verify that a Rust CStringView has the same layout as std::string_view
bool verify_string_view_layout(const void* rust_view, const char* expected_data, std::size_t expected_len)
{
    if (!rust_view)
    {
        return false;
    }

    // Cast the Rust view to std::string_view and check if we can read it correctly
    const auto* cpp_view = static_cast<const std::string_view*>(rust_view);

    return cpp_view->data() == expected_data && cpp_view->size() == expected_len;
}

}  // extern "C"
