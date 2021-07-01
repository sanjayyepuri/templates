cc_binary(
    name = "server",
    srcs = ["src/main.cc"],
    copts = ["-Iexternal/liburing/src/include"],
    deps = ["@liburing//:main"]
)