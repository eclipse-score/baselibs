cc_library(
    name = "libcap2",
    srcs = select({
        "@platforms//cpu:aarch64": [
            "@libcap2-deb-aarch64//:lib/aarch64-linux-gnu/libcap.so.2",
            "@libcap2-deb-aarch64//:lib/aarch64-linux-gnu/libcap.so.2.25",
        ],
        "//conditions:default": [
            "@libcap2-deb//:lib/x86_64-linux-gnu/libcap.so.2",
            "@libcap2-deb//:lib/x86_64-linux-gnu/libcap.so.2.25",
        ],
    }),
    hdrs = [
        "usr/include/sys/capability.h",
    ],
    includes = [
        "usr/include/",
        "usr/include/sys/",
    ],
      visibility = ["//visibility:public"],
)