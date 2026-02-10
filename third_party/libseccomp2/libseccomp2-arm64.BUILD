cc_library(
    name = "libseccomp2",
    srcs = [
        "@libseccomp2-deb-arm64//:lib/aarch64-linux-gnu/libseccomp.so.2",
        "@libseccomp2-deb-arm64//:lib/aarch64-linux-gnu/libseccomp.so.2.5.1",
    ],
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