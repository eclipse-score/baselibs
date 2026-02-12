alias(
    name = "libseccomp2",
    actual = select({
        "@platforms//cpu:aarch64": "@libseccomp2-dev-deb-aarch64//:libseccomp2",
        "//conditions:default": "@libseccomp2-dev-deb//:libseccomp2",
    }),
    visibility = ["//visibility:public"],
)
