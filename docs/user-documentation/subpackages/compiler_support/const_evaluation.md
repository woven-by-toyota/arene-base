<!-- Copyright 2024, Toyota Motor Corporation -->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compiler_support_const_evaluation Facilities For Detecting Constant Evaluation

<!-- markdownlint-enable MD041 -->

\arene_base provides a helper for detecting if the current execution **may** be
executing under a constant expression. The public header is:

```cpp
#include "arene/base/compiler_support/is_possibly_constant_evaluated.hpp
```

The `arene::base::is_possibly_constant_evaluated()` API is similar, but not
identical to, C++20's
[`std::is_constant_evaluated`](https://en.cppreference.com/w/cpp/types/is_constant_evaluated)
: while `std::is_constant_evaluated` returns `true` if and only if the current
context **is** a constant evaluated context,
`arene::base::is_possible_constant_evaluated` returns `false` if and only if the
current context is **not** a constant evaluated context.

This query can be used to provide alternative implementations of logic that may
be more efficient but could not be executed inside a constant evaluated context.
The following is an example of a 16-bit `byte_swap` implementation which invokes
a non-`constexpr` compiler builtin when guaranteed to be outside a constant
evaluated context, and falls back to a hand-written implementation otherwise:

```cpp
constexpr std::uint16_t byte_swap(std::uint16_t value) noexcept {
#if ARENE_HAS_BUILTIN(__builtin_bswap16)
  if (!arene::base::is_possibly_constant_evaluated()) {
    return __builtin_bswap16(value);
  }
#endif
  return static_cast<std::uint16_t>(value << 8U) | static_cast<std::uint16_t>(value >> 8U);
}
```

\note Determining if the current execution context is constant-evaluated
requires compiler support through the `__builtin_is_constant_evaluated`
intrinsic, which was added specifically to allow the implementation of
`std::is_constant_evaluated` . Therefore on compilers lacking these intrinsics,
it is not possible to determine if the current context is constant-evaluated, so
`is_possibly_constant_evaluated` always returns `true` in those cases. Relaxing
the semantics of this API allows code to be written in pre-C++20 contexts to
perform this query, and if the compiler supports it, benefit, while always
safely falling back to the `constexpr` compatible implementation when compiled
with compilers which do not support the intrinsic.
`__builtin_is_constant_evaluated` is supported in `gcc >= 9`, and `clang >= 9`.
