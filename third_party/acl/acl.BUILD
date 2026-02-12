cc_library(
    name = "acl",
    srcs = select({
        "@platforms//cpu:aarch64": ["lib/aarch64-linux-gnu/libacl.a"],
        "//conditions:default": ["lib/x86_64-linux-gnu/libacl.a"],
    }),
    hdrs = [
        "usr/include/acl/libacl.h",
        "usr/include/sys/acl.h",
    ],
    includes = ["usr/include/"],
    visibility = ["//visibility:public"],
)