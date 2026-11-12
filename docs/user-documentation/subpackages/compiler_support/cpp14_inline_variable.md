<!--
Copyright 2026, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- markdownlint-disable MD013 -->
<!-- prettier-ignore -->
\page compiler_support_cpp14_inline_variable Facilities For Creating Inline Variables in C++14

<!-- markdownlint-enable MD041 -->
<!-- markdownlint-enable MD013 -->

C++17 added support for declaring `inline` variables, which allow variables to
be defined in header files without violating the One Definition Rule (ODR). To
allow similar functionality in C++14, \arene_base provides a facility,
`ARENE_CPP14_INLINE_VARIABLE(type,name)`, for creating "inline variables" in
C++14. This primarily intended to ease the creation of Niebloid-like function
objects. The public header is:

```cpp
#include "arene/base/compiler_support/cpp14_inline.hpp"
```

An example of defining a function object using the macro can be seen below:

<!-- prettier-ignore-start -->
\snippet docs/examples/compiler_support_examples.cpp arene_cpp14_inline_variable_fo
<!-- prettier-ignore-end -->

\note The type used with the macro must be `constexpr` default constructible.

# Initializer-expression form

Some "inline variables" cannot be produced by default-constructing their type:
for example, the result of `arene::base::bind_back(callable, args...)` captures
the bound callable and arguments as state, so the same value cannot be
re-created from the type alone. For these cases \arene_base provides a companion
macro that takes an initializer expression instead of a type:

```cpp
ARENE_CPP14_INLINE_VARIABLE_INIT(name, init_expression)
```

The deduced type is whatever `init_expression` evaluates to, just like a plain
`constexpr auto name = init_expression`. The initializer is variadic so that
expressions containing commas (e.g. template arguments of the constructed type)
do not need extra parentheses at the call site.

In a `>= C++17` context this expands to
`inline constexpr auto name = init_expression`, which the standard guarantees is
a single instance across translation units. In C++14 contexts the macro uses the
variable-template + reference pattern documented in \ref extern_constexpr to
obtain a single ODR-safe instance: an `extern constexpr` variable template
tagged by `std::nullptr_t` is declared in a per-macro-invocation namespace, and
a `static constexpr` reference in the enclosing namespace binds to that one
instance so the trailing `<>` is hidden at use sites.

Unlike `ARENE_CPP14_INLINE_VARIABLE` this form does not require
default-constructibility, but it does require that the initializer be a constant
expression suitable for a `constexpr` variable.
