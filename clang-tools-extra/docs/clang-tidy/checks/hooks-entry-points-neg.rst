.. title:: clang-tidy - hooks-entry-points-neg

hooks-entry-points-neg
======================

A Hook always implements and exports exactly two functions: cbak and
hook.

This check warns about extern function definitions with unexpected
(that is, neither "hook" nor "cbak") names.
