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
#include "score/mw/log/configuration/target_config_reader.h"

#include "score/callback.hpp"
#include "score/json/json_parser.h"
#include "score/memory/split_string_view.h"
#include "score/mw/log/detail/error.h"
#include "score/mw/log/detail/initialization_reporter.h"

#include <functional>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{

constexpr StringLiteral kEcuIdKey{"ecuId"};
constexpr StringLiteral kAppIdKey{"appId"};
constexpr StringLiteral kAppDescriptionKey{"appDesc"};
constexpr StringLiteral kLogFilePathKey{"logFilePath"};
constexpr StringLiteral kLogModeKey{"logMode"};
constexpr StringLiteral kLogLevelKey{"logLevel"};
constexpr StringLiteral kLogLevelThresholdConsoleKey{"logLevelThresholdConsole"};
constexpr StringLiteral kContextConfigsKey{"contextConfigs"};
constexpr StringLiteral kContextNameKey{"name"};
constexpr StringLiteral kStackBufferSizeKey{"stackBufferSize"};
constexpr StringLiteral kRingBufferSizeKey{"ringBufferSize"};
constexpr StringLiteral kOverwriteOnFullKey{"overwriteOnFull"};
constexpr StringLiteral kNumberOfSlotsKey{"numberOfSlots"};
constexpr StringLiteral kSlotSizeBytesKey{"slotSizeBytes"};
constexpr StringLiteral kDatarouterUidKey{"datarouterUid"};
constexpr StringLiteral kDynamicDatarouterIdentifiersKey{"dynamicDatarouterIdentifiers"};

constexpr StringLiteral kLogFileSizePolicyKey{"logFileSizePolicy"};
constexpr StringLiteral kEnabledKey{"enabled"};
constexpr StringLiteral kMaxFileSizeInBytesKey{"maxFileSizeInBytes"};
constexpr StringLiteral kNumberOfFilesKey{"numberOfFiles"};
constexpr StringLiteral kOverwriteExistingFilesKey{"overwriteExistingFiles"};
constexpr StringLiteral kTruncateOnRotationKey{"truncateOnRotation"};

// Suppress Coverity warning because:
// 1. 'constexpr' cannot be used with std::unordered_map.
// 2. Defining it as a local variable does not resolve the Coverity warning.
// 3. Converting to `if-else` statements would introduce complexity and trigger complexity warnings.
// coverity[autosar_cpp14_a3_3_2_violation]
const std::unordered_map<std::string_view, LogLevel> kStringToLogLevel{{{"kOff", LogLevel::kOff},
                                                                        {"kFatal", LogLevel::kFatal},
                                                                        {"kError", LogLevel::kError},
                                                                        {"kWarn", LogLevel::kWarn},
                                                                        {"kWarning", LogLevel::kWarn},
                                                                        {"kInfo", LogLevel::kInfo},
                                                                        {"kDebug", LogLevel::kDebug},
                                                                        {"kVerbose", LogLevel::kVerbose}}};

constexpr std::string::value_type kLogModeCombineChar = '|';

// coverity[autosar_cpp14_a3_3_2_violation]
const std::unordered_map<std::string_view, LogMode> kStringToLogMode{{{"kRemote", LogMode::kRemote},
                                                                      {"kConsole", LogMode::kConsole},
                                                                      {"kFile", LogMode::kFile},
                                                                      {"kSystem", LogMode::kSystem}}};

/// \brief Provide user feedback in case a configuration file contains errors.
template <typename T>
void ReportOnError(score::Result<T> result, const std::string& file_name) noexcept
{
    if (result.has_value() == true)
    {
        return;
    }

    ReportInitializationError(result.error(), std::string_view{file_name.data(), file_name.size()});
}

// Forward declare GetElementAsImpl
template <typename ResultType, typename AsType = ResultType>
class GetElementAsImpl;

template <typename ResultType, typename AsType = ResultType>
auto GetElementAs(const score::json::Object& obj, const StringLiteral key) noexcept
{
    // To prevent using function template specializations, we use class template specialization in the implementation of
    // GetElementAs()
    return GetElementAsImpl<ResultType, AsType>::GetElementAs(obj, key);
}

template <typename ResultType, typename AsType>
class GetElementAsImpl
{
  public:
    static score::Result<ResultType> GetElementAs(const score::json::Object& obj, const StringLiteral key) noexcept
    {
        const auto find_result = obj.find(key);
        if (find_result == obj.end())
        {
            return score::MakeUnexpected(Error::kConfigurationOptionalJsonKeyNotFound, key);
        }

        return find_result->second.As<AsType>();
    }
};

template <typename T>
score::Result<std::reference_wrapper<const T>> GetElementAsRef(const score::json::Object& obj,
                                                             const StringLiteral key) noexcept
{
    return GetElementAs<std::reference_wrapper<const T>, T>(obj, key);
}

template <typename T>
using GetElementCallback = score::cpp::callback<void(T)>;

template <typename T>
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from GetElementAndThen as it declared as noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
score::ResultBlank GetElementAndThen(const score::json::Object& obj,
                                   const StringLiteral key,
                                   GetElementCallback<T> update) noexcept
{
    const auto parser_result = GetElementAs<T>(obj, key);
    if (parser_result.has_value() == false)
    {
        return score::MakeUnexpected<score::Blank>(parser_result.error());
    }

    update(parser_result.value());

    return {};
}

score::ResultBlank ParseEcuId(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::string_view>(
        root,
        kEcuIdKey,
        [&config](auto value) noexcept { config.SetEcuId(value); }
    );
    // clang-format on
}

score::ResultBlank ParseAppId(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::string_view>(
        root,
        kAppIdKey,
        [&config](auto value) noexcept { config.SetAppId(value); }
    );
    // clang-format on
}

score::ResultBlank ParseAppDescription(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::string_view>(
        root,
        kAppDescriptionKey,
        [&config](auto value) noexcept { config.SetAppDescription(value); }
    );
    // clang-format on
}

score::ResultBlank ParseLogFilePath(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::string_view>(
        root,
        kLogFilePathKey,
        [&config](auto value) noexcept { config.SetLogFilePath(value); }
    );
    // clang-format on
}

/// \brief Returns the corresponding log mode of the string.
score::Result<LogMode> LogModeFromString(const std::string_view str) noexcept
{
    const auto result = kStringToLogMode.find(str);

    if (result == kStringToLogMode.end())
    {
        return MakeUnexpected(Error::kInvalidLogModeString, "Expected `kRemote`, `kConsole`, `kSystem` or `kFile`.");
    }

    return result->second;
}

/// \brief Returns the corresponding combined log mode(s) of the string.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from LogModesFromString as it declared as noexcept.
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<std::unordered_set<LogMode>> LogModesFromString(const std::string_view str) noexcept
{
    const auto segments = score::memory::LazySplitStringView{{str.data(), str.size()}, kLogModeCombineChar};

    std::unordered_set<LogMode> result;

    for (const auto segment : segments)
    {
        const auto log_mode = LogModeFromString({segment.data(), segment.size()});
        if (log_mode.has_value() == false)
        {
            return MakeUnexpected(Error::kInvalidLogModeString);
        }
        std::ignore = result.emplace(log_mode.value());
    }
    return result;
}

score::ResultBlank ParseLogMode(const score::json::Object& root, Configuration& config) noexcept
{
    // Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
    // std::terminate() will not  implicitly be called from GetElementAndThen as it declared as noexcept.
    // coverity[autosar_cpp14_a15_5_3_violation]
    return GetElementAndThen<std::string_view>(root, kLogModeKey, [&config](auto value) noexcept {
        const auto log_mode_result = LogModesFromString(value);
        if (log_mode_result.has_value())
        {
            config.SetLogMode(log_mode_result.value());  // LCOV_EXCL_BR_LINE: no branches here to be covered.
        }
    });
}

/// \brief Returns the corresponding log level of the string.
score::Result<LogLevel> LogLevelFromString(const std::string_view str) noexcept
{
    const auto result = kStringToLogLevel.find(str);
    if (result == kStringToLogLevel.end())
    {
        return MakeUnexpected(Error::kInvalidLogLevelString,
                              "Expected `kOff`, `kFatal`, `kWarn`, `kError`, `kInfo`, `kDebug` or `kVerbose`.");
    }
    return result->second;
}

/// \brief Returns the element of a JSON object as a LogLevel.
template <>
class GetElementAsImpl<LogLevel>
{
  public:
    // Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
    // std::terminate() will not  implicitly be called from GetElementAs as it declared as noexcept.
    // coverity[autosar_cpp14_a15_5_3_violation]
    static score::Result<LogLevel> GetElementAs(const score::json::Object& obj, const StringLiteral key) noexcept
    {
        const auto string_result = GetElementAsImpl<std::string_view>::GetElementAs(obj, key);
        if (string_result.has_value() == false)
        {
            return score::MakeUnexpected<LogLevel>(string_result.error());
        }

        return LogLevelFromString(string_result.value());
    }
};

score::ResultBlank ParseLogLevel(const score::json::Object& root, Configuration& config) noexcept
{
    // Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
    // std::terminate() will not  implicitly be called from GetElementAndThen as it declared as noexcept.
    // coverity[autosar_cpp14_a15_5_3_violation]
    return GetElementAndThen<std::string_view>(root, kLogLevelKey, [&config](auto value) noexcept {
        const auto log_level_result = LogLevelFromString(value);
        if (log_level_result.has_value())
        {
            config.SetDefaultLogLevel(log_level_result.value());  // LCOV_EXCL_BR_LINE: no branches here to be covered.
        }
    });
}

score::ResultBlank ParseLogLevelConsole(const score::json::Object& root, Configuration& config) noexcept
{
    // Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
    // std::terminate() will not  implicitly be called from GetElementAndThen as it declared as noexcept.
    // coverity[autosar_cpp14_a15_5_3_violation]
    return GetElementAndThen<std::string_view>(root, kLogLevelThresholdConsoleKey, [&config](auto value) noexcept {
        const auto log_level_result = LogLevelFromString(value);
        if (log_level_result.has_value())
        {
            config.SetDefaultConsoleLogLevel(log_level_result.value());
        }
    });
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from ParseContextLogLevel as it declared as noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
score::ResultBlank ParseContextLogLevel(const score::json::Object& root,
                                      Configuration& config,
                                      const std::string& path_for_reporting) noexcept
{
    const auto context_config = GetElementAsRef<score::json::List>(root, kContextConfigsKey);
    if (context_config.has_value() == false)
    {
        return score::MakeUnexpected<score::Blank>(context_config.error());
    }

    auto context_config_map = config.GetContextLogLevel();

    for (const auto& context_item : context_config.value().get())
    {
        const auto context_result = context_item.As<score::json::Object>();
        if (context_result.has_value() == false)
        {
            ReportOnError(context_result, path_for_reporting);
            continue;
        }
        const auto context_obj = context_result.value();

        const auto context_name_result = GetElementAsRef<std::string>(context_obj, kContextNameKey);
        if (context_name_result.has_value() == false)
        {
            ReportOnError(context_name_result, path_for_reporting);
            continue;
        }

        const auto context_log_level_result = GetElementAs<LogLevel>(context_obj, kLogLevelKey);
        if (context_log_level_result.has_value() == false)
        {
            ReportOnError(context_name_result, path_for_reporting);
            continue;
        }

        context_config_map[LoggingIdentifier{context_name_result.value().get()}] = context_log_level_result.value();
    }
    config.SetContextLogLevel(context_config_map);

    return {};
}

score::ResultBlank ParseStackBufferSize(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        root,
        kStackBufferSizeKey,
        [&config](auto value) noexcept { config.SetStackBufferSize(value); }
    );
    // clang-format on
}

score::ResultBlank ParseRingBufferSize(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        root,
        kRingBufferSizeKey,
        [&config](auto value) noexcept { config.SetRingBufferSize(value); }
    );
    // clang-format on
}

score::ResultBlank ParseOverwriteOnFull(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<bool>(
        root,
        kOverwriteOnFullKey,
        [&config](auto value) noexcept { config.SetRingBufferOverwriteOnFull(value); }
    );
    // clang-format on
}

score::ResultBlank ParseNumberOfSlots(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        root,
        kNumberOfSlotsKey,
        [&config](auto value) noexcept { config.SetNumberOfSlots(value); }
    );
    // clang-format on
}

score::ResultBlank ParseSlotSizeBytes(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        root,
        kSlotSizeBytesKey,
        [&config](auto value) noexcept { config.SetSlotSizeInBytes(value); }
    );
    // clang-format on
}

score::ResultBlank ParseDatarouterUid(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        root,
        kDatarouterUidKey,
        [&config](const auto value) noexcept { config.SetDataRouterUid(value); }
    );
    // clang-format on
}

score::ResultBlank ParseDynamicDatarouterIdentifiers(const score::json::Object& root, Configuration& config) noexcept
{
    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        root,
        kDynamicDatarouterIdentifiersKey,
        [&config](const auto value) noexcept { config.SetDynamicDatarouterIdentifiers(static_cast<bool>(value)); }
    );
    // clang-format on
}

score::ResultBlank ParseCircularFileLogging(const score::json::Object& root, Configuration& config) noexcept
{
    const auto file_rotation_obj_result = GetElementAsRef<score::json::Object>(root, kLogFileSizePolicyKey);
    if (!file_rotation_obj_result.has_value())
    {
        return {}; // The whole fileRotation object is optional.
    }

    // clang-format off
    return GetElementAndThen<bool>(
        file_rotation_obj_result.value().get(),
        kEnabledKey,
        [&config](auto value) noexcept { config.SetCircularFileLogging(value); }
    );
    // clang-format on
}

score::ResultBlank ParseMaxLogFileSizeBytes(const score::json::Object& root, Configuration& config) noexcept
{
    const auto file_rotation_obj_result = GetElementAsRef<score::json::Object>(root, kLogFileSizePolicyKey);
    if (!file_rotation_obj_result.has_value())
    {
        return {}; // The whole fileRotation object is optional.
    }

    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        file_rotation_obj_result.value().get(),
        kMaxFileSizeInBytesKey,
        [&config](auto value) noexcept { config.SetMaxLogFileSizeBytes(value); }
    );
    // clang-format on
}

score::ResultBlank ParseOverwriteLogOnFull(const score::json::Object& root, Configuration& config) noexcept
{
    const auto file_rotation_obj_result = GetElementAsRef<score::json::Object>(root, kLogFileSizePolicyKey);
    if (!file_rotation_obj_result.has_value())
    {
        return {}; // The whole fileRotation object is optional.
    }

    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<bool>(
        file_rotation_obj_result.value().get(),
        kOverwriteExistingFilesKey,
        [&config](auto value) noexcept { config.SetOverwriteLogOnFull(value); }
    );
    // clang-format on
}

score::ResultBlank ParseNoOfLogFiles(const score::json::Object& root, Configuration& config) noexcept
{
    const auto file_rotation_obj_result = GetElementAsRef<score::json::Object>(root, kLogFileSizePolicyKey);
    if (!file_rotation_obj_result.has_value())
    {
        return {}; // The whole fileRotation object is optional.
    }

    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<std::size_t>(
        file_rotation_obj_result.value().get(),
        kNumberOfFilesKey,
        [&config](auto value) noexcept { config.SetNoOfLogFiles(value); }
    );
    // clang-format on
}

score::ResultBlank ParseDeleteOldLogFiles(const score::json::Object& root, Configuration& config) noexcept
{
    const auto file_rotation_obj_result = GetElementAsRef<score::json::Object>(root, kLogFileSizePolicyKey);
    if (!file_rotation_obj_result.has_value())
    {
        return {}; // The whole fileRotation object is optional.
    }

    // Disabling clang-format to address Coverity warning: autosar_cpp14_a7_1_7_violation
    // clang-format off
    return GetElementAndThen<bool>(
        file_rotation_obj_result.value().get(),
        kTruncateOnRotationKey,
        [&config](auto value) noexcept { config.SetDeleteOldLogFiles(value); }
    );
    // clang-format on
}

void ParseConfigurationElements(const score::json::Object& root, const std::string& path, Configuration& config) noexcept
{
    ReportOnError(ParseEcuId(root, config), path);
    ReportOnError(ParseAppId(root, config), path);
    ReportOnError(ParseAppDescription(root, config), path);
    ReportOnError(ParseLogFilePath(root, config), path);
    ReportOnError(ParseLogMode(root, config), path);
    ReportOnError(ParseLogLevel(root, config), path);
    ReportOnError(ParseLogLevelConsole(root, config), path);
    ReportOnError(ParseContextLogLevel(root, config, path), path);
    ReportOnError(ParseStackBufferSize(root, config), path);
    ReportOnError(ParseRingBufferSize(root, config), path);
    ReportOnError(ParseOverwriteOnFull(root, config), path);
    ReportOnError(ParseNumberOfSlots(root, config), path);
    ReportOnError(ParseSlotSizeBytes(root, config), path);
    ReportOnError(ParseDatarouterUid(root, config), path);
    ReportOnError(ParseDynamicDatarouterIdentifiers(root, config), path);
    ReportOnError(ParseCircularFileLogging(root, config), path);
    ReportOnError(ParseMaxLogFileSizeBytes(root, config), path);
    ReportOnError(ParseOverwriteLogOnFull(root, config), path);
    ReportOnError(ParseNoOfLogFiles(root, config), path);
    ReportOnError(ParseDeleteOldLogFiles(root, config), path);
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from ParseAndUpdateConfiguration as it declared as noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<Configuration> ParseAndUpdateConfiguration(const std::string& path, Configuration config) noexcept
{
    const score::json::JsonParser json_parser_obj;
    // FromFile() is safe, if the JSON file is stored in stored on qtsafefs (integrity protection).
    // See broken_link_c/item/13777082 and broken_link_c/issue/6576406
    // NOLINTNEXTLINE(score-banned-function) - Argumentation is above.
    const auto json_result = json_parser_obj.FromFile(path);
    if (json_result.has_value() == false)
    {
        return score::MakeUnexpected<Configuration>(json_result.error());
    }

    const auto root_result = json_result.value().As<score::json::Object>();
    if (root_result.has_value() == false)
    {
        return score::MakeUnexpected<Configuration>(root_result.error());
    }

    ParseConfigurationElements(root_result->get(), path, config);
    return config;
}

}  // namespace

TargetConfigReader::TargetConfigReader(std::unique_ptr<IConfigurationFileDiscoverer> discoverer) noexcept
    : ITargetConfigReader{}, discoverer_(std::move(discoverer))
{
}

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// std::terminate() will not  implicitly be called from ReadConfig as it declared as noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
score::Result<Configuration> TargetConfigReader::ReadConfig() const noexcept
{
    const auto config_files = discoverer_->FindConfigurationFiles();

    if (config_files.empty())
    {
        return MakeUnexpected(Error::kConfigurationFilesNotFound);
    }

    Configuration config{};

    // Update the config instance by iterating over the config_files
    // Each config file can overwrite the previous config files.
    for (const auto& config_file : config_files)
    {
        auto result = ParseAndUpdateConfiguration(config_file, config);

        if (result.has_value() == false)
        {
            ReportOnError(result, config_file);
            continue;
        }

        config = std::move(result.value());
    }

    return config;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
