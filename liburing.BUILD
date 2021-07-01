cc_library(
    name = "main",
    srcs = glob(["src/*.c"]),
    hdrs = glob(["src/*.h", "src/include/*.h", "src/include/liburing/*.h"]),
    copts = ["-Iexternal/liburing/src/include"],
    visibility = ["//visibility:public"]
)