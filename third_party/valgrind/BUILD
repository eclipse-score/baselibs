alias(
    name = "valgrind",
    actual = select({
        "@platforms//cpu:aarch64": "@valgrind-deb-aarch64//:valgrind",
        "//conditions:default": "@valgrind-deb//:valgrind",
    }),
    visibility = ["//visibility:public"],
)
