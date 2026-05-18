<!--
Copyright 2024, Toyota Motor Corporation

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable MD041 -->
<!-- prettier-ignore -->
\page compiler_support_diagnostics Facilities Suppressing Compiler Diagnostics

<!-- markdownlint-enable MD041 -->

\arene_base provides facilities for uniformly suppressing compiler diagnostics
in a cross-compiler manner without exposing `#pragma` to user code. The public
header is:

```cpp
#include "arene/base/compiler_support/diagnostics.hpp"
```

\warning Users should _only_ use the facilities discussed below with proper
justification. Compiler diagnostics should not be suppressed without specific,
documented justification, such as working around a compiler bug, or unit testing
error handling for behavior that would otherwise be prevented by non-default
compiler diagnostics. They should **never** be suppressed in order to land code
without resolving the warning to meet deadlines.

## Suppressing Compiler Diagnostics {#suppressing-compiler-diagnostics}

The process for suppressing one or more diagnostics is as follows:

1. Invoke the `ARENE_IGNORE_START()` macro.
2. Invoke `ARENE_IGNORE_XXX(diagnostic, rational)` macros as appropraite.
   **ALWAYS INCLUDE JUSTIFICATION IN THE RATIONAL FIELD!**.
3. Execute the statements which need diagnostic suppression.
4. Invoke the `ARENE_IGNORE_END()` macro.

The amount of code contained between `ARENE_IGNORE_START()` and
`ARENE_IGNORE_END()` should be the _minimum amount of logic possible_. Do
**not** simply suppress diagnostics at the start of a source file and disable
them at the end.

Example usage of suppressing `-Wfloat-equal` in an implementation where an exact
floating point comparison to the precise value of positive zero is desired:

```cpp
ARENE_IGNORE_START();
ARENE_IGNORE_ALL(
    "-Wfloat-equal",
    "In this situation testing for exact equality to zero to handle edge cases with NaN and Inf in floats.");
/// @brief Calculate the absolute value of a 32-bit or 64-bit floating number.
/// @param value
/// @returns The absolute value of @c value .
template <class Number, constraints<std::enable_if_t<(std::is_floating_point<Number>::value)>> = nullptr>
constexpr Number abs(Number value) noexcept {
  constexpr auto zero = static_cast<Number>(0);
  return value < zero ? -value : (value == zero ? zero : (value > zero ? value : -value));
}
ARENE_IGNORE_END();
```

## Supported Suppression Macros {#supported-suppression-macros}

|              Macro               |                                     Description                                      |
| :------------------------------: | :----------------------------------------------------------------------------------: |
|  `ARENE_IGNORE_GCC(s, comment)`  |  The diagnostic should only be attempted to be suppressed when compiling with `gcc`  |
| `ARENE_IGNORE_CLANG(s, comment)` | The diagnostic should only be attempted to be suppressed when compiling with `clang` |
|  `ARENE_IGNORE_ALL(s, comment)`  |              The diagnostic should always be attempted to be suppressed              |

\note Currently, `ARENE_IGNORE_ALL` only supports `clang` and `gcc`.

\warning If a diagnostic is passed to `ARENE_IGNORE_ALL` which is unknown to the
current compiler, this generally will result in a compiler error. Only use this
when you are positive that the given diagnostic name is the same across all
compilers.
