alias(
    name = "acl",
    actual = select({
        "@platforms//cpu:aarch64": "@acl-deb-aarch64//:acl",
        "//conditions:default": "@acl-deb//:acl",
    }),
    visibility = ["//visibility:public"],
)
