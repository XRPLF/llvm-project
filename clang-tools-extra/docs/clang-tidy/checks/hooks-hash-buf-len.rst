.. title:: clang-tidy - hooks-hash-buf-len

hooks-hash-buf-len
==================

Function `util_sha512h` has fixed-size hash output.

This check warns about too-small size of its output buffer (if it's
specified by a constant - variable parameter is ignored).
