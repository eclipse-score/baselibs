load("@score_baselibs//:bazel/unit_tests.bzl", "cc_unit_test_suites_for_host_and_qnx")

COMPILER_WARNING_FEATURES = [
    "treat_warnings_as_errors",
    "additional_warnings",
    "strict_warnings",
]

cc_library(
    name = "file_recorder",
    srcs = [
        "dlt_message_builder.cpp",
        "dlt_message_builder.h",
        "dlt_message_builder_types.h",
        "file_recorder.cpp",
    ],
    hdrs = [
        "file_recorder.h",
        "svp_time.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail:__pkg__",
        "@score_baselibs//score/mw/log/detail/common:__pkg__",
    ],
    deps = [
        "@score_baselibs//score/language/futurecpp",
        "@score_baselibs//score/mw/log:recorder",
        "@score_baselibs//score/mw/log:shared_types",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/mw/log/detail:backend_interface",
        "@score_baselibs//score/mw/log/detail/common:dlt_content_formatting",
        "@score_baselibs//score/mw/log/detail/common:statistics_reporter",
        "@score_baselibs//score/mw/log/detail/text_recorder:file_output_backend",
        "@score_baselibs//score/mw/log/detail/text_recorder:message_builder_interface",
        "@score_baselibs//score/os/utils:high_resolution_steady_clock",
        "@score_baselibs//score/static_reflection_with_serialization/serialization",
    ],
)

cc_library(
    name = "file_recorder_factory",
    srcs = [
        "file_recorder_factory.cpp",
    ],
    hdrs = [
        "file_recorder_factory.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    tags = ["FFI"],
    visibility = [
        "@score_baselibs//score/mw/log/detail:__pkg__",
        "@score_baselibs//score/mw/log/detail/common:__pkg__",
    ],
    deps = [
        ":file_recorder",
        "@score_baselibs//score/mw/log/configuration",
        "@score_baselibs//score/mw/log/detail:empty_recorder",
        "@score_baselibs//score/mw/log/detail:initialization_reporter",
        "@score_baselibs//score/mw/log/detail:log_recorder_factory",
    ],
)

cc_test(
    name = "unit_test",
    srcs = [
        "dlt_message_builder_test.cpp",
        "file_recorder_factory_test.cpp",
        "file_recorder_test.cpp",
    ],
    data = [
    ],
    features = COMPILER_WARNING_FEATURES + [
        "aborts_upon_exception",
    ],
    tags = ["unit"],
    deps = [
        ":file_recorder",
        ":file_recorder_factory",
        "@googletest//:gtest_main",
        "@score_baselibs//score/mw/log/detail:backend_mock",
        "@score_baselibs//score/mw/log/detail/text_recorder:file_output_backend",
        "@score_baselibs//score/mw/log/detail/text_recorder:file_output_backend_mocks",
        "@score_baselibs//score/mw/log/test/console_logging_environment",
        "@score_baselibs//score/os/mocklib:fcntl_mock",
    ],
)

cc_unit_test_suites_for_host_and_qnx(
    name = "unit_tests",
    cc_unit_tests = [
        ":unit_test",
    ],
    visibility = [
        "@score_baselibs//score/mw/log/detail:__pkg__",
    ],
)
