/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

/// @file
/// @brief Google Benchmark suite for score::json::JsonParser parsing performance.
///
/// The suite parses synthetic JSON documents that isolate individual cost
/// centers of the parser so that bottlenecks can be attributed to a concrete
/// value category:
///   * integer numbers      -> integer number-parser path
///   * floating point        -> floating point number-parser path
///   * plain strings         -> string copy / string-buffer path
///   * escaped strings       -> escape-sequence decoding path
///   * booleans / nulls      -> keyword-matching path
///   * flat object keys      -> key parsing + object (unordered_map) insertion
///   * deep nesting          -> depth counter / recursion path
///   * realistic config      -> mixed, representative workload
///
/// Every benchmark scales the document size via state.range(0) and reports both
/// bytes/s (throughput) and items/s. Comparing throughput across categories and
/// across sizes reveals which value category dominates and whether a category
/// scales super-linearly (a likely algorithmic bottleneck).
///
/// The realistic-config workload is additionally parsed both from a buffer and
/// from a file, so file-IO overhead can be separated from pure parsing cost.

#include "score/json/json_parser.h"

#include <benchmark/benchmark.h>

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <string>
#include <string_view>

namespace score
{
namespace json
{
namespace
{

constexpr std::size_t kMaxRange{32768};

std::string MakeIntegerArray(const std::size_t count)
{
    std::string out{};
    out.reserve((count * 12U) + 2U);
    out.push_back('[');
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        // Vary magnitude and sign to exercise the whole integer parser.
        const std::int64_t value = (i % 2U == 0U) ? static_cast<std::int64_t>(i * 7919U)
                                                   : -static_cast<std::int64_t>(i * 104729U);
        out += std::to_string(value);
    }
    out.push_back(']');
    return out;
}

std::string MakeDoubleArray(const std::size_t count)
{
    std::string out{};
    out.reserve((count * 20U) + 2U);
    out.push_back('[');
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        // Fractions, negatives and exponents stress the floating-point path.
        const double value = (static_cast<double>(i) * 0.333333) - 12345.6789;
        char buffer[32];
        const int written = std::snprintf(buffer, sizeof(buffer), "%.9e", value);
        out.append(buffer, static_cast<std::size_t>(written));
    }
    out.push_back(']');
    return out;
}

std::string MakeStringArray(const std::size_t count)
{
    std::string out{};
    out.reserve((count * 32U) + 2U);
    out.push_back('[');
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        out += "\"value_field_";
        out += std::to_string(i);
        out += "_abcdefghijklmnop\"";
    }
    out.push_back(']');
    return out;
}

std::string MakeEscapedStringArray(const std::size_t count)
{
    std::string out{};
    out.reserve((count * 40U) + 2U);
    out.push_back('[');
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        // Every value contains quotes, backslashes and the control escapes that
        // the parser supports (\b \f \n \r \t \\ \/ \"), stressing the
        // escape-decoding branch. Note: \u (unicode) is intentionally not
        // emitted because this parser rejects it.
        out += "\"esc\\t";
        out += std::to_string(i);
        out += "\\n\\\"quoted\\\"\\\\path\\/end\\r\\b\\f\"";
    }
    out.push_back(']');
    return out;
}

std::string MakeBoolArray(const std::size_t count)
{
    std::string out{};
    out.reserve((count * 6U) + 2U);
    out.push_back('[');
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        out += (i % 2U == 0U) ? "true" : "false";
    }
    out.push_back(']');
    return out;
}

std::string MakeNullArray(const std::size_t count)
{
    std::string out{};
    out.reserve((count * 5U) + 2U);
    out.push_back('[');
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        out += "null";
    }
    out.push_back(']');
    return out;
}

std::string MakeFlatObject(const std::size_t count)
{
    std::string out{};
    out.reserve((count * 32U) + 2U);
    out.push_back('{');
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        out += "\"key_";
        out += std::to_string(i);
        out += "\":\"val_";
        out += std::to_string(i);
        out += "\"";
    }
    out.push_back('}');
    return out;
}

// Produces {"n":{"n":{ ... {"leaf":0} ... }}} with `depth` nested objects.
std::string MakeNestedObject(const std::size_t depth)
{
    std::string out{};
    out.reserve((depth * 5U) + 16U);
    for (std::size_t i = 0U; i < depth; ++i)
    {
        out += "{\"n\":";
    }
    out += "0";
    for (std::size_t i = 0U; i < depth; ++i)
    {
        out.push_back('}');
    }
    return out;
}

// A mixed document with `count` records, each holding several value categories,
// representative of a typical configuration file.
std::string MakeRealisticConfig(const std::size_t count)
{
    std::string out{};
    out.reserve(count * 128U);
    out += "{\"version\":2,\"enabled\":true,\"records\":[";
    for (std::size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
        {
            out.push_back(',');
        }
        out += "{\"id\":";
        out += std::to_string(i);
        out += ",\"name\":\"record_";
        out += std::to_string(i);
        out += "\",\"weight\":";
        char buffer[32];
        const int written = std::snprintf(buffer, sizeof(buffer), "%.4f", static_cast<double>(i) * 1.5);
        out.append(buffer, static_cast<std::size_t>(written));
        out += ",\"active\":";
        out += (i % 3U == 0U) ? "false" : "true";
        out += ",\"tags\":[\"a\",\"b\",\"c\"],\"meta\":null}";
    }
    out += "]}";
    return out;
}

using Generator = std::string (*)(std::size_t);

template <Generator generate>
void BM_ParseBuffer(benchmark::State& state)
{
    const auto count = static_cast<std::size_t>(state.range(0));
    const std::string payload = generate(count);
    const JsonParser parser{};

    for (auto _ : state)
    {
        auto result = parser.FromBuffer(std::string_view{payload});
        benchmark::DoNotOptimize(result);
        if (!result.has_value())
        {
            state.SkipWithError("JSON parsing failed");
            break;
        }
    }

    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) *
                            static_cast<std::int64_t>(payload.size()));
    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) *
                            static_cast<std::int64_t>(count));
    state.counters["document_bytes"] = benchmark::Counter(static_cast<double>(payload.size()));
}

// The realistic document is parsed both from a buffer and from a file on
// identical content so that pure parsing cost can be separated from file-IO
// overhead (open / read into memory) by comparing the two results below.
void BM_RealisticConfig_FromBuffer(benchmark::State& state)
{
    const auto count = static_cast<std::size_t>(state.range(0));
    const std::string payload = MakeRealisticConfig(count);
    const JsonParser parser{};

    for (auto _ : state)
    {
        auto result = parser.FromBuffer(std::string_view{payload});
        benchmark::DoNotOptimize(result);
        if (!result.has_value())
        {
            state.SkipWithError("FromBuffer parsing failed");
            break;
        }
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) *
                            static_cast<std::int64_t>(payload.size()));
}

void BM_RealisticConfig_FromFile(benchmark::State& state)
{
    const auto count = static_cast<std::size_t>(state.range(0));
    const std::string payload = MakeRealisticConfig(count);
    const std::string path =
        "/tmp/json_parser_benchmark_" + std::to_string(::getpid()) + "_" + std::to_string(count) + ".json";
    {
        std::ofstream stream{path, std::ios::binary | std::ios::trunc};
        stream.write(payload.data(), static_cast<std::streamsize>(payload.size()));
    }

    const JsonParser parser{};
    for (auto _ : state)
    {
        auto result = parser.FromFile(path);
        benchmark::DoNotOptimize(result);
        if (!result.has_value())
        {
            state.SkipWithError("FromFile parsing failed");
            break;
        }
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) *
                            static_cast<std::int64_t>(payload.size()));

    static_cast<void>(std::remove(path.c_str()));
}

BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeIntegerArray)->RangeMultiplier(8)->Range(8, kMaxRange);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeDoubleArray)->RangeMultiplier(8)->Range(8, kMaxRange);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeStringArray)->RangeMultiplier(8)->Range(8, kMaxRange);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeEscapedStringArray)->RangeMultiplier(8)->Range(8, kMaxRange);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeBoolArray)->RangeMultiplier(8)->Range(8, kMaxRange);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeNullArray)->RangeMultiplier(8)->Range(8, kMaxRange);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeFlatObject)->RangeMultiplier(8)->Range(8, kMaxRange);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeNestedObject)->RangeMultiplier(8)->Range(8, 512);
BENCHMARK_TEMPLATE(BM_ParseBuffer, MakeRealisticConfig)->RangeMultiplier(8)->Range(8, 4096);

BENCHMARK(BM_RealisticConfig_FromBuffer)->RangeMultiplier(8)->Range(8, 4096);
BENCHMARK(BM_RealisticConfig_FromFile)->RangeMultiplier(8)->Range(8, 4096);

}  // namespace
}  // namespace json
}  // namespace score
