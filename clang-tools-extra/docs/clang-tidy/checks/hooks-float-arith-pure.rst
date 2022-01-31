.. title:: clang-tidy - hooks-float-arith-pure

hooks-float-arith-pure
======================

Hooks can compute floating-point values in XFL format by calling
functions `float_multiply`, `float_mulratio`, `float_negate`,
`float_sum`, `float_invert` and `float_divide`, convert the results to
integers by calling `float_int` and access their constituent parts by
calling `float_exponent`, `float_mantissa` and `float_sign`. If the
inputs of the computation never change, a more efficient way to do
this is to precompute it.

This check warns about calls of the aforementioned functions with
constant inputs and proposes to add a tracing statement showing the
computed value (so that the user can use it to replace the call).
