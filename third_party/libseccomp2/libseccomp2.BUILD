cc_library(
    name = "libseccomp2",
    srcs = select({
        "@platforms//cpu:aarch64": [
            "@libseccomp2-deb-aarch64//:usr/lib/aarch64-linux-gnu/libseccomp.so.2",
            "@libseccomp2-deb-aarch64//:usr/lib/aarch64-linux-gnu/libseccomp.so.2.5.3",
        ],
        "//conditions:default": [
            "@libseccomp2-deb//:lib/x86_64-linux-gnu/libseccomp.so.2",
            "@libseccomp2-deb//:lib/x86_64-linux-gnu/libseccomp.so.2.5.1",
        ],
    }),
    hdrs = [
        "usr/include/seccomp.h",
        "usr/include/seccomp-syscalls.h",
    ],
    includes = [
        "usr/include/",
        "usr/include/sys/",
    ],
    visibility = ["//visibility:public"],
)