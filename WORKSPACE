workspace(name = "templates")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "liburing",
    urls = ["https://github.com/axboe/liburing/archive/refs/tags/liburing-2.0.tar.gz"],
    sha256 = "ca069ecc4aa1baf1031bd772e4e97f7e26dfb6bb733d79f70159589b22ab4dc0",
    strip_prefix = "liburing-liburing-2.0",
    build_file = "@//:liburing.BUILD",

    patch_cmds=["./configure"]
)