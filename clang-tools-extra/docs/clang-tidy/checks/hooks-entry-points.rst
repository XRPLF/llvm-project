.. title:: clang-tidy - hooks-entry-points

hooks-entry-points
==================

A Hook always implements and exports exactly two functions: cbak and
hook.

This check warns on (non-empty) translation units that do not have
them.
