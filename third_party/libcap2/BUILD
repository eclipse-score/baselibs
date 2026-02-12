alias(
    name = "libcap2",
    actual = select({
        "@platforms//cpu:aarch64": "@libcap2-dev-deb-aarch64//:libcap2",
        "//conditions:default": "@libcap2-dev-deb//:libcap2",
    }),
    visibility = ["//visibility:public"],
)
