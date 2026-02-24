cc_library(
    name = "acl",
    srcs = select({
        "@platforms//cpu:aarch64": ["usr/lib/libacl.a"],
        "@platforms//cpu:x86_64": ["usr/lib/libacl.a"],
    }),
    hdrs = [
        "usr/include/acl/libacl.h",
        "usr/include/sys/acl.h", 
    ],
    includes = ["usr/include/"],
    visibility = ["//visibility:public"],
)